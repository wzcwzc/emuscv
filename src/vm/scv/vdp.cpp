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

	// Get screen pointer
	scrntype_t* d = emu->get_screen_ptr();

	// Clear screen
	scrntype_t *d1;
	if(config.window_displayfullmemory == SETTING_DISPLAYFULLMEMORY_YES_VAL)
	{
		// Clear sprite zone
		uint16_t xmin = 0;
		uint16_t xmax = 3;
		uint16_t ymin = 0;
		uint16_t ymax = SCREEN_HEIGHT - 1;
		uint16_t ys = SCREEN_WIDTH - xmax - 1 + xmin;
		scrntype_t c = palette_pc[0x1];
		d1 = d + xmax + ymax * SCREEN_WIDTH;
		for(int16_t x, y = ymax + 1; --y >= ymin; )
		{
			for (x = xmax + 1; --x >= xmin; )
				*d1-- = c;
			d1 -= ys;
		}
		xmin = 4;
		xmax = SCREEN_WIDTH - 1;
		ymin = SCREEN_HEIGHT - 2;
		ys = SCREEN_WIDTH - xmax - 1 + xmin;
		d1 = d + xmax + ymax * SCREEN_WIDTH;
		for(int16_t x, y = ymax + 1; --y >= ymin; )
		{
			for (x = xmax + 1; --x >= xmin; )
				*d1-- = c;
			d1 -= ys;
		}
		// Clear text zone
		ymin = 0;
		ymax = SCREEN_HEIGHT - 3;
		c = palette_pc[vdc1 & 0xf];
		ys = SCREEN_WIDTH - xmax - 1 + xmin;
		d1 = d + xmax + ymax * SCREEN_WIDTH;
		for(int16_t x, y = ymax + 1; --y >= ymin; )
		{
			for (x = xmax + 1; --x >= xmin; )
				*d1-- = c;
			d1 -= ys;
		}

		draw_screen(d);

		// Screen contour
		scrntype_t *d2;
		switch(config.screen_display)
		{
			case SETTING_DISPLAY_EPOCH_VAL:
				// EPOCH
				// Screen contour RED
				d1 = d + 225 + 23 * SCREEN_WIDTH;
				d2 = d + 225 + 248 * SCREEN_WIDTH;
				c = palette_pc[0x8];
				for(int16_t x = 226; --x > 25; )
					*d1-- = *d2-- = c;
				d1 = d + 26 + 247 * SCREEN_WIDTH;
				d2 = d + 225 + 247 * SCREEN_WIDTH;
				for(int16_t y = 248; --y > 23; d1 -= SCREEN_WIDTH, d2 -= SCREEN_WIDTH)
					*d1 = *d2 = c;
				break;
			case SETTING_DISPLAY_YENO_VAL:
				// YENO
				// Screen border BLACK
				d1 = d + 215 + 255 * SCREEN_WIDTH;
				c = palette_pc[0x1];
				for(int16_t y = 256; --y >= 0; d1 -= SCREEN_WIDTH)
					d1[0] = d1[1] = d1[2] = c;
				d1 = d + 217 + 263 * SCREEN_WIDTH;
				for(int16_t x, y = 264; --y > 255; )
				{
					for(x = 218; --x >= 31; )
						*d1-- = c;
					d1 -= SCREEN_WIDTH - 187;
				}
				// Screen contour BLUE
				d1 = d + 218 + 264 * SCREEN_WIDTH;
				c = palette_pc[0x2];
				for(int x = 219; --x >= 31; )
					*d1-- = c;
				d1 = d + 31 + 263 * SCREEN_WIDTH;
				d2 = d + 218 + 263 * SCREEN_WIDTH;
				for(int16_t y = 264; --y >= 0; d1 -= SCREEN_WIDTH, d2 -= SCREEN_WIDTH)
					*d1 = *d2 = c;
				break;
			case SETTING_DISPLAY_EMUSCV_VAL:
			default:
				// EMUSCV
				// Screen contour GREEN
				d1 = d + 222 + 27 * SCREEN_WIDTH;
				d2 = d + 222 + 244 * SCREEN_WIDTH;
				c = palette_pc[0x4];
				for(int x = 223; --x > 28; )
					*d1-- = *d2-- = c;
				d1 = d + 29 + 243 * SCREEN_WIDTH;
				d2 = d + 222 + 243 * SCREEN_WIDTH;
				for(int16_t y = 244; --y > 27; d1 -= SCREEN_WIDTH, d2 -= SCREEN_WIDTH)
					*d1 = *d2 = c;
				break;
		}
	}
	else
	{
		// Clear text zone
		d1 = d + config.mix_x_max - 1 + (config.mix_y_max - 1) * SCREEN_WIDTH;
		scrntype_t c = palette_pc[vdc1 & 0xf];
		uint16_t ys = SCREEN_WIDTH + config.mix_x_min - config.mix_x_max;
		for(int16_t x, y = config.mix_y_max; --y >= config.mix_y_min; )
		{
			for (x = config.mix_x_max; --x >= config.mix_x_min; )
				*d1-- = c;
			d1 -= ys;
		}

		draw_screen(d);
	}
}

inline void VDP::draw_screen(scrntype_t *d)
{
	// draw text screen
	draw_text_screen(d);

	// draw sprite screen
	memset(sprite, 0, sizeof(sprite));
	if(vdc0 & 0x10)
		draw_sprite_screen(d);

	// Reset scanline
	if(resetting)
	{
		scrntype_t c = palette_pc[0x1];
		uint16_t ymin = resetscanband;
		uint16_t ymax = resetscanband + 30;
		if(ymin < 0)
			ymin = 0;
		if(ymax > SCREEN_HEIGHT)
			ymax = SCREEN_HEIGHT;
		if(ymin < ymax)
		{
			scrntype_t *p = d + ymax * SCREEN_WIDTH;
			for(int16_t x, y = ymax ; --y >= ymin; )
				for (x = SCREEN_WIDTH; --x >= 0; )
					*p-- = c;
		}
		resetscanband -= 3;
		if(resetscanband + 30 < 0)
			resetscanband = SCREEN_HEIGHT + 30;
		resetting = false;
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
	uint8_t cb = vdc3 & 0xF;
	uint8_t cg = vdc1 >> 4;

	for(int16_t y = config.text_y_max; --y >= config.text_y_min; )
	{
		bool t = (ys <= y && y < ye);
		int32_t y32 = y << 5;
		for(int16_t x = config.text_x_max; --x >= config.text_x_min; )
		{
			uint32_t data = vram1[y32 + x];
			if (t && (xs <= x && x < xe)) draw_text(d, x, y, data & 0x7f, ct, cb); // draw text
			else if ((vdc0 & 3) == 1) draw_graph(d, x, y, data, cg); // semi graph
			else if ((vdc0 & 3) == 3) draw_block(d, x, y, data); // block
		}
	}
}

inline void VDP::draw_text(scrntype_t *d, int32_t dx, int32_t dy, uint8_t data, uint8_t tcol, uint8_t bcol)
{
	int32_t dx8 = (dx << 3) + 6;
	int32_t dy16 = dy << 4;
	scrntype_t *p = d + dx8 + (dy16 + 15) * SCREEN_WIDTH;
	scrntype_t col = palette_pc[bcol];
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

inline void VDP::draw_block(scrntype_t *d, int32_t dx, int32_t dy, uint8_t data)
{
	int32_t dx8 = (dx << 3) + 4;
	int32_t dy16 = dy << 4;
	uint8_t cu = data >> 4;
	uint8_t cl = data & 0xF;
	scrntype_t *p;
	scrntype_t col;
	int8_t l;
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

inline void VDP::draw_graph(scrntype_t *d, int32_t dx, int32_t dy, uint8_t data, uint8_t col)
{
	int8_t l;
	scrntype_t *p = d + (dx << 3) + (dy << 4) * SCREEN_WIDTH;
	scrntype_t *pp;
	scrntype_t c = palette_pc[col];
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

inline void VDP::draw_sprite_screen(scrntype_t *d)
{
	// Patch for Kung-Fu Road
	int32_t nbsprite = ((vdc0 & 0xf7) == 0x17 && (vdc2 & 0xef) == 0x4f) ? 64 : 128;

	int16_t index2, dx, dy;
	uint8_t atb0, atb1, atb2, atb3, col1, col2;
	bool conx, cony;
	int16_t sx, sy, ex, ey;
	int8_t no1, no2, no3, no4;
	for(int16_t index = -1; ++index < nbsprite; )
	{
		index2 = index << 2;

		atb1 = vram1[0x201 + index2];	// 11110000: clip y for top
										// 00001111: color
		col1 = atb1 & 0x0f;
		if(!col1)	// Transparent sprite
			continue;
		atb0 = vram1[0x200 + index2];	// 11111110: position y (0 not displayed)
										// 00000001: double height
		atb2 = vram1[0x202 + index2];	// 11111110: position x
										// 00000001: double width
		dx = atb2 & 0xfe;
		dy = atb0 & 0xfe;
		if((index & 0xf0) && (dx == 0 || dy == 0))
			continue;

		atb3 = vram1[0x203 + index2];	// 10000000: half width
										// 01000000: half height
										// 01111111: sprite leaf

		conx = ((atb2 & 1) != 0);
		cony = ((atb0 & 1) != 0);

		sx = 0;
		ex = 4;
		sy = atb1 >> 4;
		ey = 8;

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
		draw_sprite(d, dx, dy, sx, ex, sy, ey, no1, col1);
		if(conx && cony)
		{
			int no3 = atb3 | 9;
			int no3x = atb3 ^ 9;
			if(no3 != no3x)
				int i = 0;
//			int col2 = ((vdc0 & 0x20) ? color_pair_xy[col1] : col1);
//			draw_sprite(d, dx + 16, dy+16, 0, 4, sy, ey, no3, col1);
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
//			draw_sprite(d, dx + 16, dy, 0, 4, sy, ey, no3, col1);
		}
*/

 		if((index & 0x20) && (vdc0 & 0x20))
 		{
			// 2 colors sprite
			no1 = atb3;
			if(conx || cony)	// Normal 2 colors sprite
			{
				no2;
				col2;
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

				draw_sprite(d, dx, dy, sx, ex, sy, ey, no1, col1);
				draw_sprite(d, dx, dy, sx, ex, sy, ey, no2, col2);
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

				no2 = 11;// or 9... but why?
				col2 = color_pair_xy[col1];

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

				draw_sprite(d, dx, dy, sx, ex, sy, ey, no1, col2);
				draw_sprite(d, dx, dy, sx, ex, sy, ey, no2, col1);
			}
			else	// Simple 2 colors sprite
			{
				draw_sprite(d, dx, dy, sx, ex, sy, ey, no1, col1);
			}
 		}
 		else
 		{
			// mono color sprite
			no1 = atb3;

			draw_sprite(d, dx, dy, sx, ex, sy, ey, no1, col1);
			if(cony)
			{
				no2 = atb3 | 1;
				draw_sprite(d, dx, dy + 16, sx, ex, sy - 8, 8, no2, col1);
			}
			if(conx)
			{
				no3 = atb3 | 8;
				draw_sprite(d, dx + 16, dy, 0, 4, sy, ey, no3, col1);
			}
			if(conx && cony)
			{
				no4 = atb3 | 9;
				draw_sprite(d, dx + 16, dy + 16, 0, 4, sy - 8, 8, no4, col1);
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
		draw_sprite(d, xx, yy, 0, 4, 0, 8, nn, cc);
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

inline void VDP::draw_sprite(scrntype_t *d, int32_t dx, int32_t dy, int32_t sx, int32_t ex, int32_t sy, int32_t ey, int32_t no, uint8_t col)
{
	if(dx < config.sprite_x_min || dx > config.sprite_x_max || dy < config.sprite_y_min || dy > config.sprite_y_max)
		return;

	sy <<= 1;
	if(sy < 0)
		sy = 0;

	scrntype_t no32 = no << 5, c = palette_pc[col], y2u, y2l, *x4, y4, *du, *dl;
	uint8_t p;

 	for(int16_t x, y2 = ey << 1 ; --y2, --y2 >= sy; )
	 {
		y2u = (y2 + dy + 2) * SCREEN_WIDTH;
		y2l = (y2 + dy + 3) * SCREEN_WIDTH;
		y4 = (y2 << 1) + no32;
		for(x = ex; --x >= sx; )
		{
			x4 = d + ((dx + (x << 2)) & 0xFF);
			du = x4 + y2u;
			dl = x4 + y2l;
			p = vram0[y4 + x];
			if(p & 0x80)
				du[0] = c;
			if(p & 0x40)
				du[1] = c;
			if(p & 0x20)
				du[2] = c;
			if(p & 0x10)
				du[3] = c;
			if(p & 0x08)
				dl[0] = c;
			if(p & 0x04)
				dl[1] = c;
			if(p & 0x02)
				dl[2] = c;
			if(p & 0x01)
				dl[3] = c;
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
