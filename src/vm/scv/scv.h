/*
	EPOCH Super Cassette Vision Emulator 'eSCV'

	Author : Takeda.Toshiya
	Date   : 2006.08.21 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

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
#define FRAMES_PER_SEC_MAX			FRAMES_PER_SEC
#define FRAMES_PER_SEC_MIN			50
#define AUDIO_SAMPLING_RATE			44100

// Mix buffer size
#define SCREEN_WIDTH				324
#define SCREEN_HEIGHT				322

// EPOCH console:	198x224
// YENO Console:	186x248 (including a black band of 3 pixels on right side and a black band of 8 pixels on the bottom)
// eSCV:			192x222	(-> 288x222)
// emuSCV:			192x216 (-> 864x648)

// Square:      192x216 / 324x432 / 576x648
// Rectangular: 288x216 / 576x432 / 864x648

#define DRAW_X_EMUSCV				30
#define DRAW_Y_EMUSCV				28
#define DRAW_WIDTH_EMUSCV			192
#define DRAW_HEIGHT_EMUSCV			216
#define DRAW_X_EPOCH				27
#define DRAW_Y_EPOCH				23
#define DRAW_WIDTH_EPOCH			198
#define DRAW_HEIGHT_EPOCH			225
#define DRAW_X_YENO					32
#define DRAW_Y_YENO					0
#define DRAW_WIDTH_YENO				186
#define DRAW_HEIGHT_YENO			264
#define DRAW_X_FULL					0
#define DRAW_Y_FULL					0
#define DRAW_WIDTH_FULL				SCREEN_WIDTH
#define DRAW_HEIGHT_FULL			SCREEN_HEIGHT

// Final window
#define WINDOW_COLORS				256*256*256	// 16 Millions of colors
#define WINDOW_MODE_BASE			1
#define WINDOW_ZOOM_MAX				1

#define WINDOW_FPS_EPOCH			FRAMES_PER_SEC_MAX
#define WINDOW_FPS_YENO				FRAMES_PER_SEC_MIN

#define NUM_JOYSTICK				2
#define NUM_JOYSTICK_AXE			0
#define NUM_JOYSTICK_BUTTON			2

// memory wait
//#define UPD7801_MEMORY_WAIT

// device informations for win32
#define USE_CART					1
#define USE_SOUND_VOLUME			2
#define USE_JOYSTICK				2
//#define USE_DEBUGGER				1
#define USE_STATE					1
//#define USE_SCREEN_ROTATE			1

#define SETTING_CONSOLE_KEY					"emuscv_console"
#define SETTING_CONSOLE_VAL					"CONSOLE"
#define SETTING_CONSOLE_AUTO_KEY			"AUTO"
#define SETTING_CONSOLE_AUTO_VAL			0
#define SETTING_CONSOLE_EPOCH_KEY			"EPOCH"
#define SETTING_CONSOLE_EPOCH_VAL			1
#define SETTING_CONSOLE_YENO_KEY			"YENO"
#define SETTING_CONSOLE_YENO_VAL			2
#define SETTING_CONSOLE_EPOCHLADY_KEY		"EPOCHLADY"
#define SETTING_CONSOLE_EPOCHLADY_VAL		3

#define SETTING_DISPLAY_KEY					"emuscv_display"
#define SETTING_DISPLAY_VAL					"DISPLAY"
#define SETTING_DISPLAY_AUTO_KEY			"AUTO"
#define SETTING_DISPLAY_AUTO_VAL			0
#define SETTING_DISPLAY_EMUSCV_KEY			"EMUSCV"
#define SETTING_DISPLAY_EMUSCV_VAL			1
#define SETTING_DISPLAY_EPOCH_KEY			"EPOCH"
#define SETTING_DISPLAY_EPOCH_VAL			2
#define SETTING_DISPLAY_YENO_KEY			"YENO"
#define SETTING_DISPLAY_YENO_VAL			3

#define SETTING_PIXELASPECT_KEY				"emuscv_pixelaspect"
#define SETTING_PIXELASPECT_VAL				"PIXELASPECT"
#define SETTING_PIXELASPECT_AUTO_KEY		"AUTO"
#define SETTING_PIXELASPECT_AUTO_VAL		0
#define SETTING_PIXELASPECT_RECTANGULAR_KEY	"RECTANGULAR"
#define SETTING_PIXELASPECT_RECTANGULAR_VAL	1
#define SETTING_PIXELASPECT_SQUARE_KEY		"SQUARE"
#define SETTING_PIXELASPECT_SQUARE_VAL		2

#define SETTING_RESOLUTION_KEY				"emuscv_resolution"
#define SETTING_RESOLUTION_VAL				"RESOLUTION"
#define SETTING_RESOLUTION_AUTO_KEY			"AUTO"
#define SETTING_RESOLUTION_AUTO_VAL			0
#define SETTING_RESOLUTION_LOW_KEY			"LOW"
#define SETTING_RESOLUTION_LOW_VAL			1
#define SETTING_RESOLUTION_MEDIUM_KEY		"MEDIUM"
#define SETTING_RESOLUTION_MEDIUM_VAL		2
#define SETTING_RESOLUTION_HIGH_KEY			"HIGH"
#define SETTING_RESOLUTION_HIGH_VAL			3

#define SETTING_PALETTE_KEY					"emuscv_palette"
#define SETTING_PALETTE_VAL					"PALETTE"
#define SETTING_PALETTE_AUTO_KEY			"AUTO"
#define SETTING_PALETTE_AUTO_VAL			0
#define SETTING_PALETTE_STANDARD_KEY		"STANDARD"
#define SETTING_PALETTE_STANDARD_VAL		1
#define SETTING_PALETTE_OLDNTSC_KEY			"OLDNTSC"
#define SETTING_PALETTE_OLDNTSC_VAL			2

#define SETTING_FPS_KEY						"emuscv_fps"
#define SETTING_FPS_VAL						"FPS"
#define SETTING_FPS_AUTO_KEY				"AUTO"
#define SETTING_FPS_AUTO_VAL				0
#define SETTING_FPS_EPOCH60_KEY				"EPOCH60"
#define SETTING_FPS_EPOCH60_VAL				1
#define SETTING_FPS_YENO50_KEY				"YENO50"
#define SETTING_FPS_YENO50_VAL				2

#define SETTING_DISPLAYFULLMEMORY_KEY		"emuscv_displayfullmemory"
#define SETTING_DISPLAYFULLMEMORY_VAL		"DISPLAYFULLMEMORY"
#define SETTING_DISPLAYFULLMEMORY_AUTO_KEY	"AUTO"
#define SETTING_DISPLAYFULLMEMORY_AUTO_VAL	0
#define SETTING_DISPLAYFULLMEMORY_YES_KEY	"YES"
#define SETTING_DISPLAYFULLMEMORY_YES_VAL	1
#define SETTING_DISPLAYFULLMEMORY_NO_KEY	"NO"
#define SETTING_DISPLAYFULLMEMORY_NO_VAL	2

#define SETTING_DISPLAYINPUTS_KEY			"emuscv_displayinputs"
#define SETTING_DISPLAYINPUTS_VAL			"DISPLAYINPUTS"
#define SETTING_DISPLAYINPUTS_AUTO_KEY		"AUTO"
#define SETTING_DISPLAYINPUTS_AUTO_VAL		0
#define SETTING_DISPLAYINPUTS_YES_KEY		"YES"
#define SETTING_DISPLAYINPUTS_YES_VAL		1
#define SETTING_DISPLAYINPUTS_NO_KEY		"NO"
#define SETTING_DISPLAYINPUTS_NO_VAL		2

#define SETTING_LANGAGE_KEY					"emuscv_langage"
#define SETTING_LANGAGE_VAL					"Langage"
#define SETTING_LANGAGE_AUTO_KEY			"AUTO"
#define SETTING_LANGAGE_AUTO_VAL			0
#define SETTING_LANGAGE_JP_KEY				"JP"
#define SETTING_LANGAGE_JP_VAL				1
#define SETTING_LANGAGE_FR_KEY				"FR"
#define SETTING_LANGAGE_EN_VAL				2
#define SETTING_LANGAGE_EN_KEY				"EN"
#define SETTING_LANGAGE_FR_VAL				3

#define SETTING_CHECKBIOS_KEY				"emuscv_checkbios"
#define SETTING_CHECKBIOS_VAL				"CHECKBIOS"
#define SETTING_CHECKBIOS_AUTO_KEY			"AUTO"
#define SETTING_CHECKBIOS_AUTO_VAL			0
#define SETTING_CHECKBIOS_YES_KEY			"YES"
#define SETTING_CHECKBIOS_YES_VAL			1
#define SETTING_CHECKBIOS_NO_KEY			"NO"
#define SETTING_CHECKBIOS_NO_VAL			2


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
	void reset_palette();
	void draw_screen();

	// sound generation
	void initialize_sound(int rate);//, int samples);
	int16_t* create_sound(int* extra_frames);
#ifdef USE_SOUND_VOLUME
	void set_sound_device_volume(int ch, int decibel_l, int decibel_r);
#endif
	void reset_sound();

	// user interface
	void open_cart(int drv, const _TCHAR* file_path);
	void close_cart(int drv);
	bool is_cart_inserted(int drv);

	void update_config();
	void save_state(STATE* state);
	bool load_state(STATE* state);

	// bios
	bool is_bios_found();
	bool is_bios_present();
	bool is_bios_ok();

	// ----------------------------------------
	// for each device
	// ----------------------------------------

	// devices
	DEVICE* get_device(int id);
};

#endif	// _EMUSCV_INC_VM_SCV_SCV_H_
