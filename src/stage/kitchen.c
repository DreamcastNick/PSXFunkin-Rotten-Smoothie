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
	
	IO_Data arc_grace, arc_grace_ptr[15];
	IO_Data arc_handa, arc_handa_ptr[1];
	IO_Data arc_handb, arc_handb_ptr[1];
	IO_Data arc_handc, arc_handc_ptr[1];
	
	//Textures
	Gfx_Tex tex_back0;
	Gfx_Tex tex_back1;
	Gfx_Tex tex_cut0;
	
	Gfx_Tex tex_grace;
	u8 grace_frame, grace_tex_id;
	
	Gfx_Tex tex_handa;
	u8 handa_frame, handa_tex_id;
	Gfx_Tex tex_handb;
	u8 handb_frame, handb_tex_id;
	Gfx_Tex tex_handc;
	u8 handc_frame, handc_tex_id;
	
	Animatable grace_animatable;
	
	Animatable handa_animatable;
	Animatable handb_animatable;
	Animatable handc_animatable;
	
} Back_Kitchen;

static const CharFrame grace_frame[230] = {
  {0, {  0,  0, 64, 64}, {0,0}}, //0 idle 1
  {0, { 64,  0, 64, 64}, {0,0}}, //1 idle 2
  {0, {128,  0, 64, 64}, {0,0}}, //2 idle 3
  {0, {192,  0, 64, 64}, {0,0}}, //3 idle 4
  {0, {  0, 64, 64, 64}, {0,0}}, //4 idle 5
  {0, { 64, 64, 64, 64}, {0,0}}, //5 idle 6
  {0, {128, 64, 64, 64}, {0,0}}, //6 idle 7
  {0, {192, 64, 64, 64}, {0,0}}, //7 idle 8
  {0, {  0,128, 64, 64}, {0,0}}, //8 idle 9
  {0, { 64,128, 64, 64}, {0,0}}, //9 idle 10
  {0, {128,128, 64, 64}, {0,0}}, //10 idle 11
  {0, {192,128, 64, 64}, {0,0}}, //11 idle 12
  {0, {  0,192, 64, 64}, {0,0}}, //12 idle 13
  {0, { 64,192, 64, 64}, {0,0}}, //13 idle 14
  {0, {128,192, 64, 64}, {0,0}}, //14 idle 15
  {0, {192,192, 64, 64}, {0,0}}, //15 idle 16
  {1, {  0,  0, 64, 64}, {0,0}}, //16 idle 17
  {1, { 64,  0, 64, 64}, {0,0}}, //17 idle 18
  {1, {128,  0, 64, 64}, {0,0}}, //18 idle 19
  {1, {192,  0, 64, 64}, {0,0}}, //19 idle 20
  {1, {  0, 64, 64, 64}, {0,0}}, //20 idle 21
  {1, { 64, 64, 64, 64}, {0,0}}, //21 idle 22
  {1, {128, 64, 64, 64}, {0,0}}, //22 idle 23
  {1, {192, 64, 64, 64}, {0,0}}, //23 idle 24
  {1, {  0,128, 64, 64}, {0,0}}, //24 idle 25
  {1, { 64,128, 64, 64}, {0,0}}, //25 idle 26
  {1, {128,128, 64, 64}, {0,0}}, //26 idle 27
  {1, {192,128, 64, 64}, {0,0}}, //27 idle 28
  {1, {  0,192, 64, 64}, {0,0}}, //28 idle 29
  {1, { 64,192, 64, 64}, {0,0}}, //29 idle 30
  {1, {128,192, 64, 64}, {0,0}}, //30 idle 31
  {1, {192,192, 64, 64}, {0,0}}, //31 idle 32
  {2, {  0,  0, 64, 64}, {0,0}}, //32 idle 33
  {2, { 64,  0, 64, 64}, {0,0}}, //33 idle 34
  {2, {128,  0, 64, 64}, {0,0}}, //34 idle 35
  {2, {192,  0, 64, 64}, {0,0}}, //35 idle 36
  {2, {  0, 64, 64, 64}, {0,0}}, //36 idle 37
  {2, { 64, 64, 64, 64}, {0,0}}, //37 idle 38
  {2, {128, 64, 64, 64}, {0,0}}, //38 idle 39
  {2, {192, 64, 64, 64}, {0,0}}, //39 idle 40
  {2, {  0,128, 64, 64}, {0,0}}, //40 idle 41
  {2, { 64,128, 64, 64}, {0,0}}, //41 idle 42
  {2, {128,128, 64, 64}, {0,0}}, //42 idle 43
  {2, {192,128, 64, 64}, {0,0}}, //43 idle 44
  {2, {  0,192, 64, 64}, {0,0}}, //44 idle 45
  {2, { 64,192, 64, 64}, {0,0}}, //45 idle 46
  {2, {128,192, 64, 64}, {0,0}}, //46 idle 47
  {2, {192,192, 64, 64}, {0,0}}, //47 idle 48
  {3, {  0,  0, 64, 64}, {0,0}}, //48 idle 49
  {3, { 64,  0, 64, 64}, {0,0}}, //49 idle 50
  {3, {128,  0, 64, 64}, {0,0}}, //50 idle 51
  {3, {192,  0, 64, 64}, {0,0}}, //51 idle 52
  {3, {  0, 64, 64, 64}, {0,0}}, //52 idle 53
  {3, { 64, 64, 64, 64}, {0,0}}, //53 idle 54
  {3, {128, 64, 64, 64}, {0,0}}, //54 idle 55
  {3, {192, 64, 64, 64}, {0,0}}, //55 idle 56
  {3, {  0,128, 64, 64}, {0,0}}, //56 idle 57
  {3, { 64,128, 64, 64}, {0,0}}, //57 idle 58
  {3, {128,128, 64, 64}, {0,0}}, //58 idle 59
  {3, {192,128, 64, 64}, {0,0}}, //59 idle 60
  {3, {  0,192, 64, 64}, {0,0}}, //60 idle 61
  {3, { 64,192, 64, 64}, {0,0}}, //61 idle 62
  {3, {128,192, 64, 64}, {0,0}}, //62 idle 63
  {3, {192,192, 64, 64}, {0,0}}, //63 idle 64
  {4, {  0,  0, 64, 64}, {0,0}}, //64 idle 65
  {4, { 64,  0, 64, 64}, {0,0}}, //65 idle 66
  {4, {128,  0, 64, 64}, {0,0}}, //66 idle 67
  {4, {192,  0, 64, 64}, {0,0}}, //67 idle 68
  {4, {  0, 64, 64, 64}, {0,0}}, //68 idle 69
  {4, { 64, 64, 64, 64}, {0,0}}, //69 idle 70
  {4, {128, 64, 64, 64}, {0,0}}, //70 idle 71
  {4, {192, 64, 64, 64}, {0,0}}, //71 idle 72
  {4, {  0,128, 64, 64}, {0,0}}, //72 idle 73
  {4, { 64,128, 64, 64}, {0,0}}, //73 idle 74
  {4, {128,128, 64, 64}, {0,0}}, //74 idle 75
  {4, {192,128, 64, 64}, {0,0}}, //75 idle 76
  {4, {  0,192, 64, 64}, {0,0}}, //76 idle 77
  {4, { 64,192, 64, 64}, {0,0}}, //77 idle 78
  {4, {128,192, 64, 64}, {0,0}}, //78 idle 79
  {4, {192,192, 64, 64}, {0,0}}, //79 idle 80
  {5, {  0,  0, 64, 64}, {0,0}}, //80 idle 81
  {5, { 64,  0, 64, 64}, {0,0}}, //81 idle 82
  {5, {128,  0, 64, 64}, {0,0}}, //82 idle 83
  {5, {192,  0, 64, 64}, {0,0}}, //83 idle 84
  {5, {  0, 64, 64, 64}, {0,0}}, //84 idle 85
  {5, { 64, 64, 64, 64}, {0,0}}, //85 idle 86
  {5, {128, 64, 64, 64}, {0,0}}, //86 idle 87
  {5, {192, 64, 64, 64}, {0,0}}, //87 idle 88
  {5, {  0,128, 64, 64}, {0,0}}, //88 idle 89
  {5, { 64,128, 64, 64}, {0,0}}, //89 idle 90
  {5, {128,128, 64, 64}, {0,0}}, //90 idle 91
  {5, {192,128, 64, 64}, {0,0}}, //91 idle 92
  {5, {  0,192, 64, 64}, {0,0}}, //92 idle 93
  {5, { 64,192, 64, 64}, {0,0}}, //93 idle 94
  {5, {128,192, 64, 64}, {0,0}}, //94 idle 95
  {5, {192,192, 64, 64}, {0,0}}, //95 idle 96
  {6, {  0,  0, 64, 64}, {0,0}}, //96 idle 97
  {6, { 64,  0, 64, 64}, {0,0}}, //97 idle 98
  {6, {128,  0, 64, 64}, {0,0}}, //98 idle 99
  {6, {192,  0, 64, 64}, {0,0}}, //99 idle 100
  {6, {  0, 64, 64, 64}, {0,0}}, //100 idle 101
  {6, { 64, 64, 64, 64}, {0,0}}, //101 idle 102
  {6, {128, 64, 64, 64}, {0,0}}, //102 idle 103
  {6, {192, 64, 64, 64}, {0,0}}, //103 idle 104
  {6, {  0,128, 64, 64}, {0,0}}, //104 idle 105
  {6, { 64,128, 64, 64}, {0,0}}, //105 idle 106
  {6, {128,128, 64, 64}, {0,0}}, //106 idle 107
  {6, {192,128, 64, 64}, {0,0}}, //107 idle 108
  {6, {  0,192, 64, 64}, {0,0}}, //108 idle 109
  {6, { 64,192, 64, 64}, {0,0}}, //109 idle 110
  {6, {128,192, 64, 64}, {0,0}}, //110 idle 111
  {6, {192,192, 64, 64}, {0,0}}, //111 idle 112
  {7, {  0,  0, 64, 64}, {0,0}}, //112 idle 113
  {7, { 64,  0, 64, 64}, {0,0}}, //113 idle 114
  {7, {128,  0, 64, 64}, {0,0}}, //114 idle 115
  {7, {192,  0, 64, 64}, {0,0}}, //115 idle 116
  {7, {  0, 64, 64, 64}, {0,0}}, //116 idle 117
  {7, { 64, 64, 64, 64}, {0,0}}, //117 idle 118
  {7, {128, 64, 64, 64}, {0,0}}, //118 idle 119
  {7, {192, 64, 64, 64}, {0,0}}, //119 idle 120
  {7, {  0,128, 64, 64}, {0,0}}, //120 idle 121
  {7, { 64,128, 64, 64}, {0,0}}, //121 idle 122
  {7, {128,128, 64, 64}, {0,0}}, //122 idle 123
  {7, {192,128, 64, 64}, {0,0}}, //123 idle 124
  {7, {  0,192, 64, 64}, {0,0}}, //124 idle 125
  {7, { 64,192, 64, 64}, {0,0}}, //125 idle 126
  {7, {128,192, 64, 64}, {0,0}}, //126 idle 127
  {7, {192,192, 64, 64}, {0,0}}, //127 idle 128
  {8, {  0,  0, 64, 64}, {0,0}}, //128 idle 129
  {8, { 64,  0, 64, 64}, {0,0}}, //129 idle 130
  {8, {128,  0, 64, 64}, {0,0}}, //130 idle 131
  {8, {192,  0, 64, 64}, {0,0}}, //131 idle 132
  {8, {  0, 64, 64, 64}, {0,0}}, //132 idle 133
  {8, { 64, 64, 64, 64}, {0,0}}, //133 idle 134
  {8, {128, 64, 64, 64}, {0,0}}, //134 idle 135
  {8, {192, 64, 64, 64}, {0,0}}, //135 idle 136
  {8, {  0,128, 64, 64}, {0,0}}, //136 idle 137
  {8, { 64,128, 64, 64}, {0,0}}, //137 idle 138
  {8, {128,128, 64, 64}, {0,0}}, //138 idle 139
  {8, {192,128, 64, 64}, {0,0}}, //139 idle 140
  {8, {  0,192, 64, 64}, {0,0}}, //140 idle 141
  {8, { 64,192, 64, 64}, {0,0}}, //141 idle 142
  {8, {128,192, 64, 64}, {0,0}}, //142 idle 143
  {8, {192,192, 64, 64}, {0,0}}, //143 idle 144
  {9, {  0,  0, 64, 64}, {0,0}}, //144 idle 145
  {9, { 64,  0, 64, 64}, {0,0}}, //145 idle 146
  {9, {128,  0, 64, 64}, {0,0}}, //146 idle 147
  {9, {192,  0, 64, 64}, {0,0}}, //147 idle 148
  {9, {  0, 64, 64, 64}, {0,0}}, //148 idle 149
  {9, { 64, 64, 64, 64}, {0,0}}, //149 idle 150
  {9, {128, 64, 64, 64}, {0,0}}, //150 idle 151
  {9, {192, 64, 64, 64}, {0,0}}, //151 idle 152
  {9, {  0,128, 64, 64}, {0,0}}, //152 idle 153
  {9, { 64,128, 64, 64}, {0,0}}, //153 idle 154
  {9, {128,128, 64, 64}, {0,0}}, //154 idle 155
  {9, {192,128, 64, 64}, {0,0}}, //155 idle 156
  {9, {  0,192, 64, 64}, {0,0}}, //156 idle 157
  {9, { 64,192, 64, 64}, {0,0}}, //157 idle 158
  {9, {128,192, 64, 64}, {0,0}}, //158 idle 159
  {9, {192,192, 64, 64}, {0,0}}, //159 idle 160
  {10, {  0,  0, 64, 64}, {0,0}}, //160 idle 161
  {10, { 64,  0, 64, 64}, {0,0}}, //161 idle 162
  {10, {128,  0, 64, 64}, {0,0}}, //162 idle 163
  {10, {192,  0, 64, 64}, {0,0}}, //163 idle 164
  {10, {  0, 64, 64, 64}, {0,0}}, //164 idle 165
  {10, { 64, 64, 64, 64}, {0,0}}, //165 idle 166
  {10, {128, 64, 64, 64}, {0,0}}, //166 idle 167
  {10, {192, 64, 64, 64}, {0,0}}, //167 idle 168
  {10, {  0,128, 64, 64}, {0,0}}, //168 idle 169
  {10, { 64,128, 64, 64}, {0,0}}, //169 idle 170
  {10, {128,128, 64, 64}, {0,0}}, //170 idle 171
  {10, {192,128, 64, 64}, {0,0}}, //171 idle 172
  {10, {  0,192, 64, 64}, {0,0}}, //172 idle 173
  {10, { 64,192, 64, 64}, {0,0}}, //173 idle 174
  {10, {128,192, 64, 64}, {0,0}}, //174 idle 175
  {10, {192,192, 64, 64}, {0,0}}, //175 idle 176
  {11, {  0,  0, 64, 64}, {0,0}}, //176 idle 177
  {11, { 64,  0, 64, 64}, {0,0}}, //177 idle 178
  {11, {128,  0, 64, 64}, {0,0}}, //178 idle 179
  {11, {192,  0, 64, 64}, {0,0}}, //179 idle 180
  {11, {  0, 64, 64, 64}, {0,0}}, //180 idle 181
  {11, { 64, 64, 64, 64}, {0,0}}, //181 idle 182
  {11, {128, 64, 64, 64}, {0,0}}, //182 idle 183
  {11, {192, 64, 64, 64}, {0,0}}, //183 idle 184
  {11, {  0,128, 64, 64}, {0,0}}, //184 idle 185
  {11, { 64,128, 64, 64}, {0,0}}, //185 idle 186
  {11, {128,128, 64, 64}, {0,0}}, //186 idle 187
  {11, {192,128, 64, 64}, {0,0}}, //187 idle 188
  {11, {  0,192, 64, 64}, {0,0}}, //188 idle 189
  {11, { 64,192, 64, 64}, {0,0}}, //189 idle 190
  {11, {128,192, 64, 64}, {0,0}}, //190 idle 191
  {11, {192,192, 64, 64}, {0,0}}, //191 idle 192
  {12, {  0,  0, 64, 64}, {0,0}}, //192 idle 193
  {12, { 64,  0, 64, 64}, {0,0}}, //193 idle 194
  {12, {128,  0, 64, 64}, {0,0}}, //194 idle 195
  {12, {192,  0, 64, 64}, {0,0}}, //195 idle 196
  {12, {  0, 64, 64, 64}, {0,0}}, //196 idle 197
  {12, { 64, 64, 64, 64}, {0,0}}, //197 idle 198
  {12, {128, 64, 64, 64}, {0,0}}, //198 idle 199
  {12, {192, 64, 64, 64}, {0,0}}, //199 idle 200
  {12, {  0,128, 64, 64}, {0,0}}, //200 idle 201
  {12, { 64,128, 64, 64}, {0,0}}, //201 idle 202
  {12, {128,128, 64, 64}, {0,0}}, //202 idle 203
  {12, {192,128, 64, 64}, {0,0}}, //203 idle 204
  {12, {  0,192, 64, 64}, {0,0}}, //204 idle 205
  {12, { 64,192, 64, 64}, {0,0}}, //205 idle 206
  {12, {128,192, 64, 64}, {0,0}}, //206 idle 207
  {12, {192,192, 64, 64}, {0,0}}, //207 idle 208
  {13, {  0,  0, 64, 64}, {0,0}}, //208 idle 209
  {13, { 64,  0, 64, 64}, {0,0}}, //209 idle 210
  {13, {128,  0, 64, 64}, {0,0}}, //210 idle 211
  {13, {192,  0, 64, 64}, {0,0}}, //211 idle 212
  {13, {  0, 64, 64, 64}, {0,0}}, //212 idle 213
  {13, { 64, 64, 64, 64}, {0,0}}, //213 idle 214
  {13, {128, 64, 64, 64}, {0,0}}, //214 idle 215
  {13, {192, 64, 64, 64}, {0,0}}, //215 idle 216
  {13, {  0,128, 64, 64}, {0,0}}, //216 idle 217
  {13, { 64,128, 64, 64}, {0,0}}, //217 idle 218
  {13, {128,128, 64, 64}, {0,0}}, //218 idle 219
  {13, {192,128, 64, 64}, {0,0}}, //219 idle 220
  {13, {  0,192, 64, 64}, {0,0}}, //220 idle 221
  {13, { 64,192, 64, 64}, {0,0}}, //221 idle 222
  {13, {128,192, 64, 64}, {0,0}}, //222 idle 223
  {13, {192,192, 64, 64}, {0,0}}, //223 idle 224
  {14, {  0,  0, 64, 64}, {0,0}}, //224 idle 225
  {14, { 64,  0, 64, 64}, {0,0}}, //225 idle 226
  {14, {128,  0, 64, 64}, {0,0}}, //226 idle 227
  {14, {192,  0, 64, 64}, {0,0}}, //227 idle 228
  {14, {  0, 64, 64, 64}, {0,0}}, //228 idle 229
  {14, { 64, 64, 64, 64}, {0,0}}, //229 idle 230
};

static const CharFrame handa_frame[3] = {
  {0, {  0,  0, 50, 51}, {160,160}}, //0 handa 1
  {0, { 50,  0, 50, 51}, {160,160}}, //1 handa 2
  {0, { 100,  0, 50, 51}, {160,160}}, //2 handa 3
};

static const CharFrame handb_frame[3] = {
  {0, {0,  0, 41,  71}, {160,160}}, //3 handb 1
  {0, {41,  0, 41, 71}, {169,160}}, //4 handb 2
  {0, {82,  0, 41, 71}, {160,160}}, //5 handb 3
};

static const CharFrame handc_frame[3] = {
  {0, {  0, 0, 44, 72}, {160,160}}, //6 handc 1
  {0, { 44, 0, 44, 72}, {160,160}}, //7 handc 2
  {0, { 88, 0, 44, 72}, {160,160}}, //8 handc 3
};

static const Animation grace_anim[1] = {
	{5, (const u8[]) { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, ASCR_BACK, 0}},
};

static const Animation handa_anim[1] = {
	{6, (const u8[]) {0, 1, 2, ASCR_BACK, 0}},
};

static const Animation handb_anim[1] = {
	{6, (const u8[]) {0, 1, 2, ASCR_BACK, 0}},
};

static const Animation handc_anim[1] = {
	{6, (const u8[]) {0, 1, 2, ASCR_BACK, 0}},
};

void Kitchen_Grace_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->grace_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &grace_frame[this->grace_frame = frame];
		if (cframe->tex != this->grace_tex_id)
			Gfx_LoadTex(&this->tex_grace, this->arc_grace_ptr[this->grace_tex_id = cframe->tex], 0);
	}
}

void Kitchen_Grace_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &grace_frame[this->grace_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w + 280) << FIXED_SHIFT, (src.h + 200) << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_grace, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_HandA_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->handa_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &handa_frame[this->handa_frame = frame];
		if (cframe->tex != this->handa_tex_id)
			Gfx_LoadTex(&this->tex_handa, this->arc_handa_ptr[this->handa_tex_id = cframe->tex], 0);
	}
}

void Kitchen_HandB_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->handb_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &handb_frame[this->handb_frame = frame];
		if (cframe->tex != this->handb_tex_id)
			Gfx_LoadTex(&this->tex_handb, this->arc_handb_ptr[this->handb_tex_id = cframe->tex], 0);
	}
}

void Kitchen_HandC_SetFrame(void* user, u8 frame)
{
	Back_Kitchen* this = (Back_Kitchen*)user;

	//Check if this is a new frame
	if (frame != this->handc_frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &handc_frame[this->handc_frame = frame];
		if (cframe->tex != this->handc_tex_id)
			Gfx_LoadTex(&this->tex_handc, this->arc_handc_ptr[this->handc_tex_id = cframe->tex], 0);
	}
}

void Kitchen_HandA_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &handa_frame[this->handa_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (-src.w * 3) << FIXED_SHIFT, (src.h * 3) << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 4, stage.camera.x >> 1, stage.camera.y >> 1);
	Stage_DrawTex(&this->tex_handa, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_HandB_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &handb_frame[this->handb_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (src.w * 3) << FIXED_SHIFT, (src.h * 3) << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 5, stage.camera.x >> 1, stage.camera.y >> 1);
	Stage_DrawTex(&this->tex_handb, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Kitchen_HandC_Draw(Back_Kitchen* this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame* cframe = &handc_frame[this->handc_frame];

	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);

	RECT src = { cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3] };
	RECT_FIXED dst = { ox, oy, (-src.w * 3) << FIXED_SHIFT, (src.h * 3) << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 6, stage.camera.x >> 1, stage.camera.y >> 1);
	Stage_DrawTex(&this->tex_handc, &src, &dst, stage.camera.bzoom, stage.camera.angle);
}

void Back_Kitchen_DrawFG(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;
	
	fixed_t fx, fy;
	
	fx = stage.camera.x >> 3;
	fy = stage.camera.y >> 3;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step)
		{
			case 1312:
				Animatable_SetAnim(&this->grace_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->grace_animatable, (void*)this, Kitchen_Grace_SetFrame);
	if (stage.song_step >= 1312 && stage.song_step <= 1816)
	{
		Kitchen_Grace_Draw(this, FIXED_DEC(-160, 1) - fx, FIXED_DEC(-125, 1) - fy);
	}
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

void Back_Kitchen_DrawMD(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;
	
	fixed_t fx, fy;
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 4)
		{
			case 0:
				Animatable_SetAnim(&this->handa_animatable, 0);
			break;
		}
	}
	Animatable_Animate(&this->handa_animatable, (void*)this, Kitchen_HandA_SetFrame);
	Kitchen_HandA_Draw(this, FIXED_DEC(140, 1) - fx, FIXED_DEC(-36, 1) - fy);
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 4)
		{
			case 0:
				Animatable_SetAnim(&this->handb_animatable, 0);
			break;
		}
	}
	Animatable_Animate(&this->handb_animatable, (void*)this, Kitchen_HandB_SetFrame);
	Kitchen_HandB_Draw(this, FIXED_DEC(0, 1) - fx, FIXED_DEC(-32, 1) - fy);
	
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step % 4)
		{
			case 0:
				Animatable_SetAnim(&this->handc_animatable, 0);
			break;
		}
	}
	Animatable_Animate(&this->handc_animatable, (void*)this, Kitchen_HandC_SetFrame);
	Kitchen_HandC_Draw(this, FIXED_DEC(0, 1) - fx, FIXED_DEC(-100, 1) - fy);
}

void Back_Kitchen_Free(StageBack *back)
{
	Back_Kitchen *this = (Back_Kitchen*)back;
	
	Mem_Free(this->arc_grace);
	Mem_Free(this->arc_handa);
	Mem_Free(this->arc_handb);
	Mem_Free(this->arc_handc);
	
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
	this->back.draw_hud = NULL;
	this->back.draw_fg = Back_Kitchen_DrawFG;
	this->back.draw_md = Back_Kitchen_DrawMD;
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
	
	//Load grace textures
	this->arc_grace = IO_Read("\\KITCHEN\\GRACE.ARC;1");
	this->arc_grace_ptr[0] = Archive_Find(this->arc_grace, "grace0.tim");
	this->arc_grace_ptr[1] = Archive_Find(this->arc_grace, "grace1.tim");
	this->arc_grace_ptr[2] = Archive_Find(this->arc_grace, "grace2.tim");
	this->arc_grace_ptr[3] = Archive_Find(this->arc_grace, "grace3.tim");
	this->arc_grace_ptr[4] = Archive_Find(this->arc_grace, "grace4.tim");
	this->arc_grace_ptr[5] = Archive_Find(this->arc_grace, "grace5.tim");
	this->arc_grace_ptr[6] = Archive_Find(this->arc_grace, "grace6.tim");
	this->arc_grace_ptr[7] = Archive_Find(this->arc_grace, "grace7.tim");
	this->arc_grace_ptr[8] = Archive_Find(this->arc_grace, "grace8.tim");
	this->arc_grace_ptr[9] = Archive_Find(this->arc_grace, "grace9.tim");
	this->arc_grace_ptr[10] = Archive_Find(this->arc_grace, "grace10.tim");
	this->arc_grace_ptr[11] = Archive_Find(this->arc_grace, "grace11.tim");
	this->arc_grace_ptr[12] = Archive_Find(this->arc_grace, "grace12.tim");
	this->arc_grace_ptr[13] = Archive_Find(this->arc_grace, "grace13.tim");
	this->arc_grace_ptr[14] = Archive_Find(this->arc_grace, "grace14.tim");

	//Initialize grace state
	Animatable_Init(&this->grace_animatable, grace_anim);
	
	Animatable_SetAnim(&this->grace_animatable, 0);
	
	this->grace_frame = this->grace_tex_id = 0xFF; //Force art load
	
	//Load hand textures
	this->arc_handa = IO_Read("\\KITCHEN\\HANDA.ARC;1");
	this->arc_handa_ptr[0] = Archive_Find(this->arc_handa, "handa.tim");
	this->arc_handb = IO_Read("\\KITCHEN\\HANDB.ARC;1");
	this->arc_handb_ptr[0] = Archive_Find(this->arc_handb, "handb.tim");
	this->arc_handc = IO_Read("\\KITCHEN\\HANDC.ARC;1");
	this->arc_handc_ptr[0] = Archive_Find(this->arc_handc, "handc.tim");

	//Initialize hand state
	Animatable_Init(&this->handa_animatable, handa_anim);
	Animatable_Init(&this->handb_animatable, handb_anim);
	Animatable_Init(&this->handc_animatable, handc_anim);
	
	Animatable_SetAnim(&this->handa_animatable, 0);
	Animatable_SetAnim(&this->handb_animatable, 0);
	Animatable_SetAnim(&this->handc_animatable, 0);
	
	this->handa_frame = this->handa_tex_id = 0xFE; //Force art load
	this->handb_frame = this->handb_tex_id = 0xFD; //Force art load
	this->handc_frame = this->handc_tex_id = 0xFC; //Force art load
	
	return (StageBack*)this;
}
