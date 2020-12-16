/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modifed by Maxime MARCONATO (aka MaaaX)
	for Libretro-EmuSCV

	[ EPOCH TV-1 palette of colors ]
*/

#ifndef _EMUSCV_INC_VM_SCV_VDP_COLORS_H_
#define _EMUSCV_INC_VM_SCV_VDP_COLORS_H_

#define XXX 0x0	// Not tested
#define OOO 0x8	// Tested but unable to find color

static const scrntype_t palette_pc[16] =
{
	RGB_COLOR(  0,   0, 136),	//  0 0x0 BLUE 1 (MEDIUM) for text  / TRANSPARENT for sprites
	RGB_COLOR(  8,   8,   8),	//  1 0x1 BLACK
	RGB_COLOR(  0,   0, 224),	//  2 0x2 BLUE 2 (LIGHT)
	RGB_COLOR(208,   0, 248),	//  3 0x3 PURPLE
	RGB_COLOR(  0, 240,   0),	//  4 0x4 GREEN 1 (LIGHT)
	RGB_COLOR(128, 232, 120),	//  5 0x5 GREEN 2 (SOFT)
	RGB_COLOR(  0, 224, 216),	//  6 0x6 CYAN
	RGB_COLOR(  0, 216,   0),	//  7 0x7 GREEN 3 (MEDIUM)
	RGB_COLOR(232,   0,   0),	//  8 0x8 RED
	RGB_COLOR(240, 160,  32),	//  9 0x9 ORANGE
	RGB_COLOR(240,  64, 232),	// 10 0xA MAGENTA
	RGB_COLOR(248, 184, 200),	// 11 0xB PINK / NUOE
	RGB_COLOR(232, 232,   0),	// 12 0xC YELLOW
	RGB_COLOR(208, 184,  16),	// 13 0xD GREEN 4 (DARK) / MAROON
	RGB_COLOR(168, 168, 168),	// 14 0xE GRAY
	RGB_COLOR(232, 232, 232)	// 15 0xF WHITE
};
// Only use multiples of 8
//   0   8  16  24  32  40  48  56
//  64  72  80  88  96 104 112 120 
// 128 136 144 152 160 168 176 184
// 192 200 208 216 224 232 240 248

// Pairs of color used for two colors sprtites
//                                        0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7  0x8  0x9  0xA  0xB  0xC  0xD  0xE  0xF
static const uint8_t color_pair_x[16]  = {XXX, XXX, 0x8, 0x9, 0x2, 0x3, 0xA, 0xB, 0x4, 0x5, 0xB, 0xC, 0x6, 0x4, 0xE, XXX};
static const uint8_t color_pair_y[16]  = {XXX, XXX, XXX, XXX, 0x8, XXX, 0xC, XXX, XXX, 0x3, 0x6, XXX, XXX, 0xB, XXX, XXX};
static const uint8_t color_pair_xy[16] = {XXX, 0xF, 0xC, XXX, 0xA, XXX, 0x8, XXX, 0x6, 0x7, 0x5, 0x4, 0x2, 0x3, XXX, 0x1};

#endif	// _EMUSCV_INC_VM_SCV_VDP_COLORS_H_
