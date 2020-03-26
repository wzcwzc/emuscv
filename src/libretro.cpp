// Libretro includes
#include <libretro.h>
/*
#include <streams/file_stream.h>
#include <vfs/vfs_implementation.h>
#include <compat/fopen_utf8.h>
#include <compat/strl.h>
#include <encodings/utf.h>
*/
// EmuSCV includes
#include "emuscv.h"
#include "res/binary.emuscv64x64xrgba8888.data.h"
#include <SDL2/SDL.h>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>
#include <zlib/zlib.h>
#include <unzip/unzip.h>
#include <lpng/png.h>

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

static bool retro_support_no_game = true;
static bool retro_use_audio_cb = false;
static unsigned retro_performance_level = 4;

static unsigned long cycles_per_frame = CYCLES_PER_FRAME;

static double fps				= FRAMES_PER_SEC;
static unsigned base_width		= WINDOW_WIDTH;
static unsigned base_height		= WINDOW_HEIGHT;
static float aspect_ratio		= WINDOW_ASPECT_RATIO;

static SDL_Surface *frame_surface;
static SDL_Renderer *frame_renderer;

static float frame_time = 0;

static uint8_t framecounter = 0;
static uint8_t colorIndex = 0;

static char retro_base_directory[4096];
static char retro_save_directory[4096];
static char retro_game_path[4096];

static uint16_t phase = 0;

static bool libretro_supports_bitmasks = false;

static bool button_pressed = false;
static bool button_power = true;

static bool game_loaded = false;

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
	const char *dir = NULL;

	log_printf_cb(RETRO_LOG_INFO, "[%s] retro_init()\n", APP_NAME);

	retro_base_directory[0] = 0;
	if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
	{
		snprintf(retro_base_directory, sizeof(retro_base_directory), "%s", dir);
	}
	log_printf_cb(RETRO_LOG_INFO, "[%s] Base directory: %s\n", APP_NAME, retro_base_directory);

	retro_save_directory[0] = 0;
	if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &dir) && dir)
	{
		snprintf(retro_save_directory, sizeof(retro_save_directory), "%s", dir);
		log_printf_cb(RETRO_LOG_INFO, "[%s] Save directory: %s\n", APP_NAME, retro_save_directory);
	}
/*
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
	log_printf_cb(RETRO_LOG_INFO, "[%s] retro_deinit()\n", APP_NAME);

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

   libretro_supports_bitmasks = false;
*/
}

unsigned retro_api_version(void)
{
	return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
	log_printf_cb(RETRO_LOG_INFO, "[%s] Device %u plugged into port %u.\n", APP_NAME, device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
	memset(info, 0, sizeof(*info));
	info->library_name     = APP_NAME;
	info->library_version  = APP_VERSION;
	info->valid_extensions = APP_EXTENSIONS;
	info->need_fullpath    = false;
	info->block_extract    = true;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
	info->timing.fps            = fps;
	info->timing.sample_rate    = AUDIO_SAMPLING_RATE;
	info->geometry.base_width   = base_width;
	info->geometry.base_height  = WINDOW_HEIGHT;
	info->geometry.max_width    = base_width * WINDOW_ZOOM_MAX;
	info->geometry.max_height   = WINDOW_HEIGHT * WINDOW_ZOOM_MAX;
	info->geometry.aspect_ratio = aspect_ratio;
}
/*
static void check_variables(void)
{
}
*/
/*
static void update_variables(void)
{
	struct retro_variable var =
	{
		.key = "emuscv_speed",
	};

	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
*/
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
/*
	}
}
*/

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
/*
	struct retro_variable variable[] =
	{
		{ "emuscv_speed", "Speed; x1|x2|x3|x4|x5|x10|x100|x1000|x10000|/2|/3|/4|/5|/10|/100|/1000|/10000" },
		{ NULL },
	};
	cb(RETRO_ENVIRONMENT_SET_VARIABLES, variable);
*/
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
    log_printf_cb(RETRO_LOG_INFO, "[%s] retro_reset()\n", APP_NAME);
//	game_reset();
}

/*
static void frame_time_cb(retro_usec_t usec)
{
	frame_time = usec / 1000000.0;
//	log_printf_cb(RETRO_LOG_INFO, "[%s] Frame time = %f second.\n", APP_NAME, frame_time);
}
*/

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

png_byte *volatile image_data = NULL;
png_bytep *volatile image_row_data = NULL;
png_uint_32 image_width = 0;
png_uint_32 image_height = 0;
int image_bit_depth = 0;
int image_color_type = -1;

void retro_run(void)
{
//	log_printf_cb(RETRO_LOG_INFO, "[%s] retro_run()\n", APP_NAME);

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
	keyPower = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_o);
	keyReset = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_r);
	keyPause = input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_SPACE)
            || input_state_cb(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_p);

/*
	// Check updated variables
	bool updated = false;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
		check_variables();
*/
	// Clear background
	// changing color every 60 frames
	framecounter++;
	if (framecounter > FRAMES_PER_SEC)
	{
		framecounter = 0;
		colorIndex++;
		if (colorIndex > 16)
			colorIndex = 1;
	}
	color = palette_pc[colorIndex];
	SDL_SetRenderDrawColor(frame_renderer, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
	SDL_RenderClear(frame_renderer);

    // Draw a lonely pixel (using an other color than background)
    // with an ellipse around
	color = palette_pc[colorIndex < 12 ? colorIndex + 4 : colorIndex - 12 ];  // Different color of background color
	posx = 10;
	posy = 10;
	boxRGBA(frame_renderer, 4*posx, 3*posy, 4*posx+3, 3*posy+2, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
	ellipseRGBA(frame_renderer, 4*posx+1, 3*posy+1, 10, 12, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);

	// Draw an embedded binary image (64x97 pixels in ARGB8888 format)
	posx = 12;
	posy = 60;
	sizx = 64;
	sizy = 64;
	for (uint32_t y = 0; y < sizy; y++)
		for (uint32_t x = 0; x < sizx; x++)
			pixelRGBA(frame_renderer, posx+x, posy+y, src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)], src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)+1], src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)+2], src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)+3]);

	// Draw a PNG image read from ZIP file
	if(image_data != NULL)
	{
		posx = 80;
		posy = 30;
		sizx = image_width;
		sizy = image_height;
		for (uint32_t y = 0; y < sizy; y++)
			for (uint32_t x = 0; x < sizx; x++)
				pixelRGBA(frame_renderer, posx+x, posy+sizy-y, image_data[4*(x+y*sizx)], image_data[4*(x+y*sizx)+1], image_data[4*(x+y*sizx)+2], image_data[4*(x+y*sizx)+3]);
	}

	// Reset button state
	button_pressed = FALSE;

	// FIRST CONTROLLER (LEFT SIDE, ORANGE)
	posx = 100;
	posy = 350;
	// Contour
	color = palette_pc[1];	// Black
    rectangleRGBA(frame_renderer, posx+29, posy, posx+60, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// L3
    rectangleRGBA(frame_renderer, posx+29, posy+32, posx+60, posy+63, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// L2
    rectangleRGBA(frame_renderer, posx+29, posy+64, posx+60, posy+95, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// L1
    rectangleRGBA(frame_renderer, posx+29, posy+96, posx+60, posy+126, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// UP
    rectangleRGBA(frame_renderer, posx, posy+125, posx+30, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// LEFT
    rectangleRGBA(frame_renderer, posx+59, posy+125, posx+89, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// RIGHT
    rectangleRGBA(frame_renderer, posx+29, posy+155, posx+60, posy+185, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// DOWN
    rectangleRGBA(frame_renderer, posx+14, posy+186, posx+75, posy+247, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// LEFT STICK
    rectangleRGBA(frame_renderer, posx+90, posy+125, posx+121, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// SELECT
    rectangleRGBA(frame_renderer, posx+122, posy+125, posx+153, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// START
    rectangleRGBA(frame_renderer, posx+154, posy+125, posx+185, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// BUTTON 4
    rectangleRGBA(frame_renderer, posx+212, posy+125, posx+243, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// BUTTON 1
    rectangleRGBA(frame_renderer, posx+168, posy+186, posx+229, posy+247, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// RIGHT STICK
    rectangleRGBA(frame_renderer, posx+183, posy+154, posx+214, posy+185, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// BUTTON 2
    rectangleRGBA(frame_renderer, posx+183, posy+96, posx+214, posy+127, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// BUTTON 3
    rectangleRGBA(frame_renderer, posx+183, posy+64, posx+214, posy+95, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// R1
    rectangleRGBA(frame_renderer, posx+183, posy+32, posx+214, posy+63, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// R2
    rectangleRGBA(frame_renderer, posx+183, posy, posx+214, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// R3
	color = palette_pc[14];	// Gray
    boxRGBA(frame_renderer, posx+30, posy+126, posx+58, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// Cross center
    boxRGBA(frame_renderer, posx+15, posy+187, posx+73, posy+245, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// LEFT STICK
    boxRGBA(frame_renderer, posx+169, posy+187, posx+227, posy+245, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// RIGHT STICK
	color = palette_pc[1];	// Black
	lineRGBA(frame_renderer, posx+44-15, posy+216, posx+44+15, posy+216, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// LEFT STICK
	lineRGBA(frame_renderer, posx+44, posy+216-15, posx+44, posy+216+15, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// LEFT STICK
	lineRGBA(frame_renderer, posx+198-15, posy+216, posx+198+15, posy+216, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// RIGHT STICK
	lineRGBA(frame_renderer, posx+198, posy+216-15, posx+198, posy+216+15, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// RIGHT STICK
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
    boxRGBA(frame_renderer, posx+30, posy+1, posx+58, posy+29, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+30, posy+33, posx+58, posy+61, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+30, posy+65, posx+58, posy+93, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+30, posy+97, posx+58, posy+125, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+1, posy+126, posx+29, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+59, posy+126, posx+87, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+30, posy+155, posx+58, posy+183, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
	// Left analog stick
	if (analogleftx0 <= -32 || analogleftx0 >= 32 || analoglefty0 <= -32 || analoglefty0 >= 32)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[15];	// White
	}
    boxRGBA(frame_renderer, posx+44-3+26*analogleftx0/INT16_MAX, posy+216-3+26*analoglefty0/INT16_MAX, posx+44+3+26*analogleftx0/INT16_MAX, posy+216+3+26*analoglefty0/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+91, posy+126, posx+119, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+123, posy+126, posx+151, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+155, posy+126, posx+183, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+213, posy+126, posx+241, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
	// Right analog stick
	if (analogrightx0 <= -32 || analogrightx0 >= 32 || analogrighty0 <= -32 || analogrighty0 >= 32)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[15];	// White
	}
    boxRGBA(frame_renderer, posx+198-3+26*analogrightx0/INT16_MAX, posy+216-3+26*analogrighty0/INT16_MAX, posx+198+3+26*analogrightx0/INT16_MAX, posy+216+3+26*analogrighty0/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+155, posx+212, posy+183, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+97, posx+212, posy+125, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+65, posx+212, posy+93, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+33, posx+212, posy+61, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+1, posx+212, posy+29, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);

	// SECOND CONTROLLER (RIGHT SIDE, BLUE)
	posx = 500;
	posy = 350;
	// Contour
	color = palette_pc[1];	// Black
    rectangleRGBA(frame_renderer, posx+29, posy, posx+60, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// L3
    rectangleRGBA(frame_renderer, posx+29, posy+32, posx+60, posy+63, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// L2
    rectangleRGBA(frame_renderer, posx+29, posy+64, posx+60, posy+95, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// L1
    rectangleRGBA(frame_renderer, posx+29, posy+96, posx+60, posy+126, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// UP
    rectangleRGBA(frame_renderer, posx, posy+125, posx+30, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// LEFT
    rectangleRGBA(frame_renderer, posx+59, posy+125, posx+89, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// RIGHT
    rectangleRGBA(frame_renderer, posx+29, posy+155, posx+60, posy+185, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// DOWN
    rectangleRGBA(frame_renderer, posx+14, posy+186, posx+75, posy+247, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// LEFT STICK
    rectangleRGBA(frame_renderer, posx+90, posy+125, posx+121, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// SELECT
    rectangleRGBA(frame_renderer, posx+122, posy+125, posx+153, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// START
    rectangleRGBA(frame_renderer, posx+154, posy+125, posx+185, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// BUTTON 4
    rectangleRGBA(frame_renderer, posx+212, posy+125, posx+243, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// BUTTON 1
    rectangleRGBA(frame_renderer, posx+168, posy+186, posx+229, posy+247, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// RIGHT STICK
    rectangleRGBA(frame_renderer, posx+183, posy+154, posx+214, posy+185, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// BUTTON 2
    rectangleRGBA(frame_renderer, posx+183, posy+96, posx+214, posy+127, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// BUTTON 3
    rectangleRGBA(frame_renderer, posx+183, posy+64, posx+214, posy+95, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// R1
    rectangleRGBA(frame_renderer, posx+183, posy+32, posx+214, posy+63, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// R2
    rectangleRGBA(frame_renderer, posx+183, posy, posx+214, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// R3
	color = palette_pc[14];	// Gray
    boxRGBA(frame_renderer, posx+30, posy+126, posx+58, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// Cross center
    boxRGBA(frame_renderer, posx+15, posy+187, posx+73, posy+245, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// LEFT STICK
    boxRGBA(frame_renderer, posx+169, posy+187, posx+227, posy+245, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);			// RIGHT STICK
	color = palette_pc[1];	// Black
	lineRGBA(frame_renderer, posx+44-15, posy+216, posx+44+15, posy+216, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// LEFT STICK
	lineRGBA(frame_renderer, posx+44, posy+216-15, posx+44, posy+216+15, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);		// LEFT STICK
	lineRGBA(frame_renderer, posx+198-15, posy+216, posx+198+15, posy+216, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// RIGHT STICK
	lineRGBA(frame_renderer, posx+198, posy+216-15, posx+198, posy+216+15, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);	// RIGHT STICK
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
    boxRGBA(frame_renderer, posx+30, posy+1, posx+58, posy+29, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+30, posy+33, posx+58, posy+61, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+30, posy+65, posx+58, posy+93, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+30, posy+97, posx+58, posy+125, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+1, posy+126, posx+29, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+59, posy+126, posx+87, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+30, posy+155, posx+58, posy+183, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
	// Left analog stick
	if (analogleftx1 <= -32 || analogleftx1 >= 32 || analoglefty1 <= -32 || analoglefty1 >= 32)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[15];	// White
	}
    boxRGBA(frame_renderer, posx+44-3+26*analogleftx1/INT16_MAX, posy+216-3+26*analoglefty1/INT16_MAX, posx+44+3+26*analogleftx1/INT16_MAX, posy+216+3+26*analoglefty1/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+91, posy+126, posx+119, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+123, posy+126, posx+151, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+155, posy+126, posx+183, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+213, posy+126, posx+241, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
	// Right analog stick
	if (analogrightx1 <= -32 || analogrightx1 >= 32 || analogrighty1 <= -32 || analogrighty1 >= 32)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
	}
	else
	{
		color = palette_pc[15];	// White
	}
    boxRGBA(frame_renderer, posx+198-3+26*analogrightx1/INT16_MAX, posy+216-3+26*analogrighty1/INT16_MAX, posx+198+3+26*analogrightx1/INT16_MAX, posy+216+3+26*analogrighty1/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+155, posx+212, posy+183, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+97, posx+212, posy+125, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+65, posx+212, posy+93, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+33, posx+212, posy+61, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+184, posy+1, posx+212, posy+29, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);

	// KEYBOARD
	posx = 550;
	posy = 100;
	// Contour
	color = palette_pc[1];	// Black
    rectangleRGBA(frame_renderer, posx, posy, posx+73, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);                      // POWER ON/OFF BUTTON
    rectangleRGBA(frame_renderer, posx, posy+48, posx+73, posy+48+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);                // RESET BUTTON
    rectangleRGBA(frame_renderer, posx, posy+96, posx+73, posy+96+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);                // PAUSE BUTTON
    rectangleRGBA(frame_renderer, posx+74, posy, posx+74+31, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74+32, posy, posx+74+32+31, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74+64, posy, posx+74+64+31, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74, posy+32, posx+74+31, posy+32+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74+32, posy+32, posx+74+32+31, posy+32+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74+64, posy+32, posx+74+64+31, posy+32+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74, posy+64, posx+74+31, posy+64+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74+32, posy+64, posx+74+32+31, posy+64+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74+64, posy+64, posx+74+64+31, posy+64+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74, posy+96, posx+74+31, posy+96+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74+32, posy+96, posx+74+32+31, posy+96+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    rectangleRGBA(frame_renderer, posx+74+64, posy+96, posx+74+64+31, posy+96+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    // Key POWER ON/OFF
	if (keyPower != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
        button_power = !button_power;
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	boxRGBA(frame_renderer, posx+1, posy+1, posx+1+70, posy+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
	color = palette_pc[1];	// Black
	if (button_power == true)
	{
        rectangleRGBA(frame_renderer, posx+43, posy+1, posx+72, posy+30, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
//		if (keyPower != 0)
//			log_printf_cb(RETRO_LOG_INFO, "[%s] Key POWER set to ON.\n", APP_NAME);
	}
	else
	{
        rectangleRGBA(frame_renderer, posx+1, posy+1, posx+30, posy+30, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
//		if (keyPower != 0)
//			log_printf_cb(RETRO_LOG_INFO, "[%s] Key POWER set to OFF.\n", APP_NAME);
		environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
	}
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
    boxRGBA(frame_renderer, posx+1, posy+48+1, posx+1+70, posy+48+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+1, posy+97, posx+1+70, posy+97+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+74+1, posy+1, posx+74+1+28, posy+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+74+33, posy+1, posx+74+33+28, posy+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+74+65, posy+1, posx+74+65+28, posy+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    // Key 4
	if (key4 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 4 pressed.\n", APP_NAME);
		if (fps != FRAMES_PER_SEC)
		{
			fps = FRAMES_PER_SEC;
			struct retro_system_av_info av_info;
			retro_get_system_av_info(&av_info);
			environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &av_info);
			framecounter = FRAMES_PER_SEC;
		}
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
    boxRGBA(frame_renderer, posx+74+1, posy+33, posx+74+1+28, posy+33+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    // Key 5
	if (key5 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 5 pressed.\n", APP_NAME);
		if (fps != 50)
		{
			fps = 50;
			struct retro_system_av_info av_info;
			retro_get_system_av_info(&av_info);
			environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &av_info);
			framecounter = FRAMES_PER_SEC;
		}
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
    boxRGBA(frame_renderer, posx+74+33, posy+33, posx+74+33+28, posy+33+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+74+65, posy+33, posx+74+65+28, posy+33+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
	// Key 1
	if (key1 != 0)
	{
		button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 1 pressed.\n", APP_NAME);
		if (base_width != WINDOW_WIDTH || base_height != WINDOW_HEIGHT || aspect_ratio != WINDOW_ASPECT_RATIO)
		{
			base_width   = WINDOW_WIDTH;
			base_height  = WINDOW_HEIGHT;
			aspect_ratio = WINDOW_ASPECT_RATIO;
			struct retro_system_av_info av_info;
			retro_get_system_av_info(&av_info);
			environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &av_info);
		}
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
	boxRGBA(frame_renderer, posx+74+1, posy+65, posx+74+1+28, posy+65+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    // Key 2
	if (key2 != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key 2 pressed.\n", APP_NAME);
		if (base_width != 1280  || base_height != 720 || aspect_ratio != 16.0/9.0)
		{
			base_width   = 1280;
			base_height  = 720;
			aspect_ratio = 16.0/9.0;
			struct retro_system_av_info av_info;
			retro_get_system_av_info(&av_info);
			environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &av_info);
		}
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
    boxRGBA(frame_renderer, posx+74+33, posy+65, posx+74+33+28, posy+65+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+74+65, posy+65, posx+74+65+28, posy+65+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
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
    boxRGBA(frame_renderer, posx+74+1, posy+97, posx+74+1+28, posy+97+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    // Key CL
	if (keyClear != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key CL pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
    boxRGBA(frame_renderer, posx+74+33, posy+97, posx+74+33+28, posy+97+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);
    // Key EN
	if (keyEnter != 0)
	{
        button_pressed = TRUE;
		color = palette_pc[4];	// Green light
//		log_printf_cb(RETRO_LOG_INFO, "[%s] Key EN pressed.\n", APP_NAME);
	}
	else
	{
		color = palette_pc[14];	// Gray
	}
    boxRGBA(frame_renderer, posx+74+65, posy+97, posx+74+65+28, posy+97+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), 255);

	if (retro_use_audio_cb == false)
		audio_callback();

	video_cb(frame_surface->pixels, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH*sizeof(uint32_t));
}

static void audio_set_state(bool enable)
{
   (void)enable;
}

#ifdef _WIN32
	#define dir_delimiter '\\'
#else
	#define dir_delimiter '/'
#endif
#define MAX_FILENAME 512
#define PNGSIGBYTES 8

typedef struct
{
	const png_bytep data;
	const png_size_t size;
} PngDataHandle;

typedef struct
{
	const PngDataHandle datahandle;
	png_size_t offset;
} PngReadDataHandle;

void png_read_data_from_inputstream(png_structp png_ptr, png_bytep raw_data, png_size_t read_length)
{
	PngReadDataHandle *handle = (PngReadDataHandle *)png_get_io_ptr(png_ptr);
	if(handle == NULL)
	{
		log_printf_cb(RETRO_LOG_ERROR, "[%s] png_get_io_ptr() KO\n", APP_NAME);
		return;
	}

	const png_bytep png_src = handle->datahandle.data + handle->offset;

	if(memcpy((char *)raw_data, (char *)png_src, (size_t)read_length) == NULL)
	{
		log_printf_cb(RETRO_LOG_ERROR, "[%s] memcpy() KO\n", APP_NAME);
		return;
	}
	handle->offset += read_length;
}

bool retro_load_game(const struct retro_game_info *info)
{
	log_printf_cb(RETRO_LOG_INFO, "[%s] retro_load_game()\n", APP_NAME);

/*
	// Load custom core settings
	update_variables();
*/
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

	// SDL Surface
    frame_surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 8*sizeof(uint32_t), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    if (frame_surface == NULL)
	{
        log_printf_cb(RETRO_LOG_ERROR, "[%s] SDL surface creation failed! %s\n", APP_NAME, SDL_GetError());
        return false;
	}
	log_printf_cb(RETRO_LOG_INFO, "[%s] SDL surface created.\n", APP_NAME);

	// SDL Surface Renderer
    frame_renderer = SDL_CreateSoftwareRenderer(frame_surface);
    if (!frame_renderer)
	{
        log_printf_cb(RETRO_LOG_ERROR, "[%s] SDL renderer creation for surface failed! %s\n", APP_NAME, SDL_GetError());
        return false;
    }

	// Paint it black
	SDL_SetRenderDrawColor(frame_renderer, 0, 0, 0, 255);
	SDL_RenderClear(frame_renderer);

	struct retro_audio_callback audio_cb = { audio_callback, audio_set_state };
	retro_use_audio_cb = environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &audio_cb);
/*
	check_variables();
*/
/*
    struct retro_frame_time_callback frame_cb;
    frame_cb.callback  = frame_time_cb;
    frame_cb.reference = CPU_CLOCKS / FRAMES_PER_SEC;
    frame_cb.callback(frame_cb.reference);
    environ_cb(RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK, &frame_cb);
*/
	retro_game_path[0] = 0;
    if (info && info->data)
	{
		snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);
		log_printf_cb(RETRO_LOG_INFO, "[%s] load game: %s\n", APP_NAME, retro_game_path);
/*
		// Test libzip
		int err = 0;
		zip *z = zip_open(retro_game_path, 0, &err);
		if (z)
		{
			log_printf_cb(RETRO_LOG_INFO, "[%s] zip open OK\n", APP_NAME);
			zip_close(z);
		}
		else
		{
			log_printf_cb(RETRO_LOG_INFO, "[%s] zip open KO\n", APP_NAME);
		}
		//Search for the file of given name
		const char *name = "cartfrtop.png";
		struct zip_stat st;
		zip_stat_init(&st);
		zip_stat(z, name, 0, &st);
		//Alloc memory for its uncompressed contents
		char *contents = new char[st.size];
		//Read the compressed file
		zip_file *f = zip_fopen(z, name, 0);
		zip_fread(f, contents, st.size);
		zip_fclose(f);
		//And close the archive
		//Do something with the contents
		//delete allocated memory
		delete[] contents;
*/
		unzFile zipfile = unzOpen(retro_game_path);
		if (zipfile == NULL)
		{
			log_printf_cb(RETRO_LOG_ERROR, "[%s] game ZIP archive not found!\n", APP_NAME);
			return false;
		}
		log_printf_cb(RETRO_LOG_INFO, "[%s] game ZIP archive found\n", APP_NAME);

		// Get info about the zip file
		unz_global_info global_info;
		if (unzGetGlobalInfo( zipfile, &global_info ) != UNZ_OK)
		{
			log_printf_cb(RETRO_LOG_ERROR, "[%s] can't read game ZIP archive!\n", APP_NAME);
			unzClose(zipfile);
			return false;
		}
//		log_printf_cb(RETRO_LOG_ERROR, "[%s] game ZIP archive read\n", APP_NAME);
		log_printf_cb(RETRO_LOG_ERROR, "[%s] number of entries %d\n", APP_NAME, global_info.number_entry);

		// Buffer to hold data read from the zip file.
		uint8_t *read_buffer = NULL;
/*
		// Loop to extract all files
		for (uLong i = 0; i < global_info.number_entry; i++)
*/
		unz_file_info file_info;
		if (unzLocateFile(zipfile, "cartjpfront.png", 2) == UNZ_OK)
		{
			// Get info about current file.
			char filename[MAX_FILENAME];
			if (unzGetCurrentFileInfo(
				zipfile,
				&file_info,
				filename,
				MAX_FILENAME,
				NULL, 0, NULL, 0 ) != UNZ_OK)
			{
				log_printf_cb(RETRO_LOG_ERROR, "[%s] could not read file info\n", APP_NAME);
				unzClose(zipfile);
				return false;
			}
//			log_printf_cb(RETRO_LOG_INFO, "[%s] file info read\n", APP_NAME);

			// Check if this entry is a directory or file.
/*
			const size_t filename_length = strlen( filename );
			if ( filename[filename_length-1] == dir_delimiter )
			{
				// Entry is a directory, so create it.
				log_printf_cb(RETRO_LOG_INFO, "[%s] dir: %s\n", APP_NAME, filename);
				mkdir( filename );
			}
			else
*/
/*
			if (unzStringFileNameCompare(filename, "cartfrfront.png", 2) == 0)
			{
*/
				// Entry is a file, so extract it.
				log_printf_cb(RETRO_LOG_INFO, "[%s] file: %s\n", APP_NAME, filename);
				log_printf_cb(RETRO_LOG_INFO, "[%s] uncompressed size: %u\n", APP_NAME, file_info.uncompressed_size);
				log_printf_cb(RETRO_LOG_INFO, "[%s] crc: %u\n", APP_NAME, file_info.crc);

				if ( unzOpenCurrentFile( zipfile ) != UNZ_OK )
				{
					log_printf_cb(RETRO_LOG_ERROR, "[%s] can't open file\n", APP_NAME);
					unzClose( zipfile );
					return false;
				}
/*
				// Open a file to write out the data.
				FILE *out = fopen( filename, "wb" );
				if ( out == NULL )
				{
					printf( "could not open destination file\n" );
					unzCloseCurrentFile( zipfile );
					unzClose( zipfile );
					return -1;
				}
*/
				read_buffer = (uint8_t*)malloc(file_info.uncompressed_size);
				if ( read_buffer == NULL )
				{
					log_printf_cb(RETRO_LOG_ERROR, "[%s] can't allocate memory for the image!\n", APP_NAME);
					unzCloseCurrentFile( zipfile );
					unzClose( zipfile );
					return false;
				}
				log_printf_cb(RETRO_LOG_INFO, "[%s] Memory allocated for PNG image!\n", APP_NAME);

				int error = UNZ_OK;
				do
				{
					error = unzReadCurrentFile( zipfile, read_buffer, file_info.uncompressed_size );
					if ( error < 0 )
					{
						log_printf_cb(RETRO_LOG_ERROR, "[%s] error %d\n", APP_NAME, error);
						unzCloseCurrentFile( zipfile );
						unzClose( zipfile );
						return false;
					}
					log_printf_cb(RETRO_LOG_INFO, "[%s] PNG image loaded into memory\n", APP_NAME);

/*
					// Write data to file.
					if ( error > 0 )
					{
						fwrite( read_buffer, error, 1, out ); // You should check return of fwrite...
					}
*/
				} while ( error > 0 );
/*
				fclose( out );
*/
/*
			}
*/
			unzCloseCurrentFile( zipfile );
			log_printf_cb(RETRO_LOG_INFO, "[%s] ZIP file closed\n", APP_NAME);

/*
			// Go the the next entry listed in the zip file.
			if ( ( i+1 ) < global_info.number_entry )
			{
				if ( unzGoToNextFile( zipfile ) != UNZ_OK )
				{
					log_printf_cb(RETRO_LOG_ERROR, "[%s] cound not read next file\n", APP_NAME);
					unzClose( zipfile );
					return false;
				}
			}
*/
		}
		unzClose(zipfile);


		if (read_buffer != NULL)
		{
/*
			uint8_t *read_buffer_sig[9];
			memset(read_buffer_sig,0,9);
			memcpy(read_buffer_sig, read_buffer, 8);
			log_printf_cb(RETRO_LOG_INFO, "[%s] PNG signature = %s\n", APP_NAME, read_buffer_sig);
*/
			if(png_check_sig(read_buffer,8))
			{
				log_printf_cb(RETRO_LOG_INFO, "[%s] PNG signature OK\n", APP_NAME);

				// get PNG file info struct (memory is allocated by libpng)
				png_structp png_ptr = NULL;
				png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

				if(png_ptr != NULL)
				{
					log_printf_cb(RETRO_LOG_INFO, "[%s] PNG read struct created\n", APP_NAME);

					// get PNG image data info struct (memory is allocated by libpng)
					png_infop info_ptr = NULL;
					info_ptr = png_create_info_struct(png_ptr);

					if(info_ptr != NULL)
					{
						log_printf_cb(RETRO_LOG_INFO, "[%s] PNG info struct created\n", APP_NAME);
						PngReadDataHandle a = {{read_buffer,file_info.uncompressed_size},0};
						png_set_read_fn(png_ptr, &a, png_read_data_from_inputstream);
						log_printf_cb(RETRO_LOG_INFO, "[%s] PNG read fn setted\n", APP_NAME);
						png_read_info(png_ptr, info_ptr);
						log_printf_cb(RETRO_LOG_INFO, "[%s] png_read_info()\n", APP_NAME);
						image_width = png_get_image_width(png_ptr, info_ptr);
						log_printf_cb(RETRO_LOG_INFO, "[%s] image_width = %u\n", APP_NAME, image_width);
						image_height = png_get_image_height(png_ptr, info_ptr);
						log_printf_cb(RETRO_LOG_INFO, "[%s] image_height = %u\n", APP_NAME, image_height);
						image_bit_depth = png_get_bit_depth(png_ptr, info_ptr);
						log_printf_cb(RETRO_LOG_INFO, "[%s] image_bit_depth = %u\n", APP_NAME, image_bit_depth);
						image_color_type = png_get_color_type(png_ptr, info_ptr);
						log_printf_cb(RETRO_LOG_INFO, "[%s] image_color_type = %u\n", APP_NAME, image_color_type);

						// ignored image interlacing, compression and filtering.

						// force 8-bit color channels:
						if (image_bit_depth == 16)
							png_set_strip_16(png_ptr);
						else if (image_bit_depth < 8)
							png_set_packing(png_ptr);

						// force formats to RGB:
						if (image_color_type != PNG_COLOR_TYPE_RGBA)
							png_set_expand(png_ptr);
						else if (image_color_type == PNG_COLOR_TYPE_PALETTE)
							png_set_palette_to_rgb(png_ptr);
						else if (image_color_type == PNG_COLOR_TYPE_GRAY)
							png_set_gray_to_rgb(png_ptr);
						else if (image_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
							png_set_gray_to_rgb(png_ptr);

						// add full opacity alpha channel if required:
						if (image_color_type != PNG_COLOR_TYPE_RGBA)
							png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

						// apply the output transforms before reading image data:
						png_read_update_info(png_ptr, info_ptr);

						// allocate RGBA image data:
						image_data = (png_byte *)malloc((size_t)(image_width * image_height * (4)));
						if (image_data == NULL)
							log_printf_cb(RETRO_LOG_ERROR, "[%s] error allocating image buffer\n", APP_NAME);
						log_printf_cb(RETRO_LOG_INFO, "[%s] image buffer allocated\n", APP_NAME);

						// allocate row pointers:
						image_row_data = (png_bytep *)malloc((size_t)(image_height * sizeof(png_bytep)));
						if (image_row_data == NULL)
							log_printf_cb(RETRO_LOG_ERROR, "[%s] error allocating row pointers\n", APP_NAME);
						log_printf_cb(RETRO_LOG_INFO, "[%s] row pointers allocated\n", APP_NAME);

						// set the row pointers and read the RGBA image data:
						for (png_uint_32 row = 0; row < image_height; row++)
							image_row_data[row] = image_data + (image_height - (row + 1)) * (image_width * (4));
						log_printf_cb(RETRO_LOG_INFO, "[%s] row pointers set\n", APP_NAME);

						png_read_image(png_ptr, image_row_data);
						log_printf_cb(RETRO_LOG_INFO, "[%s] png_read_image()\n", APP_NAME);

						// libpng and dynamic resource unwinding:
						png_read_end(png_ptr, NULL);
						log_printf_cb(RETRO_LOG_INFO, "[%s] png_read_end()\n", APP_NAME);
						/*
						png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);
						log_printf_cb(RETRO_LOG_INFO, "[%s] png_get_IHDR()\n", APP_NAME);
						if(retval != 1)
						{
							log_printf_cb(RETRO_LOG_INFO, "[%s] PNG width:%u \n", APP_NAME, width);
							log_printf_cb(RETRO_LOG_INFO, "[%s] PNG height:%u \n", APP_NAME, height);
							log_printf_cb(RETRO_LOG_INFO, "[%s] TEST TEST TEST\n", APP_NAME);




						}
						else
						{
							log_printf_cb(RETRO_LOG_ERROR, "[%s] Can't read PNG header!\n", APP_NAME);
						}
						*/

//						png_destroy_info_struct(&png_ptr, NULL, NULL);


					}
					else
					{
						log_printf_cb(RETRO_LOG_ERROR, "[%s] Can't create PNG info struct!\n", APP_NAME);
					}
					png_destroy_read_struct(&png_ptr, NULL, NULL);
					log_printf_cb(RETRO_LOG_INFO, "[%s] png_destroy_read_struct()\n", APP_NAME);
				}
				else
				{
					log_printf_cb(RETRO_LOG_ERROR, "[%s] Can't create PNG read struct!\n", APP_NAME);
				}
			}
			else
			{
				log_printf_cb(RETRO_LOG_WARN, "[%s] PNG signature KO\n", APP_NAME);
			}



			free(read_buffer);
			read_buffer = NULL;
		}

		game_loaded = true;
	}
	else
	{
		log_printf_cb(RETRO_LOG_INFO, "[%s] no game\n", APP_NAME);
		game_loaded = false;
	}


    return true;
}

void retro_unload_game(void)
{
	log_printf_cb(RETRO_LOG_INFO, "[%s] retro_unload_game()\n", APP_NAME);

	if(image_data != NULL)
	{
		free(image_data);
		image_data = NULL;
	}
	if(image_row_data != NULL)
	{
		free(image_row_data);
		image_row_data = NULL;
	}
	image_width = 0;
	image_height = 0;
	image_bit_depth = 0;
	image_color_type = -1;

	SDL_FreeSurface(frame_surface);
	frame_surface = NULL;

	game_loaded = false;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
	log_printf_cb(RETRO_LOG_INFO, "[%s] retro_load_game_special()\n", APP_NAME);
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
