/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "kitchen.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Kitchen background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	IO_Data arc_apple, arc_apple_ptr[1];
	IO_Data arc_fruit, arc_fruit_ptr[5];
	IO_Data arc_hand, arc_hand_ptr[1];
	IO_Data arc_orange, arc_orange_ptr[18];
	IO_Data arc_plat, arc_plat_ptr[3];
	IO_Data arc_rapple, arc_rapple_ptr[1];
	IO_Data arc_speed, arc_speed_ptr[3];
	IO_Data arc_static, arc_static_ptr[1];
	
	//Textures
	Gfx_Tex tex_back0;
	Gfx_Tex tex_back1;
	Gfx_Tex tex_cut0;
	
	Gfx_Tex tex_apple;
	u8 apple_frame, apple_tex_id;

	Gfx_Tex tex_fruit;
	u8 fruit_frame, fruit_tex_id;
	
	Gfx_Tex tex_hand;
	u8 hand_frame, hand_tex_id;

	Gfx_Tex tex_orange;
	u8 orange_frame, orange_tex_id;
	
	Gfx_Tex tex_plat;
	u8 plat_frame, plat_tex_id;

	Gfx_Tex tex_rapple;
	u8 rapple_frame, rapple_tex_id;
	
	Gfx_Tex tex_speed;
	u8 speed_frame, speed_tex_id;

	Gfx_Tex tex_static;
	u8 static_frame, static_tex_id;

	Animatable appleshock_animatable;
	Animatable applestand_animatable;
	
	Animatable fruitnormal_animatable;
	Animatable fruitscary_animatable;
	
	Animatable handa_animatable;
	Animatable handb_animatable;
	Animatable handc_animatable;
	
	Animatable orangefinal_animatable;
	Animatable orangesinging_animatable;
	Animatable orangetaunt_animatable;
	Animatable orangewatching_animatable;
	Animatable orangeover_animatable;
	
	Animatable plat_animatable;
	
	Animatable rappleidle_animatable;
	Animatable rapplefall_animatable;
	Animatable rapplehit_animatable;
	
	Animatable speedfall_animatable;
	Animatable speedhit_animatable;
	
	Animatable static_animatable;
} Back_Kitchen;

static const CharFrame apple_frame[5] = {
  {0, {  0,  0, 57, 64}, {160,160}}, //0 shock 1
  {0, { 57,  0, 57, 64}, {160,160}}, //1 shock 2
  {0, {114,  0, 57, 64}, {160,160}}, //2 shock 3
  {0, {171,  0, 57, 64}, {160,160}}, //3 shock 4

  {0, {  0, 64, 41,145}, {147,240}}, //4 stand 1
};

static const CharFrame fruit_frame[10] = {
  {0, {  0,  0,220,108}, {160,160}}, //0 normal 1

  {0, {  0,108,219,109}, {159,160}}, //1 scary 1
  {1, {  0,  0,220,109}, {160,160}}, //2 scary 2
  {1, {  0,109,216,109}, {159,160}}, //3 scary 3
  {2, {  0,  0,219,109}, {159,160}}, //4 scary 4
  {2, {  0,109,218,109}, {160,160}}, //5 scary 5
  {3, {  0,  0,218,109}, {158,160}}, //6 scary 6
  {3, {  0,109,217,109}, {157,160}}, //7 scary 7
  {4, {  0,  0,222,109}, {159,160}}, //8 scary 8
  {4, {  0,109,216,109}, {157,160}}, //9 scary 9
};

static const CharFrame hand_frame[9] = {
  {0, {  0,  0, 50, 46}, {160,133}}, //0 handa 1
  {0, { 50,  0, 45, 50}, {160,137}}, //1 handa 2
  {0, { 95,  0, 40, 51}, {160,138}}, //2 handa 3

  {0, {135,  0, 29, 69}, {160,156}}, //3 handb 1
  {0, {164,  0, 27, 71}, {169,158}}, //4 handb 2
  {0, {191,  0, 41, 62}, {160,149}}, //5 handb 3

  {0, {  0, 71, 32, 72}, {171,159}}, //6 handc 1
  {0, { 32, 71, 44, 60}, {183,147}}, //7 handc 2
  {0, { 76, 71, 30, 67}, {170,154}}, //8 handc 3
};

static const CharFrame orange_frame[74] = {
  {0, {  0,  0,109,100}, {160,160}}, //0 final 1
  {0, {109,  0,109,100}, {160,160}}, //1 final 2
  {0, {  0,100,109,100}, {160,160}}, //2 final 3
  {0, {109,100,109,100}, {160,160}}, //3 final 4
  {1, {  0,  0,109,100}, {160,160}}, //4 final 5
  {1, {109,  0,109,100}, {160,160}}, //5 final 6
  {1, {  0,100,109,100}, {160,160}}, //6 final 7
  {1, {109,100,109,100}, {160,160}}, //7 final 8
  {2, {  0,  0,109,100}, {160,160}}, //8 final 9
  {2, {109,  0,109,100}, {160,160}}, //9 final 10
  {2, {  0,100,109,100}, {160,160}}, //10 final 11
  {2, {109,100,109,100}, {160,160}}, //11 final 12
  {3, {  0,  0,109,100}, {160,160}}, //12 final 13
  {3, {109,  0,109,100}, {160,160}}, //13 final 14
  {3, {  0,100,109,100}, {160,160}}, //14 final 15

  {3, {109,100,109,100}, {160,160}}, //15 singing 1
  {4, {  0,  0,109,100}, {160,160}}, //16 singing 2
  {4, {109,  0,109,100}, {160,160}}, //17 singing 3
  {4, {  0,100,109,100}, {160,160}}, //18 singing 4
  {4, {109,100,109,100}, {160,160}}, //19 singing 5
  {5, {  0,  0,109,100}, {160,160}}, //20 singing 6
  {5, {109,  0,109,100}, {160,160}}, //21 singing 7
  {5, {  0,100,109,100}, {160,160}}, //22 singing 8
  {5, {109,100,109,100}, {160,160}}, //23 singing 9
  {6, {  0,  0,109,100}, {160,160}}, //24 singing 10
  {6, {109,  0,109,100}, {160,160}}, //25 singing 11
  {6, {  0,100,109,100}, {160,160}}, //26 singing 12
  {6, {109,100,109,100}, {160,160}}, //27 singing 13
  {7, {  0,  0,109,100}, {160,160}}, //28 singing 14
  {7, {109,  0,109,100}, {160,160}}, //29 singing 15
  {7, {  0,100,109,100}, {160,160}}, //30 singing 16
  {7, {109,100,109,100}, {160,160}}, //31 singing 17
  {8, {  0,  0,109,100}, {160,160}}, //32 singing 18
  {8, {109,  0,109,100}, {160,160}}, //33 singing 19
  {8, {  0,100,109,100}, {160,160}}, //34 singing 20
  {8, {109,100,109,100}, {160,160}}, //35 singing 21
  {9, {  0,  0,109,100}, {160,160}}, //36 singing 22
  {9, {109,  0,109,100}, {160,160}}, //37 singing 23

  {9, {  0,100,109,100}, {160,160}}, //38 taunt 1
  {9, {109,100,109,100}, {160,160}}, //39 taunt 2
  {10, {  0,  0,109,100}, {160,160}}, //40 taunt 3
  {10, {109,  0,109,100}, {160,160}}, //41 taunt 4
  {10, {  0,100,109,100}, {160,160}}, //42 taunt 5
  {10, {109,100,109,100}, {160,160}}, //43 taunt 6
  {11, {  0,  0,109,100}, {160,160}}, //44 taunt 7
  {11, {109,  0,109,100}, {160,160}}, //45 taunt 8
  {11, {  0,100,109,100}, {160,160}}, //46 taunt 9
  {11, {109,100,109,100}, {160,160}}, //47 taunt 10
  {12, {  0,  0,109,100}, {160,160}}, //48 taunt 11
  {12, {109,  0,109,100}, {160,160}}, //49 taunt 12
  {12, {  0,100,109,100}, {160,160}}, //50 taunt 13
  {12, {109,100,109,100}, {160,160}}, //51 taunt 14
  {13, {  0,  0,109,100}, {160,160}}, //52 taunt 15
  {13, {109,  0,109,100}, {160,160}}, //53 taunt 16
  {13, {  0,100,109,100}, {160,160}}, //54 taunt 17

  {13, {109,100, 86, 77}, {148,136}}, //55 watching 1
  {14, {  0,  0, 86, 77}, {148,136}}, //56 watching 2
  {14, { 86,  0, 86, 77}, {148,136}}, //57 watching 3

  {14, {  0, 77, 94, 86}, {151,146}}, //58 over 1
  {14, { 94, 77, 94, 86}, {151,146}}, //59 over 2
  {14, {  0,163, 94, 86}, {151,146}}, //60 over 3
  {14, { 94,163, 94, 86}, {151,146}}, //61 over 4
  {15, {  0,  0, 94, 86}, {151,146}}, //62 over 5
  {15, { 94,  0, 94, 86}, {151,146}}, //63 over 6
  {15, {  0, 86, 94, 86}, {151,146}}, //64 over 7
  {15, { 94, 86, 94, 86}, {151,146}}, //65 over 8
  {16, {  0,  0, 94, 86}, {151,146}}, //66 over 9
  {16, { 94,  0, 94, 86}, {151,146}}, //67 over 10
  {16, {  0, 86, 94, 86}, {151,146}}, //68 over 11
  {16, { 94, 86, 94, 86}, {151,146}}, //69 over 12
  {17, {  0,  0, 94, 86}, {151,146}}, //70 over 13
  {17, { 94,  0, 94, 86}, {151,146}}, //71 over 14
  {17, {  0, 86, 94, 86}, {151,146}}, //72 over 15
  {17, { 94, 86, 94, 86}, {151,146}}, //73 over 16
};

static const CharFrame plat_frame[12] = {
  {0, {  0,  0, 62, 73}, {160,148}}, //0 rise 1
  {0, { 62,  0, 62,113}, {160,152}}, //1 rise 2
  {0, {124,  0, 62,143}, {160,155}}, //2 rise 3
  {0, {186,  0, 62,166}, {160,157}}, //3 rise 4
  {1, {  0,  0, 62,181}, {160,159}}, //4 rise 5
  {1, { 62,  0, 62,191}, {160,160}}, //5 rise 6
  {1, {124,  0, 62,195}, {160,160}}, //6 rise 7
  {1, {186,  0, 62,196}, {160,160}}, //7 rise 8
  {2, {  0,  0, 62,195}, {160,160}}, //8 rise 9
  {2, { 62,  0, 62,192}, {160,160}}, //9 rise 10
  {2, {124,  0, 62,188}, {160,159}}, //10 rise 11
  {2, {186,  0, 62,186}, {160,159}}, //11 rise 12
};

static const CharFrame rapple_frame[9] = {
  {0, {  0,  0, 63, 66}, {160,160}}, //0 idle 1

  {0, { 63,  0, 67, 69}, {161,158}}, //1 fall 1
  {0, {130,  0, 71, 70}, {163,158}}, //2 fall 2
  {0, {  0, 70, 67, 68}, {158,157}}, //3 fall 3
  {0, { 67, 70, 73, 68}, {164,157}}, //4 fall 4
  {0, {140, 70, 68, 67}, {161,162}}, //5 fall 5
  {0, {  0,140, 69, 69}, {160,157}}, //6 fall 6
  {0, { 69,140, 73, 68}, {164,159}}, //7 fall 7

  {0, {142,140,101, 57}, {192,144}}, //8 dead 1
};

static const CharFrame speed_frame[7] = {
  {0, {  0,  0,155, 97}, {164,157}}, //0 fall 1
  {0, {  0, 97,155,116}, {164,161}}, //1 fall 2
  {1, {  0,  0,155, 89}, {164,156}}, //2 fall 3
  
  {2, {  0,  0,132, 53}, {160,160}}, //3 hit 1
  {2, {  0, 53,132, 53}, {160,160}}, //4 hit 2
  {2, {  0,106,132, 53}, {160,160}}, //5 hit 3
  {2, {  0,159,132, 53}, {160,160}}, //6 hit 4
};

static const CharFrame static_frame[2] = {
  {0, {  0,  0,128,128}, {160,160}}, //0 idle 1
  {0, {128,  0,128,128}, {160,160}}, //1 idle 2
};


static const Animation appleshock_anim[] = {
	{12, (const u8[]) { 0, 1, 2, 3, ASCR_BACK, 0}},
};

static const Animation applestand_anim[] = {
	{12, (const u8[]) { 4, 4, ASCR_BACK, 0}},
};

static const Animation fruitnormal_anim[] = {
	{12, (const u8[]) { 0, 0, ASCR_BACK, 0}},
};

static const Animation fruitscary_anim[] = {
	{24, (const u8[]) { 1, 2, 3, 4, 5, 6, 7, 8, 9, ASCR_BACK, 0}},
};

static const Animation handa_anim[] = {
	{6, (const u8[]) { 0, 1, 2, ASCR_BACK, 0}},
};

static const Animation handb_anim[] = {
	{6, (const u8[]) { 3, 4, 5, ASCR_BACK, 0}},
};

static const Animation handc_anim[] = {
	{6, (const u8[]) { 6, 7, 8, ASCR_BACK, 0}},
};

static const Animation orangefinal_anim[] = {
	{24, (const u8[]) {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 4, 5, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 9, 9, 11, 11, 11, 11, 9, 9, 9, 9, 10, 10, 10, 10, 9, 9, 11, 11, 11, 11, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, ASCR_BACK, 0}},
};

static const Animation orangesinging_anim[] = {
	{24, (const u8[]) {15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, ASCR_BACK, 0}},
};

static const Animation orangetaunt_anim[] = {
	{24, (const u8[]) {40, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46,  47, 47, 47, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 50, 51, 51, 50, 49, 49, 49, 49, 49, 50, 51, 51, 50, 49, 49, 49, 49, 49, 50, 51, 51, 50, 49, 49, 49, 49, 49, 49, 49, 49, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 53, 53, 54, 54, ASCR_BACK, 0}},
};

static const Animation orangewatching_anim[] = {
	{12, (const u8[]) { 55, 55, 56, 56, 57, 57, ASCR_BACK, 0}},
};

static const Animation orangeover_anim[] = {
	{24, (const u8[]) {58, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 60, 60, 61, 61, 61, 61, 61, 61, 62, 62, 63, 63, 63, 63, 63, 63, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 67, 67, 67, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 69, 69, 70, 70, 70, 70, 70, 70, 70, 70, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 72, 72, 72, 72, 73, 73, 73, 73, ASCR_BACK, 0}},
};

static const Animation plat_anim[] = {
	{12, (const u8[]) { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, ASCR_BACK, 0}},
};

static const Animation rappleidle_anim[] = {
	{12, (const u8[]) { 0, 0, ASCR_BACK, 0}},
};

static const Animation rapplefall_anim[] = {
	{12, (const u8[]) { 1, 2, 3, 4, 5, 6, 7, ASCR_BACK, 0}},
};

static const Animation rapplehit_anim[] = {
	{12, (const u8[]) { 8, 8, ASCR_BACK, 0}},
};

static const Animation speedfall_anim[] = {
	{12, (const u8[]) { 0, 1, 2, ASCR_BACK, 0}},
};

static const Animation speedhit_anim[] = {
	{12, (const u8[]) { 3, 4, 5, 6, ASCR_BACK, 0}},
};

static const Animation static_anim[] = {
	{12, (const u8[]) { 0, 1, 0, 1, ASCR_BACK, 0}},
};

void Kitchen_Apple_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->apple_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &apple_frame[this->apple_frame = frame];
		if (cframe->tex != this->apple_tex_id)
			Gfx_LoadTex(&this->tex_apple, this->arc_apple_ptr[this->apple_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Apple_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &apple_frame[this->apple_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 2) << FIXED_SHIFT, (src.h * 2) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 4, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_apple, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_Fruit_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->fruit_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &fruit_frame[this->fruit_frame = frame];
		if (cframe->tex != this->fruit_tex_id)
			Gfx_LoadTex(&this->tex_fruit, this->arc_fruit_ptr[this->fruit_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Fruit_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &fruit_frame[this->fruit_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 2) << FIXED_SHIFT, (src.h * 2) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 5, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_fruit, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_Hand_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->hand_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &hand_frame[this->hand_frame = frame];
		if (cframe->tex != this->hand_tex_id)
			Gfx_LoadTex(&this->tex_hand, this->arc_hand_ptr[this->hand_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Hand_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &hand_frame[this->hand_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 2) << FIXED_SHIFT, (src.h * 2) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 6, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_hand, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_Orange_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->orange_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &orange_frame[this->orange_frame = frame];
		if (cframe->tex != this->orange_tex_id)
			Gfx_LoadTex(&this->tex_orange, this->arc_orange_ptr[this->orange_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Orange_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &orange_frame[this->orange_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 2) << FIXED_SHIFT, (src.h * 2) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_orange, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_Plat_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->plat_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &plat_frame[this->plat_frame = frame];
		if (cframe->tex != this->plat_tex_id)
			Gfx_LoadTex(&this->tex_plat, this->arc_plat_ptr[this->plat_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Plat_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &plat_frame[this->plat_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 2) << FIXED_SHIFT, (src.h * 2) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_plat, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_Rapple_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->rapple_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &rapple_frame[this->rapple_frame = frame];
		if (cframe->tex != this->rapple_tex_id)
			Gfx_LoadTex(&this->tex_rapple, this->arc_rapple_ptr[this->rapple_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Rapple_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &rapple_frame[this->rapple_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 2) << FIXED_SHIFT, (src.h * 2) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 9, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_rapple, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_Speed_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->speed_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &speed_frame[this->speed_frame = frame];
		if (cframe->tex != this->speed_tex_id)
			Gfx_LoadTex(&this->tex_speed, this->arc_speed_ptr[this->speed_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Speed_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &speed_frame[this->speed_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 2) << FIXED_SHIFT, (src.h * 2) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 10, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_speed, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_Static_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->static_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &static_frame[this->static_frame = frame];
		if (cframe->tex != this->static_tex_id)
			Gfx_LoadTex(&this->tex_static, this->arc_static_ptr[this->static_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Static_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &static_frame[this->static_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 8) << FIXED_SHIFT, (src.h * 4) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 11, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_static, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Back_Kitchen_DrawHUD(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;
	
	fixed_t fx, fy;
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 5)
		{
			case 0:
				Animatable_SetAnim(&this->appleshock_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->appleshock_animatable, (void*)this, Kitchen_Apple_SetFrame);
	//Kitchen_Apple_Draw(this, FIXED_DEC(-20, 1) - fx, FIXED_DEC(5, 1) - fy);
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 5)
		{
			case 0:
				Animatable_SetAnim(&this->fruitnormal_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->fruitnormal_animatable, (void*)this, Kitchen_Fruit_SetFrame);
	//Kitchen_Fruit_Draw(this, FIXED_DEC(-20, 1) - fx, FIXED_DEC(5, 1) - fy);
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 5)
		{
			case 0:
				Animatable_SetAnim(&this->orangewatching_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->orangewatching_animatable, (void*)this, Kitchen_Orange_SetFrame);
	//Kitchen_Orange_Draw(this, FIXED_DEC(-20, 1) - fx, FIXED_DEC(5, 1) - fy);
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 5)
		{
			case 0:
				Animatable_SetAnim(&this->plat_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->plat_animatable, (void*)this, Kitchen_Plat_SetFrame);
	//Kitchen_Plat_Draw(this, FIXED_DEC(-20, 1) - fx, FIXED_DEC(5, 1) - fy);
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 5)
		{
			case 0:
				Animatable_SetAnim(&this->rappleidle_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->rappleidle_animatable, (void*)this, Kitchen_Rapple_SetFrame);
	//Kitchen_Rapple_Draw(this, FIXED_DEC(-20, 1) - fx, FIXED_DEC(5, 1) - fy);
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 5)
		{
			case 0:
				Animatable_SetAnim(&this->speedfall_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->speedfall_animatable, (void*)this, Kitchen_Speed_SetFrame);
	//Kitchen_Speed_Draw(this, FIXED_DEC(-20, 1) - fx, FIXED_DEC(5, 1) - fy);
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 5)
		{
			case 0:
				Animatable_SetAnim(&this->static_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->static_animatable, (void*)this, Kitchen_Static_SetFrame);
	//Kitchen_Static_Draw(this, FIXED_DEC(-20, 1) - fx, FIXED_DEC(5, 1) - fy);
}


//Kitchen background functions
void Back_Kitchen_DrawBG(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;
	
	fixed_t fx, fy;
	
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT back_src = {0, 0, 128, 128};
	RECT_FIXED back_dst = {
		FIXED_DEC(-275,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(553,1),
		FIXED_DEC(367,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom, stage.camera.angle);
}

void Back_Kitchen_Free(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;
	
	Mem_Free(this->arc_apple);
	Mem_Free(this->arc_fruit);
	Mem_Free(this->arc_hand);
	Mem_Free(this->arc_orange);
	Mem_Free(this->arc_plat);
	Mem_Free(this->arc_rapple);
	Mem_Free(this->arc_speed);
	Mem_Free(this->arc_static);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Kitchen_New(void)
{
	//Allocate background structure
	Back_Kitchen *this = (Back_Kitchen*)Mem_Alloc(sizeof(Back_Kitchen));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_hud = Back_Kitchen_DrawHUD;
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Kitchen_DrawBG;
	this->back.free = Back_Kitchen_Free;
	
	//Use pitch black background
	Gfx_SetClear(0, 0, 0);
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\KITCHEN\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_cut0, Archive_Find(arc_back, "cut0.tim"), 0);
	Mem_Free(arc_back);
	
	//Load hand textures
	this->arc_apple = IO_Read("\\KITCHEN\\APPLE.ARC;1");
	this->arc_apple_ptr[0] = Archive_Find(this->arc_apple, "apple0.tim");

	//Initialize hand state
	Animatable_Init(&this->appleshock_animatable, appleshock_anim);
	Animatable_Init(&this->applestand_animatable, applestand_anim);
	
	Animatable_SetAnim(&this->appleshock_animatable, 0);
	Animatable_SetAnim(&this->applestand_animatable, 0);
	
	this->apple_frame = this->apple_tex_id = 0xFF; //Force art load
	
	//Load fruit textures
	this->arc_fruit = IO_Read("\\KITCHEN\\FRUIT.ARC;1");
	this->arc_fruit_ptr[0] = Archive_Find(this->arc_fruit, "fruits0.tim");
	this->arc_fruit_ptr[1] = Archive_Find(this->arc_fruit, "fruits1.tim");
	this->arc_fruit_ptr[2] = Archive_Find(this->arc_fruit, "fruits2.tim");
	this->arc_fruit_ptr[3] = Archive_Find(this->arc_fruit, "fruits3.tim");
	this->arc_fruit_ptr[4] = Archive_Find(this->arc_fruit, "fruits4.tim");


	//Initialize fruit state
	Animatable_Init(&this->fruitnormal_animatable, fruitnormal_anim);
	Animatable_Init(&this->fruitscary_animatable, fruitscary_anim);
	
	Animatable_SetAnim(&this->fruitnormal_animatable, 0);
	Animatable_SetAnim(&this->fruitscary_animatable, 0);
	
	this->fruit_frame = this->fruit_tex_id = 0xFE; //Force art load
	
	//Load hand textures
	this->arc_hand = IO_Read("\\KITCHEN\\HAND.ARC;1");
	this->arc_hand_ptr[0] = Archive_Find(this->arc_hand, "hand0.tim");

	//Initialize hand state
	Animatable_Init(&this->handa_animatable, handa_anim);
	Animatable_Init(&this->handb_animatable, handb_anim);
	Animatable_Init(&this->handc_animatable, handc_anim);
	
	Animatable_SetAnim(&this->handa_animatable, 0);
	Animatable_SetAnim(&this->handb_animatable, 0);
	Animatable_SetAnim(&this->handc_animatable, 0);
	
	this->hand_frame = this->hand_tex_id = 0xFD; //Force art load
	
	//Load orange textures
	this->arc_orange = IO_Read("\\KITCHEN\\ORANGE.ARC;1");
	this->arc_orange_ptr[0] = Archive_Find(this->arc_orange, "orange0.tim");
	this->arc_orange_ptr[1] = Archive_Find(this->arc_orange, "orange1.tim");
	this->arc_orange_ptr[2] = Archive_Find(this->arc_orange, "orange2.tim");
	this->arc_orange_ptr[3] = Archive_Find(this->arc_orange, "orange3.tim");
	this->arc_orange_ptr[4] = Archive_Find(this->arc_orange, "orange4.tim");
	this->arc_orange_ptr[5] = Archive_Find(this->arc_orange, "orange5.tim");
	this->arc_orange_ptr[6] = Archive_Find(this->arc_orange, "orange6.tim");
	this->arc_orange_ptr[7] = Archive_Find(this->arc_orange, "orange7.tim");
	this->arc_orange_ptr[8] = Archive_Find(this->arc_orange, "orange8.tim");
	this->arc_orange_ptr[9] = Archive_Find(this->arc_orange, "orange9.tim");
	this->arc_orange_ptr[10] = Archive_Find(this->arc_orange, "orange10.tim");
	this->arc_orange_ptr[11] = Archive_Find(this->arc_orange, "orange11.tim");
	this->arc_orange_ptr[12] = Archive_Find(this->arc_orange, "orange12.tim");
	this->arc_orange_ptr[13] = Archive_Find(this->arc_orange, "orange13.tim");
	this->arc_orange_ptr[14] = Archive_Find(this->arc_orange, "orange14.tim");
	this->arc_orange_ptr[15] = Archive_Find(this->arc_orange, "orange15.tim");
	this->arc_orange_ptr[16] = Archive_Find(this->arc_orange, "orange16.tim");
	this->arc_orange_ptr[17] = Archive_Find(this->arc_orange, "orange17.tim");

	//Initialize orange state
	Animatable_Init(&this->orangefinal_animatable, orangefinal_anim);
	Animatable_Init(&this->orangesinging_animatable, orangesinging_anim);
	Animatable_Init(&this->orangetaunt_animatable, orangetaunt_anim);
	Animatable_Init(&this->orangewatching_animatable, orangewatching_anim);
	Animatable_Init(&this->orangeover_animatable, orangeover_anim);
	
	Animatable_SetAnim(&this->orangefinal_animatable, 0);
	Animatable_SetAnim(&this->orangesinging_animatable, 0);
	Animatable_SetAnim(&this->orangetaunt_animatable, 0);
	Animatable_SetAnim(&this->orangewatching_animatable, 0);
	Animatable_SetAnim(&this->orangeover_animatable, 0);
	
	this->orange_frame = this->orange_tex_id = 0xFC; //Force art load
	
	//Load plat textures
	this->arc_plat = IO_Read("\\KITCHEN\\PLAT.ARC;1");
	this->arc_plat_ptr[0] = Archive_Find(this->arc_plat, "plat0.tim");
	this->arc_plat_ptr[1] = Archive_Find(this->arc_plat, "plat1.tim");
	this->arc_plat_ptr[2] = Archive_Find(this->arc_plat, "plat2.tim");

	//Initialize plat state
	Animatable_Init(&this->plat_animatable, plat_anim);
	Animatable_SetAnim(&this->plat_animatable, 0);
	this->plat_frame = this->plat_tex_id = 0xFB; //Force art load
	
	//Load rapple textures
	this->arc_rapple = IO_Read("\\KITCHEN\\RAPPLE.ARC;1");
	this->arc_rapple_ptr[0] = Archive_Find(this->arc_rapple, "rapple0.tim");

	//Initialize rapple state
	Animatable_Init(&this->rappleidle_animatable, rappleidle_anim);
	Animatable_Init(&this->rapplefall_animatable, rapplefall_anim);
	Animatable_Init(&this->rapplehit_animatable, rapplehit_anim);
	
	Animatable_SetAnim(&this->rappleidle_animatable, 0);
	Animatable_SetAnim(&this->rapplefall_animatable, 0);
	Animatable_SetAnim(&this->rapplehit_animatable, 0);
	
	this->rapple_frame = this->rapple_tex_id = 0xFA; //Force art load
	
	//Load speed textures
	this->arc_speed = IO_Read("\\KITCHEN\\SPEED.ARC;1");
	this->arc_speed_ptr[0] = Archive_Find(this->arc_speed, "speed0.tim");
	this->arc_speed_ptr[1] = Archive_Find(this->arc_speed, "speed1.tim");
	this->arc_speed_ptr[2] = Archive_Find(this->arc_speed, "speed2.tim");

	//Initialize speed state
	Animatable_Init(&this->speedfall_animatable, speedfall_anim);
	Animatable_Init(&this->speedhit_animatable, speedhit_anim);
	
	Animatable_SetAnim(&this->speedfall_animatable, 0);
	Animatable_SetAnim(&this->speedhit_animatable, 0);
	
	this->speed_frame = this->speed_tex_id = 0xF9; //Force art load
	
	//Load static textures
	this->arc_static = IO_Read("\\KITCHEN\\STATIC.ARC;1");
	this->arc_static_ptr[0] = Archive_Find(this->arc_static, "static0.tim");

	//Initialize static state
	Animatable_Init(&this->static_animatable, static_anim);
	Animatable_SetAnim(&this->static_animatable, 0);
	this->static_frame = this->static_tex_id = 0xF8; //Force art load
	
	return (StageBack*)this;
}
