#ifndef EMUSCV_H__
#define EMUSCV_H__

#ifndef _SCV
	#define _SCV
#endif

#ifndef _LIBRETRO
	#define _LIBRETRO
#endif

#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif
#include <math.h>

// SCV includes
#include "emu.h"

#define APP_NAME					"Libretro-EmuSCV"
#define APP_DESCRIPTION				"EmuSCV is an open source emulator of EPOCH/YENO Super Cassette Vision under General Public Licence (GPL), based on SCV and MESS/MAME"
#ifndef APP_VERSION_MAJOR
    #define APP_VERSION_MAJOR		"0"
#endif
#ifndef APP_VERSION_MINOR
    #define APP_VERSION_MINOR		"00"
#endif
#ifndef APP_VERSION_DATETIME
    #define APP_VERSION_DATETIME	""
#endif
#ifndef APP_VERSION
    #define APP_VERSION				"0.00"
#endif
#define APP_AUTHOR					"Maxime MARCONATO (aka MaaaX) - maxime@maaax.com"
#define APP_EXTENSIONS				"bin|rom|scv|0"

// device informations for virtual machine
#define CYCLES_PER_FRAME			CPU_CLOCKS / FRAMES_PER_SEC

static const scrntype_t palette_pc[16] =
{
	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(0, 0, 136),		// 0 BLUE 1 (MEDIUM)

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(8, 8, 8),			// 1 BLACK

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(0, 0, 224),		// 2 BLUE (LIGHT)

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(104, 0, 200),		// 3 PURPLE

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(0, 248, 0),		// 4 GREEN 1 (LIGHT)

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(128, 232, 122),	// 5 GREEN 2 (SOFT)

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(0, 248, 224),		// 6 CYAN

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(0, 152, 0),		// 7 GREEN 3 (MEDIUM)

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(216, 0, 0),		// 8 RED

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(224, 152, 0),		// 9 ORANGE

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(184, 0, 200),		// 10 FUSCHIA

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(248, 168, 160),	// 11 PINK / NUDE

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(248, 240, 16),	// 12 YELLOW

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(120, 136, 0), 	// 13 GREEN 4 (DARK) / MAROON

	//RGB_COLOR(128, 128, 128),
	RGB_COLOR(136, 136, 128),	// 14 GRAY

	//RGB_COLOR(128, 128, 128)
	RGB_COLOR(232, 232, 232)	// 15 WHITE
};

class cEmuSCV
{
	public:
		cEmuSCV();
		~cEmuSCV();

	private:

	protected:

};

#endif // EMUSCV_H__
