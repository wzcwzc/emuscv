/*
	Libretro-EmuSCV

	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ Libretro core ]
*/

#include "emuscv.h"

#include "config.h"
#include "emu.h"
#include "vm/vm.h"
#include "vm/scv/vdp_colors.h"

// Embedded binary resources
#include "res/binary.emuscv32x32xrgba8888.data.h"
#include "res/binary.emuscv64x64xrgba8888.data.h"
#include "res/binary.emuscv128x128xrgba8888.data.h"
#include "res/binary.emuscvlady32x32xrgba8888.data.h"
#include "res/binary.emuscvlady64x64xrgba8888.data.h"
#include "res/binary.emuscvlady128x128xrgba8888.data.h"
#include "res/binary.keyboardepoch115x78xrgba8888.data.h"
#include "res/binary.keyboardepoch230x156xrgba8888.data.h"
#include "res/binary.keyboardepoch460x312xrgba8888.data.h"
#include "res/binary.keyboardlady115x78xrgba8888.data.h"
#include "res/binary.keyboardlady230x156xrgba8888.data.h"
#include "res/binary.keyboardlady460x312xrgba8888.data.h"
#include "res/binary.keyboardyeno115x78xrgba8888.data.h"
#include "res/binary.keyboardyeno230x156xrgba8888.data.h"
#include "res/binary.keyboardyeno460x312xrgba8888.data.h"
#include "res/binary.recalbox624x160xrgba8888.data.h"
#include "res/binary.recalbox312x80xrgba8888.data.h"
#include "res/binary.recalbox156x40xrgba8888.data.h"

#define NOISE_WIDTH  444
#define NOISE_HEIGHT 333



//
// Libretro: Default log handler, to use if any log callback handler is defined by the frontend
//
void EmuSCV_RetroLogFallback(enum retro_log_level level, const char *fmt, ...)
{
	(void)level;

	va_list va;

	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
}

//
// EmuSCV class constructor.
//
cEmuSCV::cEmuSCV()
{
	// Log
	RetroLogPrintf = EmuSCV_RetroLogFallback;
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::cEmuSCV()\n", EMUSCV_NAME);

	// Init variables
	retro_core_initialized		= false;
	retro_audio_enable			= false;
	retro_frame_counter			= 0;
	retro_frame_time			= 0;
	retro_input_support_bitmask	= false;
	retro_game_loaded			= false;

	is_power_on					= true;

	memset(retro_base_directory, 0, sizeof(retro_base_directory));
	memset(retro_save_directory, 0, sizeof(retro_save_directory));
	memset(retro_game_path, 0, sizeof(retro_game_path));

	// Init functions
	RetroEnvironment		= NULL;
	RetroVideoRefresh		= NULL;
	RetroAudioSample		= NULL;
	RetroAudioSampleBatch	= NULL;
	RetroInputPoll			= NULL;
	RetroInputState			= NULL;

	escv_emu = NULL;

	frame_surface		= NULL;
	frame_renderer		= NULL;
	noise_surface		= NULL;
	noise_renderer		= NULL;
	keyboard_surface	= NULL;
	keyboard_renderer	= NULL;
	keyboard_counter	= 0;

	run_frames_last				= 0;
	run_frames_total			= 0;
	draw_frames_total			= 0;

	config.window_height = DRAW_HEIGHT_EMUSCV;
	config.window_width = config.window_height*4.0/3.0;
	config.window_aspect_ratio = (float)(config.window_width)/(float)(config.window_height);
	config.window_fps = WINDOW_FPS_EPOCH;

	window_width_old = config.window_width;
	window_height_old = config.window_height;
	window_fps_old = config.window_fps;

	key_pressed_0 = false;
	key_pressed_1 = false;
	key_pressed_2 = false;
	key_pressed_3 = false;
	key_pressed_4 = false;
	key_pressed_5 = false;
	key_pressed_6 = false;
	key_pressed_7 = false;
	key_pressed_8 = false;
	key_pressed_9 = false;
	key_pressed_cl = false;
	key_pressed_en = false;
	key_pressed_power = false;
	key_pressed_pause = false;
	key_pressed_reset = false;
	key_pressed_up = false;
	key_pressed_down = false;
	key_pressed_left = false;
	key_pressed_right = false;
	key_pressed_keyboard_stay_opened = false;
	key_pressed_keyboard_close = false;

	button_keyboard_pressed = false;
	button_menu_pressed = false;

	start_up_counter_power = 0;
	start_up_counter_logo = 0;

	is_keyboard_displayed = false;
	is_menu_displayed = false;
	keyboard_x = 1;
	keyboard_y = 2;

	for (int32_t i = EMUSCV_NOISE_SAMPLES-1; i >= 0; i--)
	{
		sound_buffer_noise[i] = (rand() % 256) - 128;
	}
	sound_buffer_noise_index = rand() % (EMUSCV_NOISE_SAMPLES - 1);
	sound_buffer_samples = 0;
	sound_buffer_size = 0;
}

//
// EmuSCV class destructor.
//
cEmuSCV::~cEmuSCV()
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::~cEmuSCV()\n", EMUSCV_NAME);

	if (retro_game_loaded == TRUE)
	{
		RetroUnloadGame();
		retro_game_loaded = FALSE;
	}
	if (retro_core_initialized == TRUE)
	{
		RetroDeinit();
		retro_core_initialized = FALSE;
	}
}

//
// Libretro: set the environment
//
void cEmuSCV::RetroSetEnvironment(retro_environment_t cb)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetEnvironment()\n", EMUSCV_NAME);

	// Set the Libretro environment callback
	RetroEnvironment = cb;
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Environment callback set\n", EMUSCV_NAME);

	// Set the log callback
	static struct retro_log_callback log_callback;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log_callback))
	{
		RetroLogPrintf = log_callback.log;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Log callback set to LibRetro\n", EMUSCV_NAME);
	}
	else
	{
		RetroLogPrintf = EmuSCV_RetroLogFallback;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Log callback set to EmuSCV\n", EMUSCV_NAME);
	}

	// The core can be run without ROM (to show display test if no game loaded)
	static bool support_no_game = TRUE;
	RetroEnvironment(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &support_no_game);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] support_no_game set\n", EMUSCV_NAME);

	// Set the controllers description
	static const struct retro_controller_description controller_joypad[] =
	{
		{ "Controller", RETRO_DEVICE_JOYPAD },	// controllers
		{ 0 }
	};
	static const struct retro_controller_description controller_keyboard[] =
	{
		{ "Keyboard", RETRO_DEVICE_KEYBOARD },	// keyboard on the console
		{ 0 }
	};
	// 2 controllers and 1 keyboard
	static const struct retro_controller_info controller_info[] =
	{
		{ controller_joypad,   1 },	// port 0
		{ controller_joypad,   1 },	// port 1
		{ controller_keyboard, 1 },	// port 2
		{ NULL, 0 },
	};
	RetroEnvironment(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)controller_info);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Controller infos set\n", EMUSCV_NAME);

	// Set the controller descriptor
	struct retro_input_descriptor input_descriptor[] =
	{
		// Joypad 1: left side orange controller
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, EMUSCV_INPUTDESC_BUTTON_SELECT },	// Display console options, keyboard, manuals, etc.
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  EMUSCV_INPUTDESC_BUTTON_START },	// Pause
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   EMUSCV_INPUTDESC_BUTTON_LEFT },		// Controller joystick
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  EMUSCV_INPUTDESC_BUTTON_RIGHT },	// Controller joystick
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     EMUSCV_INPUTDESC_BUTTON_UP },		// Controller joystick
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   EMUSCV_INPUTDESC_BUTTON_DOWN },		// Controller joystick
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      EMUSCV_INPUTDESC_BUTTON_1 },		// Right controller button
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      EMUSCV_INPUTDESC_BUTTON_2 },		// Left controller button
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      EMUSCV_INPUTDESC_BUTTON_3},
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      EMUSCV_INPUTDESC_BUTTON_4 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      EMUSCV_INPUTDESC_BUTTON_L1 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      EMUSCV_INPUTDESC_BUTTON_R1 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,     EMUSCV_INPUTDESC_BUTTON_L2 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     EMUSCV_INPUTDESC_BUTTON_R2 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,     EMUSCV_INPUTDESC_BUTTON_L3 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,     EMUSCV_INPUTDESC_BUTTON_R3 },
		{ 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_X, EMUSCV_INPUTDESC_ANALOG_LEFT_X },	// Controller joystick
		{ 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_Y, EMUSCV_INPUTDESC_ANALOG_LEFT_Y },	// Controller joystick
		{ 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X, EMUSCV_INPUTDESC_ANALOG_RIGHT_X },
		{ 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y, EMUSCV_INPUTDESC_ANALOG_RIGHT_Y },

		// Joypad 2: right side blue controller
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, EMUSCV_INPUTDESC_BUTTON_SELECT },	// Display console options, keyboard, manuals, etc.
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  EMUSCV_INPUTDESC_BUTTON_START },	// Pause
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   EMUSCV_INPUTDESC_BUTTON_LEFT },		// Controller joystick
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  EMUSCV_INPUTDESC_BUTTON_RIGHT },	// Controller joystick
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     EMUSCV_INPUTDESC_BUTTON_UP },		// Controller joystick
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   EMUSCV_INPUTDESC_BUTTON_DOWN },		// Controller joystick
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      EMUSCV_INPUTDESC_BUTTON_1 },		// Right controller button
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      EMUSCV_INPUTDESC_BUTTON_2 },		// Left controller button
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      EMUSCV_INPUTDESC_BUTTON_3},
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      EMUSCV_INPUTDESC_BUTTON_4 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      EMUSCV_INPUTDESC_BUTTON_L1 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      EMUSCV_INPUTDESC_BUTTON_R1 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,     EMUSCV_INPUTDESC_BUTTON_L2 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     EMUSCV_INPUTDESC_BUTTON_R2 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,     EMUSCV_INPUTDESC_BUTTON_L3 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,     EMUSCV_INPUTDESC_BUTTON_R3 },
		{ 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_X, EMUSCV_INPUTDESC_ANALOG_LEFT_X },	// Controller joystick
		{ 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_Y, EMUSCV_INPUTDESC_ANALOG_LEFT_Y },	// Controller joystick
		{ 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X, EMUSCV_INPUTDESC_ANALOG_RIGHT_X },
		{ 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y, EMUSCV_INPUTDESC_ANALOG_RIGHT_Y },

		{ 0 }
	};
	RetroEnvironment(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, input_descriptor);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Input descriptors set\n", EMUSCV_NAME);

	// Set config variables
	struct retro_variable variable[] =
	{
		{ SETTING_CONSOLE_KEY, "CONSOLE; AUTO|EPOCH|YENO|EPOCHLADY" },
		{ SETTING_DISPLAY_KEY, "DISPLAY; AUTO|EMUSCV|EPOCH|YENO" },
		{ SETTING_PIXELASPECT_KEY, "PIXELASPECT; AUTO|RECTANGULAR|SQUARE" },
		{ SETTING_RESOLUTION_KEY, "RESOLUTION; AUTO|LOW|MEDIUM|HIGH" },
		{ SETTING_PALETTE_KEY, "PALETTE; AUTO|STANDARD|OLDNTSC" },
		{ SETTING_FPS_KEY, "FPS; AUTO|EPOCH60|YENO50" },
		{ SETTING_DISPLAYFULLMEMORY_KEY, "DISPLAYFULLMEMORY; AUTO|YES|NO" },
		{ SETTING_DISPLAYINPUTS_KEY, "DISPLAYINPUTS; AUTO|YES|NO" },
		{ SETTING_LANGAGE_KEY, "LANGAGE; AUTO|JP|FR|EN" },
		{ SETTING_CHECKBIOS_KEY, "CHECKBIOS; AUTO|YES|NO" },
		{ NULL, NULL }
	};
	RetroEnvironment(RETRO_ENVIRONMENT_SET_VARIABLES, variable);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Variables set\n", EMUSCV_NAME);
}

// 
// Libretro: set the video refresh callback
// 
void cEmuSCV::RetroSetVideoRefresh(retro_video_refresh_t cb)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetVideoRefresh()\n", EMUSCV_NAME);

	// Set the Libretro video callback
	RetroVideoRefresh = cb;
}

// 
// Libretro: set the audio sample callback
// 
void cEmuSCV::RetroSetAudioSample(retro_audio_sample_t cb)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetAudioSample()\n", EMUSCV_NAME);

	// Set the Libretro audio sample callback
	RetroAudioSample = cb;
}

// 
// Libretro: set the audio batch callback
// 
void cEmuSCV::RetroSetAudioSampleBatch(retro_audio_sample_batch_t cb)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetAudioSampleBatch()\n", EMUSCV_NAME);

	// Set the Libretro audio sample batch callback
	RetroAudioSampleBatch = cb;
}

// 
// Libretro: set the input poll callback
// 
void cEmuSCV::RetroSetInputPoll(retro_input_poll_t cb)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetInputPoll()\n", EMUSCV_NAME);

	RetroInputPoll = cb;
}

// 
// Libretro: set the input state callback
// 
void cEmuSCV::RetroSetInputState(retro_input_state_t cb)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetInputState()\n", EMUSCV_NAME);

	RetroInputState = cb;
}

//
// Libretro: return the version used by the core for compatibility check with the frontend
//
unsigned cEmuSCV::RetroGetApiVersion(void)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroApiVersion() => %d\n", EMUSCV_NAME, RETRO_API_VERSION);

	return RETRO_API_VERSION;
}

//
// Libretro: return the video standard used
//
unsigned cEmuSCV::RetroGetVideoRegion(void)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroGetVideoRegion() => RETRO_REGION_PAL\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroGetVideoRegion() => RETRO_REGION_NTSC\n", EMUSCV_NAME);

//	return RETRO_REGION_PAL;
	return RETRO_REGION_NTSC;
}

// 
// Libretro: get the system infos
// 
void cEmuSCV::RetroGetSystemInfo(struct retro_system_info *info)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroGetSystemInfo()\n     library_name     = %s\n     library_version  = %s\n     valid_extensions = %s\n     need_fullpath    = false\n     block_extract    = false\n", EMUSCV_NAME, EMUSCV_NAME, EMUSCV_VERSION, EMUSCV_EXTENSIONS);

	memset(info, 0, sizeof(*info));
	info->library_name		= EMUSCV_NAME;
	info->library_version	= EMUSCV_VERSION;
	info->valid_extensions	= EMUSCV_EXTENSIONS;
	info->need_fullpath		= true;
	info->block_extract		= true;
}

// 
// Libretro: get the audio/video infos
// 
void cEmuSCV::RetroGetAudioVideoInfo(struct retro_system_av_info *info)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV:RetroGetAudioVideoInfo()\n     timing.fps            = %f\n     timing.sample_rate    = AUDIO_SAMPLING_RATE\n     geometry.base_width   = %d\n     geometry.base_height  = %d\n     geometry.max_width    = %d\n     geometry.max_height   = %d\n     geometry.aspect_ratio = %f\n", EMUSCV_NAME, config.window_fps, config.window_width, config.window_height, config.window_width * WINDOW_ZOOM_MAX, config.window_height * WINDOW_ZOOM_MAX, config.window_aspect_ratio);

	memset(info, 0, sizeof(*info));
	info->timing.fps            = config.window_fps;
	info->timing.sample_rate    = AUDIO_SAMPLING_RATE;
	info->geometry.base_width   = config.window_width;
	info->geometry.base_height  = config.window_height;
	info->geometry.max_width    = config.window_width * WINDOW_ZOOM_MAX;
	info->geometry.max_height   = config.window_height * WINDOW_ZOOM_MAX;
	info->geometry.aspect_ratio = config.window_aspect_ratio;
}

//
// Libretro: initialize the core
//
void cEmuSCV::RetroInit(retro_audio_callback_t RetroAudioCb, retro_audio_set_state_callback_t RetroAudioSetStateCb, retro_frame_time_callback_t RetroFrameTimeCb)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroInit()\n", EMUSCV_NAME);

	if (retro_core_initialized == TRUE)
	{
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Initialisaztions already done\n", EMUSCV_NAME);
		return;
	}

//	// Set audio callbacks
//	struct retro_audio_callback audio_callback = { RetroAudioCb, RetroAudioSetStateCb };
//	retro_use_audio_cb = RetroEnvironment(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &audio_callback);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Audio callback set\n", EMUSCV_NAME);

	// Set frame time callback
	struct retro_frame_time_callback frame_time_callback;
	frame_time_callback.callback  = RetroFrameTimeCb;
	frame_time_callback.reference = 1000000 / FRAMES_PER_SEC;
	frame_time_callback.callback(frame_time_callback.reference);
	RetroEnvironment(RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK, &frame_time_callback);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Frame time callback set\n", EMUSCV_NAME);

	// Retrieve base directory
	const char *dir = NULL;
	memset(retro_base_directory, 0, sizeof(retro_base_directory));
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
	{
		snprintf(retro_base_directory, sizeof(retro_base_directory), "%s", dir);
		set_libretro_system_directory(dir);
//		RetroLogPrintf(RETRO_LOG_INFO, "[%s] Base directory: %s\n", EMUSCV_NAME, retro_base_directory);
	}
	else
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Base directory not set\n", EMUSCV_NAME);
		return;
	}

	// Retrieve save directory	retro_base_path
	memset(retro_save_directory, 0, sizeof(retro_save_directory));
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &dir) && dir)
	{
		snprintf(retro_save_directory, sizeof(retro_save_directory), "%s", dir);
		set_libretro_save_directory(dir);
//		RetroLogPrintf(RETRO_LOG_INFO, "[%s] Save directory: %s\n", EMUSCV_NAME, retro_save_directory);
	}
	else
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Save directory not set\n", EMUSCV_NAME);
		return;
	}

	// Init core variables
	is_power_on					= false;
	retro_core_initialized 		= false;
	retro_frame_time			= 0;
	retro_game_loaded			= false;
	retro_audio_enable			= false;

	retro_input_support_bitmask	= RetroEnvironment(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL);
//	if(retro_input_support_bitmask)
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Input supports bitmask\n", EMUSCV_NAME);
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] retro_input_support_bitmask set (false)\n", EMUSCV_NAME);

	// Initialize eSCV settings
	initialize_config();
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] config initialized\n", EMUSCV_NAME);

	// Load EmuSCV settings
	RetroLoadSettings();
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] settings (variables) loaded\n", EMUSCV_NAME);

	// initialize the eSCV emulation core
	escv_emu = new EMU();
	if(!escv_emu->is_bios_present())
	{
		is_power_on = false;

		if(!escv_emu->is_bios_found())
		{
			// Affichage "BIOS non trouvé"
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] BIOS not found!\n", EMUSCV_NAME);
		}
		else if(!escv_emu->is_bios_ok())
		{
			// Affichage "BIOS incorrect"
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Bad BIOS!\n", EMUSCV_NAME);
		}
		else
		{
			// Affichage "BIOS non chargé"
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] BIOS not loaded!\n", EMUSCV_NAME);
		}
#ifdef USE_NOTIFY_POWER_OFF

		// notify power off
		escv_emu->notify_power_off();
#endif

		delete(escv_emu);
		escv_emu = NULL;
	}
	else
		is_power_on = true;

	retro_core_initialized	= true;

//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] All initialisations done\n", EMUSCV_NAME);
}

//
// Libretro: deinitialize the core
//
void cEmuSCV::RetroDeinit(void)
{
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroDeinit()\n", EMUSCV_NAME);

	if (!retro_core_initialized)
	{
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] All deinitialisaztions already done\n", EMUSCV_NAME);
		return;
	}

	if (retro_game_loaded)
	{
		RetroUnloadGame();
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game unloaded\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game not loaded, nothing to unload\n", EMUSCV_NAME);	

	// Destroy eSCV
	if (escv_emu)
	{
#ifdef USE_NOTIFY_POWER_OFF
		// notify power off
		escv_emu->notify_power_off();

#endif
		delete(escv_emu);
		escv_emu = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu deleted\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu don't exists, nothing to delete\n", EMUSCV_NAME);

	// Reinit core variables
	retro_frame_time			= 0;
	retro_game_loaded			= FALSE;
    retro_input_support_bitmask	= FALSE;
	retro_audio_enable			= FALSE;
	retro_core_initialized 		= FALSE;

//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] All deinitialisaztions done\n", EMUSCV_NAME);
}

// 
// Libretro: set controller port device
// 
void cEmuSCV::RetroSetControllerPortDevice(unsigned port, unsigned device)
{
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetControllerPortDevice()\n          Set device %d into port %d\n", EMUSCV_NAME, device, port);
}

/*
// 
// Libretro: audio callback
// 
void cEmuSCV::RetroAudioCb(void)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroAudioCb()\n", EMUSCV_NAME);
	int16_t val = 0x0000;
//	if (retro_audio_enable == TRUE)// && button_pressed == TRUE)
//	{
//		// Play A 440Hz
//		for (unsigned i = 0; i < AUDIO_SAMPLES_PER_FRAME; i++, phase++)
//		{
//			int16_t val = INT16_MAX * sinf(2.0f * M_PI * phase * 440.0f / AUDIO_SAMPLING_RATE);
//			audio_cb(val, val);
//		}
//	}
//	else
//	{
		// Mute
//		for (unsigned i = 0; i < AUDIO_SAMPLES_PER_FRAME; i++)

		for (uint16_t i = 0; i < AUDIO_SAMPLING_RATE; i++)
			RetroAudioSample(val, val);

//	}
	retro_audio_phase %= AUDIO_SAMPLING_RATE;
//RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] cEmuSCV::RetroAudioCb() => RetroAudioSample()\n", EMUSCV_NAME);
//int16_t val = 0;
//for (uint32_t i = 0; i < AUDIO_SAMPLING_RATE; i++)
//	RetroAudioSample(val, val);
}
*/

// 
// Libretro: audio set state enable/disable callback
// 
void cEmuSCV::RetroAudioSetStateCb(bool enable)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroAudioSetStateCb()\n          Set audio state = %s\n", EMUSCV_NAME, (enable ? "ON" : "OFF"));

	// Set audio state
	retro_audio_enable = enable;
}

// 
// Libretro: frame time callback
// 
void cEmuSCV::RetroFrameTimeCb(retro_usec_t usec)
{
	int64_t usec_corrected = usec*FRAMES_PER_SEC/config.window_fps;

	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] cEmuSCV::RetroFrameTimeCb()\n          Frame time = %d microseconds\n", EMUSCV_NAME, usec_corrected);
	
	// Memorise current frame time
	retro_frame_time = usec_corrected;
}

// 
// Libretro: load game
// 
bool cEmuSCV::RetroLoadGame(const struct retro_game_info *info)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroLoadGame()\n", EMUSCV_NAME);

	unsigned char *image;
	uint32_t posx			= 0;
	uint32_t posy			= 0;
	uint32_t sizx			= 0;
	uint32_t sizy			= 0;


	keyboard_x = 1;
	keyboard_y = 2;

	if(retro_game_loaded)
	{
		RetroUnloadGame();
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Previous game unloded\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No previous game, nothing to unload\n", EMUSCV_NAME);
	
	retro_game_loaded = false;

	// Set the performance level is guide to Libretro to give an idea of how intensive this core is to run for the game
	// It should be set in retro_load_game()
	static unsigned performance_level = 4;
	RetroEnvironment(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &performance_level);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Performance level set (4)\n", EMUSCV_NAME);

    enum retro_pixel_format pixel_format = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!RetroEnvironment(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format))
    {
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Pixel format not supported! (XRGB8888)\n", EMUSCV_NAME);
        return FALSE;
    }
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Pixel format set (XRGB8888)\n", EMUSCV_NAME);

	bool updated = false;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
	{
		RetroLoadSettings();
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Settings (variable) loaded\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Settings not updated, already loaded\n", EMUSCV_NAME);

	// Create SDL main frame surface
    frame_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, config.window_width, config.window_height, 8*sizeof(uint32_t), 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (frame_surface == NULL)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
        return FALSE;
	}
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface created\n", EMUSCV_NAME);

	// Create SDL main frame renderer
    frame_renderer = SDL_CreateSoftwareRenderer(frame_surface);
    if (frame_renderer == NULL)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface renderer creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
        return FALSE;
    }
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface renderer created\n", EMUSCV_NAME);

	// Paint frame surface in black
	SDL_SetRenderDrawColor(frame_renderer, 0, 0, 0, 255);
	SDL_RenderClear(frame_renderer);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface cleared\n", EMUSCV_NAME);

	// create SDL TV static noise surface
    noise_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 2*NOISE_WIDTH, 2*NOISE_HEIGHT, 8*sizeof(uint32_t), 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (noise_surface == NULL)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
        return FALSE;
	}
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface created\n", EMUSCV_NAME);

	// Create SDL TV static noise renderer
    noise_renderer = SDL_CreateSoftwareRenderer(noise_surface);
    if (noise_renderer == NULL)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface renderer creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
        return FALSE;
    }
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer created\n", EMUSCV_NAME);

	// Draw TV static noise
	for(int y = 0; y < noise_surface->h; y++)
	{
		for(int x = 0; x < noise_surface->w; x++)
		{
			uint8_t noise_color = rand() % 255;
			pixelRGBA(noise_renderer, x, y, noise_color, noise_color, noise_color, 255);
		}
	}
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] TV static noise drawn\n", EMUSCV_NAME);

	// Create quick menu SDL surface & renderer
	if(!CreateKeyboardSurface())
		return FALSE;

	// Create EmuSCV logo SDL surface & renderer
	if(!CreateLogoSurface())
		return FALSE;

	// Create Recalbox logo SDL surface & renderer
	if(!CreateRecalboxSurface())
		return FALSE;

	// reset the frame counter
	retro_frame_counter = 0;

	// Get game rom path
	memset(retro_game_path, 0, sizeof(retro_game_path));
    if(info && strcmp(info->path, "") != 0)
	{
		snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);
		RetroLogPrintf(RETRO_LOG_INFO, "[%s] Try to load game: %s\n", EMUSCV_NAME, retro_game_path);

		// Insert cart
		if(escv_emu && escv_emu->is_bios_present() && strcmp(retro_game_path, "") != 0)
		{
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Try to load %s\n", EMUSCV_NAME, retro_game_path);
			escv_emu->open_cart(0,retro_game_path);
			if(escv_emu->is_cart_inserted(0))
			{
				retro_game_loaded = true;
				RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game loaded\n", EMUSCV_NAME);
			}
			else
				RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Game not loaded\n", EMUSCV_NAME);
		}
		else
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Bios not present or empty game path -> game not loaded\n", EMUSCV_NAME);
//RetroLogPrintf(RETRO_LOG_INFO, "[%s] WE DON'T TRY TO LOAD GAME\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_INFO, "[%s] no game\n", EMUSCV_NAME);

//RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game loading done\n", EMUSCV_NAME);

	sound_buffer_samples = (AUDIO_SAMPLING_RATE / config.window_fps + 0.5);

	return true;
}

// 
// Libretro: unload game
// 
void cEmuSCV::RetroUnloadGame(void)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroUnloadGame()\n", EMUSCV_NAME);

	// Free eSCV
	if(escv_emu && escv_emu->is_cart_inserted(0))
	{
		escv_emu->close_cart(0);
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game unloaded\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No game loaded, nothing to unload\n", EMUSCV_NAME);

	// Free SDL TV static noise renderer
	if (noise_renderer != NULL)
	{
		SDL_DestroyRenderer(noise_renderer);
		noise_renderer = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer destroyed\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface renderer, nothing to destroy\n", EMUSCV_NAME);

	// Free SDL TV static noise surface
	if (noise_surface != NULL)
	{
		SDL_FreeSurface(noise_surface);
		noise_surface = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface destroyed\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface, nothing to destroy\n", EMUSCV_NAME);

	// Free SDL main frame renderer
	if (frame_renderer != NULL)
	{
		SDL_DestroyRenderer(frame_renderer);
		frame_renderer = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface renderer destroyed\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL main surface renderer, nothing to destroy\n", EMUSCV_NAME);

	// Free SDL main frame surface
	if (frame_surface != NULL)
	{
		SDL_FreeSurface(frame_surface);
		frame_surface = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface destroyed\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL main surface, nothing to destroy\n", EMUSCV_NAME);

	retro_game_loaded = FALSE;
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game unloading done\n", EMUSCV_NAME);
}

//
// Libretro: run for only one frame
//
void cEmuSCV::RetroRun(void)
{
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] cEmuSCV::RetroRun()\n", EMUSCV_NAME);

	unsigned port0			= 0;
	unsigned port1			= 1;
	int16_t ret0			= 0;
	int16_t ret1			= 0;
	int16_t key0			= 0;
	int16_t key1			= 0;
	int16_t key2			= 0;
	int16_t key3			= 0;
	int16_t key4			= 0;
	int16_t key5			= 0;
	int16_t key6			= 0;
	int16_t key7			= 0;
	int16_t key8			= 0;
	int16_t key9			= 0;
	int16_t keyEnter		= 0;
	int16_t keyClear		= 0;
	int16_t keyPower		= 0;
	int16_t keyReset		= 0;
	int16_t keyPause		= 0;
	int16_t analogleftx0	= 0;
	int16_t analoglefty0	= 0;
	int16_t analogrightx0	= 0;
	int16_t analogrighty0	= 0;
	int16_t analogleftx1	= 0;
	int16_t analoglefty1	= 0;
	int16_t analogrightx1	= 0;
	int16_t analogrighty1	= 0;

	uint32_t posx			= 0;
	uint32_t posy			= 0;
	uint8_t  alpha			= 127;
	uint32_t sizx			= 0;
	uint32_t sizy			= 0;
	uint32_t space			= 4;

	uint8_t spacex			= 0;
	uint8_t spacey			= 0;
	uint8_t sizex			= 0;
	uint8_t sizey			= 0;

	int16_t joyposx			= 0;
	int16_t joyposy			= 0;
	uint32_t color			= 0xFF000000;

	bool config_changed = false;

	// Load previous save state
	if(state.IsReadyToLoad())
	{
		if(escv_emu)
			escv_emu->load_state(&state);
		RetroLoadSettings();
		state.SetReadyToLoad(false);
	}

	bool updated = false;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
	{
		RetroLoadSettings();
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Settings (variables) loaded\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Settings (variables) unchanged, already loaded\n", EMUSCV_NAME);

	// Get inputs
	RetroInputPoll();
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Imputs polled\n", EMUSCV_NAME);

	// Controllers buttons
	if (retro_input_support_bitmask)
	{
		ret0 = RetroInputState(port0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
		ret1 = RetroInputState(port1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
	}
	else
	{
		unsigned i;
		for (i = RETRO_DEVICE_ID_JOYPAD_B; i < RETRO_DEVICE_ID_JOYPAD_R3+1; i++)
		{
			if (RetroInputState(port0, RETRO_DEVICE_JOYPAD, 0, i))
				ret0 |= (1 << i);
		}
		for (i = RETRO_DEVICE_ID_JOYPAD_B; i < RETRO_DEVICE_ID_JOYPAD_R3+1; i++)
		{
			if (RetroInputState(port1, RETRO_DEVICE_JOYPAD, 0, i))
				ret1 |= (1 << i);
		}
	}
	// Controllers analog sticks
	analogleftx0	= RetroInputState(port0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
	analoglefty0	= RetroInputState(port0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
	analogrightx0	= RetroInputState(port0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
	analogrighty0	= RetroInputState(port0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);
	analogleftx1	= RetroInputState(port1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
	analoglefty1	= RetroInputState(port1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
	analogrightx1	= RetroInputState(port1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
	analogrighty1	= RetroInputState(port1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);
	// Keyboard
	key1 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_1)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP1);
	key2 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_2)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP2);
	key3 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_3)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP3);
	key4 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_4)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP4);
	key5 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_5)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP5);
	key6 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_6)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP6);
	key7 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_7)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP7);
	key8 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_8)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP8);
	key9 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_9)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP9);
	key0 = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_0)
		|| RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP0);
	keyEnter = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN)
            || RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP_ENTER);
	keyClear = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_BACKSPACE)
            || RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_DELETE);
	keyPower = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_F12);
	keyReset = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_F11);
	keyPause = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_F9);
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Imputs parsed\n", EMUSCV_NAME);

	// Button SELECT
	// open configuration
/*
	if ((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)))
	{
		if(!button_menu_pressed)
		{
			button_menu_pressed = true;
			is_menu_displayed = !is_menu_displayed
			if(is_menu_displayed)
				is_keyboard_displayed = false;
		}
	}
	else
		button_menu_pressed = false;
*/

	// Other buttons
	// open the console keyboard overlay
	if ((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT))
	|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2))/* || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L3)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))*/
	|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2))/* || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L3)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))*/)
	{
		if(!button_keyboard_pressed)
		{
			button_keyboard_pressed = true;
			is_keyboard_displayed = !is_keyboard_displayed;
			if(is_keyboard_displayed)
				is_menu_displayed = false;
		}
	}
	else
		button_keyboard_pressed = false;

	// Open or close fading the keyboard
	if(is_keyboard_displayed)
	{
		if(keyboard_counter < EMUSCV_KEYBOARD_DELAY)
		{
			keyboard_counter += 300/config.window_fps;
			if(keyboard_counter >= EMUSCV_KEYBOARD_DELAY)
				keyboard_counter = EMUSCV_KEYBOARD_DELAY;
		}
	}
	else
	{
		if(keyboard_counter > 0)
		{
			keyboard_counter -= 300/config.window_fps;
			if(keyboard_counter < 0)
				keyboard_counter = 0;
		}
	}

	// Key UP with controller
	if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) || (analoglefty0 <= EMUSCV_AXIS_NEUTRAL_MIN) || (analogrighty0 <= EMUSCV_AXIS_NEUTRAL_MIN)
	|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) || (analoglefty1 <= EMUSCV_AXIS_NEUTRAL_MIN) || (analogrighty1 <= EMUSCV_AXIS_NEUTRAL_MIN))
	{
		if(!key_pressed_up)
		{
			key_pressed_up = true;
			if(is_keyboard_displayed && keyboard_y > 0)
			{
				if(keyboard_x == 0 && (keyboard_y == 1 || keyboard_y == 2))
					keyboard_y = 0;
				else
					keyboard_y--;
			}
		}
	}
	else
		key_pressed_up = false;

	// Key DOWN with controller
	if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) || (analoglefty0 >= EMUSCV_AXIS_NEUTRAL_MAX) || (analogrighty0 >= EMUSCV_AXIS_NEUTRAL_MAX)
	|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) || (analoglefty1 >= EMUSCV_AXIS_NEUTRAL_MAX) || (analogrighty1 >= EMUSCV_AXIS_NEUTRAL_MAX))
	{
		if(!key_pressed_down)
		{
			key_pressed_down = true;
			if(is_keyboard_displayed && keyboard_y < 3)
			{
				if(keyboard_x == 0 && (keyboard_y == 1 || keyboard_y == 2))
					keyboard_y = 3;
				else
					keyboard_y++;
			}
		}
	}
	else
		key_pressed_down = false;

	// Key LEFT with controller
	if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) || (analogleftx0 <= EMUSCV_AXIS_NEUTRAL_MIN) || (analogrightx0 <= EMUSCV_AXIS_NEUTRAL_MIN)
	|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) || (analogleftx1 <= EMUSCV_AXIS_NEUTRAL_MIN) || (analogrightx1 <= EMUSCV_AXIS_NEUTRAL_MIN))
	{
		if(!key_pressed_left)
		{
			key_pressed_left = true;
			if(is_keyboard_displayed && keyboard_x > 0)
				keyboard_x--;
		}
	}
	else
		key_pressed_left = false;

	// Key RIGHT with controller
	if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) || (analogleftx0 >= EMUSCV_AXIS_NEUTRAL_MAX) || (analogrightx0 >= EMUSCV_AXIS_NEUTRAL_MAX)
	|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) || (analogleftx1 >= EMUSCV_AXIS_NEUTRAL_MAX) || (analogrightx1 >= EMUSCV_AXIS_NEUTRAL_MAX))
	{
		if(!key_pressed_right)
		{
			key_pressed_right = true;
			if(is_keyboard_displayed && keyboard_x < 3)
				keyboard_x++;
		}
	}
	else
		key_pressed_right = false;

	key_pressed_keyboard_stay_opened = false;
	key_pressed_keyboard_close = false;

	// Controllers
	if(escv_emu)
	{
		if(!is_keyboard_displayed && keyboard_counter == 0 && !is_menu_displayed
		&& !key_pressed_0 && !key_pressed_cl && !key_pressed_en
		&& !key_pressed_1 && !key_pressed_2 && !key_pressed_3
		&& !key_pressed_4 && !key_pressed_5 && !key_pressed_6
		&& !key_pressed_7 && !key_pressed_8 && !key_pressed_9
		&& !key_pressed_power && !key_pressed_reset && !key_pressed_pause)
		{
			// LEFT CONTROLLER 1, orange
			// Directionnal cross, analog stick left or analog stick right
			escv_emu->get_osd()->set_joy_status(	0,
													(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) || (analoglefty0 <= EMUSCV_AXIS_NEUTRAL_MIN) || (analogrighty0 <= EMUSCV_AXIS_NEUTRAL_MIN),
													(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) || (analoglefty0 >= EMUSCV_AXIS_NEUTRAL_MAX) || (analogrighty0 >= EMUSCV_AXIS_NEUTRAL_MAX),
													(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) || (analogleftx0 <= EMUSCV_AXIS_NEUTRAL_MIN) || (analogrightx0 <= EMUSCV_AXIS_NEUTRAL_MIN),
													(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) || (analogleftx0 >= EMUSCV_AXIS_NEUTRAL_MAX) || (analogrightx0 >= EMUSCV_AXIS_NEUTRAL_MAX),
													(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)),
													(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)),
													false, false, false, false, false, false, false, false, false, false, false, false, false, false, 0, 0, 0, 0, 0);

			// LEFT CONTROLLER 2, blue
			// Directionnal cross, analog stick left or analog stick right
			escv_emu->get_osd()->set_joy_status(	1,
													(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) || (analoglefty1 <= EMUSCV_AXIS_NEUTRAL_MIN) || (analogrighty1 <= EMUSCV_AXIS_NEUTRAL_MIN),
													(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) || (analoglefty1 >= EMUSCV_AXIS_NEUTRAL_MAX) || (analogrighty1 >= EMUSCV_AXIS_NEUTRAL_MAX),
													(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) || (analogleftx1 <= EMUSCV_AXIS_NEUTRAL_MIN) || (analogrightx1 <= EMUSCV_AXIS_NEUTRAL_MIN),
													(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) || (analogleftx1 >= EMUSCV_AXIS_NEUTRAL_MAX) || (analogrightx1 >= EMUSCV_AXIS_NEUTRAL_MAX),
													(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)),
													(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)),
													false, false, false, false, false, false, false, false, false, false, false, false, false, false, 0, 0, 0, 0, 0);
		}
		else if ((is_keyboard_displayed || is_menu_displayed) && ((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START))))
		{
			// LEFT CONTROLLER 1, orange
			// Buttons A & B
			escv_emu->get_osd()->set_joy_status(0, false, false, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, 0, 0, 0, 0, 0);

			// LEFT CONTROLLER 2, blue
			// Buttons A & B
			escv_emu->get_osd()->set_joy_status(1, false, false, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, 0, 0, 0, 0, 0);

			key_pressed_keyboard_close = true;
		}
	}

	if(start_up_counter_power < 150)
	{
		// Auto power ON after 0,5 seconds
		// Power button inactive
		start_up_counter_power += 300/config.window_fps;
		is_power_on = (start_up_counter_power >= 150);
	}
	else
	{
		// Key POWER ON/OFF
		if(keyPower != 0
		|| (is_keyboard_displayed && keyboard_x == 0 && keyboard_y == 0
		&& (
			(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
			|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
			|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
			|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		)))
		{
			if (!key_pressed_power)
			{
				is_power_on = !is_power_on;
				if(is_power_on)
					RetroReset(false);
				else if(escv_emu)
					escv_emu->save_cart(0);
			}
			key_pressed_power = true;
			keyboard_x = 0;
			keyboard_y = 0;
			if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
			|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
				key_pressed_keyboard_close = true;
			else
				key_pressed_keyboard_stay_opened = true;
		}
		else
			key_pressed_power = false;
	}

//	if (!is_power_on)
//	{
//#ifdef USE_NOTIFY_POWER_OFF
//		// notify power off
//		if(escv_emu)
//		{
//			escv_emu->notify_power_off();
//		}
//#endif
//		RetroEnvironment(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
//	}

    // Key RESET
	if (keyReset != 0
	|| (is_keyboard_displayed && keyboard_x == 0 && (keyboard_y == 1 || keyboard_y == 2)
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
		RetroReset(true);
		key_pressed_reset = true;
		keyboard_x = 0;
		if(keyboard_y < 2)
			keyboard_y = 1;
		else
			keyboard_y = 2;
		if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else
		key_pressed_reset = false;

    // Key PAUSE
	if (keyPause != 0
	|| (is_keyboard_displayed && keyboard_x == 0 && keyboard_y == 3
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
		if(escv_emu)
			escv_emu->key_down(VK_SPACE, false, key_pressed_pause);
		key_pressed_pause = true;
		keyboard_x = 0;
		keyboard_y = 3;
		if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_pause)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_SPACE, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_SPACE, false);
			key_pressed_pause = false;
		}
	}

	// Key 0
	if (key0 != 0
	|| (is_keyboard_displayed && keyboard_x == 1 && keyboard_y == 3
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD0, false, key_pressed_0);
		key_pressed_0 = true;
		keyboard_x = 1;
		keyboard_y = 3;
		if(key0 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_0)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD0, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD0, false);
			key_pressed_0 = false;
		}
	}

	// Key 1
	if (key1 != 0
	|| (is_keyboard_displayed && keyboard_x == 1 && keyboard_y == 2
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
//		if(!key_pressed_1)
//		{
//			config.scv_display = SETTING_DISPLAY_EMUSCV_VAL;
//			config_changed = true;
//		}
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD1, false, key_pressed_1);
		key_pressed_1 = true;
		keyboard_x = 1;
		keyboard_y = 2;
		if(key1 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_1)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD1, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD1, false);
			key_pressed_1 = false;
		}
	}

	// Key 2
	if (key2 != 0
	|| (is_keyboard_displayed && keyboard_x == 2 && keyboard_y == 2
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
//		config.scv_display = SETTING_DISPLAY_EPOCH_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD2, false, key_pressed_2);
		key_pressed_2 = true;
		keyboard_x = 2;
		keyboard_y = 2;
		if(key2 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_2)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD2, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD2, false);
			key_pressed_2 = false;
		}
	}

	// Key 3
	if (key3 != 0
	|| (is_keyboard_displayed && keyboard_x == 3 && keyboard_y == 2
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
//		config.scv_display = SETTING_DISPLAY_YENO_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD3, false, key_pressed_3);
		key_pressed_3 = true;
		keyboard_x = 3;
		keyboard_y = 2;
		if(key3 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_3)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD3, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD3, false);
			key_pressed_3 = false;
		}
	}

	// Key 4
	if (key4 != 0
	|| (is_keyboard_displayed && keyboard_x == 1 && keyboard_y == 1
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
//		config.scv_displayfull = SETTING_DISPLAYFULL_NO_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD4, false, key_pressed_4);
		key_pressed_4 = true;
		keyboard_x = 1;
		keyboard_y = 1;
		if(key4 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_4)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD4, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD4, false);
			key_pressed_4 = false;
		}
	}

	// Key 5
	if (key5 != 0
	|| (is_keyboard_displayed && keyboard_x == 2 && keyboard_y == 1
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
//		config.scv_displayfull = SETTING_DISPLAYFULL_YES_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD5, false, key_pressed_5);
		key_pressed_5 = true;
		keyboard_x = 2;
		keyboard_y = 1;
		if(key5 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_5)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD5, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD5, false);
			key_pressed_5 = false;
		}
	}

	// Key 6
	if (key6 != 0
	|| (is_keyboard_displayed && keyboard_x == 3 && keyboard_y == 1
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD6, false, key_pressed_6);
		key_pressed_6 = true;
		keyboard_x = 3;
		keyboard_y = 1;
		if(key6 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_6)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD6, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD6, false);
			key_pressed_6 = false;
		}
	}

	// Key 7
	if (key7 != 0
	|| (is_keyboard_displayed && keyboard_x == 1 && keyboard_y == 0
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
//		config.scv_displayfps = SETTING_DISPLAYFPS_EPOCH60_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD7, false, key_pressed_7);
		key_pressed_7 = true;
		keyboard_x = 1;
		keyboard_y = 0;
		if(key7 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_7)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD7, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD7, false);
			key_pressed_7 = false;
		}
	}

	// Key 8
	if (key8 != 0
	|| (is_keyboard_displayed && keyboard_x == 2 && keyboard_y == 0
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
//		config.scv_displayfps = SETTING_DISPLAYFPS_YENO50_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD8, false, key_pressed_8);
		key_pressed_8 = true;
		keyboard_x = 2;
		keyboard_y = 0;
		if(key8 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_8)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD8, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD8, false);
			key_pressed_8 = false;
		}
	}

	// Key 9
	if (key9 != 0
	|| (is_keyboard_displayed && keyboard_x == 3 && keyboard_y == 0
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD9, false, key_pressed_9);
		key_pressed_9 = true;
		keyboard_x = 3;
		keyboard_y = 0;
		if(key9 != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_9)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_NUMPAD9, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_NUMPAD9, false);
			key_pressed_0 = false;
		}
	}

	// Key CL
	if (keyClear != 0
	|| (is_keyboard_displayed && keyboard_x == 2 && keyboard_y == 3
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
		if(escv_emu)
			escv_emu->key_down(VK_BACK, false, key_pressed_cl);
		key_pressed_cl = true;
		keyboard_x = 2;
		keyboard_y = 3;
		if(keyClear != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_cl)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_BACK, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_BACK, false);
			key_pressed_cl = false;
		}
	}

	// Key EN + Controllers 1 & 2's Button START
	if (keyEnter != 0 || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START))
	|| (is_keyboard_displayed && keyboard_x == 3 && keyboard_y == 3
	&& (
		(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	)))
	{
		if(escv_emu)
			escv_emu->key_down(VK_RETURN, false, key_pressed_en);
		key_pressed_en = true;
		keyboard_x = 3;
		keyboard_y = 3;
		if(keyEnter != 0
		|| (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
		|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			key_pressed_keyboard_close = true;
		else
			key_pressed_keyboard_stay_opened = true;
	}
	else if(key_pressed_en)
	{
		if(!is_keyboard_displayed && keyboard_counter > 0)
		{
			if(escv_emu)
				escv_emu->key_down(VK_RETURN, false, true);
		}
		else
		{
			if(escv_emu)
				escv_emu->key_up(VK_RETURN, false);
			key_pressed_en = false;
		}
	}

	if(key_pressed_keyboard_close)
		is_keyboard_displayed = false;

	if(config_changed)
	{
		apply_display_config();
		RetroSaveSettings();
	}

	// If video config change
	if (config.window_width != window_width_old || config.window_height != window_height_old || config.window_fps != window_fps_old)
	{
		struct retro_system_av_info av_info;
		retro_get_system_av_info(&av_info);
		av_info.timing.fps = config.window_fps;
		RetroEnvironment(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &av_info);

		window_width_old = config.window_width;
		window_height_old = config.window_height;
		window_fps_old = config.window_fps;

		// Recreate SDL surfaces and renderers if the size change
		if(frame_surface->w != config.window_width || frame_surface->h != config.window_height)
		{
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Resolution change detected\n", EMUSCV_NAME);

			// Free SDL main frame renderer
			if (frame_renderer != NULL)
			{
				SDL_DestroyRenderer(frame_renderer);
				frame_renderer = NULL;
//				RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface renderer destroyed\n", EMUSCV_NAME);
			}
//			else
//				RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL main surface renderer, nothing to destroy\n", EMUSCV_NAME);

			// Free SDL main frame surface
			if (frame_surface != NULL)
			{
				SDL_FreeSurface(frame_surface);
				frame_surface = NULL;
//				RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface destroyed\n", EMUSCV_NAME);
			}
//			else
//				RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL main surface, nothing to destroy\n", EMUSCV_NAME);

			// Create SDL main frame surface
			frame_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, config.window_width, config.window_height, 8*sizeof(uint32_t), 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
			if (frame_surface == NULL)
			{
				RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
				return;
			}
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface created\n", EMUSCV_NAME);

			// Create SDL main frame renderer
			frame_renderer = SDL_CreateSoftwareRenderer(frame_surface);
			if (frame_renderer == NULL)
			{
				RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface renderer creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
				return;
			}
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface renderer created\n", EMUSCV_NAME);
		}

		// Paint it black
		SDL_SetRenderDrawColor(frame_renderer, 0, 0, 0, 255);
		SDL_RenderClear(frame_renderer);
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface cleared\n", EMUSCV_NAME);
	}

	// Increment the frame counter
	retro_frame_counter++;

	if(is_power_on && escv_emu)
	{
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu->run()\n", EMUSCV_NAME);
		run_frames_last = escv_emu->run();
		run_frames_total += run_frames_last;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu->draw_screen()\n", EMUSCV_NAME);
		draw_frames_total += escv_emu->draw_screen();

		// Copy screen
		SDL_Rect RectSrc;
		RectSrc.x = config.draw_x;
		RectSrc.y = config.draw_y;
		RectSrc.w = config.draw_width;
		RectSrc.h = config.draw_height;
		SDL_SetSurfaceBlendMode(escv_emu->get_osd()->get_vm_screen_buffer()->frame_surface, SDL_BLENDMODE_NONE);
		if(SDL_BlitScaled(escv_emu->get_osd()->get_vm_screen_buffer()->frame_surface, &RectSrc, frame_surface, NULL) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled emu_scv->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled emu_scv->main ok\n", EMUSCV_NAME);
	}
	else
	{
		SDL_Rect RectSrc;
		RectSrc.x = rand() % NOISE_WIDTH;
		RectSrc.y = rand() % NOISE_HEIGHT;
		RectSrc.w = NOISE_WIDTH;
		RectSrc.h = NOISE_HEIGHT;
		SDL_SetSurfaceBlendMode(noise_surface, SDL_BLENDMODE_NONE);
		if(SDL_BlitScaled(noise_surface, &RectSrc, frame_surface, NULL) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok\n", EMUSCV_NAME);
	}

//	// Clear background, changing color every 60 frames
//	if (retro_frame_counter % FRAMES_PER_SEC == 0)
//	{
//		if (retro_frame_counter > UINT64_MAX - FRAMES_PER_SEC)
//			retro_frame_counter = 0;
//		color_index++;
//		if (color_index > 16)
//			color_index = 1;
//	}
//	color = palette_pc[color_index];
//	SDL_SetRenderDrawColor(frame_renderer, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
//	SDL_RenderClear(frame_renderer);

//    // Draw a lonely pixel (using an other color than background)
//    // with an ellipse around
//	color = palette_pc[colorIndex < 12 ? colorIndex + 4 : colorIndex - 12 ];  // Different color of background color
//	posx = 10;
//	posy = 10;
//	boxRGBA(frame_renderer, 4*posx, 3*posy, 4*posx+3, 3*posy+2, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
//	ellipseRGBA(frame_renderer, 4*posx+1, 3*posy+1, 10, 12, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);

	// Logo at startup
	if(start_up_counter_logo < EMUSCV_LOGO_DURATION)
	{
		start_up_counter_logo += 300/config.window_fps;
		if(start_up_counter_logo > EMUSCV_LOGO_DURATION)
			start_up_counter_logo = EMUSCV_LOGO_DURATION;
	}
	if(start_up_counter_logo < EMUSCV_LOGO_DURATION)
	{
/*
		// Draw an embedded binary image (128x128 or 64x64 or 32x32 pixels in ARGB8888 format)
		uint8_t alpha = (start_up_counter_logo < 1200-100 ? 255: 255*(1200-start_up_counter_logo)/100);
		unsigned char *image;
		if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
		{
			if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
				image = (unsigned char *)src_res_emuscvlady128x128xrgba8888_data;
			else
				image = (unsigned char *)src_res_emuscv128x128xrgba8888_data;
			sizx = 128;
			sizy = 8;
		}
		else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
		{
			if(config.scv_console == SETTING_CONSOLE_EPOCHLADY_VAL)
				image = (unsigned char *)src_res_emuscvlady64x64xrgba8888_data;
			else
				image = (unsigned char *)src_res_emuscv64x64xrgba8888_data;
			sizx = 64;
			sizy = 4;
		}
		else //if(config.window_resolution == SETTING_RESOLUTION_LOW_VAL)
		{
			if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
				image = (unsigned char *)src_res_emuscvlady32x32xrgba8888_data;
			else
				image = (unsigned char *)src_res_emuscv32x32xrgba8888_data;
			sizx = 32;
			sizy = 2;
		}
		posx = config.window_width - sizx - sizy;
		posy = config.window_height - sizx - sizy;
		for (int x, y = sizx; --y >= 0; )
			for (x = sizx; --x >= 0; )
				pixelRGBA(frame_renderer, posx+x, posy+y, image[4*(x+y*sizx)], image[4*(x+y*sizx)+1], image[4*(x+y*sizx)+2], alpha*image[4*(x+y*sizx)+3]/255);
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Logo drawn\n", EMUSCV_NAME);
*/
		SDL_Rect RectSrc;
		SDL_Rect RectDst;

		// Alpha for logos
		alpha = (start_up_counter_logo < EMUSCV_LOGO_DURATION-100 ? EMUSCV_LOGO_ALPHA: EMUSCV_LOGO_ALPHA*(EMUSCV_LOGO_DURATION-start_up_counter_logo)/100);
		
		// Draw EmuSCV logo
		RectSrc.x = 0;
		RectSrc.y = 0;
		RectSrc.w = RectDst.w = logo_surface->w;
		RectSrc.h = RectDst.h = logo_surface->h;
		RectDst.x = config.window_width - config.window_space - RectDst.w;
		RectDst.y = config.window_height - config.window_space - RectDst.h;
		SDL_SetSurfaceBlendMode(logo_surface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(logo_surface, alpha);
		if(SDL_BlitScaled(logo_surface, &RectSrc, frame_surface, &RectDst) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! (emuSCV logo) %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok (emuSCV logo)\n", EMUSCV_NAME);

		// Draw Recalbox logo
		RectSrc.x = 0;
		RectSrc.y = 0;
		RectSrc.w = RectDst.w = recalbox_surface->w;
		RectSrc.h = RectDst.h = recalbox_surface->h;
		RectDst.x = config.window_space;
		RectDst.y = config.window_height - config.window_space - RectDst.h;
		SDL_SetSurfaceBlendMode(recalbox_surface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(recalbox_surface, alpha);
		if(SDL_BlitScaled(recalbox_surface, &RectSrc, frame_surface, &RectDst) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! (Recalbox logo) %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok (Recalbox logo)\n", EMUSCV_NAME);
	}

//	// Draw a PNG image read from ZIP file
//	if(image_data != NULL)
//	{
//		posx = 80;
//		posy = 30;
//		sizx = image_width;
//		sizy = image_height;
//		for (uint32_t y = 0; y < sizy; y++)
//			for (uint32_t x = 0; x < sizx; x++)
//				pixelRGBA(frame_renderer, posx+x, posy+sizy-y, image_data[4*(x+y*sizx)], image_data[4*(x+y*sizx)+1], image_data[4*(x+y*sizx)+2], image_data[4*(x+y*sizx)+3]);
//	}

	// Display keyboard over picture?
	if(keyboard_counter > 0)
	{
		// Draw quick keyboard
		SDL_Rect RectSrc;
		SDL_Rect RectDst;

		// Bottom
		RectSrc.x = 0;
		RectSrc.w = RectDst.w = keyboard_surface->w;
		if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
			RectSrc.y = 100;
		else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
			RectSrc.y = 50;
		else
			RectSrc.y = 25;
		RectSrc.h = RectDst.h = keyboard_surface->h - RectSrc.y;
		RectDst.x = config.window_width - RectDst.w - 8 * config.window_space;
		RectDst.y = RectSrc.y + 4 * config.window_space;
		SDL_SetSurfaceBlendMode(keyboard_surface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(keyboard_surface, EMUSCV_KEYBOARD_ALPHA * keyboard_counter / EMUSCV_KEYBOARD_DELAY);
		if(SDL_BlitScaled(keyboard_surface, &RectSrc, frame_surface, &RectDst) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok\n", EMUSCV_NAME);

		// Left
		RectSrc.x = 0;
		if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
		{
			RectSrc.y = 60;
			RectSrc.h = RectDst.h = 40;
			RectSrc.w = RectDst.w = 64;
		}
		else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
		{
			RectSrc.y = 30;
			RectSrc.h = RectDst.h = 20;
			RectSrc.w = RectDst.w = 32;
		}
		else
		{
			RectSrc.y = 15;
			RectSrc.h = RectDst.h = 10;
			RectSrc.w = RectDst.w = 16;
		}
		RectDst.x = config.window_width - keyboard_surface->w - 8 * config.window_space;
		RectDst.y = RectSrc.y + 4 * config.window_space;
		SDL_SetSurfaceBlendMode(keyboard_surface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(keyboard_surface, EMUSCV_KEYBOARD_ALPHA * keyboard_counter / EMUSCV_KEYBOARD_DELAY);
		if(SDL_BlitScaled(keyboard_surface, &RectSrc, frame_surface, &RectDst) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok\n", EMUSCV_NAME);

		// Power button
		if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
		{
			RectSrc.x = (is_power_on ? 52 : 76);
			RectDst.x = config.window_width - keyboard_surface->w - 8 * config.window_space + 64;
			RectSrc.y = 60;
			RectSrc.h = RectDst.h = 40;
			RectSrc.w = RectDst.w = 68;
		}
		else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
		{
			RectSrc.x = (is_power_on ? 26 : 38);
			RectDst.x = config.window_width - keyboard_surface->w - 8 * config.window_space + 32;
			RectSrc.y = 30;
			RectSrc.h = RectDst.h = 20;
			RectSrc.w = RectDst.w = 34;
		}
		else
		{
			RectSrc.x = (is_power_on ? 13 : 19);
			RectDst.x = config.window_width - keyboard_surface->w - 8 * config.window_space + 16;
			RectSrc.y = 15;
			RectSrc.h = RectDst.h = 10;
			RectSrc.w = RectDst.w = 17;
		}
		RectDst.y = RectSrc.y + 4 * config.window_space;
		SDL_SetSurfaceBlendMode(keyboard_surface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(keyboard_surface, EMUSCV_KEYBOARD_ALPHA * keyboard_counter / EMUSCV_KEYBOARD_DELAY);
		if(SDL_BlitScaled(keyboard_surface, &RectSrc, frame_surface, &RectDst) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok\n", EMUSCV_NAME);

		// Right
		if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
		{
			RectSrc.x = 132;
			RectSrc.y = 60;
			RectSrc.h = RectDst.h = 40;
		}
		else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
		{
			RectSrc.x = 66;
			RectSrc.y = 30;
			RectSrc.h = RectDst.h = 20;
		}
		else
		{
			RectSrc.x = 33;
			RectSrc.y = 15;
			RectSrc.h = RectDst.h = 10;
		}
		RectSrc.w = RectDst.w = keyboard_surface->w - RectSrc.x;
		RectDst.x = config.window_width - RectSrc.w - 8 * config.window_space;
		RectDst.y = RectSrc.y + 4 * config.window_space;
		SDL_SetSurfaceBlendMode(keyboard_surface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(keyboard_surface, EMUSCV_KEYBOARD_ALPHA * keyboard_counter / EMUSCV_KEYBOARD_DELAY);
		if(SDL_BlitScaled(keyboard_surface, &RectSrc, frame_surface, &RectDst) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok\n", EMUSCV_NAME);

		// Top
		RectSrc.x = 0;
		RectSrc.y = 0;
		RectSrc.w = RectDst.w = keyboard_surface->w;
		if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
			RectSrc.h = RectDst.h = 60;
		else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
			RectSrc.h = RectDst.h = 30;
		else
			RectSrc.h = RectDst.h = 15;
		RectDst.x = config.window_width - RectDst.w - 8 * config.window_space;
		RectDst.y = 4 * config.window_space;
		SDL_SetSurfaceBlendMode(keyboard_surface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(keyboard_surface, EMUSCV_KEYBOARD_ALPHA * keyboard_counter / EMUSCV_KEYBOARD_DELAY);
		if(SDL_BlitScaled(keyboard_surface, &RectSrc, frame_surface, &RectDst) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
//		else
//			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok\n", EMUSCV_NAME);

		// Draw current selected key
		if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
			color = palette_pc[0xA];	// Magenta
		else
			color = palette_pc[0x4];	// Green
		if(key_pressed_keyboard_stay_opened || key_pressed_keyboard_close)
			alpha = EMUSCV_KEYBOARD_ALPHAKEYDOWN;
		else
			alpha = EMUSCV_KEYBOARD_ALPHAKEYUP;
		if(keyboard_x == 0)
		{
			if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
			{
				if(keyboard_y == 0)
				{
					posy = 60;
					sizey = 40;
				}
				else
				{
					posy = 78;
					sizey = 38;
					spacey = 42;
				}
				posx = 47;
				sizex = 102;
			}
			else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
			{
				if(keyboard_y == 0)
				{
					posy = 30;
					sizey = 20;
				}
				else
				{
					posy = 39;
					sizey = 19;
					spacey = 21;
				}
				posx = 23;
				sizex = 51;
			}
			else
			{
				if(keyboard_y == 0)
				{
					posy = 15;
					sizey = 10;
				}
				else
				{
					posy = 19;
					sizey = 10;
					spacey = 10;
				}
				posx = 11;
				sizex = 26;
			}
			uint8_t keyboard_y0 = keyboard_y;
			if(keyboard_y0 > 1)
				keyboard_y0--;
			boxRGBA(frame_renderer, RectDst.x + posx, RectDst.y + posy + keyboard_y0 * (sizey + spacey), RectDst.x + posx + sizex, RectDst.y + posy + keyboard_y0 * (sizey + spacey) + sizey, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);

//			switch (keyboard_y)
//			{
//				case 0:
//					break;
//				case 1:
//					break;
//				case 2:
//				default:
//					break;
//			}
		}
		else
		{
			if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
			{
				posx = 240;
				posy = 67;
				sizex = 38;
				sizey = 31;
				spacex = 20;
				spacey = 26;
			}
			else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
			{
				posx = 120;
				posy = 34;
				sizex = 18;
				sizey = 17;
				spacex = 11;
				spacey = 11;
			}
			else
			{
				posx = 60;
				posy = 16;
				sizex = 10;
				sizey = 8;
				spacex = 4;
				spacey = 6;
			}
			boxRGBA(frame_renderer, RectDst.x + posx + (keyboard_x - 1) * (sizex + spacex), RectDst.y + posy + keyboard_y * (sizey + spacey), RectDst.x + posx + (keyboard_x - 1) * (sizex + spacex) + sizex, RectDst.y + posy + keyboard_y * (sizey + spacey) + sizey, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		}
	}
//	// Display menu over picture?
//	else if(is_menu_displayed)
//	{
//
//	}

	// Display controls over all?
	if(config.scv_displayinputs == SETTING_DISPLAYINPUTS_YES_VAL)
	{
		alpha = EMUSCV_CONTROLS_ALPHA;

		if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
			space = 16;
		else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
			space = 8;
		else //if(config.window_resolution == SETTING_RESOLUTION_LOW_VAL)
			space = 4;


		// FIRST CONTROLLER (LEFT SIDE, ORANGE)
		posx = space;
		posy = config.window_height-26-17*space;
		// Contour
		color = palette_pc[1];	// Black
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy,             posx+5+4*space,   posy+3+2*space,   R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// L3
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy+4+2*space,   posx+5+4*space,   posy+7+4*space,   R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// L2
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy+8+4*space,   posx+5+4*space,   posy+11+6*space,  R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// L1
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy+12+6*space,  posx+5+4*space,   posy+15+8*space,  R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// UP
		rectangleRGBA(frame_renderer, posx,             posy+14+8*space,  posx+3+2*space,   posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT
		rectangleRGBA(frame_renderer, posx+4+4*space,   posy+14+8*space,  posx+7+6*space,   posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy+16+10*space, posx+5+4*space,   posy+19+12*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// DOWN
		rectangleRGBA(frame_renderer, posx+1+space,     posy+20+12*space, posx+6+5*space,   posy+25+16*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT STICK
		rectangleRGBA(frame_renderer, posx+8+6*space,   posy+14+8*space,  posx+11+8*space,  posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// SELECT
		rectangleRGBA(frame_renderer, posx+12+8*space,  posy+14+8*space,  posx+15+10*space, posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// START
		rectangleRGBA(frame_renderer, posx+18+12*space, posy,             posx+21+14*space, posy+3+2*space,   R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R3
		rectangleRGBA(frame_renderer, posx+18+12*space, posy+4+2*space,   posx+21+14*space, posy+7+4*space,   R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R2
		rectangleRGBA(frame_renderer, posx+18+12*space, posy+8+4*space,   posx+21+14*space, posy+11+6*space,  R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R1
		rectangleRGBA(frame_renderer, posx+18+12*space, posy+12+6*space,  posx+21+14*space, posy+15+8*space,  R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 3 (up)
		rectangleRGBA(frame_renderer, posx+16+10*space, posy+14+8*space,  posx+19+12*space, posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 4 (left)
		rectangleRGBA(frame_renderer, posx+20+14*space, posy+14+8*space,  posx+23+16*space, posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 2 (right)
		rectangleRGBA(frame_renderer, posx+18+12*space, posy+16+10*space, posx+21+14*space, posy+19+12*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 1 (down)
		rectangleRGBA(frame_renderer, posx+17+11*space, posy+20+12*space, posx+22+15*space, posy+25+16*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT STICK
		color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+2+space,     posy+21+12*space, posx+4+5*space,   posy+23+16*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// LEFT STICK
		boxRGBA(frame_renderer, posx+18+11*space, posy+21+12*space, posx+20+15*space, posy+23+16*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// RIGHT STICK
		color = palette_pc[1];	// Black
		lineRGBA(frame_renderer, posx+3+3*space, posy+21+13*space, posx+3+3*space, posy+22+15*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// LEFT STICK
		lineRGBA(frame_renderer, posx+2+2*space, posy+22+14*space, posx+3+4*space, posy+22+14*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// LEFT STICK
		lineRGBA(frame_renderer, posx+18+13*space, posy+21+13*space, posx+18+13*space, posy+22+15*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// RIGHT STICK
		lineRGBA(frame_renderer, posx+17+12*space, posy+22+14*space, posx+18+14*space, posy+22+14*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// RIGHT STICK
		// Button L3
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L3))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[6];	// Cyan
		boxRGBA(frame_renderer, posx+3+2*space, posy+1, posx+3+4*space, posy+1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button L2
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[2];	// Blue Light
		boxRGBA(frame_renderer, posx+3+2*space, posy+5+2*space, posx+3+4*space, posy+5+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button L1
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[0];	// Blue medium
		boxRGBA(frame_renderer, posx+3+2*space, posy+9+4*space, posx+3+4*space, posy+9+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button UP
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
			color = palette_pc[4];	// Green light
		else if(analoglefty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty0 <= EMUSCV_AXIS_NEUTRAL_MIN)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+3+2*space, posy+13+6*space, posx+3+4*space, posy+13+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button LEFT
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
			color = palette_pc[4];	// Green light
		else if(analogleftx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx0 <= EMUSCV_AXIS_NEUTRAL_MIN)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+15+8*space, posx+1+2*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button RIGHT
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
			color = palette_pc[4];	// Green light
		else if(analogleftx0 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrightx0 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+5+4*space, posy+15+8*space, posx+5+6*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button DOWN
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
			color = palette_pc[4];	// Green light
		else if(analoglefty0 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrighty0 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+3+2*space, posy+17+10*space, posx+3+4*space, posy+17+12*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Left stick
		if(analogleftx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogleftx0 >= EMUSCV_AXIS_NEUTRAL_MAX || analoglefty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analoglefty0 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[15];	// White
		joyposx = (analogleftx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogleftx0 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogleftx0 : 0);
		joyposy = (analoglefty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analoglefty0 >= EMUSCV_AXIS_NEUTRAL_MAX ? analoglefty0 : 0);
		boxRGBA(frame_renderer, (int16_t)(posx+3+3*space-space/4)+(int16_t)(1+2*space-space/4)*joyposx/INT16_MAX, (int16_t)(posy+22+14*space-space/4)+(int16_t)(1+2*space-space/4)*joyposy/INT16_MAX, (int16_t)(posx+3+3*space+space/4)+(int16_t)(1+2*space-space/4)*joyposx/INT16_MAX, (int16_t)(posy+22+14*space+space/4)+(int16_t)(1+2*space-space/4)*joyposy/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button SELECT
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[13];	// Green dark
		boxRGBA(frame_renderer, posx+9+6*space, posy+15+8*space, posx+9+8*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button START
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[5];	// Green soft
		boxRGBA(frame_renderer, posx+13+8*space, posy+15+8*space, posx+13+10*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R3
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[11];	// Pink
		boxRGBA(frame_renderer, posx+19+12*space, posy+1, posx+19+14*space, posy+1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R2
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[10];	// Fuschia
		boxRGBA(frame_renderer, posx+19+12*space, posy+5+2*space, posx+19+14*space, posy+5+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R1
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[3];	// Purple
		boxRGBA(frame_renderer, posx+19+12*space, posy+9+4*space, posx+19+14*space, posy+9+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 3 (up)
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[9];	// Orange
		boxRGBA(frame_renderer, posx+19+12*space, posy+13+6*space,  posx+19+14*space, posy+13+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 4 (left)
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[2];	// Blue light
		boxRGBA(frame_renderer, posx+17+10*space, posy+15+8*space, posx+17+12*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 2 (right)
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[7];	// Green medium
		boxRGBA(frame_renderer, posx+21+14*space, posy+15+8*space,  posx+21+16*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 1 (down)
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[8];	// Red
		boxRGBA(frame_renderer, posx+19+12*space, posy+17+10*space, posx+19+14*space, posy+17+12*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Right stick
		if(analogrightx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx0 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrighty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty0 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[15];	// White
		joyposx = (analogrightx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx0 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogrightx0 : 0);
		joyposy = (analogrighty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty0 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogrighty0 : 0);
		boxRGBA(frame_renderer, (int16_t)(posx+18+13*space-space/4)+(int16_t)(1+2*space-space/4)*joyposx/INT16_MAX, (int16_t)(posy+22+14*space-space/4)+(int16_t)(1+2*space-space/4)*joyposy/INT16_MAX, (int16_t)(posx+18+13*space+space/4)+(int16_t)(1+2*space-space/4)*joyposx/INT16_MAX, (int16_t)(posy+22+14*space+space/4)+(int16_t)(1+2*space-space/4)*joyposy/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);


		// SECOND CONTROLLER (RIGHT SIDE, BLUE)
		posx = config.window_width-24-17*space;
		posy = config.window_height-26-17*space;
		// Contour
		color = palette_pc[1];	// Black
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy,             posx+5+4*space,   posy+3+2*space,   R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// L3
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy+4+2*space,   posx+5+4*space,   posy+7+4*space,   R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// L2
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy+8+4*space,   posx+5+4*space,   posy+11+6*space,  R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// L1
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy+12+6*space,  posx+5+4*space,   posy+15+8*space,  R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// UP
		rectangleRGBA(frame_renderer, posx,             posy+14+8*space,  posx+3+2*space,   posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT
		rectangleRGBA(frame_renderer, posx+4+4*space,   posy+14+8*space,  posx+7+6*space,   posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT
		rectangleRGBA(frame_renderer, posx+2+2*space,   posy+16+10*space, posx+5+4*space,   posy+19+12*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// DOWN
		rectangleRGBA(frame_renderer, posx+1+space,     posy+20+12*space, posx+6+5*space,   posy+25+16*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT STICK
		rectangleRGBA(frame_renderer, posx+8+6*space,   posy+14+8*space,  posx+11+8*space,  posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// SELECT
		rectangleRGBA(frame_renderer, posx+12+8*space,  posy+14+8*space,  posx+15+10*space, posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// START
		rectangleRGBA(frame_renderer, posx+18+12*space, posy,             posx+21+14*space, posy+3+2*space,   R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R3
		rectangleRGBA(frame_renderer, posx+18+12*space, posy+4+2*space,   posx+21+14*space, posy+7+4*space,   R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R2
		rectangleRGBA(frame_renderer, posx+18+12*space, posy+8+4*space,   posx+21+14*space, posy+11+6*space,  R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R1
		rectangleRGBA(frame_renderer, posx+18+12*space, posy+12+6*space,  posx+21+14*space, posy+15+8*space,  R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 3 (up)
		rectangleRGBA(frame_renderer, posx+16+10*space, posy+14+8*space,  posx+19+12*space, posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 4 (left)
		rectangleRGBA(frame_renderer, posx+20+14*space, posy+14+8*space,  posx+23+16*space, posy+17+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 2 (right)
		rectangleRGBA(frame_renderer, posx+18+12*space, posy+16+10*space, posx+21+14*space, posy+19+12*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 1 (down)
		rectangleRGBA(frame_renderer, posx+17+11*space, posy+20+12*space, posx+22+15*space, posy+25+16*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT STICK
		color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+2+space,     posy+21+12*space, posx+4+5*space,   posy+23+16*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// LEFT STICK
		boxRGBA(frame_renderer, posx+18+11*space, posy+21+12*space, posx+20+15*space, posy+23+16*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// RIGHT STICK
		color = palette_pc[1];	// Black
		lineRGBA(frame_renderer, posx+3+3*space, posy+21+13*space, posx+3+3*space, posy+22+15*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// LEFT STICK
		lineRGBA(frame_renderer, posx+2+2*space, posy+22+14*space, posx+3+4*space, posy+22+14*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// LEFT STICK
		lineRGBA(frame_renderer, posx+18+13*space, posy+21+13*space, posx+18+13*space, posy+22+15*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// RIGHT STICK
		lineRGBA(frame_renderer, posx+17+12*space, posy+22+14*space, posx+18+14*space, posy+22+14*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// RIGHT STICK
		// Button L3
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L3))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[6];	// Cyan
		boxRGBA(frame_renderer, posx+3+2*space, posy+1, posx+3+4*space, posy+1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button L2
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[2];	// Blue Light
		boxRGBA(frame_renderer, posx+3+2*space, posy+5+2*space, posx+3+4*space, posy+5+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button L1
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[0];	// Blue medium
		boxRGBA(frame_renderer, posx+3+2*space, posy+9+4*space, posx+3+4*space, posy+9+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button UP
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
			color = palette_pc[4];	// Green light
		else if(analoglefty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty1 <= EMUSCV_AXIS_NEUTRAL_MIN)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+3+2*space, posy+13+6*space, posx+3+4*space, posy+13+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button LEFT
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
			color = palette_pc[4];	// Green light
		else if(analogleftx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx1 <= EMUSCV_AXIS_NEUTRAL_MIN)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+15+8*space, posx+1+2*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button RIGHT
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
			color = palette_pc[4];	// Green light
		else if(analogleftx1 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrightx1 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+5+4*space, posy+15+8*space, posx+5+6*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button DOWN
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
			color = palette_pc[4];	// Green light
		else if(analoglefty1 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrighty1 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+3+2*space, posy+17+10*space, posx+3+4*space, posy+17+12*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Left stick
		if(analogleftx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogleftx1 >= EMUSCV_AXIS_NEUTRAL_MAX || analoglefty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analoglefty1 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[15];	// White
		joyposx = (analogleftx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogleftx1 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogleftx1 : 0);
		joyposy = (analoglefty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analoglefty1 >= EMUSCV_AXIS_NEUTRAL_MAX ? analoglefty1 : 0);
		boxRGBA(frame_renderer, (int16_t)(posx+3+3*space-space/4)+(int16_t)(1+2*space-space/4)*joyposx/INT16_MAX, (int16_t)(posy+22+14*space-space/4)+(int16_t)(1+2*space-space/4)*joyposy/INT16_MAX, (int16_t)(posx+3+3*space+space/4)+(int16_t)(1+2*space-space/4)*joyposx/INT16_MAX, (int16_t)(posy+22+14*space+space/4)+(int16_t)(1+2*space-space/4)*joyposy/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button SELECT
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[13];	// Green dark
		boxRGBA(frame_renderer, posx+9+6*space, posy+15+8*space, posx+9+8*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button START
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[5];	// Green soft
		boxRGBA(frame_renderer, posx+13+8*space, posy+15+8*space, posx+13+10*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R3
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[11];	// Pink
		boxRGBA(frame_renderer, posx+19+12*space, posy+1, posx+19+14*space, posy+1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R2
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[10];	// Fuschia
		boxRGBA(frame_renderer, posx+19+12*space, posy+5+2*space, posx+19+14*space, posy+5+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R1
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[3];	// Purple
		boxRGBA(frame_renderer, posx+19+12*space, posy+9+4*space, posx+19+14*space, posy+9+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 3 (up)
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[9];	// Orange
		boxRGBA(frame_renderer, posx+19+12*space, posy+13+6*space,  posx+19+14*space, posy+13+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 4 (left)
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[2];	// Blue light
		boxRGBA(frame_renderer, posx+17+10*space, posy+15+8*space, posx+17+12*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 2 (right)
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[7];	// Green medium
		boxRGBA(frame_renderer, posx+21+14*space, posy+15+8*space,  posx+21+16*space, posy+15+10*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 1 (down)
		if(ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[8];	// Red
		boxRGBA(frame_renderer, posx+19+12*space, posy+17+10*space, posx+19+14*space, posy+17+12*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Right stick
		if(analogrightx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx1 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrighty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty1 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[15];	// White
		joyposx = (analogrightx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx1 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogrightx1 : 0);
		joyposy = (analogrighty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty1 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogrighty1 : 0);
		boxRGBA(frame_renderer, (int16_t)(posx+18+13*space-space/4)+(int16_t)(1+2*space-space/4)*joyposx/INT16_MAX, (int16_t)(posy+22+14*space-space/4)+(int16_t)(1+2*space-space/4)*joyposy/INT16_MAX, (int16_t)(posx+18+13*space+space/4)+(int16_t)(1+2*space-space/4)*joyposx/INT16_MAX, (int16_t)(posy+22+14*space+space/4)+(int16_t)(1+2*space-space/4)*joyposy/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);


		// KEYBOARD
		posx = config.window_width/2-6*space;
		posy = config.window_height-1-12*space;
		// Contour
		color = palette_pc[1];	// Black
		rectangleRGBA(frame_renderer, posx,           posy,           posx+4+4*space,  posy+1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// POWER ON/OFF BUTTON
		rectangleRGBA(frame_renderer, posx,           posy+3+3*space, posx+4+4*space,  posy+4+5*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RESET BUTTON
		rectangleRGBA(frame_renderer, posx,           posy+6+6*space, posx+4+4*space,  posy+7+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// PAUSE BUTTON
		rectangleRGBA(frame_renderer, posx+5*space,   posy,           posx+7*space,    posy+1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 7
		rectangleRGBA(frame_renderer, posx+1+7*space, posy,           posx+1+9*space,  posy+1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 8
		rectangleRGBA(frame_renderer, posx+2+9*space, posy,           posx+2+11*space, posy+1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 9
		rectangleRGBA(frame_renderer, posx+5*space,   posy+2+2*space, posx+7*space,    posy+3+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 4
		rectangleRGBA(frame_renderer, posx+1+7*space, posy+2+2*space, posx+1+9*space,  posy+3+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 5
		rectangleRGBA(frame_renderer, posx+2+9*space, posy+2+2*space, posx+2+11*space, posy+3+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 6
		rectangleRGBA(frame_renderer, posx+5*space,   posy+4+4*space, posx+7*space,    posy+5+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 1
		rectangleRGBA(frame_renderer, posx+1+7*space, posy+4+4*space, posx+1+9*space,  posy+5+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 2
		rectangleRGBA(frame_renderer, posx+2+9*space, posy+4+4*space, posx+2+11*space, posy+5+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 3
		rectangleRGBA(frame_renderer, posx+5*space,   posy+6+6*space, posx+7*space,    posy+7+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// 0
		rectangleRGBA(frame_renderer, posx+1+7*space, posy+6+6*space, posx+1+9*space,  posy+7+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// CL
		rectangleRGBA(frame_renderer, posx+2+9*space, posy+6+6*space, posx+2+11*space, posy+7+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// EN

		// Key POWER ON/OFF
		if (keyPower != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+1, posx+2+4*space, posy-1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		color = palette_pc[1];	// Black
		if (is_power_on)
//			rectangleRGBA(frame_renderer, posx+3+3*space, posy+1, posx+3+4*space, posy+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
			rectangleRGBA(frame_renderer, posx+3+2*space, posy+1, posx+3+3*space, posy+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		else
//			rectangleRGBA(frame_renderer, posx+1, posy+1, posx+1+space, posy+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
			rectangleRGBA(frame_renderer, posx+1+space, posy+1, posx+1+2*space, posy+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key RESET
		if (key_pressed_reset)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+4+3*space, posx+2+4*space, posy+2+5*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key PAUSE
		if (keyPause != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+7+6*space, posx+2+4*space, posy+5+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 7
		if (key7 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1+5*space, posy+1, posx-2+7*space, posy-1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 8
		if (key8 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+2+7*space, posy+1, posx-1+9*space,  posy-1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 9
		if (key9 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+3+9*space, posy+1, posx+11*space, posy-1+2*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 4
		if (key4 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1+5*space, posy+2+2*space, posx-2+7*space, posy+1+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 5
		if (key5 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+2+7*space, posy+3+2*space, posx-1+9*space,  posy+1+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 6
		if (key6 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer,posx+3+9*space, posy+3+2*space, posx+11*space, posy+1+4*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 1
		if (key1 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1+5*space, posy+5+4*space, posx-2+7*space, posy+3+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 2
		if (key2 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+2+7*space, posy+5+4*space, posx-1+9*space, posy+3+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 3
		if (key3 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+3+9*space, posy+5+4*space, posx+11*space, posy+3+6*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 0
		if (key0 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1+5*space, posy+7+6*space, posx-2+7*space, posy+5+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key CL
		if (keyClear != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+2+7*space, posy+7+6*space, posx-1+9*space,  posy+5+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key EN
		if (keyEnter != 0)
			color = palette_pc[4];	// Green light
		else if ((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+3+9*space, posy+7+6*space, posx+11*space, posy+5+8*space, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Inputs drawn\n", EMUSCV_NAME);
	}

	// Call audio callback
	if(escv_emu && is_power_on)
	{
		int16_t *sound_ptr = escv_emu->get_osd()->get_sound_ptr();
		if(!sound_ptr)
		{
			for (uint32_t i = sound_buffer_samples+1; --i > 0; )
				RetroAudioSample(0, 0);
		}
		else
		{
			int16_t sound_val;
			for (uint32_t i = 0; i < sound_buffer_samples; )
			{
				sound_val = sound_ptr[i];
				RetroAudioSample(sound_val, sound_val);
				i++;
			}
		}
	}
	else
	{
		for (uint32_t i = sound_buffer_samples+1; --i > 0; )
		{
			RetroAudioSample(sound_buffer_noise[sound_buffer_noise_index], sound_buffer_noise[sound_buffer_noise_index]);
			if(++sound_buffer_noise_index >= EMUSCV_NOISE_SAMPLES)
				sound_buffer_noise_index = rand() % (EMUSCV_NOISE_SAMPLES - 1);
		}
	}
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] => RetroAudioSample()\n", EMUSCV_NAME);

	// Call video callback
	RetroVideoRefresh(frame_surface->pixels, config.window_width, config.window_height,  config.window_width*sizeof(uint32_t));
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] RetroVideoRefresh()\n", EMUSCV_NAME);
}

//
// Libretro: load core settings
//
void cEmuSCV::RetroLoadSettings(void)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroLoadSettings()\n", EMUSCV_NAME);

	config.sound_frequency = 5;
	config.sound_latency = 0;

	struct retro_variable var;
	
	// CONSOLE
	config.scv_console = SETTING_CONSOLE_AUTO_VAL;
	var.key   = SETTING_CONSOLE_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[16];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_CONSOLE_EPOCH_KEY)) == 0)
			config.scv_console = SETTING_CONSOLE_EPOCH_VAL;
		else if(strcmp(str, _T(SETTING_CONSOLE_YENO_KEY)) == 0)
			config.scv_console = SETTING_CONSOLE_YENO_VAL;
		else if(strcmp(str, _T(SETTING_CONSOLE_EPOCHLADY_KEY)) == 0)
			config.scv_console = SETTING_CONSOLE_EPOCHLADY_VAL;
	}
	switch(config.scv_console)
	{
		case SETTING_CONSOLE_EPOCH_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Console setting: EPOCH\n", EMUSCV_NAME);
			break;
		case SETTING_CONSOLE_YENO_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Console setting: YENO\n", EMUSCV_NAME);
			break;
		case SETTING_CONSOLE_EPOCHLADY_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Console setting: EPOCH LADY\n", EMUSCV_NAME);
			break;
		case SETTING_CONSOLE_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Console setting: automatic (EPOCH)\n", EMUSCV_NAME);
			break;
	}

	// DISPLAY
	config.scv_display = SETTING_DISPLAY_AUTO_VAL;
	var.key   = SETTING_DISPLAY_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_DISPLAY_EMUSCV_KEY)) == 0)
			config.scv_display = SETTING_DISPLAY_EMUSCV_VAL;
		else if(strcmp(str, _T(SETTING_DISPLAY_EPOCH_KEY)) == 0)
			config.scv_display = SETTING_DISPLAY_EPOCH_VAL;
		else if(strcmp(str, _T(SETTING_DISPLAY_YENO_KEY)) == 0)
			config.scv_display = SETTING_DISPLAY_YENO_VAL;
	}
	switch(config.scv_display)
	{
		case SETTING_DISPLAY_EMUSCV_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display setting: as EmuSCV (custom)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAY_EPOCH_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display setting: as EPOCH (genuine)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAY_YENO_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display setting: as YENO (genuine)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAY_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display setting: automatic (depending of console option)\n", EMUSCV_NAME);
			break;
	}

	// PIXEL ASPECT
	config.scv_pixelaspect = SETTING_PIXELASPECT_AUTO_VAL;
	var.key   = SETTING_PIXELASPECT_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_PIXELASPECT_RECTANGULAR_KEY)) == 0)
			config.scv_pixelaspect = SETTING_PIXELASPECT_RECTANGULAR_VAL;
		else if(strcmp(str, _T(SETTING_PIXELASPECT_SQUARE_KEY)) == 0)
			config.scv_pixelaspect = SETTING_PIXELASPECT_SQUARE_VAL;
	}
	switch(config.scv_pixelaspect)
	{
		case SETTING_PIXELASPECT_RECTANGULAR_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Pixel aspect setting: rectangular (genuine)\n", EMUSCV_NAME);
			break;
		case SETTING_PIXELASPECT_SQUARE_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Pixel aspect setting: square (custom)\n", EMUSCV_NAME);
			break;
		case SETTING_PIXELASPECT_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Pixel aspect setting: automatic (rectangular)\n", EMUSCV_NAME);
			break;
	}

	// RESOLUTION
	config.scv_resolution = SETTING_RESOLUTION_AUTO_VAL;
	var.key   = SETTING_RESOLUTION_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_RESOLUTION_LOW_KEY)) == 0)
			config.scv_resolution = SETTING_RESOLUTION_LOW_VAL;
		else if(strcmp(str, _T(SETTING_RESOLUTION_MEDIUM_KEY)) == 0)
			config.scv_resolution = SETTING_RESOLUTION_MEDIUM_VAL;
		else if(strcmp(str, _T(SETTING_RESOLUTION_HIGH_KEY)) == 0)
			config.scv_resolution = SETTING_RESOLUTION_HIGH_VAL;
	}
	switch(config.scv_resolution)
	{
		case SETTING_RESOLUTION_LOW_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Resolution setting: low\n", EMUSCV_NAME);
			break;
		case SETTING_RESOLUTION_MEDIUM_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Resolution setting: medium\n", EMUSCV_NAME);
			break;
		case SETTING_RESOLUTION_HIGH_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Resolution setting: high\n", EMUSCV_NAME);
			break;
		case SETTING_RESOLUTION_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Resolution setting: automatic (depending of the platform)\n", EMUSCV_NAME);
			break;
	}

	// PALETTE
	config.scv_palette = SETTING_PALETTE_AUTO_VAL;
	var.key   = SETTING_PALETTE_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_PALETTE_STANDARD_KEY)) == 0)
			config.scv_palette = SETTING_PALETTE_STANDARD_VAL;
		else if(strcmp(str, _T(SETTING_PALETTE_OLDNTSC_KEY)) == 0)
			config.scv_palette = SETTING_PALETTE_OLDNTSC_VAL;
	}
	switch(config.scv_palette)
	{
		case SETTING_PALETTE_STANDARD_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Palette setting: standard PAL/NEW NTSC colors\n", EMUSCV_NAME);
			break;
		case SETTING_PALETTE_OLDNTSC_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Resolution setting: old NTSC colors\n", EMUSCV_NAME);
			break;
		case SETTING_PALETTE_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Resolution setting: automatic (standard PAL/NEW NTSC colors)\n", EMUSCV_NAME);
			break;
	}

	// FPS
	config.scv_fps = SETTING_FPS_AUTO_VAL;
	var.key   = SETTING_FPS_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_FPS_EPOCH60_KEY)) == 0)
			config.scv_fps = SETTING_FPS_EPOCH60_VAL;
		else if(strcmp(str, _T(SETTING_FPS_YENO50_KEY)) == 0)
			config.scv_fps = SETTING_FPS_YENO50_VAL;
	}
	switch(config.scv_fps)
	{
		case SETTING_FPS_EPOCH60_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] FPS setting: 60 (EPOCH)\n", EMUSCV_NAME);
			break;
		case SETTING_FPS_YENO50_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] FPS setting: 50 (YENO)\n", EMUSCV_NAME);
			break;
		case SETTING_FPS_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] FPS setting: automatic (depending of the console option)\n", EMUSCV_NAME);
			break;
	}

	// DISPLAY FULL
	config.scv_displayfullmemory = SETTING_DISPLAYFULLMEMORY_AUTO_VAL;
	var.key   = SETTING_DISPLAYFULLMEMORY_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_DISPLAYFULLMEMORY_YES_KEY)) == 0)
			config.scv_displayfullmemory = SETTING_DISPLAYFULLMEMORY_YES_VAL;
		else if(strcmp(str, _T(SETTING_DISPLAYFULLMEMORY_NO_KEY)) == 0)
			config.scv_displayfullmemory = SETTING_DISPLAYFULLMEMORY_NO_VAL;
	}
	switch(config.scv_displayfullmemory)
	{
		case SETTING_DISPLAYFULLMEMORY_YES_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display full memory setting: yes (developer mode)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAYFULLMEMORY_NO_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display full memory setting: no (normal mode)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAYFULLMEMORY_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display full memory setting: automatic (no)\n", EMUSCV_NAME);
			break;
	}

	// DISPLAY INPUTS
	config.scv_displayinputs = SETTING_DISPLAYINPUTS_AUTO_VAL;
	var.key   = SETTING_DISPLAYINPUTS_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_DISPLAYINPUTS_YES_KEY)) == 0)
			config.scv_displayinputs = SETTING_DISPLAYINPUTS_YES_VAL;
		else if(strcmp(str, _T(SETTING_DISPLAYINPUTS_NO_KEY)) == 0)
			config.scv_displayinputs = SETTING_DISPLAYINPUTS_NO_VAL;
	}
	switch(config.scv_displayinputs)
	{
		case SETTING_DISPLAYINPUTS_YES_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display inputs setting: yes (developer mode)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAYINPUTS_NO_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display inputs setting: no (normal mode)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAYINPUTS_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display inputs setting: automatic (no)\n", EMUSCV_NAME);
			break;
	}

	// LANGAGE
	config.scv_langage	= SETTING_LANGAGE_AUTO_VAL;
	var.key		= SETTING_LANGAGE_KEY;
	var.value	= NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_LANGAGE_JP_KEY)) == 0)
			config.scv_langage = SETTING_LANGAGE_JP_VAL;
		else if(strcmp(str, _T(SETTING_LANGAGE_FR_KEY)) == 0)
			config.scv_langage = SETTING_LANGAGE_FR_VAL;
		else if(strcmp(str, _T(SETTING_LANGAGE_EN_KEY)) == 0)
			config.scv_langage = SETTING_LANGAGE_EN_VAL;
	}
	switch(config.scv_langage)
	{
		case SETTING_LANGAGE_JP_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Langage setting: Japanese (genuine EPOCH)\n", EMUSCV_NAME);
			break;
		case SETTING_LANGAGE_FR_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Langage setting: French (genuine YENO)\n", EMUSCV_NAME);
			break;
		case SETTING_LANGAGE_EN_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Langage setting: English (custom)\n", EMUSCV_NAME);
			break;
		case SETTING_LANGAGE_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Langage setting: automatic (depending of console)\n", EMUSCV_NAME);
			break;
	}

	// CHECK BIOS
	config.scv_checkbios = SETTING_CHECKBIOS_AUTO_VAL;
	var.key   = SETTING_CHECKBIOS_KEY;
	var.value = NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_CHECKBIOS_YES_KEY)) == 0)
			config.scv_checkbios = SETTING_CHECKBIOS_YES_VAL;
		else if(strcmp(str, _T(SETTING_CHECKBIOS_NO_KEY)) == 0)
			config.scv_checkbios = SETTING_CHECKBIOS_NO_VAL;
	}
	switch(config.scv_checkbios)
	{
		case SETTING_CHECKBIOS_YES_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Check BIOS setting: yes\n", EMUSCV_NAME);
			break;
		case SETTING_CHECKBIOS_NO_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Check BIOS setting: no\n", EMUSCV_NAME);
			break;
		case SETTING_CHECKBIOS_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Check BIOS setting: automatic (yes)\n", EMUSCV_NAME);
			break;
	}

	apply_display_config();	
	if(config.window_palette == SETTING_PALETTE_OLDNTSC_VAL)
		palette_pc = (scrntype_t *)palette_ntsc;
	else
		palette_pc = (scrntype_t *)palette_pal;
	if(escv_emu)
	{
		escv_emu->get_osd()->reset_palette();
		escv_emu->get_osd()->reset_sound();
	}
	sound_buffer_samples = (AUDIO_SAMPLING_RATE / config.window_fps + 0.5);

	// Create quick keyboard SDL surface & renderer
	CreateKeyboardSurface();

	// Create EmuSCV logo SDL surface & renderer
	CreateLogoSurface();

	// Create Recalbox logo SDL surface & renderer
	CreateRecalboxSurface();

//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] apply_display_config()\n", EMUSCV_NAME);
}

//
// Libretro: save core settings
//
void cEmuSCV::RetroSaveSettings(void)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSaveSettings()\n", EMUSCV_NAME);

	struct retro_variable variable[11];

	variable[0].key = SETTING_CONSOLE_KEY;
	switch(config.scv_console)
	{
		case SETTING_CONSOLE_EPOCH_VAL:
			variable[0].value = "CONSOLE; EPOCH";
			break;
		case SETTING_CONSOLE_YENO_VAL:
			variable[0].value = "CONSOLE; YENO";
			break;
		case SETTING_CONSOLE_EPOCHLADY_VAL:
			variable[0].value = "CONSOLE; EPOCHLADY";
			break;
		case SETTING_CONSOLE_AUTO_VAL:
		default:
			variable[0].value = "CONSOLE; AUTO";
			break;
	}
	
	variable[1].key = SETTING_DISPLAY_KEY;
	switch(config.scv_display)
	{
		case SETTING_DISPLAY_EMUSCV_VAL:
			variable[1].value = "DISPLAY; EMUSCV";
			break;
		case SETTING_DISPLAY_EPOCH_VAL:
			variable[1].value = "DISPLAY; EPOCH";
			break;
		case SETTING_DISPLAY_YENO_VAL:
			variable[1].value = "DISPLAY; YENO";
			break;
		case SETTING_DISPLAY_AUTO_VAL:
		default:
			variable[1].value = "DISPLAY; AUTO";
			break;
	}
	
	variable[2].key = SETTING_PIXELASPECT_KEY;
	switch(config.scv_pixelaspect)
	{
		case SETTING_PIXELASPECT_SQUARE_VAL:
			variable[2].value = "PIXELASPECT; SQUARE";
			break;
		case SETTING_PIXELASPECT_RECTANGULAR_VAL:
			variable[2].value = "PIXELASPECT; RECTANGULAR";
			break;
		case SETTING_PIXELASPECT_AUTO_VAL:
		default:
			variable[2].value = "PIXELASPECT; AUTO";
			break;
	}
	
	variable[3].key = SETTING_RESOLUTION_KEY;
	switch(config.scv_resolution)
	{
		case SETTING_RESOLUTION_LOW_VAL:
			variable[3].value = "RESOLUTION; LOW";
			break;
		case SETTING_RESOLUTION_MEDIUM_VAL:
			variable[3].value = "RESOLUTION; MEDIUM";
			break;
		case SETTING_RESOLUTION_HIGH_VAL:
			variable[3].value = "RESOLUTION; HIGH";
			break;
		case SETTING_RESOLUTION_AUTO_VAL:
		default:
			variable[3].value = "RESOLUTION; AUTO";
			break;
	}
	
	variable[4].key = SETTING_PALETTE_KEY;
	switch(config.scv_palette)
	{
		case SETTING_PALETTE_STANDARD_VAL:
			variable[4].value = "PALETTE; STANDARD";
			break;
		case SETTING_PALETTE_OLDNTSC_VAL:
			variable[4].value = "PALETTE; OLDNTSC";
			break;
		case SETTING_PALETTE_AUTO_VAL:
		default:
			variable[4].value = "PALETTE; AUTO";
			break;
	}
	
	variable[5].key = SETTING_FPS_KEY;
	switch(config.scv_fps)
	{
		case SETTING_FPS_EPOCH60_VAL:
			variable[5].value = "FPS; EPOCH60";
			break;
		case SETTING_FPS_YENO50_VAL:
			variable[5].value = "FPS; YENO50";
			break;
		case SETTING_FPS_AUTO_VAL:
		default:
			variable[5].value = "FPS; AUTO";
			break;
	}

	variable[6].key = SETTING_DISPLAYFULLMEMORY_KEY;
	switch(config.scv_displayfullmemory)
	{
		case SETTING_DISPLAYFULLMEMORY_YES_VAL:
			variable[6].value = "DISPLAYFULLMEMORY; YES";
			break;
		case SETTING_DISPLAYFULLMEMORY_NO_VAL:
			variable[6].value = "DISPLAYFULLMEMORY; NO";
			break;
		case SETTING_DISPLAYFULLMEMORY_AUTO_VAL:
		default:
			variable[6].value = "DISPLAYFULLMEMORY; AUTO";
			break;
	}
	
	variable[7].key = SETTING_DISPLAYINPUTS_KEY;
	switch(config.scv_displayinputs)
	{
		case SETTING_DISPLAYINPUTS_YES_VAL:
			variable[7].value = "DISPLAYINPUTS; YES";
			break;
		case SETTING_DISPLAYINPUTS_NO_VAL:
			variable[7].value = "DISPLAYINPUTS; NO";
			break;
		case SETTING_DISPLAYINPUTS_AUTO_VAL:
		default:
			variable[7].value = "DISPLAYINPUTS; AUTO";
			break;
	}
	
	variable[8].key = SETTING_LANGAGE_KEY;
	switch(config.scv_langage)
	{
		case SETTING_LANGAGE_JP_VAL:
			variable[8].value = "LANGAGE; JP";
			break;
		case SETTING_LANGAGE_FR_VAL:
			variable[8].value = "LANGAGE; FR";
			break;
		case SETTING_LANGAGE_EN_VAL:
			variable[8].value = "LANGAGE; EN";
			break;
		case SETTING_LANGAGE_AUTO_VAL:
		default:
			variable[8].value = "LANGAGE; AUTO";
			break;
	}
	
	variable[9].key = SETTING_CHECKBIOS_KEY;
	switch(config.scv_checkbios)
	{
		case SETTING_CHECKBIOS_NO_VAL:
			variable[9].value = "CHECK BIOS; NO";
			break;
		case SETTING_CHECKBIOS_YES_VAL:
			variable[9].value = "CHECK BIOS; YES";
			break;
		case SETTING_CHECKBIOS_AUTO_VAL:
		default:
			variable[9].value = "CHECK BIOS; AUTO";
			break;
	}

	variable[10].key = NULL;
	variable[10].value = NULL;

	RetroEnvironment(RETRO_ENVIRONMENT_SET_VARIABLES, variable);

	// Set possible values
	variable[0].value = "CONSOLE; AUTO|EPOCH|YENO|EPOCHLADY";
	variable[1].value = "DISPLAY; AUTO|EMUSCV|EPOCH|YENO";
	variable[2].value = "PIXELASPECT; AUTO|RECTANGULAR|SQUARE";
	variable[3].value = "RESOLUTION; AUTO|LOW|MEDIUM|HIGH";
	variable[4].value = "PALETTE; AUTO|STANDARD|OLDNTSC";
	variable[5].value = "FPS; AUTO|EPOCH60|YENO50";
	variable[6].value = "DISPLAYFULLMEMORY; AUTO|YES|NO";
	variable[7].value = "DISPLAYINPUTS; AUTO|YES|NO";
	variable[8].value = "LANGAGE; AUTO|JP|FR|EN";
	variable[9].value = "CHECKBIOS; AUTO|YES|NO";
	RetroEnvironment(RETRO_ENVIRONMENT_SET_VARIABLES, variable);

	apply_display_config();
	if(config.window_palette == SETTING_PALETTE_OLDNTSC_VAL)
		palette_pc = (scrntype_t *)palette_ntsc;
	else
		palette_pc = (scrntype_t *)palette_pal;
	if(escv_emu)
	{
		escv_emu->get_osd()->reset_palette();
		escv_emu->get_osd()->reset_sound();
	}
	sound_buffer_samples = (AUDIO_SAMPLING_RATE / config.window_fps + 0.5);
}

//
// Libretro: reset the Libretro core
//
void cEmuSCV::RetroReset(bool save_cart)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroReset()\n", EMUSCV_NAME);
	if(escv_emu)
	{
		if(save_cart)
			escv_emu->save_cart(0);
		escv_emu->reset();
	}
}

// 
// Create the SDL surface and the SDL renderer and load the picture for the quick keyboard overlay
// 
bool cEmuSCV::CreateKeyboardSurface()
{
	int x, y, w, h;
	unsigned char *image;


	DestroyKeyboardSurface();

	if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
	{
		w = 460;
		h = 312;
		if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
			image = (unsigned char *)src_res_keyboardlady460x312xrgba8888_data;
		else if(config.window_console == SETTING_CONSOLE_YENO_VAL)
			image = (unsigned char *)src_res_keyboardyeno460x312xrgba8888_data;
		else
			image = (unsigned char *)src_res_keyboardepoch460x312xrgba8888_data;
	}
	else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
	{
		w = 230;
		h = 156;
		if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
			image = (unsigned char *)src_res_keyboardlady230x156xrgba8888_data;
		else if(config.window_console == SETTING_CONSOLE_YENO_VAL)
			image = (unsigned char *)src_res_keyboardyeno230x156xrgba8888_data;
		else
			image = (unsigned char *)src_res_keyboardepoch230x156xrgba8888_data;
	}
	else
	{
		w = 115;
		h = 78;
		if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
			image = (unsigned char *)src_res_keyboardlady115x78xrgba8888_data;
		else if(config.window_console == SETTING_CONSOLE_YENO_VAL)
			image = (unsigned char *)src_res_keyboardyeno115x78xrgba8888_data;
		else
			image = (unsigned char *)src_res_keyboardepoch115x78xrgba8888_data;
	}

	// Create SDL quick keyboard surface
    keyboard_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8*sizeof(uint32_t), 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!keyboard_surface)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface creation failed! (quick keyboard) %s\n", EMUSCV_NAME, SDL_GetError());
        return false;
	}
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface created (quick keyboard)\n", EMUSCV_NAME);

	// Create SDL quick keyboard renderer
    keyboard_renderer = SDL_CreateSoftwareRenderer(keyboard_surface);
    if (!keyboard_renderer)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface renderer creation failed! (quick keyboard) %s\n", EMUSCV_NAME, SDL_GetError());
        return false;
    }
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer created (quick keyboard)\n", EMUSCV_NAME);

	// Draw an embedded binary image
	for (int16_t x, y = h; --y >= 0; )
		for (x = w; --x >= 0; )
			pixelRGBA(keyboard_renderer, x, y, image[4*(x+y*w)], image[4*(x+y*w)+1], image[4*(x+y*w)+2], image[4*(x+y*w)+3]);

	return true;
}

// 
// Destroy the SDL surface and the SDL renderer for the quick keyboard overlay
// 
bool cEmuSCV::DestroyKeyboardSurface()
{
	// Free SDL quick keyboard renderer
	if (keyboard_renderer)
	{
		SDL_DestroyRenderer(keyboard_renderer);
		keyboard_renderer = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer destroyed (quick keyboard)\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface renderer (quick keyboard), nothing to destroy\n", EMUSCV_NAME);

	// Free SDL quick keyboard surface
	if (keyboard_surface)
	{
		SDL_FreeSurface(keyboard_surface);
		keyboard_surface = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface destroyed (quick keyboard)\n", EMUSCV_NAME);
	}

	return true;
}

// 
// Create the SDL surface and the SDL renderer and load the picture for the emuSCV logo
// 
bool cEmuSCV::CreateLogoSurface()
{
	int x, y, w, h;
	unsigned char *image;


	DestroyLogoSurface();

	if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
	{
		w = 128;
		h = 128;
		if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
			image = (unsigned char *)src_res_emuscvlady128x128xrgba8888_data;
		else
			image = (unsigned char *)src_res_emuscv128x128xrgba8888_data;
	}
	else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
	{
		w = 64;
		h = 64;
		if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
			image = (unsigned char *)src_res_emuscvlady64x64xrgba8888_data;
		else
			image = (unsigned char *)src_res_emuscv64x64xrgba8888_data;
	}
	else
	{
		w = 32;
		h = 32;
		if(config.window_console == SETTING_CONSOLE_EPOCHLADY_VAL)
			image = (unsigned char *)src_res_emuscvlady32x32xrgba8888_data;
		else
			image = (unsigned char *)src_res_emuscv32x32xrgba8888_data;
	}

	// Create SDL quick keyboard surface
    logo_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8*sizeof(uint32_t), 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!logo_surface)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface creation failed! (emuSCV logo) %s\n", EMUSCV_NAME, SDL_GetError());
        return false;
	}
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface created (emuSCV logo)\n", EMUSCV_NAME);

	// Create SDL quick keyboard renderer
    logo_renderer = SDL_CreateSoftwareRenderer(logo_surface);
    if (!logo_renderer)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface renderer creation failed! (emuSCV logo) %s\n", EMUSCV_NAME, SDL_GetError());
        return false;
    }
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer created (emuSCV logo)\n", EMUSCV_NAME);

	// Draw an embedded binary image
	for (int16_t x, y = h; --y >= 0; )
		for (x = w; --x >= 0; )
			pixelRGBA(logo_renderer, x, y, image[4*(x+y*w)], image[4*(x+y*w)+1], image[4*(x+y*w)+2], image[4*(x+y*w)+3]);

	return true;
}

// 
// Destroy the SDL surface and the SDL renderer for the EmuSCV logo
// 
bool cEmuSCV::DestroyLogoSurface()
{
	// Free SDL quick keyboard renderer
	if (logo_renderer)
	{
		SDL_DestroyRenderer(logo_renderer);
		logo_renderer = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer destroyed (emuSCV logo)\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface renderer v, nothing to destroy\n", EMUSCV_NAME);

	// Free SDL quick keyboard surface
	if (logo_surface)
	{
		SDL_FreeSurface(logo_surface);
		logo_surface = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface destroyed (emuSCV logo)\n", EMUSCV_NAME);
	}

	return true;
}

// 
// Create the SDL surface and the SDL renderer and load the picture for the Recalbox logo
// 
bool cEmuSCV::CreateRecalboxSurface()
{
	int x, y, w, h;
	unsigned char *image;


	DestroyRecalboxSurface();

	if(config.window_resolution == SETTING_RESOLUTION_HIGH_VAL)
	{
		w = 624;
		h = 160;
		image = (unsigned char *)src_res_recalbox624x160xrgba8888_data;
	}
	else if(config.window_resolution == SETTING_RESOLUTION_MEDIUM_VAL)
	{
		w = 312;
		h = 80;
		image = (unsigned char *)src_res_recalbox312x80xrgba8888_data;
	}
	else
	{
		w = 156;
		h = 40;
		image = (unsigned char *)src_res_recalbox156x40xrgba8888_data;
	}

	// Create SDL quick keyboard surface
    recalbox_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8*sizeof(uint32_t), 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if(!recalbox_surface)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface creation failed! (Recalbox logo) %s\n", EMUSCV_NAME, SDL_GetError());
        return false;
	}
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface created (Recalbox logo)\n", EMUSCV_NAME);

	// Create SDL Recalbox logo renderer
    recalbox_renderer = SDL_CreateSoftwareRenderer(recalbox_surface);
    if(!recalbox_renderer)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface renderer creation failed! (Recalbox logo) %s\n", EMUSCV_NAME, SDL_GetError());
        return false;
    }
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer created (Recalbox logo)\n", EMUSCV_NAME);

	// Draw an embedded binary image
	for(int16_t x, y = h; --y >= 0; )
		for(x = w; --x >= 0; )
			pixelRGBA(recalbox_renderer, x, y, image[4*(x+y*w)], image[4*(x+y*w)+1], image[4*(x+y*w)+2], image[4*(x+y*w)+3]);

	return true;
}

// 
// Destroy the SDL surface and the SDL renderer for the quick keyboard overlay
// 
bool cEmuSCV::DestroyRecalboxSurface()
{
	// Free SDL quick keyboard renderer
	if(recalbox_renderer != NULL)
	{
		SDL_DestroyRenderer(recalbox_renderer);
		recalbox_renderer = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer destroyed (Recalbox logo)\n", EMUSCV_NAME);
	}
//	else
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface renderer (Recalbox logo), nothing to destroy\n", EMUSCV_NAME);

	// Free SDL quick keyboard surface
	if (recalbox_surface != NULL)
	{
		SDL_FreeSurface(recalbox_surface);
		recalbox_surface = NULL;
//		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface destroyed (Recalbox logo)\n", EMUSCV_NAME);
	}

	return true;
}

// 
// Libretro: return save state size
// 
size_t cEmuSCV::RetroSaveStateSize(void)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSaveStateSize()\n", EMUSCV_NAME);

	// Save current state
	state.Init();
	if(escv_emu)
		escv_emu->save_state(&state, true);
	
	return state.GetDataSize();

//	return STATE_MAX_DATA_SIZE;
}

// 
// Libretro: return save state data pointer
// 
void *cEmuSCV::RetroSaveStateData(void)
{
	// Log
//	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSaveStateData()\n", EMUSCV_NAME);

	return state.GetDataPtr();
}

bool cEmuSCV::RetroSaveState(void *data, size_t size)
{
	size_t serialize_size = 0;

	// Save current state
	state.Init();
	if(escv_emu)
		escv_emu->save_state(&state, false);

	// If size is lower than serialize size, we must retrun false
	serialize_size = state.GetDataSize();
	if (size < serialize_size)
		return false;
	// Copy data
	uint8_t *dst = (uint8_t *)data;
	uint8_t *src = (uint8_t *)state.GetDataPtr();
	for (int32_t i = serialize_size; --i >= 0; )
    	*dst++ = *src++;
	return true;
}

bool cEmuSCV::RetroLoadState(void *data, size_t size)
{
	size_t serialize_size = 0;

	// If size is lower than serialize size, we must retrun false
	serialize_size = state.GetDataSize();
	if (size < serialize_size)
		return false;
	// Copy data
	uint8_t *dst = (uint8_t *)state.GetDataPtr();
	uint8_t *src = (uint8_t *)data;
	for (int32_t i = size; --i >= 0; )
    	*dst++ = *src++;
	state.SetReadyToLoad(true);
	return true;
}
