/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ EPOCH TV-1 ]
*/

#ifndef _EMUSCV_INC_VM_SCV_VDP_H_
#define _EMUSCV_INC_VM_SCV_VDP_H_

#include "../device.h"
#include "vdp_colors.h"
#include "vdp_characters.h"

class VDP : public DEVICE
{
private:
	DEVICE* d_cpu;

	uint8_t* vram0;
	uint8_t* vram1;
	uint8_t* font_ptr;
	uint8_t text[320][320];
	uint8_t sprite[320][320];
	uint8_t mix[SCREEN_HEIGHT][SCREEN_WIDTH];
	uint8_t vdc0;
	uint8_t vdc1;
	uint8_t vdc2;
	uint8_t vdc3;
	bool resetting;
	int resetscanband;

	inline void draw_text_screen();
	inline void draw_text(int dx, int dy, uint8_t data, uint8_t tcol, uint8_t bcol);
	inline void draw_block(int dx, int dy, uint8_t data);
	inline void draw_graph(int dx, int dy, uint8_t data, uint8_t col);

	inline void draw_sprite_screen();
	inline void draw_sprite(int dx, int dy, int sx, int ex, int sy, int ey, int no, uint8_t col);

public:
	VDP(VM_TEMPLATE* parent_vm, EMU* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("VDP"));
	}
	~VDP() {}

	// common functions
	void initialize();
	void reset();
	void event_vline(int v, int clock);
	bool process_state(FILEIO* state_fio, bool loading);

	// unique functions
	void set_context_cpu(DEVICE* device)
	{
		d_cpu = device;
	}
	void set_font_ptr(uint8_t* ptr)
	{
		font_ptr = ptr;
	}
	void set_vram_ptr(uint8_t* ptr)
	{
		vram0 = ptr; vram1 = ptr + 0x1000;
	}
	void draw_screen();
};

#endif	// _EMUSCV_INC_VM_SCV_VDP_H_
