#include "str.h"

#include "psx.h"
#include "main.h"
#include "timer.h"
#include "io.h"
#include "gfx.h"
#include "audio.h"
#include "pad.h"
#include "psn00b/strnoob.h"
#include "mem.h"
#include "stage.h"

boolean movie_is_playing = false;
boolean movie_needs_init;
extern char error_msg[0x200]; // Adjust size as necessary

#ifdef DISP_24BPP
#define BLOCK_SIZE 24
#else
#define BLOCK_SIZE 16
//#define DRAW_OVERLAY
#endif

#define VRAM_X_COORD(x) ((x) * BLOCK_SIZE / 16)

typedef struct {
    u16 magic;          // Always 0x0160
    u16 type;           // 0x8001 for MDEC
    u16 sector_id;      // Chunk number (0 = first chunk of this frame)
    u16 sector_count;   // Total number of chunks for this frame
    u32 frame_id;       // Frame number
    u32 bs_length;      // Total length of this frame in bytes

    u16 width, height;
    u8  bs_header[8];
    u32 _reserved;
} STR_Header;

typedef struct {
    u16 width, height;
    u32 bs_data[0x2000];    // Bitstream data read from the disc
    u32 mdec_data[0x8000];  // Decompressed data to be fed to the MDEC
} StreamBuffer;

typedef struct {
    StreamBuffer frames[2];
    u32 slices[2][BLOCK_SIZE * SCREEN_HEIGHT / 2];

    int frame_id, sector_count;
    int dropped_frames;
    RECT slice_pos;
    int frame_width;

    volatile s8 sector_pending, frame_ready;
    volatile s8 cur_frame, cur_slice;
} StreamContext;

typedef struct {
    const char* name;
    StageId id;
    boolean is_final; //Should play in the final of the song
} STR_Def;

static StreamContext* str_ctx;
static STR_Header* sector_header;

static const STR_Def str_def[] = {
    #include "strdef.h"
};

void cd_sector_handler(void) {
    StreamBuffer *frame = &str_ctx->frames[str_ctx->cur_frame];

    CdGetSector(sector_header, sizeof(STR_Header) / 4);

    if (sector_header->magic != 0x0160) {
        str_ctx->frame_ready = -1;
        return;
    }

    if (sector_header->type != 0x8001)
        return;

    if ((int) sector_header->frame_id < str_ctx->frame_id) {
        str_ctx->frame_ready = -1;
        return;
    }

    if ((int) sector_header->frame_id > str_ctx->frame_id) {
        if (str_ctx->sector_count)
            str_ctx->dropped_frames++;
        else
            str_ctx->frame_ready = 1;

        str_ctx->frame_id     = sector_header->frame_id;
        str_ctx->sector_count = sector_header->sector_count;
        str_ctx->cur_frame   ^= 1;

        frame = &str_ctx->frames[str_ctx->cur_frame];

        frame->width  = (sector_header->width  + 15) & 0xfff0;
        frame->height = (sector_header->height + 15) & 0xfff0;
    }

    str_ctx->sector_count--;
    CdGetSector(
        &(frame->bs_data[2016 / 4 * sector_header->sector_id]),
        2016 / 4
    );
}

void mdec_dma_handler(void) {
    if (str_ctx->sector_pending) {
        cd_sector_handler();
        str_ctx->sector_pending = 0;
    }

    LoadImage(&str_ctx->slice_pos, str_ctx->slices[str_ctx->cur_slice]);

    str_ctx->cur_slice   ^= 1;
    str_ctx->slice_pos.x += BLOCK_SIZE;

    if (str_ctx->slice_pos.x < str_ctx->frame_width)
        DecDCTout(
            str_ctx->slices[str_ctx->cur_slice],
            BLOCK_SIZE * str_ctx->slice_pos.h / 2
        );
}

void cd_event_handler(u8 event, u8 *payload) {
    if (event != CdlDataReady)
        return;

    if (DecDCTinSync(1))
        str_ctx->sector_pending = 1;
    else
        cd_sector_handler();
}

StreamBuffer *get_next_frame(void) {
    while (!str_ctx->frame_ready)
        __asm__ volatile("");

    if (str_ctx->frame_ready < 0) {
        return 0;
    }

    str_ctx->frame_ready = 0;
    return &str_ctx->frames[str_ctx->cur_frame ^ 1];
}

static void STR_InitStream(void) {
    Audio_StopXA();
    EnterCriticalSection();
    DecDCToutCallback(&mdec_dma_handler);
    CdReadyCallback(&cd_event_handler);
    ExitCriticalSection();

    DecDCTvlcCopyTableV3((VLC_TableV3*) 0x1f800000);

    movie_is_playing = true;
    str_ctx->cur_frame = 0;
    str_ctx->cur_slice = 0;
}

static void STR_StopStream(void) {
    CdControlB(CdlPause, 0, 0);
    EnterCriticalSection();
    CdReadyCallback(NULL);
    DecDCToutCallback(NULL);
    ExitCriticalSection();
    movie_is_playing = false;
}

void Str_Update(void) {
    StreamBuffer *frame = get_next_frame();

    if (!frame) {
        // If no frame is available, stop the stream
        movie_is_playing = false;
        STR_StopStream();
        Mem_Free(str_ctx);
        Mem_Free(sector_header);

        Audio_PlayXA_Track(stage.stage_def->music_track, 0x40, stage.stage_def->music_channel, 0);
        Audio_SetPos(164);
        Gfx_EnableClear();
        return;
    }

    // Start decoding the video frame
    VLC_Context vlc_ctx;
    DecDCTvlcStart(&vlc_ctx, frame->mdec_data, sizeof(frame->mdec_data) / 4, frame->bs_data);

    // Decompress the MDEC data
    DecDCTin(frame->mdec_data, DECDCT_MODE_16BPP);

    // Calculate display position
    RECT *fb_clip = Gfx_GetDrawClip();
    int x_offset = (fb_clip->w - frame->width) / 2;
    int y_offset = (fb_clip->h - frame->height) / 2;

    // Set the slice position and dimensions
    str_ctx->slice_pos.x = fb_clip->x + VRAM_X_COORD(x_offset);
    str_ctx->slice_pos.y = fb_clip->y + y_offset;
    str_ctx->slice_pos.w = BLOCK_SIZE;
    str_ctx->slice_pos.h = frame->height;
    str_ctx->frame_width = VRAM_X_COORD(frame->width);

    // Output the DCT data to VRAM
    DecDCTout(str_ctx->slices[str_ctx->cur_slice], BLOCK_SIZE * str_ctx->slice_pos.h / 2);
}

void Str_Init(void) {
    DecDCTReset(0);
    movie_is_playing = false;
    movie_needs_init = true;
}

void Str_Play(const char *filedir)
{
    Gfx_DisableClear();
    CdInit();
    str_ctx = Mem_Alloc(sizeof(StreamContext));
    sector_header = Mem_Alloc(sizeof(STR_Header));
    STR_InitStream();

    str_ctx->frame_id       = -1;
    str_ctx->dropped_frames =  0;
    str_ctx->sector_pending =  0;
    str_ctx->frame_ready    =  0;

    CdlFILE file;

    IO_FindFile(&file, filedir);
    CdSync(0, 0);

    // Configure the CD drive to read at 2x speed and to play any XA-ADPCM
    // sectors that might be interleaved with the video data.
    u8 mode = CdlModeRT | CdlModeSpeed;
    CdControl(CdlSetmode, (u8 *) &mode, 0);

    // Start reading in real-time mode (i.e. without retrying in case of read
    // errors) and wait for the first frame to be buffered.
    CdControl(CdlReadS, (u8*)&file.pos, 0);

    get_next_frame();
}

void Str_CanPlayBegin(void) {
    for (u8 i = 0; i < COUNT_OF(str_def); i++) {
        if (str_def[i].id == stage.stage_id && !str_def[i].is_final && stage.story) {
            Str_Play(str_def[i].name);
        }
    }
}

void Str_CanPlayFinal(void) {
    for (u8 i = 0; i < COUNT_OF(str_def); i++) {
        if (str_def[i].id == stage.stage_id && str_def[i].is_final && stage.story) {
            Str_Play(str_def[i].name);
        }
    }
}

// Function to update the game state
void Game_Update(void) {
    Str_Update();
}
