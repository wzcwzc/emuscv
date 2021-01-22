/*
	EPOCH/YENO Super Cassette Vision Emulator 'Libretro-EmuSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ EPOCH TV-1 table of characters ]
*/

#ifndef _EMUSCV_INC_VM_SCV_VDP_CHARACTERS_H_
#define _EMUSCV_INC_VM_SCV_VDP_CHARACTERS_H_

static const uint8_t font[128][8] = {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	//   0 : Blank
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x04, 0x74, 0x88, 0x88, 0x74, 0x00},	//   1 : Alpha
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░██░░░░
	// ░░██████░░██░░░░
	// ██░░░░░░██░░░░░░
	// ██░░░░░░██░░░░░░
	// ░░██████░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x30, 0x48, 0x70, 0x48, 0x70, 0x40, 0x40, 0x40},	//   2 : Beta
	// ░░░░████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░██████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░██████░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	{0x00, 0x00, 0x44, 0xA8, 0x10, 0x10, 0x10, 0x10},	//   3 : Upsilon
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░██░░░░░░██░░░░
	// ██░░██░░██░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	{0x00, 0x00, 0xF8, 0x50, 0x50, 0x54, 0x88, 0x00},	//   4 : Pi
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██████████░░░░░░
	// ░░██░░██░░░░░░░░
	// ░░██░░██░░░░░░░░
	// ░░██░░██░░██░░░░
	// ██░░░░░░██░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xFC, 0x84, 0x40, 0x20, 0x40, 0x84, 0xFC, 0x00},	//   5 : Epsilon
	// ████████████░░░░
	// ██░░░░░░░░██░░░░
	// ░░██░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ██░░░░░░░░██░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x30, 0x48, 0x84, 0x84, 0x84, 0x48, 0xCC, 0x00},	//   6 : Omega
	// ░░░░████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ████░░░░████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x20, 0x70, 0xF8, 0xF8, 0xF8, 0x20, 0x70, 0x00},	//   7 : Spade
	// ░░░░██░░░░░░░░░░
	// ░░██████░░░░░░░░
	// ██████████░░░░░░
	// ██████████░░░░░░
	// ██████████░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x50, 0xF8, 0xF8, 0xF8, 0xF8, 0x70, 0x20, 0x00},	//   8 : Heart
	// ░░██░░██░░░░░░░░
	// ██████████░░░░░░
	// ██████████░░░░░░
	// ██████████░░░░░░
	// ██████████░░░░░░
	// ░░██████░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x20, 0x20, 0x70, 0xF8, 0x70, 0x20, 0x20, 0x00},	//   9 : Diamond
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██████░░░░░░░░
	// ██████████░░░░░░
	// ░░██████░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x20, 0x70, 0xA8, 0xF8, 0xA8, 0x20, 0x70, 0x00},	//  10 : Club
	// ░░░░██░░░░░░░░░░
	// ░░██████░░░░░░░░
	// ██░░██░░██░░░░░░
	// ██████████░░░░░░
	// ██░░██░░██░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x30, 0x48, 0xB4, 0xA4, 0xA4, 0xB4, 0x48, 0x30},	//  11 : Copyright
	// ░░░░████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ██░░████░░██░░░░
	// ██░░██░░░░██░░░░
	// ██░░██░░░░██░░░░
	// ██░░████░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	{0x80, 0x80, 0x80, 0x90, 0x48, 0x3C, 0x08, 0x10},	//  12 : Down arrow right
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░██░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████████░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░██░░░░░░░░
	{0x04, 0x04, 0x04, 0x24, 0x48, 0xF0, 0x40, 0x20},	//  13 : Down arrow left
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░██░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ████████░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	{0x20, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x20, 0x00},	//  14 : Divide
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00},	//  15 : Backslash
	// ░░░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x78, 0x84, 0x84, 0x84, 0x84, 0x78, 0x00},	//  16 : Empty circle
	// ░░░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x78, 0xFC, 0xFC, 0xFC, 0xFC, 0x78, 0x00},	//  17 : Full circle
	// ░░░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ████████████░░░░
	// ████████████░░░░
	// ████████████░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x78, 0x84, 0xB4, 0xB4, 0x84, 0x78, 0x00},	//  18 : ?
	// ░░░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░████░░██░░░░
	// ██░░████░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xFC, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0xFC},	//  19 : Empty square
	// ████████████░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ████████████░░░░
	{0xA8, 0x54, 0xA8, 0x54, 0xA8, 0x54, 0xA8, 0x54},	//  20 : Grid
	// ██░░██░░██░░░░░░
	// ░░██░░██░░██░░░░
	// ██░░██░░██░░░░░░
	// ░░██░░██░░██░░░░
	// ██░░██░░██░░░░░░
	// ░░██░░██░░██░░░░
	// ██░░██░░██░░░░░░
	// ░░██░░██░░██░░░░
	{0xFC, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},	//  21 : corner upper left
	// ████████████░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	{0xFC, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},	//  22 : corner upper right
	// ████████████░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFC},	//  23 : corner bottom left
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ████████████░░░░
	{0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0xFC},	//  24 : corner bottom right
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ████████████░░░░
	{0xF8, 0x00, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x00},	//  25 : Japan post logo
	// ██████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██████████░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x10, 0x18, 0x14, 0x14, 0x14, 0x74, 0xF0, 0x60},	//  26 : Full note
	// ░░░░░░██░░░░░░░░
	// ░░░░░░████░░░░░░
	// ░░░░░░██░░██░░░░
	// ░░░░░░██░░██░░░░
	// ░░░░░░██░░██░░░░
	// ░░██████░░██░░░░
	// ████████░░░░░░░░
	// ░░████░░░░░░░░░░
	{0x10, 0x18, 0x14, 0x14, 0x14, 0x74, 0x90, 0x60},	//  27 : Empty note
	// ░░░░░░██░░░░░░░░
	// ░░░░░░████░░░░░░
	// ░░░░░░██░░██░░░░
	// ░░░░░░██░░██░░░░
	// ░░░░░░██░░██░░░░
	// ░░██████░░██░░░░
	// ██░░░░██░░░░░░░░
	// ░░████░░░░░░░░░░
	{0x00, 0x10, 0x08, 0xFC, 0x08, 0x10, 0x00, 0x00},	//  28 : Right arrow
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░██░░░░░░
	// ████████████░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x20, 0x40, 0xFC, 0x40, 0x20, 0x00, 0x00},	//  29 : Left arrow
	// ░░░░░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ████████████░░░░
	// ░░██░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x10, 0x38, 0x54, 0x10, 0x10, 0x10, 0x10},	//  30 : Up arrow
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██████░░░░░░
	// ░░██░░██░░██░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	{0x00, 0x10, 0x10, 0x10, 0x10, 0x54, 0x38, 0x10},	//  31 : Down arrow
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░██░░██░░██░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░██░░░░░░░░
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	//  32 : space " "
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x00},	//  33 : "!"
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x48, 0x48, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00},	//  34 : Double quote "
	// ░░██░░░░██░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x48, 0x48, 0xFC, 0x48, 0xFC, 0x48, 0x48, 0x00},	//  35 : hash "#"
	// ░░██░░░░██░░░░░░
	// ░░██░░░░██░░░░░░
	// ████████████░░░░
	// ░░██░░░░██░░░░░░
	// ████████████░░░░
	// ░░██░░░░██░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00},	//  36 : dollar "$"
	// ░░░░██░░░░░░░░░░
	// ░░████████░░░░░░
	// ██░░██░░░░░░░░░░
	// ░░██████░░░░░░░░
	// ░░░░██░░██░░░░░░
	// ████████░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0xC4, 0xC8, 0x10, 0x20, 0x4C, 0x8C, 0x00},	//  37 : percent "%"
	// ░░░░░░░░░░░░░░░░
	// ████░░░░░░██░░░░
	// ████░░░░██░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██░░░░████░░░░
	// ██░░░░░░████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x60, 0x90, 0x90, 0x60, 0x94, 0x88, 0x74, 0x00},	//  38 : commercial and "&"
	// ░░████░░░░░░░░░░
	// ██░░░░██░░░░░░░░
	// ██░░░░██░░░░░░░░
	// ░░████░░░░░░░░░░
	// ██░░░░██░░██░░░░
	// ██░░░░░░██░░░░░░
	// ░░██████░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00},	//  39 : Apostrophe '
	// ░░░░░░██░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x04, 0x08, 0x10, 0x10, 0x10, 0x08, 0x04, 0x00},	//  40 : "("
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x40, 0x20, 0x10, 0x10, 0x10, 0x20, 0x40, 0x00},	//  41 : ")"
	// ░░██░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x10, 0x54, 0x38, 0x7C, 0x38, 0x54, 0x10, 0x00},	//  42 : "*"
	// ░░░░░░██░░░░░░░░
	// ░░██░░██░░██░░░░
	// ░░░░██████░░░░░░
	// ░░██████████░░░░
	// ░░░░██████░░░░░░
	// ░░██░░██░░██░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00},	//  43 : "+"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░██████████░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x20, 0x00},	//  44 : ","
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00},	//  45 : "-"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░██████████░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00},	//  46 : "."
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00},	//  47 : ?
	// ░░░░░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x84, 0x8C, 0xB4, 0xC4, 0x84, 0x78, 0x00},	//  48 : "0: 
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░████░░░░
	// ██░░████░░██░░░░
	// ████░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x7C, 0x00},	//  49 : "1"
	// ░░░░░░██░░░░░░░░
	// ░░░░████░░░░░░░░
	// ░░██░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░██████████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x84, 0x04, 0x18, 0x60, 0x80, 0xFC, 0x00},	//  50 : "2"
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░████░░░░░░
	// ░░████░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x84, 0x04, 0x38, 0x04, 0x84, 0x78, 0x00},	//  51 : "3"
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x08, 0x18, 0x28, 0x48, 0xFC, 0x08, 0x08, 0x00},	//  52 : "4"
	// ░░░░░░░░██░░░░░░
	// ░░░░░░████░░░░░░
	// ░░░░██░░██░░░░░░
	// ░░██░░░░██░░░░░░
	// ████████████░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xFC, 0x80, 0xF8, 0x04, 0x04, 0x84, 0x78, 0x00},	//  53 : "5"
	// ████████████░░░░
	// ██░░░░░░░░░░░░░░
	// ██████████░░░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x38, 0x40, 0x80, 0xF8, 0x84, 0x84, 0x78, 0x00},	//  54 : "6"
	// ░░░░██████░░░░░░
	// ░░██░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xFC, 0x84, 0x08, 0x10, 0x20, 0x20, 0x20, 0x00},	//  55 : "7"
	// ████████████░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x84, 0x84, 0x78, 0x84, 0x84, 0x78, 0x00},	//  56 : "8"
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x84, 0x84, 0x7C, 0x04, 0x08, 0x70, 0x00},	//  57 : "9"
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░██████████░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░██░░░░░░
	// ░░██████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 0x00},	//  58 : ":"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x10, 0x20},	//  59 : ";"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██░░░░░░░░░░
	{0x1C, 0x30, 0x60, 0xC0, 0x60, 0x30, 0x1C, 0x00},	//  60 : "<"
	// ░░░░░░██████░░░░
	// ░░░░████░░░░░░░░
	// ░░████░░░░░░░░░░
	// ████░░░░░░░░░░░░
	// ░░████░░░░░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░░░██████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0xFC, 0x00, 0xFC, 0x00, 0x00, 0x00},	//  61 : "="
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xE0, 0x30, 0x18, 0x0C, 0x18, 0x30, 0xE0, 0x00},	//  62 : ">"
	// ██████░░░░░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░░░████░░░░░░
	// ░░░░░░░░████░░░░
	// ░░░░░░████░░░░░░
	// ░░░░████░░░░░░░░
	// ██████░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x84, 0x04, 0x18, 0x20, 0x00, 0x20, 0x00},	//  63 : "?"
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░████░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x38, 0x44, 0x94, 0xAC, 0x98, 0x40, 0x38, 0x00},	//  64 : "@"
	// ░░░░██████░░░░░░
	// ░░██░░░░░░██░░░░
	// ██░░░░██░░██░░░░
	// ██░░██░░████░░░░
	// ██░░░░████░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x30, 0x48, 0x84, 0xFC, 0x84, 0x84, 0x84, 0x00},	//  65 : "A"
	// ░░░░████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ██░░░░░░░░██░░░░
	// ████████████░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0xF8, 0x44, 0x44, 0x78, 0x44, 0x44, 0xF8, 0x00},	//  66 : "B"
	// ██████████░░░░░░
	// ░░██░░░░░░██░░░░
	// ░░██░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░██░░░░░░██░░░░
	// ░░██░░░░░░██░░░░
	// ██████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x38, 0x44, 0x80, 0x80, 0x80, 0x44, 0x38, 0x00},	// 67 : "C"
	// ░░░░██████░░░░░░
	// ░░██░░░░░░██░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ░░██░░░░░░██░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xF0, 0x48, 0x44, 0x44, 0x44, 0x48, 0xF0, 0x00},	//  68 : "D"
	// ████████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░██░░░░░░██░░░░
	// ░░██░░░░░░██░░░░
	// ░░██░░░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ████████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xFC, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xFC, 0x00},	//  69 : "E"
	// ████████████░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ████████░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	{0xFC, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x00},	//  70 : "F"
	// ████████████░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ████████░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x38, 0x44, 0x80, 0x9C, 0x84, 0x44, 0x38, 0x00},	//  71 : "G"
	// ░░░░██████░░░░░░
	// ░░██░░░░░░██░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░██████░░░░
	// ██░░░░░░░░██░░░░
	// ░░██░░░░░░██░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x84, 0x84, 0x84, 0xFC, 0x84, 0x84, 0x84, 0x00},	//  72 : "H"
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ████████████░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00},	//  73 : "I"
	// ░░░░██████░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x1C, 0x08, 0x08, 0x08, 0x08, 0x88, 0x70, 0x00},	//  74 : "J"
	// ░░░░░░██████░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ██░░░░░░██░░░░░░
	// ██░░░░░░██░░░░░░
	// ░░██████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x84, 0x88, 0x90, 0xE0, 0x90, 0x88, 0x84, 0x00},	//  75 : "K"
	// ██░░░░░░░░██░░░░
	// ██░░░░░░██░░░░░░
	// ██░░░░██░░░░░░░░
	// ██████░░░░░░░░░░
	// ██░░░░██░░░░░░░░
	// ██░░░░░░██░░░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFC, 0x00},	//  76 : "L"
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x84, 0xCC, 0xB4, 0xB4, 0x84, 0x84, 0x84, 0x00},	//  77 : "M"
	// ██░░░░░░░░██░░░░
	// ████░░░░████░░░░
	// ██░░████░░██░░░░
	// ██░░████░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x84, 0xC4, 0xA4, 0x94, 0x8C, 0x84, 0x84, 0x00},	//  78 : "N"
	// ██░░░░░░░░██░░░░
	// ████░░░░░░██░░░░
	// ██░░██░░░░██░░░░
	// ██░░░░██░░██░░░░
	// ██░░░░░░████░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x30, 0x48, 0x84, 0x84, 0x84, 0x48, 0x30, 0x00},	//  79 : "O"
	// ░░░░████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xF8, 0x84, 0x84, 0xF8, 0x80, 0x80, 0x80, 0x00},	//  80 : "P"
	// ██████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██████████░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x30, 0x48, 0x84, 0x84, 0x94, 0x48, 0x34, 0x00},	//  81 : "Q"
	// ░░░░████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░██░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0xF8, 0x84, 0x84, 0xF8, 0x90, 0x88, 0x84, 0x00},	//  82 : "R"
	// ██████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██████████░░░░░░
	// ██░░░░██░░░░░░░░
	// ██░░░░░░██░░░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x84, 0x80, 0x78, 0x04, 0x84, 0x78, 0x00},	//  83 : "S"
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00},	//  84 : "T"
	// ░░██████████░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x78, 0x00},	//  85 : "U"
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x84, 0x84, 0x84, 0x48, 0x48, 0x30, 0x30, 0x00},	//  86 : "V"
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x84, 0x84, 0x84, 0xB4, 0xB4, 0xCC, 0x48, 0x00},	//  87 : "W"
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░████░░██░░░░
	// ██░░████░░██░░░░
	// ████░░░░████░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x84, 0x84, 0x48, 0x30, 0x48, 0x84, 0x84, 0x00},	//  88 : "X"
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x44, 0x44, 0x44, 0x38, 0x10, 0x10, 0x10, 0x00},	//  89 : "Y"
	// ░░██░░░░░░██░░░░
	// ░░██░░░░░░██░░░░
	// ░░██░░░░░░██░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xFC, 0x04, 0x08, 0x30, 0x40, 0x80, 0xFC, 0x00},	//  90 : "Z"
	// ████████████░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x40, 0x40, 0x40, 0x40, 0x40, 0x78, 0x00},	//  91 : "["
	// ░░████████░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x44, 0x44, 0x28, 0x7C, 0x10, 0x7C, 0x10, 0x00},	//  92 : Yen
	// ░░██░░░░░░██░░░░
	// ░░██░░░░░░██░░░░
	// ░░░░██░░██░░░░░░
	// ░░██████████░░░░
	// ░░░░░░██░░░░░░░░
	// ░░██████████░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x78, 0x08, 0x08, 0x08, 0x08, 0x08, 0x78, 0x00},	//  93 : "]"
	// ░░████████░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00},	//  94 : "^"
	// ░░░░░░██░░░░░░░░
	// ░░░░██░░██░░░░░░
	// ░░██░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00},	//  95 : "_"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	//  96 : " "
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x78, 0x08, 0x78, 0x88, 0x74, 0x00},	//  97 : "a"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░████████░░░░░░
	// ██░░░░░░██░░░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x80, 0x80, 0xB8, 0xC4, 0x84, 0xC4, 0xB8, 0x00},	//  98 : "b"
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░██████░░░░░░
	// ████░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ████░░░░░░██░░░░
	// ██░░██████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x78, 0x84, 0x80, 0x84, 0x78, 0x00},	//  99 : "c"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x04, 0x04, 0x74, 0x8C, 0x84, 0x8C, 0x74, 0x00},	// 100 : "d"
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░██████░░██░░░░
	// ██░░░░░░████░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░████░░░░
	// ░░██████░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x78, 0x84, 0xFC, 0x80, 0x78, 0x00},	// 101 : "e"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ████████████░░░░
	// ██░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x18, 0x24, 0x20, 0xF8, 0x20, 0x20, 0x20, 0x00},	// 102 : "f"
	// ░░░░░░████░░░░░░
	// ░░░░██░░░░██░░░░
	// ░░░░██░░░░░░░░░░
	// ██████████░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x74, 0x8C, 0x8C, 0x74, 0x04, 0x78},	// 103 : "g"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░██████░░██░░░░
	// ██░░░░░░████░░░░
	// ██░░░░░░████░░░░
	// ░░██████░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░████████░░░░░░
	{0x80, 0x80, 0xB8, 0xC4, 0x84, 0x84, 0x84, 0x00},	// 104 : "h"
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░██████░░░░░░
	// ████░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x38, 0x00},	// 105 : "i"
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x08, 0x00, 0x18, 0x08, 0x08, 0x08, 0x48, 0x30},	// 106 : "j"
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░████░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	{0x40, 0x40, 0x44, 0x48, 0x50, 0x68, 0x44, 0x00},	// 107 : "k"
	// ░░██░░░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░██░░░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░██░░██░░░░░░░░
	// ░░████░░██░░░░░░
	// ░░██░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00},	// 108 : "l"
	// ░░░░████░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░██████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0xA8, 0x54, 0x54, 0x54, 0x54, 0x00},	// 109 : "m"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░██░░██░░░░░░
	// ░░██░░██░░██░░░░
	// ░░██░░██░░██░░░░
	// ░░██░░██░░██░░░░
	// ░░██░░██░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0xB8, 0xC4, 0x84, 0x84, 0x84, 0x00},	// 110 : "n"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░██████░░░░░░
	// ████░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x78, 0x84, 0x84, 0x84, 0x78, 0x00},	// 111: "o"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0xB8, 0xC4, 0xC4, 0xB8, 0x80, 0x80},	// 112 : "p"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░██████░░░░░░
	// ████░░░░░░██░░░░
	// ████░░░░░░██░░░░
	// ██░░██████░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	{0x00, 0x00, 0x74, 0x8C, 0x8C, 0x74, 0x04, 0x04},	// 113 : "q"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░██████░░██░░░░
	// ██░░░░░░████░░░░
	// ██░░░░░░████░░░░
	// ░░██████░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░░░░░░░░░██░░░░
	{0x00, 0x00, 0xB8, 0xC4, 0x80, 0x80, 0x80, 0x00},	// 114 : "r"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░██████░░░░░░
	// ████░░░░░░██░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ██░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x7C, 0x80, 0x78, 0x04, 0xF8, 0x00},	// 115 : "s"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░██████████░░░░
	// ██░░░░░░░░░░░░░░
	// ░░████████░░░░░░
	// ░░░░░░░░░░██░░░░
	// ██████████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x20, 0x20, 0xF8, 0x20, 0x20, 0x24, 0x18, 0x00},	// 116 : "t"
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ██████████░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░██░░░░
	// ░░░░░░████░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x84, 0x84, 0x84, 0x8C, 0x74, 0x00},	// 117 : "u"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░████░░░░
	// ░░██████░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x84, 0x84, 0x84, 0x48, 0x30, 0x00},	// 118 : "v"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x84, 0xB4, 0xB4, 0xB4, 0x48, 0x00},	// 119 : "w"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░████░░██░░░░
	// ██░░████░░██░░░░
	// ██░░████░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x84, 0x48, 0x30, 0x48, 0x84, 0x00},	// 120 : "x"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░░░░░░░██░░░░
	// ░░██░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	// ░░██░░░░██░░░░░░
	// ██░░░░░░░░██░░░░
	// ░░░░░░░░░░░░░░░░
	{0x00, 0x00, 0x84, 0x84, 0x8C, 0x74, 0x04, 0x78},	// 121 : "y"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░░░██░░░░
	// ██░░░░░░████░░░░
	// ░░██████░░██░░░░
	// ░░░░░░░░░░██░░░░
	// ░░████████░░░░░░
	{0x00, 0x00, 0xFC, 0x08, 0x30, 0x40, 0xFC, 0x00},	// 122 : "z"
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░████░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ████████████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x1C, 0x20, 0x20, 0x40, 0x20, 0x20, 0x1C, 0x00},	// 123 : "{"
	// ░░░░░░██████░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░██░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░██████░░░░
	// ░░░░░░░░░░░░░░░░
	{0x20, 0x20, 0x00, 0x00, 0x00, 0x20, 0x20, 0x00},	// 124 : pipe
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░██░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xE0, 0x10, 0x10, 0x08, 0x10, 0x10, 0xE0, 0x00},	// 125 : "}"
	// ██████░░░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░██░░░░░░░░
	// ░░░░░░██░░░░░░░░
	// ██████░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0x20, 0x54, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00},	// 126 : "~"
	// ░░░░██░░░░░░░░░░
	// ░░██░░██░░██░░░░
	// ░░░░░░░░██░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	// ░░░░░░░░░░░░░░░░
	{0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC}	// 127 : Full square
	// ████████████░░░░
	// ████████████░░░░
	// ████████████░░░░
	// ████████████░░░░
	// ████████████░░░░
	// ████████████░░░░
	// ████████████░░░░
	// ████████████░░░░
};

#endif	// _EMUSCV_INC_VM_SCV_VDP_CHARACTERS_H_
