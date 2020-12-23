/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	[ virtual machine ]
*/

#ifndef _EMUSCV_INC_VM_SCV_SCV_H_
#define _EMUSCV_INC_VM_SCV_SCV_H_

#define DEVICE_NAME		"EPOCH/YENO Super Cassette Vision"
#define CONFIG_NAME 	"scv"

// Device informations for virtual machine
#define LINES_PER_FRAME				262
#define CPU_CLOCKS					4000000
#define FRAMES_PER_SEC				60

// Mix buffer size
#define SCREEN_WIDTH				324
#define SCREEN_HEIGHT				322

// EPOCH console:	198x224
// YENO Console:	186x248 (including a black band of 3 pixels on right side and a black band of 8 pixels on the bottom)
// eSCV:			192x222	(-> 288x222)
// emuSCV:			192x216 (-> 864x648)
#define DRAW_X_EMUSCV				30
#define DRAW_Y_EMUSCV				28
#define DRAW_WIDTH_EMUSCV			192
#define DRAW_HEIGHT_EMUSCV			216
#define DRAW_X_EPOCH				27
#define DRAW_Y_EPOCH				24
#define DRAW_WIDTH_EPOCH			198
#define DRAW_HEIGHT_EPOCH			224
#define DRAW_X_YENO					32
#define DRAW_Y_YENO					0
#define DRAW_WIDTH_YENO				186
#define DRAW_HEIGHT_YENO			248+16
#define DRAW_X_FULL					0
#define DRAW_Y_FULL					0
#define DRAW_WIDTH_FULL				SCREEN_WIDTH
#define DRAW_HEIGHT_FULL			SCREEN_HEIGHT

// Final window
#define WINDOW_COLORS				256*256*256	// 16 Millions of colors
#define WINDOW_MODE_BASE			1
#define WINDOW_ZOOM_MAX				1

#define WINDOW_WIDTH_EMUSCV			864	// Pixel perfect
#define WINDOW_HEIGHT_EMUSCV		648	// Pixel perfect
#define WINDOW_ASPECT_RATIO_EMUSCV	864/648
#define WINDOW_WIDTH_EPOCH			864
#define WINDOW_HEIGHT_EPOCH			648
#define WINDOW_ASPECT_RATIO_EPOCH	864/648
#define WINDOW_WIDTH_YENO			864
#define WINDOW_HEIGHT_YENO			648
#define WINDOW_ASPECT_RATIO_YENO	864/648
#define WINDOW_WIDTH_FULL			864
#define WINDOW_HEIGHT_FULL			648
#define WINDOW_ASPECT_RATIO_FULL	864/648

#define WINDOW_FPS_EPOCH			60
#define WINDOW_FPS_YENO				50

#define NUM_JOYSTICK		2
#define NUM_JOYSTICK_AXE	0
#define NUM_JOYSTICK_BUTTON	2

// memory wait
//#define UPD7801_MEMORY_WAIT

// device informations for win32
#define USE_CART			1
//#define USE_SOUND_VOLUME	2
#define USE_JOYSTICK		2
//#define USE_DEBUGGER		1
#define USE_STATE			1
//#define USE_SCREEN_ROTATE	1

#define SETTING_CONSOLE_KEY				"emuscv_console"
#define SETTING_CONSOLE_VAL				"CONSOLE"
#define SETTING_CONSOLE_AUTO_KEY		"AUTO"
#define SETTING_CONSOLE_AUTO_VAL		0
#define SETTING_CONSOLE_EPOCH_KEY		"EPOCH"
#define SETTING_CONSOLE_EPOCH_VAL		1
#define SETTING_CONSOLE_YENO_KEY		"YENO"
#define SETTING_CONSOLE_YENO_VAL		2
#define SETTING_CONSOLE_EPOCHLADY_KEY	"EPOCHLADY"
#define SETTING_CONSOLE_EPOCHLADY_VAL	3

#define SETTING_DISPLAY_KEY				"emuscv_display"
#define SETTING_DISPLAY_VAL				"DISPLAY"
#define SETTING_DISPLAY_AUTO_KEY		"AUTO"
#define SETTING_DISPLAY_AUTO_VAL		0
#define SETTING_DISPLAY_EMUSCV_KEY		"EMUSCV"
#define SETTING_DISPLAY_EMUSCV_VAL		1
#define SETTING_DISPLAY_EPOCH_KEY		"EPOCH"
#define SETTING_DISPLAY_EPOCH_VAL		2
#define SETTING_DISPLAY_YENO_KEY		"YENO"
#define SETTING_DISPLAY_YENO_VAL		3

#define SETTING_DISPLAYFPS_KEY			"emuscv_displaypfs"
#define SETTING_DISPLAYFPS_VAL			"DISPLAY FPS"
#define SETTING_DISPLAYFPS_AUTO_KEY		"AUTO"
#define SETTING_DISPLAYFPS_AUTO_VAL		0
#define SETTING_DISPLAYFPS_EPOCH60_KEY	"EPOCH 60 PFS"
#define SETTING_DISPLAYFPS_EPOCH60_VAL	1
#define SETTING_DISPLAYFPS_YENO50_KEY	"YENO 50 PFS"
#define SETTING_DISPLAYFPS_YENO50_VAL	2

#define SETTING_DISPLAYFULL_KEY			"emuscv_displayfull"
#define SETTING_DISPLAYFULL_VAL			"DISPLAY FULL"
#define SETTING_DISPLAYFULL_NO_KEY		"NO"
#define SETTING_DISPLAYFULL_NO_VAL		0
#define SETTING_DISPLAYFULL_YES_KEY		"YES"
#define SETTING_DISPLAYFULL_YES_VAL		1

#define SETTING_DISPLAYINPUTS_KEY		"emuscv_displayinputs"
#define SETTING_DISPLAYINPUTS_VAL		"DISPLAY INPUTS"
#define SETTING_DISPLAYINPUTS_NO_KEY	"NO"
#define SETTING_DISPLAYINPUTS_NO_VAL	0
#define SETTING_DISPLAYINPUTS_YES_KEY	"YES"
#define SETTING_DISPLAYINPUTS_YES_VAL	1

#define SETTING_LANGAGE_KEY				"emuscv_langage"
#define SETTING_LANGAGE_VAL				"LANGAGE"
#define SETTING_LANGAGE_AUTO_KEY		"AUTO"
#define SETTING_LANGAGE_AUTO_VAL		0
#define SETTING_LANGAGE_JP_KEY			"JP"
#define SETTING_LANGAGE_JP_VAL			1
#define SETTING_LANGAGE_EN_KEY			"EN"
#define SETTING_LANGAGE_EN_VAL			2
#define SETTING_LANGAGE_FR_KEY			"FR"
#define SETTING_LANGAGE_FR_VAL			3

#define SETTING_CHECKBIOS_KEY			"emuscv_checkbios"
#define SETTING_CHECKBIOS_VAL			"CHECK BIOS"
#define SETTING_CHECKBIOS_NO_KEY		"NO"
#define SETTING_CHECKBIOS_NO_VAL		0
#define SETTING_CHECKBIOS_YES_KEY		"YES"
#define SETTING_CHECKBIOS_YES_VAL		1


#include "../../common.h"
#include "../../fileio.h"
#include "../vm_template.h"

class UPD7801;
class IO;
class MEMORY;
class SOUND;
class VDP;

#ifdef USE_SOUND_VOLUME
static const _TCHAR *sound_device_caption[] = { _T("PSG"), _T("PCM"), };
#endif

class VM : public VM_TEMPLATE
{
protected:
//	EMU* emu;

	// devices
	EVENT* event;

	UPD7801* cpu;

	IO* io;
	MEMORY* memory;
	SOUND* sound;
	VDP* vdp;

public:
	// ----------------------------------------
	// initialize
	// ----------------------------------------

	VM(EMU* parent_emu);
	~VM();

	// ----------------------------------------
	// for emulation class
	// ----------------------------------------

	// drive virtual machine
	void reset();
	void run();
	EVENT* get_event(){ return event;};

#ifdef USE_DEBUGGER
	// debugger
	DEVICE *get_cpu(int index);
#endif

	// draw screen
	void draw_screen();

	// sound generation
	void initialize_sound(int rate, int samples);
	uint16_t* create_sound(int* extra_frames);
	int get_sound_buffer_ptr();
#ifdef USE_SOUND_VOLUME
	void set_sound_device_volume(int ch, int decibel_l, int decibel_r);
#endif

	// user interface
	void open_cart(int drv, const _TCHAR* file_path);
	void close_cart(int drv);
	bool is_cart_inserted(int drv);
	bool is_frame_skippable();

	void update_config();
	bool process_state(FILEIO* state_fio, bool loading);

	// bios
	bool is_bios_found();
	bool is_bios_present();
	bool is_bios_ok();

	// ----------------------------------------
	// for each device
	// ----------------------------------------

	// devices
	DEVICE* get_device(int id);
//	DEVICE* dummy;
//	DEVICE* first_device;
//	DEVICE* last_device;
};

#endif	// _EMUSCV_INC_VM_SCV_SCV_H_
