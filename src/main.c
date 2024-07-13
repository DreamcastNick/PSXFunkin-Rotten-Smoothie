#include "main.h"
#include "timer.h"
#include "io.h"
#include "gfx.h"
#include "audio.h"
#include "pad.h"
#include "str.h"

#include "menu.h"
#include "save.h"
#include "stage.h"
#include "pause.h"

// Game loop
GameLoop gameloop;

// Error handler
char error_msg[0x200];

void ErrorLock(void) {
    while (1) {
#ifdef PSXF_PC
        MsgPrint(error_msg);
        exit(1);
#else
        FntPrint("A fatal error has occurred\n~c700%s\n", error_msg);
        Gfx_Flip();
#endif
    }
}

// Memory heap
#define MEM_STAT // This will enable the Mem_GetStat function which returns information about available memory in the heap

#define MEM_IMPLEMENTATION
#include "mem.h"
#undef MEM_IMPLEMENTATION

#ifndef PSXF_STDMEM
static u8 malloc_heap[0x1B0000];  // 8MB heap size (8 * 1024 * 1024 bytes)
#endif

// Entry point
int main(int argc, char **argv) {
    // Remember arguments
    my_argc = argc;
    my_argv = argv;
    
    // Initialize system
    PSX_Init();
    
    Mem_Init((void*)malloc_heap, sizeof(malloc_heap));
    
    IO_Init();
    Audio_Init();
    Gfx_Init();
    Pad_Init();
    Str_Init();

    MCRD_Init();
    
    Timer_Init();

    // If not found a save, enable some options
    if (ReadSave() == false) {
        // Options that's already enabled for an easier start
        stage.prefs.songtimer = true;
    }
    
    // Start game
    gameloop = GameLoop_Menu;
    Menu_Load(MenuPage_Opening);
    
    // Game loop
    while (PSX_Running()) {
        // Prepare frame
        Timer_Tick();
        Audio_ProcessXA();
        Pad_Update();

		#ifdef MEM_STAT
			// Memory stats
			size_t mem_used, mem_size, mem_max;
			Mem_GetStat(&mem_used, &mem_size, &mem_max);
			#ifndef MEM_BAR
				FntPrint("mem: %08X/%08X (max %08X)\n", mem_used, mem_size, mem_max);
			#endif
		#endif

        // Tick and draw game
        switch (gameloop) {
            case GameLoop_Menu:
                Menu_Tick();
                break;
            case GameLoop_Stage:
                if (!movie_is_playing)
                    Stage_Tick();
                else
                {
				    Game_Update();
                }
                break;
            case GameLoop_Pause:
                PausedState();
                break;
        }

        // Flip gfx buffers
        Gfx_Flip();
        audio_skipped = false;
    }

    // Deinitialize system
    Pad_Quit();
    Gfx_Quit();
    Audio_Quit();
    IO_Quit();
    PSX_Quit();
    return 0;
}