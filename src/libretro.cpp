// Libretro includes
#include <libretro.h>
#include <streams/file_stream.h>
#include <vfs/vfs_implementation.h>
#include <compat/fopen_utf8.h>
#include <compat/strl.h>
#include <encodings/utf.h>

// EmuSCV includes
#include "emuscv.h"
#include "res/binary.emuscv64x64xrgba8888.data.h"


#define CONTROLLER_BUTTON_SELECT    "BUTTON SELECT"
#define CONTROLLER_BUTTON_START     "BUTTON START"
#define CONTROLLER_BUTTON_LEFT      "BUTTON LEFT"
#define CONTROLLER_BUTTON_RIGHT     "BUTTON RIGHT"
#define CONTROLLER_BUTTON_UP        "BUTTON UP"
#define CONTROLLER_BUTTON_DOWN      "BUTTON DOWN"   // SNES / PLAYSTATION / XBOX
#define CONTROLLER_BUTTON_1         "BUTTON 1"      // A    / CIRCLE      / B
#define CONTROLLER_BUTTON_2         "BUTTON 2"      // B    / CROSS       / A
#define CONTROLLER_BUTTON_3         "BUTTON 3"      // X    / TRIANGLE    / Y
#define CONTROLLER_BUTTON_4         "BUTTON 4"      // Y    / SQUARE      / X
#define CONTROLLER_BUTTON_L1        "BUTTON L1"
#define CONTROLLER_BUTTON_R1        "BUTTON R1"
#define CONTROLLER_BUTTON_L2        "BUTTON L2"
#define CONTROLLER_BUTTON_R2        "BUTTON R2"
#define CONTROLLER_BUTTON_L3        "BUTTON L3"
#define CONTROLLER_BUTTON_R3        "BUTTON R3"
#define CONTROLLER_ANALOG_LEFT_X    "ANALOG LEFT X"
#define CONTROLLER_ANALOG_LEFT_Y    "ANALOG LEFT Y"
#define CONTROLLER_ANALOG_RIGHT_X   "ANALOG RIGHT X"
#define CONTROLLER_ANALOG_RIGHT_Y   "ANALOG RIGHT Y"

#define AUDIO_SAMPLING_RATE		44100
#define AUDIO_SAMPLES_PER_FRAME	AUDIO_SAMPLING_RATE/FRAMES_PER_SEC

static struct retro_log_callback log_callback;
static retro_log_printf_t log_printf_cb;
static retro_environment_t environ_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
//static struct retro_rumble_interface rumble_interface;

static bool retro_support_no_game = true;
static bool retro_use_audio_cb = false;
static unsigned retro_performance_level = 4;

unsigned long cycles_per_frame = CYCLES_PER_FRAME;

static uint8_t *frame_buf;
float frame_time = 0;

uint8_t framecounter = 0;
uint8_t colorIndex = 0;

char retro_base_directory[4096];
char retro_game_path[4096];

static uint16_t phase = 0;

static bool libretro_supports_bitmasks = false;

static bool button_pressed = false;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   va_list va;

   (void)level;

   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

// Return the video standard used
unsigned retro_get_region(void)
{
//    log_printf_cb(RETRO_LOG_INFO, "[%s] retro_get_region()\n", APP_NAME);
    return RETRO_REGION_PAL;
}
/*
bool file_present_in_system(std::string fname)
{
    const char *systemdirtmp = NULL;
    bool worked = environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &systemdirtmp);
    if (!worked)
        return false;

    std::string fullpath  = systemdirtmp;
    fullpath             += "/";
    fullpath             += fname;

   RFILE *fp             = filestream_open(fullpath.c_str(), RETRO_VFS_FILE_ACCESS_READ,
         RETRO_VFS_FILE_ACCESS_HINT_NONE);

   if (fp)
   {
      filestream_close(fp);
      return true;
   }

   return false;
}
*/
void retro_init(void)
{
    frame_buf = (uint8_t*)malloc(SCREEN_PIXELS * sizeof(uint32_t));
/*
    environ_cb(RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE, &rumble_interface);

    const char *dir = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
    {
        snprintf(retro_base_directory, sizeof(retro_base_directory), "%s", dir);
    }

    if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &dir) && dir)
    {
      FILE *fp;
#ifdef _WIN32
      char slash = '\\';
#else
      char slash = '/';
#endif
		char filename[1024] = {0};
		sprintf(filename, "%s%c2048.srm", dir, slash);

		fp = fopen(filename, "rb");

		if (fp)
		{
			fread(game_data(), game_data_size(), 1, fp);
			fclose(fp);
		}
		else
		{
			if (log_cb)
				log_cb(RETRO_LOG_WARN, "[2048] unable to load game data: %s.\n", strerror(errno));
		}
	}
	else
	{
//		log_printf_cb(RETRO_LOG_WARN, "["+APP_NAME+"] unable to load game data: save directory not set.\n");
	}
*/
    libretro_supports_bitmasks = environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL);
}

void retro_deinit(void)
{
    free(frame_buf);
    frame_buf = NULL;

/*
   char *savedir = NULL;
   environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &savedir);

   if (savedir)
   {
      FILE *fp;
#ifdef _WIN32
      char slash = '\\';
#else
      char slash = '/';
#endif
      char filename[1024] = {0};

      sprintf(filename, "%s%c2048.srm", savedir, slash);
      fp = fopen(filename, "wb");

      if (fp)
      {
         fwrite(game_save_data(), game_data_size(), 1, fp);
         fclose(fp);
      }
      else
      {
         if (log_cb)
            log_cb(RETRO_LOG_WARN, "[2048] unable to save game data: %s.\n", strerror(errno));
      }
   }
   else
   {
      if (log_cb)
         log_cb(RETRO_LOG_WARN, "[2048] unable to save game data: save directory not set.\n");
   }

   game_deinit();

   libretro_supports_bitmasks = false;*/
}

unsigned retro_api_version(void)
{
	return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
	log_printf_cb(RETRO_LOG_INFO, "[%s] Plugging device %u into port %u.\n", APP_NAME, device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
	const char *app_version = APP_VERSION;

	memset(info, 0, sizeof(*info));
	info->library_name     = APP_NAME;
	info->library_version  = app_version;
	info->valid_extensions = APP_EXTENSIONS;
	info->need_fullpath    = true;
//	info->block_extract    = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
	info->timing.fps            = FRAMES_PER_SEC;
	info->timing.sample_rate    = AUDIO_SAMPLING_RATE;
	info->geometry.base_width   = SCREEN_WIDTH;
	info->geometry.base_height  = SCREEN_HEIGHT;
	info->geometry.max_width    = SCREEN_WIDTH;
	info->geometry.max_height   = SCREEN_HEIGHT;
	info->geometry.aspect_ratio = SCREEN_ASPECT_RATIO;
}

static void check_variables(void)
{
}

static void update_variables(void)
{
	struct retro_variable var =
	{
		.key = "emuscv_speed",
	};

	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
/*
		char str[100];
		snprintf(str, sizeof(str), "%s", var.value);
		auto pch = strtok(str, "x");
		if (pch)
		{
			auto factor = strtoul(pch, nullptr, 0);
			cycles_per_frame = CYCLES_PER_FRAME / factor;
		}

		log_printf_cb(RETRO_LOG_DEBUG, "[emuSCV]: Running at %lu cycles per frame.\n", cycles_per_frame);
*/
	}
}

void retro_set_environment(retro_environment_t cb)
{
	environ_cb = cb;

	if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log_callback))
		log_printf_cb = log_callback.log;
	else
		log_printf_cb = fallback_log;


	// The performance level is guide to Libretro to give an idea of how intensive this core is to run
	environ_cb(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &retro_performance_level);

	// The core can be run without ROM (show display test)
	environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &retro_support_no_game);

	static const struct retro_controller_description controller[] =
	{
		{ "Controller", RETRO_DEVICE_JOYPAD },	// controllers
		{ 0 }
	};
	static const struct retro_controller_description keyboard[] =
	{
		{ "Keyboard", RETRO_DEVICE_KEYBOARD },	// keyboard on the console
		{ 0 }
	};
	static const struct retro_controller_info port[] =
	{
		{ controller, 1 },	// port 0
		{ controller, 1 },	// port 1
		{ keyboard, 1 },	// port 2
		{ NULL, 0 },
	};
	cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)port);

	struct retro_variable variable[] =
	{
		{ "emuscv_speed", "Speed; x1|x2|x3|x4|x5|x10|x100|x1000|x10000|/2|/3|/4|/5|/10|/100|/1000|/10000" },
		{ NULL },
	};
	cb(RETRO_ENVIRONMENT_SET_VARIABLES, variable);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
	audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
	audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
	input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
	input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
	video_cb = cb;
}

void retro_reset(void)
{
    log_printf_cb(RETRO_LOG_INFO, "[%s] Reset!\n", APP_NAME);
//	game_reset();
}

static void frame_time_cb(retro_usec_t usec)
{
	frame_time = usec / 1000000.0;
//	log_printf_cb(RETRO_LOG_INFO, "[%s] Frame time = %f second.\n", APP_NAME, frame_time);
}

static void audio_callback(void)
{
	if (button_pressed)
	{
		// Play A 440Hz
		for (unsigned i = 0; i < AUDIO_SAMPLES_PER_FRAME; i++, phase++)
		{
			int16_t val = INT16_MAX * sinf(2.0f * M_PI * phase * 440.0f / AUDIO_SAMPLING_RATE);
			audio_cb(val, val);
		}
	}
	else
	{
		// Mute
		for (unsigned i = 0; i < AUDIO_SAMPLES_PER_FRAME; i++, phase++)
		{
			int16_t val = 0x0000;
			audio_cb(val, val);
		}
	}
	phase %= AUDIO_SAMPLING_RATE;
}

void retro_run(void)
{
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
	uint32_t color			= 0xFF000000;
	uint32_t posx			= 0;
	uint32_t posy			= 0;
	uint32_t sizx			= 0;
	uint32_t sizy			= 0;

	// Get input state
	input_poll_cb();
	// Controllers buttons
	if (libretro_supports_bitmasks)
	{
		ret0 = input_state_cb(port0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
		ret1 = input_state_cb(port1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
	}
	else
	{
		unsigned i;
		for (i = RETRO_DEVICE_ID_JOYPAD_B; i < RETRO_DEVICE_ID_JOYPAD_R3+1; i++)
		{
			if (input_state_cb(port0, RETRO_DEVICE_JOYPAD, 0, i))
				ret0 |= (1 << i);
		}
		for (i = RETRO_DEVICE_ID_JOYPAD_B; i < RETRO_DEVICE_ID_JOYPAD_R3+1; i++)
		{
			if (input_state_cb(port1, RETRO_DEVICE_JOYPAD, 0, i))
				ret1 |= (1 << i);
		}
	}
	// Controllers analog sticks
	analogleftx0	= input_state_cb(port0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
	analoglefty0	= input_state_cb(port0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
	analogrightx0	= input_state_cb(port0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
	analogrighty0	= input_state_cb(port0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);
	analogleftx1	= input_state_cb(port1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
	analoglefty1	= input_state_cb(port1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
	analogrightx1	= input_state_cb(port1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
	analogrighty1	= input_state_cb(port1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);
	// Keyboard
	key1 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_1)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP1);
	key2 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_2)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP2);
	key3 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_3)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP3);
	key4 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_4)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP4);
	key5 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_5)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP5);
	key6 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_6)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP6);
	key7 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_7)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP7);
	key8 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_8)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP8);
	key9 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_9)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP9);
	key0 = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_0)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP0);
	keyEnter = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RETURN)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_KP_ENTER);
	keyClear = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_BACKSPACE)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_DELETE);
	keyReset = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_r);
	keyPause = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_SPACE)
		|| input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_p);

	// Check updated variables
	bool updated = false;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
		check_variables();

	// Clear background
	color = palette_pc[1];  // Black
	for (uint32_t i = 0; i < SCREEN_PIXELS; i++)
		memcpy(frame_buf + i * sizeof(uint32_t), &color, sizeof(color));

	// Draw a box around screen changing color each second (in the palette of 16 colors)
	framecounter++;
	if (framecounter > FRAMES_PER_SEC)
	{
		framecounter = 0;
		colorIndex++;
		if (colorIndex > 16)
			colorIndex = 1;
	}
	color = palette_pc[colorIndex];
	posx = 0;
	posy = 0;
	sizx = SCREEN_WIDTH;
	sizy = 5;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	posx = 0;
	posy = 5;
	sizx = 5;
	sizy = SCREEN_HEIGHT-10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	posx = SCREEN_WIDTH-5;
	posy = 5;
	sizx = 5;
	sizy = SCREEN_HEIGHT-10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	posx = 0;
	posy = SCREEN_HEIGHT-5;
	sizx = SCREEN_WIDTH;
	sizy = 5;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Draw a lonely pixel
	color = palette_pc[8];  // Red
	posx = 10;
	posy = 10;
	memcpy(frame_buf + (posx + posy * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

	// Draw an embedded binary image (64x97 pixels in ARGB8888 format)
	posx = 10;
	posy = 20;
	sizx = 64;
	sizy = 64;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
		{
			color = RGB_COLOR(src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)], src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)+1], src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)+2]);
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		}

	// FIRST CONTROLLER (LEFT SIDE, ORANGE)
	button_pressed = FALSE;
	// cross center
	color = palette_pc[14];	// Gray
	posx = 20;
	posy = 168;
	sizx = 10;
	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button UP
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 20;
	posy = 158;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button DOWN
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 20;
	posy = 178;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button LEFT
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 10;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button RIGHT
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 30;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button 1
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[8];	// Red
	}
	posx = 83;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button 2
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[7];	// Green medium
	}
	posx = 73;
	posy = 178;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button 3
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[9];	// Orange
	}
	posx = 73;
	posy = 158;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button 4
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[2];	// Blue light
	}
	posx = 63;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button SELECT
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[13];	// Green dark
	}
	posx = 41;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button START
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[5];	// Green soft
	}
	posx = 52;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button L1
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[0];	// Blue medium
	}
	posx = 20;
	posy = 147;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button R1
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[3];	// Purple
	}
	posx = 73;
	posy = 147;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button L2
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[2];	// Blue Light
	}
	posx = 20;
	posy = 136;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button R2
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[10];	// Fuschia
	}
	posx = 73;
	posy = 136;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button L3
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L3))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[6];	// Cyan
	}
	posx = 20;
	posy = 125;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button R3
	if (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[11];	// Pink
	}
	posx = 73;
	posy = 125;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Left analog stick
	// Analog stick contour
	color = palette_pc[14];	// Gray
	posx = 24;
	posy = 199;
	sizx = 10;
	sizy = 10;
	for (uint32_t x = 0; x < 2 * sizx + 1; x++)
	{
		memcpy(frame_buf + (posx - sizx + x + (posy - sizy) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		memcpy(frame_buf + (posx - sizx + x + (posy + sizy) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	}
	for (uint32_t y = 0; y < 2 * sizy + 1; y++)
	{
		memcpy(frame_buf + (posx - sizx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		memcpy(frame_buf + (posx + sizx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	}
	// Analog stick center
	posx = 24;
	posy = 199;
	sizx = 5;
	sizy = 5;
	for (uint32_t x = 0; x < 2 * sizx + 1; x++)
		memcpy(frame_buf + (posx - sizx + x + posy * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	for (uint32_t y = 0; y < 2 * sizy + 1; y++)
		memcpy(frame_buf + (posx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Analog stick position
	if (analogleftx0 <= -32 || analogleftx0 >= 32 || analoglefty0 <= -32 || analoglefty0 >= 32)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[15];	// White
	}
//	log_printf_cb(RETRO_LOG_INFO, "[%s] controller 0 analog left x %d\n", APP_NAME, analogleftx0);
//	log_printf_cb(RETRO_LOG_INFO, "[%s] controller 0 analog left y %d\n", APP_NAME, analoglefty0);
	posx = 23+9*analogleftx0/INT16_MAX;
	posy = 198+9*analoglefty0/INT16_MAX;
	sizx = 3;
	sizy = 3;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Right analog stick
	// Analog stick contour
	color = palette_pc[14];	// Gray
	posx = 77;
	posy = 199;
	sizx = 10;
	sizy = 10;
	for (uint32_t x = 0; x < 2 * sizx + 1; x++)
	{
		memcpy(frame_buf + (posx - sizx + x + (posy - sizy) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		memcpy(frame_buf + (posx - sizx + x + (posy + sizy) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	}
	for (uint32_t y = 0; y < 2 * sizy + 1; y++)
	{
		memcpy(frame_buf + (posx - sizx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		memcpy(frame_buf + (posx + sizx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	}
	// Analog stick center
	posx = 77;
	posy = 199;
	sizx = 5;
	sizy = 5;
	for (uint32_t x = 0; x < 2 * sizx + 1; x++)
		memcpy(frame_buf + (posx - sizx + x + posy * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	for (uint32_t y = 0; y < 2 * sizy + 1; y++)
		memcpy(frame_buf + (posx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Analog stick position
	if (analogrightx0 <= -32 || analogrightx0 >= 32 || analogrighty0 <= -32 || analogrighty0 >= 32)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[15];	// White
	}
//	log_printf_cb(RETRO_LOG_INFO, "[%s] controller 0 analog right x %d\n", APP_NAME, analogrightx0);
//	log_printf_cb(RETRO_LOG_INFO, "[%s] controller 0 analog right y %d\n", APP_NAME, analogrighty0);
	posx = 76+9*analogrightx0/INT16_MAX;
	posy = 198+9*analogrighty0/INT16_MAX;
	sizx = 3;
	sizy = 3;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

	// SECOND CONTROLLER (RIGHT SIDE, BLUE)
	// cross center
	color = palette_pc[14];	// Gray
	posx = 110;
	posy = 168;
	sizx = 10;
	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button UP
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 110;
	posy = 158;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button DOWN
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 110;
	posy = 178;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button LEFT
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 100;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button RIGHT
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 120;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button 1
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[8];	// Red
	}
	posx = 173;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button 2
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[7];	// Green medium
	}
	posx = 163;
	posy = 178;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button 3
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[9];	// Orange
	}
	posx = 163;
	posy = 158;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button 4
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[2];	// Blue light
	}
	posx = 153;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button SELECT
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[13];	// Green dark
	}
	posx = 131;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button START
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[5];	// Green soft
	}
	posx = 142;
	posy = 168;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button L1
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[0];	// Blue medium
	}
	posx = 110;
	posy = 147;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button R1
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[3];	// Purple
	}
	posx = 163;
	posy = 147;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button L2
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[2];	// Blue Light
	}
	posx = 110;
	posy = 136;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button R2
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[10];	// Fuschia
	}
	posx = 163;
	posy = 136;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button L3
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L3))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[6];	// Cyan
	}
	posx = 110;
	posy = 125;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Button R3
	if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[11];	// Pink
	}
	posx = 163;
	posy = 125;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Left analog stick
	// Analog stick contour
	color = palette_pc[14];	// Gray
	posx = 114;
	posy = 199;
	sizx = 10;
	sizy = 10;
	for (uint32_t x = 0; x < 2 * sizx + 1; x++)
	{
		memcpy(frame_buf + (posx - sizx + x + (posy - sizy) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		memcpy(frame_buf + (posx - sizx + x + (posy + sizy) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	}
	for (uint32_t y = 0; y < 2 * sizy + 1; y++)
	{
		memcpy(frame_buf + (posx - sizx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		memcpy(frame_buf + (posx + sizx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	}
	// Analog stick center
	posx = 114;
	posy = 199;
	sizx = 5;
	sizy = 5;
	for (uint32_t x = 0; x < 2 * sizx + 1; x++)
		memcpy(frame_buf + (posx - sizx + x + posy * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	for (uint32_t y = 0; y < 2 * sizy + 1; y++)
		memcpy(frame_buf + (posx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Analog stick position
	if (analogleftx1 <= -32 || analogleftx1 >= 32 || analoglefty1 <= -32 || analoglefty1 >= 32)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[15];	// White
	}
//	log_printf_cb(RETRO_LOG_INFO, "[%s] controller 1 analog left x %d\n", APP_NAME, analogleftx1);
//	log_printf_cb(RETRO_LOG_INFO, "[%s] controller 1 analog left y %d\n", APP_NAME, analoglefty1);
	posx = 113+9*analogleftx1/INT16_MAX;
	posy = 198+9*analoglefty1/INT16_MAX;
	sizx = 3;
	sizy = 3;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Right analog stick
	// Analog stick contour
	color = palette_pc[14];	// Gray
	posx = 167;
	posy = 199;
	sizx = 10;
	sizy = 10;
	for (uint32_t x = 0; x < 2 * sizx + 1; x++)
	{
		memcpy(frame_buf + (posx - sizx + x + (posy - sizy) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		memcpy(frame_buf + (posx - sizx + x + (posy + sizy) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	}
	for (uint32_t y = 0; y < 2 * sizy + 1; y++)
	{
		memcpy(frame_buf + (posx - sizx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
		memcpy(frame_buf + (posx + sizx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	}
	// Analog stick center
	posx = 167;
	posy = 199;
	sizx = 5;
	sizy = 5;
	for (uint32_t x = 0; x < 2 * sizx + 1; x++)
		memcpy(frame_buf + (posx - sizx + x + posy * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	for (uint32_t y = 0; y < 2 * sizy + 1; y++)
		memcpy(frame_buf + (posx + (posy - sizy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));
	// Analog stick position
	if (analogrightx1 <= -32 || analogrightx1 >= 32 || analogrighty1 <= -32 || analogrighty1 >= 32)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[15];	// White
	}
//	log_printf_cb(RETRO_LOG_INFO, "[%s] controller 1 analog right x %d\n", APP_NAME, analogrightx1);
//	log_printf_cb(RETRO_LOG_INFO, "[%s] controller 1 analog right y %d\n", APP_NAME, analogrighty1);
	posx = 166+9*analogrightx1/INT16_MAX;
	posy = 198+9*analogrighty1/INT16_MAX;
	sizx = 3;
	sizy = 3;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 1
	if (key1 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 1 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 151;
	posy = 32;
	sizx = 10;
	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 2
	if (key2 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 2 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 162;
	posy = 32;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 3
	if (key3 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 3 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 173;
	posy = 32;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 4
	if (key4 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 4 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 151;
	posy = 21;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 5
	if (key5 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 5 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 162;
	posy = 21;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 6
	if (key6 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 6 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 173;
	posy = 21;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 7
	if (key7 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 7 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 151;
	posy = 10;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 8
	if (key8 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 8 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 162;
	posy = 10;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 9
	if (key9 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 9 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 173;
	posy = 10;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key 0
	if (key0 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 0 pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 151;
	posy = 43;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key ENTER (EN)
	if (keyEnter != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key ENTER pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 162;
	posy = 43;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key CLEAR (CL)
	if (keyClear != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key CLEAR pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 173;
	posy = 43;
//	sizx = 10;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key RESET
	if (keyReset != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key RESET pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 116;
	posy = 28;
	sizx = 30;
	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

    // Key PAUSE
	if (keyPause != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key PAUSE pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	posx = 116;
	posy = 43;
//	sizx = 30;
//	sizy = 10;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			memcpy(frame_buf + (posx + x + (posy + y) * SCREEN_WIDTH) * sizeof(uint32_t), &color, sizeof(color));

	if (retro_use_audio_cb == false)
		audio_callback();
	video_cb(frame_buf, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*sizeof(uint32_t));
}

static void audio_set_state(bool enable)
{
   (void)enable;
}

bool retro_load_game(const struct retro_game_info *info)
{
    // Load custom core settings
    update_variables();

    // Set the controller descriptor
	struct retro_input_descriptor desc[] =
	{
		// Joypad 1, left side, orange
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, CONTROLLER_BUTTON_SELECT }, // Pause
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  CONTROLLER_BUTTON_START },  // Display console buttons menu
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   CONTROLLER_BUTTON_LEFT },   // Controller joystick
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  CONTROLLER_BUTTON_RIGHT },  // Controller joystick
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     CONTROLLER_BUTTON_UP },     // Controller joystick
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   CONTROLLER_BUTTON_DOWN },   // Controller joystick
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      CONTROLLER_BUTTON_1 },      // Right controller button
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      CONTROLLER_BUTTON_2 },      // Left controller button
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      CONTROLLER_BUTTON_3},
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      CONTROLLER_BUTTON_4 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      CONTROLLER_BUTTON_L1 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      CONTROLLER_BUTTON_R1 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,     CONTROLLER_BUTTON_L2 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     CONTROLLER_BUTTON_R2 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,     CONTROLLER_BUTTON_L3 },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,     CONTROLLER_BUTTON_R3 },
		{ 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X, CONTROLLER_ANALOG_LEFT_X },    // Controller joystick
		{ 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y, CONTROLLER_ANALOG_LEFT_Y },    // Controller joystick
		{ 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X, CONTROLLER_ANALOG_RIGHT_X },
		{ 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y, CONTROLLER_ANALOG_RIGHT_Y },

		// Joypad 2, right side, blue
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, CONTROLLER_BUTTON_SELECT }, // Pause
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  CONTROLLER_BUTTON_START },  // Display console buttons menu
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   CONTROLLER_BUTTON_LEFT },   // Controller joystick
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  CONTROLLER_BUTTON_RIGHT },  // Controller joystick
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     CONTROLLER_BUTTON_UP },     // Controller joystick
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   CONTROLLER_BUTTON_DOWN },   // Controller joystick
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      CONTROLLER_BUTTON_1 },      // Right controller button
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      CONTROLLER_BUTTON_2 },      // Left controller button
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      CONTROLLER_BUTTON_3},
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      CONTROLLER_BUTTON_4 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      CONTROLLER_BUTTON_L1 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      CONTROLLER_BUTTON_R1 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,     CONTROLLER_BUTTON_L2 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     CONTROLLER_BUTTON_R2 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,     CONTROLLER_BUTTON_L3 },
		{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,     CONTROLLER_BUTTON_R3 },
		{ 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X, CONTROLLER_ANALOG_LEFT_X },    // Controller joystick
		{ 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y, CONTROLLER_ANALOG_LEFT_Y },    // Controller joystick
		{ 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X, CONTROLLER_ANALOG_RIGHT_X },
		{ 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y, CONTROLLER_ANALOG_RIGHT_Y },

		{ 0 },
	};
    if (!environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc))
    {
        log_printf_cb(RETRO_LOG_ERROR, "[%s] Can't set input descriptors!\n", APP_NAME);
        return false;
    }
	log_printf_cb(RETRO_LOG_INFO, "[%s] Set input descriptors.\n", APP_NAME);

    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
    {
        log_printf_cb(RETRO_LOG_ERROR, "[%s] XRGB8888 pixel format not supported!\n", APP_NAME);
        return false;
    }
	log_printf_cb(RETRO_LOG_INFO, "[%s] Set XRGB8888 pixel format.\n", APP_NAME);

    struct retro_frame_time_callback frame_cb;
    frame_cb.callback  = frame_time_cb;
    frame_cb.reference = CPU_CLOCKS / FRAMES_PER_SEC;
    frame_cb.callback(frame_cb.reference);
    environ_cb(RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK, &frame_cb);

//	snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);

	struct retro_audio_callback audio_cb = { audio_callback, audio_set_state };
	retro_use_audio_cb = environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &audio_cb);

    check_variables();
/*
    if (info && info->data)
    {
        return oEmuSCV->LoadCartridge((const uint8_t*)info->data, info->size);
    }
*/

    return true;
}




void retro_unload_game(void)
{
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;//game_data_size();
}

bool retro_serialize(void *data_, size_t size)
{
    return false;
/*   if (size < game_data_size())
      return false;

   memcpy(data_, game_data(), game_data_size());
   return true;*/
}

bool retro_unserialize(const void *data_, size_t size)
{
    return false;
/*   if (size < game_data_size())
      return false;

   memcpy(game_data(), data_, game_data_size());
   return true;*/
}

void *retro_get_memory_data(unsigned id)
{
    return NULL;
   /*if (id != RETRO_MEMORY_SAVE_RAM)
      return NULL;

   return game_data();*/
}

size_t retro_get_memory_size(unsigned id)
{
    return 0;
    /*
   if (id != RETRO_MEMORY_SAVE_RAM)
      return 0;

   return game_data_size();*/
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
}

// Test the user input and return the state of the joysticks and buttons
/*
void get_joystick_state(unsigned port, uint8_t &x, uint8_t &y, uint8_t &b1, uint8_t &b2, uint8_t &b3, uint8_t &b4)
{
    if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
        x = 0x00;
    else if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
        x = 0xff;
    else
        x = (uint8_t) (
                (input_state_cb(port, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X) / 256) + 128);

    if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
        y = 0xff;
    else if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN ))
        y = 0x00;
    else
    {
        // retroarch y axis is inverted wrt to the vectrex
        auto y_value = input_state_cb(port, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
        y = (uint8_t) (~((y_value/256) + 128) + 1);
    }

    b1 = (unsigned char) (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A ) ? 1 : 0);
    b2 = (unsigned char) (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B ) ? 1 : 0);
    b3 = (unsigned char) (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X ) ? 1 : 0);
    b4 = (unsigned char) (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y ) ? 1 : 0);
}
*/
