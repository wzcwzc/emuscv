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
	scrntype_t* d = emu->get_screen_ptr();

	// get vdc control params
	vdc0 = vram1[0x400];
	vdc1 = vram1[0x401];
	vdc2 = vram1[0x402];
	vdc3 = vram1[0x403];

	// Clear entire screen
	if(config.window_displayfullmemory == SETTING_DISPLAYFULLMEMORY_YES_VAL && config.window_clear)
	{
		memset(d, 0, SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(uint32_t));
		config.window_clear = false;
	}
	// Clear text zone
	scrntype_t *dd = d + config.mix_x_max + 3 + (config.mix_y_max - 1) * SCREEN_WIDTH;
	scrntype_t c = palette_pc[vdc1 & 0xf];
	uint16_t yd = SCREEN_WIDTH + config.mix_x_min - config.mix_x_max;
	for(int16_t y = config.mix_y_max; --y >= config.mix_y_min; )
	{
		for (int16_t x = config.mix_x_max; --x >= config.mix_x_min; )
			*dd-- = c;
		dd -= yd;
	}

//	// draw text screen
//	memset(text, vdc1 & 0xf, sizeof(text));
	draw_text_screen(d);

	// draw sprite screen
	memset(sprite, 0, sizeof(sprite));
	if(vdc0 & 0x10)
		draw_sprite_screen();

	// Mix buffers
	if(config.window_displayfullmemory == SETTING_DISPLAYFULLMEMORY_YES_VAL)
	{
		for(int y = config.mix_y_max; --y >= config.mix_y_min; )
		{
//			scrntype_t* pt = &d[4 + y * SCREEN_WIDTH];
			scrntype_t* ps = &d[(y + 2) * SCREEN_WIDTH];
//			unsigned char *st = text[y];
			unsigned char *ss = sprite[y];
			for (int x = config.mix_x_min - 1; ++x < config.mix_x_max;)
			{
//				*pt++ = palette_pc[*st++];
				unsigned int s = *ss++;
				if (s != 0)
					ps[x] = palette_pc[s];
			}
		}
	}
	else
	{
		int16_t ymin = (config.mix_y_min > 1 ? config.mix_y_min : 1);
		scrntype_t *dd = d + config.mix_x_max - 1 + (config.mix_y_max - 1) * SCREEN_WIDTH;
		for(int16_t y = config.mix_y_max; --y >= ymin; )
		{
			for (int16_t x = config.mix_x_max; --x >= config.mix_x_min; )
			{
				uint8_t s = sprite[y - 2][x];
				if(s)
					*dd = palette_pc[s];
//				else
//					*dd = palette_pc[text[y][x - 4]];
				dd--;
			}
			dd += config.mix_x_max - config.mix_x_min - SCREEN_WIDTH;
		}
//		if(config.mix_y_min < 2)
//		{
//			dd = d + config.mix_x_max - 1 + SCREEN_WIDTH;
//			for(int16_t y = 2; --y >= config.mix_y_min; )
//			{
//				for (int16_t x = config.mix_x_max; --x >= config.mix_x_min; )
//					*dd-- = palette_pc[text[y][x - 4]];
//				dd += config.mix_x_max - config.mix_x_min - SCREEN_WIDTH;
//			}
//		}
	}


/*
	// Clear entire screen
	if(config.window_displayfullmemory == SETTING_DISPLAYFULLMEMORY_YES_VAL && config.window_clear)
	{
		memset(d, 0, SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(uint32_t));
		config.window_clear = false;
	}
	// Clear text zone
	scrntype_t *dd = d + config.mix_x_max + 3 + (config.mix_y_max - 1) * SCREEN_WIDTH;
	scrntype_t c = palette_pc[vdc1 & 0xf];
	uint16_t yd = SCREEN_WIDTH + config.mix_x_min - config.mix_x_max;
	for(int16_t y = config.mix_y_max; --y >= config.mix_y_min; )
	{
		for (int16_t x = config.mix_x_max; --x >= config.mix_x_min; )
			*dd-- = c;
		dd -= yd;
	}
	// Draw text zone
	draw_text_screen(d);
*/




	// Reset scanline
	if(resetting)
	{
		scrntype_t color = palette_pc[0x1];
		for(int y = resetscanband+30 ; --y >= resetscanband; )
			if((unsigned int)y < SCREEN_HEIGHT)
			{
				scrntype_t* p = &d[y * SCREEN_WIDTH];
				for (int x = SCREEN_WIDTH; --x >= 0; )
				*p++ = color;
			}
	}
	resetscanband -= 2;
	if(resetscanband < -30)
		resetscanband = SCREEN_HEIGHT+60;
	resetting = false;

	// Screen contour
	if(config.window_displayfullmemory == SETTING_DISPLAYFULLMEMORY_YES_VAL)
	{
		switch(config.screen_display)
		{
			case SETTING_DISPLAY_EPOCH_VAL:
				// EPOCH
				// Screen contour RED
				for(int x = 226; --x > 25; )
					d[int(x+23*SCREEN_WIDTH)] = d[int(x+248*SCREEN_WIDTH)] = palette_pc[0x8];
				for(int y = 248; --y > 23; )
					d[int(26+y*SCREEN_WIDTH)] = d[int(225+y*SCREEN_WIDTH)] = palette_pc[0x8];
				break;
			case SETTING_DISPLAY_YENO_VAL:
				// YENO
				// Screen border BLACK
				for(int y = 256; --y >= 0; )
					d[int(215+y*SCREEN_WIDTH)] = d[int(216+y*SCREEN_WIDTH)] = d[int(217+y*SCREEN_WIDTH)] = palette_pc[0x1];
				for(int y = 264; --y > 255; )
					for(int x = 218; --x > 31; )
							d[int(x+y*SCREEN_WIDTH)] = palette_pc[0x1];
				// Screen contour BLUE
				for(int x = 219; --x > 30; )
					d[int(x+264*SCREEN_WIDTH)] = palette_pc[0x2];
				for(int y = 264; --y >= 0; )
					d[int(31+y*SCREEN_WIDTH)] = d[int(218+y*SCREEN_WIDTH)] = palette_pc[0x2];
				break;
			case SETTING_DISPLAY_EMUSCV_VAL:
			default:
				// EMUSCV
				// Screen contour GREEN
				for(int x = 223; --x > 28; )
					d[int(x+27*SCREEN_WIDTH)] = d[int(x+244*SCREEN_WIDTH)] = palette_pc[0x4];
				for(int y = 244; --y > 27; )
					d[int(29+y*SCREEN_WIDTH)] = d[int(222+y*SCREEN_WIDTH)] = palette_pc[0x4];
				break;
		}
	}
}

inline void VDP::draw_text_screen(scrntype_t *d)
{
	uint8_t xmax = (vdc2 & 0xf) * 2;
	uint8_t ymax = vdc2 >> 4;
	uint8_t xs = (vdc0 & 0x40) ? xmax : 0;
	uint8_t xe = (vdc0 & 0x40) ? 32 : xmax;
	uint8_t ys = (vdc0 & 0x80) ? ymax : 0;
	uint8_t ye = (vdc0 & 0x80) ? 16 : ymax;

	uint8_t ct = vdc3 >> 4;
	uint8_t cb = vdc3 & 0xf;
	uint8_t cg = vdc1 >> 4;

	for(int16_t y = config.text_y_max; --y >= config.text_y_min; )
	{
		bool t = (ys <= y && y < ye);
		int32_t y32 = y << 5;
		for(int16_t x = config.text_x_max; --x >= config.text_x_min; )
		{
			if (t && (xs <= x && x < xe)) draw_text(d, x, y, vram1[y32 + x] & 0x7f, ct, cb); // draw text
			else if ((vdc0 & 3) == 1) draw_graph(d, x, y, vram1[y32 + x], cg); // semi graph
			else if ((vdc0 & 3) == 3) draw_block(d, x, y, vram1[y32 + x]); // block
		}
	}
}

inline void VDP::draw_text(scrntype_t *d, int32_t dx, int32_t dy, uint8_t data, uint8_t tcol, uint8_t bcol)
{
/*
	int32_t dx8 = (dx << 3)+2;
	int32_t dy16 = dy << 4;
	const uint8_t *f = font[data];
	uint8_t *p = (uint8_t *)&text[dy16][dx8];
	uint32_t c = (tcol << 8) + bcol;
	if (data)
		for(int32_t l = 8; --l >= 0; f++, p += 320)
		{
			uint32_t pat = *f;
			p[0] = (uint8_t)(c >> ((pat >> (7 - 3)) & 8));
			p[1] = (uint8_t)(c >> ((pat >> (6 - 3)) & 8));
			p[2] = (uint8_t)(c >> ((pat >> (5 - 3)) & 8));
			p[3] = (uint8_t)(c >> ((pat >> (4 - 3)) & 8));
			p[4] = (uint8_t)(c >> ((pat >> (3 - 3)) & 8));
			p[5] = (uint8_t)(c >> ((pat << (3 - 2)) & 8));
			p[6] = (uint8_t)(c >> ((pat << (3 - 1)) & 8));
			p[7] = (uint8_t)(c >> ((pat << (3 - 0)) & 8));
		}
	uint32_t *p2 = (uint32_t *)&text[dy16 + ((data != 0) ? 8 : 0)][dx8];
	c = bcol + ((uint32_t)bcol << 8); c = c + (c << 16);
	for(int32_t l = (data ? 8 : 16); --l >= 0; p2 += 320 >> 2)
		p2[0] = p2[1] = c;
*/


	int32_t dx8 = (dx << 3) + 6;
	int32_t dy16 = dy << 4;
	uint32_t *p = d + dx8 + (dy16 + 15) * SCREEN_WIDTH;
	int32_t col = palette_pc[bcol];
	int8_t l;
	for(l = (data ? 8 : 16); --l >= 0; p -= SCREEN_WIDTH)
		p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] = col;
	if(data)
	{
		const uint8_t *f = font[data];
		uint16_t c32 = (tcol << 8) + bcol;
		p = d + dx8 + dy16 * SCREEN_WIDTH;;
		for(l = 8; --l >= 0; f++, p += SCREEN_WIDTH)
		{
			p[0] = palette_pc[(uint8_t)(c32 >> ((*f >> 4) & 8))];
			p[1] = palette_pc[(uint8_t)(c32 >> ((*f >> 3) & 8))];
			p[2] = palette_pc[(uint8_t)(c32 >> ((*f >> 2) & 8))];
			p[3] = palette_pc[(uint8_t)(c32 >> ((*f >> 1) & 8))];
			p[4] = palette_pc[(uint8_t)(c32 >> ((*f >> 0) & 8))];
			p[5] = palette_pc[(uint8_t)(c32 >> ((*f << 1) & 8))];
			p[6] = palette_pc[(uint8_t)(c32 >> ((*f << 2) & 8))];
			p[7] = palette_pc[(uint8_t)(c32 >> ((*f << 3) & 8))];
		}
	}

}

//inline void VDP::draw_block(int32_t dx, int32_t dy, uint8_t data)
inline void VDP::draw_block(scrntype_t *d, int32_t dx, int32_t dy, uint8_t data)
{
/*
	int32_t dx8 = dx << 3; // Pointer to pixel is aligned to 8.
	int32_t dy16 = dy << 4;
	uint8_t cu = data >> 4;
	uint8_t cl = data & 0xf;

	if(cu != 0)
	{
		uint32_t *p = (uint32_t *)&text[dy16][dx8];
		uint32_t c = cu + ((uint32_t)cu << 8); c = c + (c << 16);
		for(int32_t i = 8; --i >= 0; p += 320 >> 2)
			p[0] = p[1] = c;
	}
	if(cl != 0)
	{
    	uint32_t *p = (uint32_t *)&text[dy16 + 8][dx8];
    	uint32_t c = cl + ((uint32_t)cl << 8); c = c + (c << 16);
    	for(int32_t i = 8; --i >= 0; p += 320 >> 2)
			p[0] = p[1] = c;
	}
*/


	int32_t dx8 = (dx << 3) + 4;
	int32_t dy16 = dy << 4;
	uint8_t cu = data >> 4;
	uint8_t cl = data & 0xF;
	uint32_t *p;
	int8_t l;
	int32_t col;
	if(cu)
	{
		p = d + dx8 + dy16 * SCREEN_WIDTH;
		col = palette_pc[cu];
		for(l = 8; --l >= 0; p += SCREEN_WIDTH)
			p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] = col;
	}
	if(cl)
	{
		p = d + dx8 + (dy16 + 8) * SCREEN_WIDTH;
		col = palette_pc[cl];
    	for(l = 8; --l >= 0; p += SCREEN_WIDTH)
			p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] = col;
	}
}

//inline void VDP::draw_graph(int32_t dx, int32_t dy, uint8_t data, uint8_t col)
inline void VDP::draw_graph(scrntype_t *d, int32_t dx, int32_t dy, uint8_t data, uint8_t col)
{
/*
	uint32_t c = col + (col << 8);
	c = c + (c << 16);
	uint32_t *p = (uint32_t *)&text[dy << 4][dx << 3];
	if(data & 0x80)
		p[0] = p[80] = p[160] = p[240] = c;
	if(data & 0x40)
		p[1] = p[81] = p[161] = p[241] = c;
	if(data & 0x20)
		p[320] = p[400] = p[480] = p[560] = c;
	if(data & 0x10)
		p[321] = p[401] = p[481] = p[561] = c;
	if(data & 0x08)
		p[640] = p[720] = p[800] = p[880] = c;
	if(data & 0x04)
		p[641] = p[721] = p[801] = p[881] = c;
	if(data & 0x02)
		p[960] = p[1040] = p[1120] = p[1200] = c;
	if(data & 0x01)
		p[961] = p[1041] = p[1121] = p[1201] = c;

*/
	
	int8_t l;
	uint32_t *p = d + (dx << 3) + (dy << 4) * SCREEN_WIDTH;
	uint32_t *pp;
	uint32_t c = palette_pc[col];
	uint16_t s = SCREEN_WIDTH << 2;
	uint16_t ss = s;
	if(data & 0x80)
	{
		pp = p;
		for(l = 4; --l >= 0; pp += SCREEN_WIDTH)
			pp[0] = pp[1] = pp[2] =  pp[3] = c;
	}
	if(data & 0x40)
	{
		pp = p + 4;
		for(l = 4; --l >= 0; pp += SCREEN_WIDTH)
			pp[0] = pp[1] = pp[2] =  pp[3] = c;
	}
	if(data & 0x20)
	{
		pp = p + ss;
		for(l = 4; --l >= 0; pp += SCREEN_WIDTH)
			pp[0] = pp[1] = pp[2] =  pp[3] = c;
	}
	if(data & 0x10)
	{
		pp = p + 4 + ss;
		for(l = 4; --l >= 0; pp += SCREEN_WIDTH)
			pp[0] = pp[1] = pp[2] =  pp[3] = c;
	}
	ss = ss << 1;
	if(data & 0x08)
	{
		pp = p + ss;
		for(l = 4; --l >= 0; pp += SCREEN_WIDTH)
			pp[0] = pp[1] = pp[2] =  pp[3] = c;
	}
	if(data & 0x04)
	{
		pp = p + 4 + ss;
		for(l = 4; --l >= 0; pp += SCREEN_WIDTH)
			pp[0] = pp[1] = pp[2] =  pp[3] = c;
	}
	ss = ss + s;
	if(data & 0x02)
	{
		pp = p + ss;
		for(l = 4; --l >= 0; pp += SCREEN_WIDTH)
			pp[0] = pp[1] = pp[2] =  pp[3] = c;
	}
	if(data & 0x01)
	{
		pp = p + 4 + ss;
		for(l = 4; --l >= 0; pp += SCREEN_WIDTH)
			pp[0] = pp[1] = pp[2] =  pp[3] = c;
	}
}

inline void VDP::draw_sprite_screen()
{
	// Patch for Kung-Fu Road
	int32_t nbsprite = ((vdc0 & 0xf7) == 0x17 && (vdc2 & 0xef) == 0x4f) ? 64 : 128;

	for(int32_t index = 0; index < nbsprite; index++)
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

		uint8_t col1 = atb1 & 0x0f;
		if(!col1)
			continue;

		int32_t dx = atb2 & 0xfe;
		int32_t dy = atb0 & 0xfe;
		if((index & 0xf0) && (dx == 0 || dy == 0))
			continue;

		bool conx = ((atb2 & 1) != 0);
		bool cony = ((atb0 & 1) != 0);

		int32_t sx = 0;
		int32_t ex = 4;
		int32_t sy = atb1 >> 4;
		int32_t ey = 8;

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
			int32_t no1 = atb3;
			if(conx || cony)	// Normal 2 colors sprite
			{
				int32_t no2;
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
// R&D
if(!col2)	// New color pair?
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

// R&D
//if((vdc0 & 0xf7) != 112 || (vdc2 & 0xef) != 34)
//	int i = 0;
//if(vdc0 != 120 || vdc1 != 17 || vdc2 != 50 || vdc3 != 241)
//	int i = 0;

				int32_t no2 = 11;// or 9... but why?
				uint8_t col2 = color_pair_xy[col1];

// R&D
if(!col2)	// // New color pair?
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
			int32_t no1 = atb3;

			draw_sprite(dx, dy, sx, ex, sy, ey, no1, col1);
			if(cony)
			{
				int32_t no2 = atb3 | 1;
				draw_sprite(dx, dy + 16, sx, ex, sy - 8, 8, no2, col1);
			}
			if(conx)
			{
				int32_t no3 = atb3 | 8;
				draw_sprite(dx + 16, dy, 0, 4, sy, ey, no3, col1);
			}
			if(conx && cony)
			{
				int32_t no4 = atb3 | 9;
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

inline void VDP::draw_sprite(int32_t dx, int32_t dy, int32_t sx, int32_t ex, int32_t sy, int32_t ey, int32_t no, uint8_t col)
{
//	// color #0 is transparent
//	if(!col)
//		return;

//dx = 311; dy = 200;
//	if(dx < 0 || dx > 311 || dy < 0 || dy > 311)	// Full
//	if(dx < 15 || dx > 221 || dy < 11 || dy > 241)	// EmuSCV
//	if(dx < 12 || dx > 270 || dy < 7 || dy > 245)	// Epoch
//	if(dx < 17 || dx > 214 || dy < 0 || dy > 253)	// Yeno
	if(dx < config.sprite_x_min || dx > config.sprite_x_max || dy < config.sprite_y_min || dy > config.sprite_y_max)
		return;

	if(sy < 0)
		sy = 0;

	int32_t no32 = no << 5;
 	for(int32_t y = ey; --y >= sy; )
	 {
 		int32_t y2u = (y << 1) + dy;
		int32_t y2l = (y << 1) + dy + 1;
		int32_t y4 = (y << 2) + no32;
		for(int32_t x = ex; --x >= sx; )
		{
			int32_t x4 = (dx + (x << 2)) & 0xFF;
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
		}
 	}
/*
	// R&D
	// Try to patch for Lupin III
	if(vdc0 == 83 && vdc2 == 64 && no == 43 && col == 0x9)
	{
		no = 42;
		col = 0xf;
		draw_sprite(dx, dy, sx, ex, sy, ey, no, col);
	}
*/
}

#define VDP_STATE_ID	901

void VDP::save_state(STATE* state, bool max_size)
{
	state->SetValue((uint16_t)VDP_STATE_ID);
	state->SetValue(this_device_id);
	state->SetValue(vdc0);
	state->SetValue(vdc1);
	state->SetValue(vdc2);
	state->SetValue(vdc3);
}

bool VDP::load_state(STATE* state)
{
	if(!state->CheckValue((uint16_t)VDP_STATE_ID))
		return false;
	if(!state->CheckValue(this_device_id))
		return false;
	state->GetValue(vdc0);
	state->GetValue(vdc1);
	state->GetValue(vdc2);
	state->GetValue(vdc3);
	
	return true;
}
