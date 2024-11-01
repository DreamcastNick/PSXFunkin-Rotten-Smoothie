#ifndef PSXF_GUARD_FONT_H
#define PSXF_GUARD_FONT_H

#include "gfx.h"

//Font types
typedef enum
{
	Font_Bold,
	Font_Arial,
	Font_CDR,
} Font;

typedef enum
{
	FontAlign_Left,
	FontAlign_Center,
	FontAlign_Right,
} FontAlign;

typedef struct FontData
{
	//Font functions and data
	fixed_t (*get_width)(struct FontData *this, const char *text);
	void (*draw_col)(struct FontData *this, const char *text, s32 x, s32 y, FontAlign align, u8 r, u8 g, u8 b);
	void (*draw)(struct FontData *this, const char *text, s32 x, s32 y, FontAlign align);
	
	Gfx_Tex tex;
} FontData;

typedef struct
{
	//font
	FontData font_cdr, font_bold, font_arial;
} Fonts;

extern Fonts fonts;

//Font functions
void Initalize_Fonts(void);
void FontData_Load(FontData *this, Font font);

#endif
