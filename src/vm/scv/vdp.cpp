/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ EPOCH TV-1 ]
*/

#include "vdp.h"
#include "scv.h"
#include "../upd7801.h"

void VDP::reset_palette()
{
	if(config.window_palette == SETTING_PALETTE_OLDNTSC_VAL)
		palette_pc = (scrntype_t *)palette_ntsc;
	else
		palette_pc = (scrntype_t *)palette_pal;
}

void VDP::initialize()
{
	resetting = true;
	resetscanband = int(SCREEN_HEIGHT*rand()/RAND_MAX);
	// register event to interrupt
	register_vline_event(this);
	reset_palette();
}

void VDP::reset()
{
	resetting = true;
}

void VDP::event_vline(int v, int clock)
{
	if(v == 239)
	{
		d_cpu->write_signal(SIG_UPD7801_INTF2, 1, 1);
	}
	else if(v == 261)
	{
		d_cpu->write_signal(SIG_UPD7801_INTF2, 0, 1);
	}
}

void VDP::draw_screen()
{
	// get vdc control params
	vdc0 = vram1[0x400];
	vdc1 = vram1[0x401];
	vdc2 = vram1[0x402];
	vdc3 = vram1[0x403];

	// draw text screen
	memset(text, vdc1 & 0xf, sizeof(text));
	draw_text_screen();

	// draw sprite screen
	memset(sprite, 0, sizeof(sprite));
	if(vdc0 & 0x10)
	{
		//clipy = ((vdc0 & 0xf7) == 0x17 && (vdc2 & 0xef) == 0x4f) ? 31 : 0;
		draw_sprite_screen();
	}

	// Mix buffers
	scrntype_t* d = emu->get_screen_ptr();
	memset(d, 0, SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(uint32_t));
	for(int y = 319; y >= 0; y--)
		for(int x = 0; x < 320; x++)
		{
			d[int(x+4+y*SCREEN_WIDTH)] = palette_pc[text[y][x]];
			if(sprite[y][x])
				d[int(x-(x > 255 ? 256 : 0)+(y+2)*SCREEN_WIDTH)] = palette_pc[sprite[y][x]];
		}

	// Reset scanline
	if(resetting)
	{
		for(int y = resetscanband; y < resetscanband+30 ; y++)
			for(int x = 0; x < SCREEN_WIDTH; x++)
				if(y >= 0 && y < SCREEN_HEIGHT)
					d[int(x+y*SCREEN_WIDTH)] = palette_pc[0x1];
	}
	resetscanband -= 2;
	if(resetscanband < -30)
		resetscanband = SCREEN_HEIGHT+60;
	resetting = false;

	// Screen border
	// YENO
	if(config.screen_display == SETTING_DISPLAY_YENO_VAL)
	{
		for(int y = 0; y < 256; y++)
			for(int x = 215; x < 218; x++)
					d[int(x+y*SCREEN_WIDTH)] = palette_pc[0x1];
		for(int y = 256; y < 264; y++)
			for(int x = 32; x < 218; x++)
					d[int(x+y*SCREEN_WIDTH)] = palette_pc[0x1];
	}

	// Screen contour
	if(config.window_displayfullmemory == SETTING_DISPLAYFULLMEMORY_YES_VAL)
	{
		switch(config.screen_display)
		{
			case SETTING_DISPLAY_EPOCH_VAL:
				// EPOCH
				for(int x = 26; x < 226; x++)
				{
					d[int(x+23*SCREEN_WIDTH)] = palette_pc[0x8];
					d[int(x+248*SCREEN_WIDTH)] = palette_pc[0x8];
				}
				for(int y = 24; y < 248; y++)
				{
					d[int(26+y*SCREEN_WIDTH)] = palette_pc[0x8];
					d[int(225+y*SCREEN_WIDTH)] = palette_pc[0x8];
				}
				break;
			case SETTING_DISPLAY_YENO_VAL:
				// YENO
				for(int x = 31; x < 219; x++)
				{
//					d[int(x-1*SCREEN_WIDTH)] = palette_pc[0x2];
					d[int(x+264*SCREEN_WIDTH)] = palette_pc[0x2];
				}
				for(int y = 0; y < 264; y++)
				{
					d[int(31+y*SCREEN_WIDTH)] = palette_pc[0x2];
					d[int(218+y*SCREEN_WIDTH)] = palette_pc[0x2];
				}
				break;
			case SETTING_DISPLAY_EMUSCV_VAL:
			default:
				// EMUSCV
				for(int x = 29; x < 223; x++)
				{
					d[int(x+27*SCREEN_WIDTH)] = palette_pc[0x4];
					d[int(x+244*SCREEN_WIDTH)] = palette_pc[0x4];
				}
				for(int y = 28; y < 244; y++)
				{
					d[int(29+y*SCREEN_WIDTH)] = palette_pc[0x4];
					d[int(222+y*SCREEN_WIDTH)] = palette_pc[0x4];
				}
				break;
		}
	}
}

inline void VDP::draw_text_screen()
{
	int xmax = (vdc2 & 0xf) * 2;
	int ymax = vdc2 >> 4;
	int xs = (vdc0 & 0x40) ? xmax : 0;
	int xe = (vdc0 & 0x40) ? 32 : xmax;
	int ys = (vdc0 & 0x80) ? ymax : 0;
	int ye = (vdc0 & 0x80) ? 16 : ymax;

	uint8_t ct = vdc3 >> 4;
	uint8_t cb = vdc3 & 0xf;
	uint8_t cg = vdc1 >> 4;

	for(int y = 15; y >= 0; y--)
	{
		bool t = (ys <= y && y < ye);
		int y32 = y << 5;

		for(int x = 0; x < 32; x++)
		{
			if(t && (xs <= x && x < xe))
			{
				// draw text
				uint8_t data = vram1[y32 + x] & 0x7f;
				draw_text(x, y, data, ct, cb);
			}
			else if((vdc0 & 3) == 1)
			{
				// semi graph
				uint8_t data = vram1[y32 + x];
				draw_graph(x, y, data, cg);
			}
			else if((vdc0 & 3) == 3)
			{
				// block
				uint8_t data = vram1[y32 + x];
				draw_block(x, y, data);
			}
		}
	}
}

inline void VDP::draw_text(int dx, int dy, uint8_t data, uint8_t tcol, uint8_t bcol)
{
	int dx8 = (dx << 3)+2;
	int dy16 = dy << 4;

	for(int l = 0; l < 8 && data; l++)
	{
		uint8_t* dest = &text[dy16 + l][dx8];
		uint8_t pat = font[data][l];

		dest[0] = (pat & 0x80) ? tcol : bcol;
		dest[1] = (pat & 0x40) ? tcol : bcol;
		dest[2] = (pat & 0x20) ? tcol : bcol;
		dest[3] = (pat & 0x10) ? tcol : bcol;
		dest[4] = (pat & 0x08) ? tcol : bcol;
		dest[5] = (pat & 0x04) ? tcol : bcol;
		dest[6] = (pat & 0x02) ? tcol : bcol;
		dest[7] = (pat & 0x01) ? tcol : bcol;
	}
	for(int l = (data ? 8 : 0); l < 16; l++)
	{
		memset(&text[dy16 + l][dx8], bcol, 8);
	}
}

inline void VDP::draw_block(int dx, int dy, uint8_t data)
{
	int dx8 = dx << 3; // Pointer to pixel is aligned to 8.
	int dy16 = dy << 4;
	uint8_t cu = data >> 4;
	uint8_t cl = data & 0xf;

	if (cu != 0)
	{
    unsigned int* p = (unsigned int*)&text[dy16][dx8];
	  uint32_t c = cu + ((uint32_t)cu << 8); c = c + (c << 16);
	  for(int i = 8; --i >= 0; p += 320 >> 2)
	    p[0] = p[1] = c;
		}
	if (cl != 0)
	{
    unsigned int* p = (unsigned int*)&text[dy16 + 8][dx8];
    uint32_t c = cl + ((uint32_t)cl << 8); c = c + (c << 16);
    for(int i = 8; --i >= 0; p += 320 >> 2)
      p[0] = p[1] = c;
		}
	}

inline void VDP::draw_graph(int dx, int dy, uint8_t data, uint8_t col)
{
	int dx8l = dx << 3;
	int dx8r = (dx << 3) + 4;
	int dy16 = dy << 4;

	if(data & 0x80)
	{
		for(int l = 0; l < 4; l++)
		{
			memset(&text[dy16 + l][dx8l], col, 4);
		}
	}
	if(data & 0x40)
	{
		for(int l = 0; l < 4; l++)
		{
			memset(&text[dy16 + l][dx8r], col, 4);
		}
	}
	if(data & 0x20)
	{
		for(int l = 4; l < 8; l++)
		{
			memset(&text[dy16 + l][dx8l], col, 4);
		}
	}
	if(data & 0x10)
	{
		for(int l = 4; l < 8; l++)
		{
			memset(&text[dy16 + l][dx8r], col, 4);
		}
	}
	if(data & 0x08)
	{
		for(int l = 8; l < 12; l++)
		{
			memset(&text[dy16 + l][dx8l], col, 4);
		}
	}
	if(data & 0x04)
	{
		for(int l = 8; l < 12; l++)
		{
			memset(&text[dy16 + l][dx8r], col, 4);
		}
	}
	if(data & 0x02)
	{
		for(int l = 12; l < 16; l++)
		{
			memset(&text[dy16 + l][dx8l], col, 4);
		}
	}
	if(data & 0x01)
	{
		for(int l = 12; l < 16; l++)
		{
			memset(&text[dy16 + l][dx8r], col, 4);
		}
	}
}

inline void VDP::draw_sprite_screen()
{
	int nbsprite = ((vdc0 & 0xf7) == 0x17 && (vdc2 & 0xef) == 0x4f) ? 64 : 128;

if(nbsprite == 64)	// Patch for Kung-Fu Road only?
	bool stop_here = true;

	for(int index = 0; index < nbsprite; index++)
	{
		uint8_t atb0 = vram1[0x200 + (index << 2)];	// 11111110: position y (0 not displayed)
													// 00000001: double height
		uint8_t atb1 = vram1[0x201 + (index << 2)];	// 11110000: clip y for top
													// 00001111: color
		uint8_t atb2 = vram1[0x202 + (index << 2)];	// 11111110: position x
													// 00000001: double width
		uint8_t atb3 = vram1[0x203 + (index << 2)];	// 10000000: half width
													// 01000000: half height
													// 01111111: sprite leaf

		int dx = atb2 & 0xfe;
		int dy = atb0 & 0xfe;
		if(index & 0xf0 && (dx == 0 || dy == 0))
			continue;

		bool conx = ((atb2 & 1) != 0);
		bool cony = ((atb0 & 1) != 0);
		
		uint8_t col1 = atb1 & 0x0f;

		int sx = 0;
		int ex = 4;
		int sy = atb1 >> 4;
		int ey = 8;

		if(atb3 & 0x80)
		{
			// half/quarter sprite
			if(atb3 & 0x40)
			{
				sy = !cony ? 0 : 4;
				ey = !cony ? 4 : 8;
				dy = !cony ? dy : dy - 8;
				cony = false;
			}
			sx = !conx ? 0 : 2;
			ex = !conx ? 2 : 4;
			dx = !conx ? dx : dx - 8;
			conx = false;

			atb3 &= 0x7f;
		}



/*
		// R&D
		int no1 = atb3;
		draw_sprite(dx, dy, sx, ex, sy, ey, no1, col1);
		if(conx && cony)
		{
			int no3 = atb3 | 9;
			int no3x = atb3 ^ 9;
			if(no3 != no3x)
				int i = 0;
//			int col2 = ((vdc0 & 0x20) ? color_pair_xy[col1] : col1);
//			draw_sprite(dx + 16, dy+16, 0, 4, sy, ey, no3, col1);
		}
		else if(cony)
		{
			int no3 = atb3 | 1;
			int no3x = atb3 ^ 1;
			if(no3 != no3x)
				int i = 0;
		}
		else if(conx)
		{
			int no3 = atb3 | 8;
			int no3x = atb3 ^ 8;
			if(no3 != no3x)
				int i = 0;
//			int col2 = ((vdc0 & 0x20) ? color_pair_x[col1] : col1);
//			draw_sprite(dx + 16, dy, 0, 4, sy, ey, no3, col1);
		}
*/


 		if((index & 0x20) && (vdc0 & 0x20))
 		{
			// 2 colors sprite
			int no1 = atb3;
			if(conx || cony)	// Normal 2 colors sprite
			{
				int no2;
				uint8_t col2;
				if(conx && cony)
				{
					no2 = atb3 ^ 9;
					col2 = color_pair_xy[col1];
				}
				else if(cony)
				{
					// Patch for Astro Wars 2
					if((index == 123 || index == 124) && cony)
					{
						no2 = atb3 ^ 8;
						col2 = color_pair_x[col1];
					}
					else
					{
						no2 = atb3 ^ 1;
						col2 = color_pair_y[col1];
					}
				}
				else// if(conx)1
				{
					no2 = atb3 ^ 8;
					col2 = color_pair_x[col1];
				}

if(col1 !=0 && col2 == 0x0)	// New color pair?
{
//	dx = 20;
//	dy = 20;
//	sx = 0;
//	sy = 0;
//	ex = 8;
//	ey = 8;
//	col1 = 0xF;
	col2 = 0xF;
}

				draw_sprite(dx, dy, sx, ex, sy, ey, no1, col1);
				draw_sprite(dx, dy, sx, ex, sy, ey, no2, col2);
			}
			else if(vdc0 == 0x78 && vdc2 == 0x32 && no1 == 0)	// Inverted 2 colors sprite	// Patch for Boulder Dash only?
			{
				// No cart:			50	0
				// Boulder Dash:	112	34

//if((vdc0 & 0xf7) != 112 || (vdc2 & 0xef) != 34)
//	int i = 0;
//if(vdc0 != 120 || vdc1 != 17 || vdc2 != 50 || vdc3 != 241)
//	int i = 0;

				int no2 = 11;// or 9... but why?
				uint8_t col2 = color_pair_xy[col1];
/*
if(col1 !=0 && col2 == 0x0)	// // New color pair?
{
//	dx = 20;
//	dy = 20;
//	sx = 0;
//	sy = 0;
//	ex = 8;
//	ey = 8;
//	col2 = 0x4;
	col2 = 0x8;
}
*/
//col1= 0x4;
//col2= 0x8;
				draw_sprite(dx, dy, sx, ex, sy, ey, no1, col2);
				draw_sprite(dx, dy, sx, ex, sy, ey, no2, col1);
			}
			else	// Simple 2 colors sprite
			{
				draw_sprite(dx, dy, sx, ex, sy, ey, no1, col1);
			}
 		}
 		else
 		{
			// mono color sprite
			int no1 = atb3;

			draw_sprite(dx, dy, sx, ex, sy, ey, no1, col1);
			if(cony)
			{
				int no2 = atb3 | 1;
				draw_sprite(dx, dy + 16, sx, ex, sy - 8, 8, no2, col1);
			}
			if(conx)
			{
				int no3 = atb3 | 8;
				draw_sprite(dx + 16, dy, 0, 4, sy, ey, no3, col1);
			}
			if(conx && cony)
			{
				int no4 = atb3 | 9;
				draw_sprite(dx + 16, dy + 16, 0, 4, sy - 8, 8, no4, col1);
			}
/*
if(index == 0)
{
	int nn = 0;
	int cc = 0x2;
	for(int yy = 1; yy < 1+15*17; yy += 17)
	for(int xx = 1; xx < 1+15*17; xx += 17)
	{
		cc= 0xF;
		if(nn == 43)
			cc = 0x8;
		draw_sprite(xx, yy, 0, 4, 0, 8, nn, cc);
		nn++;
		cc++;
		if(cc >= 0xF)
			cc = 0x2;
	}
}
*/

		}
	}
}

inline void VDP::draw_sprite(int dx, int dy, int sx, int ex, int sy, int ey, int no, uint8_t col)
{
 	// color #0 is transparent
 	if(!col)
 		return;

	if(sy < 0)
		sy = 0;

	int no32 = no << 5;
 	for(int y = sy; y < ey; y++)
	 {
 		int y2u = (y << 1) + dy;
		int y2l = (y << 1) + dy + 1;
		int y4 = (y << 2) + no32;
//		if(y2u > 0 && y2u < 317 && y2l > 0 && y2l < 317)
//		{
			for(int x = sx; x < ex; x++)
			{
				int x4 = dx + (x << 2);
//				if (x4 > 0 && x4 < 317)
//				{
					uint8_t* du = &sprite[y2u][x4];
					uint8_t* dl = &sprite[y2l][x4];
					uint8_t p = vram0[y4 + x];

					if(p & 0x80)
						du[0] = col;
					if(p & 0x40)
						du[1] = col;
					if(p & 0x20)
						du[2] = col;
					if(p & 0x10)
						du[3] = col;
					if(p & 0x08)
						dl[0] = col;
					if(p & 0x04)
						dl[1] = col;
					if(p & 0x02)
						dl[2] = col;
					if(p & 0x01)
						dl[3] = col;
/*
					if(y == sy)
					{
						du[0] = 0x8;
						du[1] = 0x8;
						du[2] = 0x8;
						du[3] = 0x8;
					}
					if(y == ey-1)
					{
						dl[0] = 0x8;
						dl[1] = 0x8;
						dl[2] = 0x8;
						dl[3] = 0x8;
					}
					if(x == sx)
					{
						du[0] = 0x8;
						dl[0] = 0x8;
					}
					if(x == ex-1)
					{
						du[3] = 0x8;
						dl[3] = 0x8;
					}
*/
//				}
			}
//		}
 	}

	// Patch for Lupin III only?
/*
	if(vdc0 == 83 && vdc2 == 64 && no == 43 && col == 0x9)
	{
		no = 42;
		col = 0xf;
		draw_sprite(dx, dy, sx, ex, sy, ey, no, col);
	}
*/
}

#define STATE_VERSION	1

bool VDP::process_state(FILEIO* state_fio, bool loading)
{
	if(!state_fio->StateCheckUint32(STATE_VERSION))
		return false;
	if(!state_fio->StateCheckInt32(this_device_id))
		return false;
	state_fio->StateValue(vdc0);
	state_fio->StateValue(vdc1);
	state_fio->StateValue(vdc2);
	state_fio->StateValue(vdc3);
	
	return true;
}
