/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2006.08.18 -

	Modified for Libretro-EmuSCV
	Author : MARCONATO Maxime (aka MaaaX)
	Date   : 2019-12-05 - 

	[ config ]
*/

#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "config.h"

config_t config;

#ifndef CONFIG_NAME
#define CONFIG_NAME "conf"
#endif

BOOL MyWritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName)
{
	return MyWritePrivateProfileString(lpAppName, lpKeyName, create_string(_T("%d"), Value), lpFileName);
}

BOOL MyWritePrivateProfileBool(LPCTSTR lpAppName, LPCTSTR lpKeyName, bool Value, LPCTSTR lpFileName)
{
	return MyWritePrivateProfileString(lpAppName, lpKeyName, create_string(_T("%d"), Value ? 1 : 0), lpFileName);
}

bool MyGetPrivateProfileBool(LPCTSTR lpAppName, LPCTSTR lpKeyName, bool bDefault, LPCTSTR lpFileName)
{
	return (MyGetPrivateProfileInt(lpAppName, lpKeyName, bDefault ? 1 : 0, lpFileName) != 0);
}

void initialize_config()
{
	// initial settings
	memset(&config, 0, sizeof(config_t));

	// memo: set only non zero value

	// control
	#if defined(USE_BOOT_MODE) && defined(BOOT_MODE_DEFAULT)
		config.boot_mode = BOOT_MODE_DEFAULT;
	#endif
	#if defined(USE_CPU_TYPE) && defined(CPU_TYPE_DEFAULT)
		config.cpu_type = CPU_TYPE_DEFAULT;
	#endif
	#if defined(USE_DIPSWITCH) && defined(DIPSWITCH_DEFAULT)
		config.dipswitch = DIPSWITCH_DEFAULT;
	#endif
	#if defined(USE_DEVICE_TYPE) && defined(DEVICE_TYPE_DEFAULT)
		config.device_type = DEVICE_TYPE_DEFAULT;
	#endif
	#if defined(USE_DRIVE_TYPE) && defined(DRIVE_TYPE_DEFAULT)
		config.drive_type = DRIVE_TYPE_DEFAULT;
	#endif
	#if defined(USE_KEYBOARD_TYPE) && defined(KEYBOARD_TYPE_DEFAULT)
		config.keyboard_type = KEYBOARD_TYPE_DEFAULT;
	#endif
	#if defined(USE_MOUSE_TYPE) && defined(MOUSE_TYPE_DEFAULT)
		config.mouse_type = MOUSE_TYPE_DEFAULT;
	#endif
	#if defined(USE_JOYSTICK_TYPE) && defined(JOYSTICK_TYPE_DEFAULT)
		config.joystick_type = JOYSTICK_TYPE_DEFAULT;
	#endif
	#if defined(USE_SOUND_TYPE) && defined(SOUND_TYPE_DEFAULT)
		config.sound_type = SOUND_TYPE_DEFAULT;
	#endif
	#if defined(USE_MONITOR_TYPE) && defined(MONITOR_TYPE_DEFAULT)
		config.monitor_type = MONITOR_TYPE_DEFAULT;
	#endif
	#if defined(USE_PRINTER_TYPE) && defined(PRINTER_TYPE_DEFAULT)
		config.printer_type = PRINTER_TYPE_DEFAULT;
	#endif
	#if defined(USE_FLOPPY_DISK)
		for(int drv = 0; drv < USE_FLOPPY_DISK; drv++) {
			#if defined(CORRECT_DISK_TIMING_DEFAULT)
				config.correct_disk_timing[drv] = CORRECT_DISK_TIMING_DEFAULT;
			#else
				config.correct_disk_timing[drv] = true;
			#endif
			#if defined(IGNORE_DISK_CRC_DEFAULT)
				config.ignore_disk_crc[drv] = IGNORE_DISK_CRC_DEFAULT;
			#endif
		}
	#endif
	#if defined(USE_TAPE)
		for(int drv = 0; drv < USE_TAPE; drv++) {
			config.wave_shaper[drv] = true;
			config.direct_load_mzt[drv] = true;
			config.baud_high[drv] = true;
		}
	#endif
	config.compress_state = true;

	// screen
	#ifndef ONE_BOARD_MICRO_COMPUTER
		config.fullscreen_stretch_type = 1;	// Stretch (Aspect)
	#endif

	// sound
	#ifdef SOUND_RATE_DEFAULT
		config.sound_frequency = SOUND_RATE_DEFAULT;
	#else
		config.sound_frequency = 6;	// 48KHz
	#endif
	config.sound_latency = 1;	// 100msec
	config.sound_strict_rendering = true;
	#ifdef USE_FLOPPY_DISK
		config.sound_noise_fdd = true;
	#endif
	#ifdef USE_TAPE
		config.sound_noise_cmt = true;
		config.sound_play_tape = true;
	#endif

	// input
	#ifdef USE_JOYSTICK
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 16; j++) {
				config.joy_buttons[i][j] = (i << 5) | j;
			}
		}
		config.use_joy_to_key = false;
		config.joy_to_key_type = 2;
		config.joy_to_key_numpad5 = false;
		config.joy_to_key_buttons[0] = -('Z');
		config.joy_to_key_buttons[1] = -('X');
	#endif

	// win32
	#ifdef _WIN32
		#ifndef ONE_BOARD_MICRO_COMPUTER
			config.use_d3d9 = true;
		#endif
		config.use_dinput = true;
		config.show_status_bar = true;
	#endif

	// qt
	#ifdef _USE_QT
		config.render_platform = CONFIG_RENDER_PLATFORM_OPENGL_MAIN;
		config.render_major_version = 2; // For crash with some devices.
		config.render_minor_version = 1;
		config.rendering_type = CONFIG_RENDER_TYPE_STD;
	#endif
}

void load_config(const _TCHAR* config_path)
{
	// initial settings
	initialize_config();

	// control
	#ifdef USE_BOOT_MODE
		config.boot_mode = MyGetPrivateProfileInt(_T("Control"), _T("BootMode"), config.boot_mode, config_path);
	#endif
	#ifdef USE_CPU_TYPE
		config.cpu_type = MyGetPrivateProfileInt(_T("Control"), _T("CPUType"), config.cpu_type, config_path);
	#endif
	#ifdef USE_DIPSWITCH
		config.dipswitch = MyGetPrivateProfileInt(_T("Control"), _T("DipSwitch"), config.dipswitch, config_path);
	#endif
	#ifdef USE_DEVICE_TYPE
		config.device_type = MyGetPrivateProfileInt(_T("Control"), _T("DeviceType"), config.device_type, config_path);
	#endif
	#ifdef USE_DRIVE_TYPE
		config.drive_type = MyGetPrivateProfileInt(_T("Control"), _T("DriveType"), config.drive_type, config_path);
	#endif
	#ifdef USE_KEYBOARD_TYPE
		config.keyboard_type = MyGetPrivateProfileInt(_T("Control"), _T("KeyboardType"), config.keyboard_type, config_path);
	#endif
	#ifdef USE_MOUSE_TYPE
		config.mouse_type = MyGetPrivateProfileInt(_T("Control"), _T("MouseType"), config.mouse_type, config_path);
	#endif
	#ifdef USE_JOYSTICK_TYPE
		config.joystick_type = MyGetPrivateProfileInt(_T("Control"), _T("JoystickType"), config.joystick_type, config_path);
	#endif
	#ifdef USE_SOUND_TYPE
		config.sound_type = MyGetPrivateProfileInt(_T("Control"), _T("SoundType"), config.sound_type, config_path);
	#endif
	#ifdef USE_MONITOR_TYPE
		config.monitor_type = MyGetPrivateProfileInt(_T("Control"), _T("MonitorType"), config.monitor_type, config_path);
	#endif
	#ifdef USE_SCANLINE
		config.scan_line = MyGetPrivateProfileBool(_T("Control"), _T("ScanLine"), config.scan_line, config_path);
	#endif
	#ifdef USE_PRINTER
		config.printer_type = MyGetPrivateProfileInt(_T("Control"), _T("PrinterType"), config.printer_type, config_path);
	#endif
	#ifdef USE_FLOPPY_DISK
		for(int drv = 0; drv < USE_FLOPPY_DISK; drv++) {
			config.correct_disk_timing[drv] = MyGetPrivateProfileBool(_T("Control"), create_string(_T("CorrectDiskTiming%d"), drv + 1), config.correct_disk_timing[drv], config_path);
			config.ignore_disk_crc[drv] = MyGetPrivateProfileBool(_T("Control"), create_string(_T("IgnoreDiskCRC%d"), drv + 1), config.ignore_disk_crc[drv], config_path);
		}
	#endif
	#ifdef USE_TAPE
		for(int drv = 0; drv < USE_TAPE; drv++) {
			config.wave_shaper[drv] = MyGetPrivateProfileBool(_T("Control"), create_string(_T("WaveShaper%d"), drv + 1), config.wave_shaper[drv], config_path);
			config.direct_load_mzt[drv] = MyGetPrivateProfileBool(_T("Control"), create_string(_T("DirectLoadMZT%d"), drv + 1), config.direct_load_mzt[drv], config_path);
			config.baud_high[drv] = MyGetPrivateProfileBool(_T("Control"), create_string(_T("BaudHigh%d"), drv + 1), config.baud_high[drv], config_path);
		}
	#endif
	config.compress_state = MyGetPrivateProfileBool(_T("Control"), _T("CompressState"), config.compress_state, config_path);

	// recent files
	#ifdef USE_CART
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialCartDir"), _T(""), config.initial_cart_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_CART; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentCartPath%d_%d"), drv + 1, i + 1), _T(""), config.recent_cart_path[drv][i], _MAX_PATH, config_path);
			}
		}
	#endif
	#ifdef USE_FLOPPY_DISK
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialDiskDir"), _T(""), config.initial_floppy_disk_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_FLOPPY_DISK; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentDiskPath%d_%d"), drv + 1, i + 1), _T(""), config.recent_floppy_disk_path[drv][i], _MAX_PATH, config_path);
			}
		}
	#endif
	#ifdef USE_QUICK_DISK
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialQuickDiskDir"), _T(""), config.initial_quick_disk_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_QUICK_DISK; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentQuickDiskPath%d_%d"), drv + 1, i + 1), _T(""), config.recent_quick_disk_path[drv][i], _MAX_PATH, config_path);
			}
		}
	#endif
	#ifdef USE_HARD_DISK
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialHardDiskDir"), _T(""), config.initial_hard_disk_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_HARD_DISK; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentHardDiskPath%d_%d"), drv + 1, i + 1), _T(""), config.recent_hard_disk_path[drv][i], _MAX_PATH, config_path);
			}
			MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("LastHardDiskPath%d"), drv + 1), _T(""), config.last_hard_disk_path[drv], _MAX_PATH, config_path);
		}
	#endif
	#ifdef USE_TAPE
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialTapeDir"), _T(""), config.initial_tape_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_TAPE; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentTapePath%d_%d"), drv + 1, i + 1), _T(""), config.recent_tape_path[drv][i], _MAX_PATH, config_path);
			}
		}
	#endif
	#ifdef USE_COMPACT_DISC
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialCompactDiscDir"), _T(""), config.initial_compact_disc_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_COMPACT_DISC; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentCompactDiscPath%d_%d"), drv + 1, i + 1), _T(""), config.recent_compact_disc_path[drv][i], _MAX_PATH, config_path);
			}
		}
	#endif
	#ifdef USE_LASER_DISC
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialLaserDiscDir"), _T(""), config.initial_laser_disc_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_LASER_DISC; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentLaserDiscPath%d_%d"), drv + 1, i + 1), _T(""), config.recent_laser_disc_path[drv][i], _MAX_PATH, config_path);
			}
		}
	#endif
	#ifdef USE_BINARY_FILE
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialBinaryDir"), _T(""), config.initial_binary_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_BINARY_FILE; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentBinaryPath%d_%d"), drv + 1, i + 1), _T(""), config.recent_binary_path[drv][i], _MAX_PATH, config_path);
			}
		}
	#endif
	#ifdef USE_BUBBLE
		MyGetPrivateProfileString(_T("RecentFiles"), _T("InitialBubbleDir"), _T(""), config.initial_bubble_casette_dir, _MAX_PATH, config_path);
		for(int drv = 0; drv < USE_BUBBLE; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyGetPrivateProfileString(_T("RecentFiles"), create_string(_T("RecentBubblePath%d_%d"), drv + 1, i + 1), _T(""), config.recent_bubble_casette_path[drv][i], _MAX_PATH, config_path);
			}
		}
	#endif

	// screen
	#ifndef ONE_BOARD_MICRO_COMPUTER
		config.window_mode = MyGetPrivateProfileInt(_T("Screen"), _T("WindowMode"), config.window_mode, config_path);
		config.window_stretch_type = MyGetPrivateProfileInt(_T("Screen"), _T("WindowStretchType"), config.window_stretch_type, config_path);
		config.fullscreen_stretch_type = MyGetPrivateProfileInt(_T("Screen"), _T("FullScreenStretchType"), config.fullscreen_stretch_type, config_path);
//		#ifdef USE_SCREEN_ROTATE
			config.rotate_type = MyGetPrivateProfileInt(_T("Screen"), _T("RotateType"), config.rotate_type, config_path);
//		#endif
	#endif

	// filter
	#ifdef USE_SCREEN_FILTER
		config.filter_type = MyGetPrivateProfileInt(_T("Screen"), _T("FilterType"), config.filter_type, config_path);
	#endif

	// sound
	config.sound_frequency = MyGetPrivateProfileInt(_T("Sound"), _T("Frequency"), config.sound_frequency, config_path);
	config.sound_latency = MyGetPrivateProfileInt(_T("Sound"), _T("Latency"), config.sound_latency, config_path);
	config.sound_strict_rendering = MyGetPrivateProfileBool(_T("Sound"), _T("StrictRendering"), config.sound_strict_rendering, config_path);
	#ifdef USE_FLOPPY_DISK
		config.sound_noise_fdd = MyGetPrivateProfileBool(_T("Sound"), _T("NoiseFDD"), config.sound_noise_fdd, config_path);;
	#endif
	#ifdef USE_TAPE
		config.sound_noise_cmt = MyGetPrivateProfileBool(_T("Sound"), _T("NoiseCMT"), config.sound_noise_cmt, config_path);;
		config.sound_play_tape = MyGetPrivateProfileBool(_T("Sound"), _T("PlayTape"), config.sound_play_tape, config_path);
	#endif
	#ifdef USE_SOUND_VOLUME
		for(int i = 0; i < USE_SOUND_VOLUME; i++) {
			int tmp_l = MyGetPrivateProfileInt(_T("Sound"), create_string(_T("VolumeLeft%d"), i + 1), config.sound_volume_l[i], config_path);
			int tmp_r = MyGetPrivateProfileInt(_T("Sound"), create_string(_T("VolumeRight%d"), i + 1), config.sound_volume_r[i], config_path);
			#ifdef _USE_QT
				// Note: when using balance , levels are -40???}20db to 0???}20db.
				config.sound_volume_l[i] = max(-60, min(20, tmp_l));
				config.sound_volume_r[i] = max(-60, min(20, tmp_r));
			#else
				config.sound_volume_l[i] = max(-40, min(0, tmp_l));
				config.sound_volume_r[i] = max(-40, min(0, tmp_r));
			#endif
		}
	#endif
	#ifdef _WIN32
		// for compatibilities
		#ifdef _X1_H_
			MyGetPrivateProfileString(_T("Sound"), _T("FMGenDll"), _T("mame2151.dll"), config.mame2151_dll_path, _MAX_PATH, config_path);
			my_tcscpy_s(config.mame2608_dll_path, _MAX_PATH, _T("mamefm.dll"));
		#else
			MyGetPrivateProfileString(_T("Sound"), _T("FMGenDll"), _T("mamefm.dll"), config.mame2608_dll_path, _MAX_PATH, config_path);
			my_tcscpy_s(config.mame2151_dll_path, _MAX_PATH, _T("mame2151.dll"));
		#endif
		MyGetPrivateProfileString(_T("Sound"), _T("YM2151GenDll"), config.mame2151_dll_path, config.mame2151_dll_path, _MAX_PATH, config_path);
		MyGetPrivateProfileString(_T("Sound"), _T("YM2608GenDll"), config.mame2608_dll_path, config.mame2608_dll_path, _MAX_PATH, config_path);
	#endif

	// input
	#ifdef USE_JOYSTICK
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 16; j++) {
				int old = (i << 4) | j;
				old = MyGetPrivateProfileInt(_T("Input"), create_string(_T("JoyButtons%d_%d"), i + 1, j + 1), old, config_path);
				old = ((old >> 4) << 5) | (old & 0x0f);
				config.joy_buttons[i][j] = MyGetPrivateProfileInt(_T("Input"), create_string(_T("JoyButtonsEx%d_%d"), i + 1, j + 1), old, config_path);
			}
		}
		config.use_joy_to_key = MyGetPrivateProfileBool(_T("Input"), _T("UseJoyToKey"), config.use_joy_to_key, config_path);
		config.joy_to_key_type = MyGetPrivateProfileInt(_T("Input"), _T("JoyToKeyType"), config.joy_to_key_type, config_path);
		config.joy_to_key_numpad5 = MyGetPrivateProfileBool(_T("Input"), _T("JoyToKeyNumPad5"), config.joy_to_key_numpad5, config_path);
		for(int i = 0; i < 16; i++) {
			config.joy_to_key_buttons[i] = MyGetPrivateProfileInt(_T("Input"), create_string(_T("JoyToKeyButtons%d"), i + 1), config.joy_to_key_buttons[i], config_path);
		}
	#endif

	// printer
	#ifdef USE_PRINTER
		MyGetPrivateProfileString(_T("Printer"), _T("PrinterDll"), _T("printer.dll"), config.printer_dll_path, _MAX_PATH, config_path);
	#endif

	// win32
	#ifdef _WIN32
		#ifndef ONE_BOARD_MICRO_COMPUTER
			config.use_d3d9 = MyGetPrivateProfileBool(_T("Win32"), _T("UseDirect3D9"), config.use_d3d9, config_path);
			config.wait_vsync = MyGetPrivateProfileBool(_T("Win32"), _T("WaitVSync"), config.wait_vsync, config_path);
		#endif
		config.use_dinput = MyGetPrivateProfileBool(_T("Win32"), _T("UseDirectInput"), config.use_dinput, config_path);
		config.disable_dwm = MyGetPrivateProfileBool(_T("Win32"), _T("DisableDwm"), config.disable_dwm, config_path);
		config.show_status_bar = MyGetPrivateProfileBool(_T("Win32"), _T("ShowStatusBar"), config.show_status_bar, config_path);
	#endif

	// qt
	#ifdef _USE_QT
		config.use_opengl_scanline = MyGetPrivateProfileBool(_T("Qt"), _T("UseOpenGLScanLine"), config.use_opengl_scanline, config_path);
		config.opengl_scanline_vert = MyGetPrivateProfileBool(_T("Qt"), _T("OpenGLScanLineVert"), config.opengl_scanline_vert, config_path);;
		config.opengl_scanline_horiz = MyGetPrivateProfileBool(_T("Qt"), _T("OpenGLScanLineHoriz"), config.opengl_scanline_horiz, config_path);;
		config.use_opengl_filters = MyGetPrivateProfileBool(_T("Qt"), _T("UseOpenGLFilters"), config.use_opengl_filters, config_path);
		config.opengl_filter_num = MyGetPrivateProfileInt(_T("Qt"), _T("OpenGLFilterNum"), config.opengl_filter_num, config_path);
		config.swap_kanji_pause = MyGetPrivateProfileBool(_T("Qt"), _T("SwapKanjiPause"), config.swap_kanji_pause, config_path);
		config.render_platform = MyGetPrivateProfileInt(_T("Qt"), _T("RenderPlatform"), config.render_platform, config_path);
		config.render_major_version = MyGetPrivateProfileInt(_T("Qt"), _T("RenderMajorVersion"), config.render_major_version, config_path);
		config.render_minor_version = MyGetPrivateProfileInt(_T("Qt"), _T("RenderMinorVersion"), config.render_minor_version, config_path);
		config.rendering_type = MyGetPrivateProfileInt(_T("Qt"), _T("RenderType"), config.rendering_type, config_path);
		if(config.rendering_type < 0) config.rendering_type = 0;
		if(config.rendering_type >= CONFIG_RENDER_TYPE_END) config.rendering_type = CONFIG_RENDER_TYPE_END - 1;
	#endif
}

void save_config(const _TCHAR* config_path)
{
	// control
	#ifdef USE_BOOT_MODE
		MyWritePrivateProfileInt(_T("Control"), _T("BootMode"), config.boot_mode, config_path);
	#endif
	#ifdef USE_CPU_TYPE
		MyWritePrivateProfileInt(_T("Control"), _T("CPUType"), config.cpu_type, config_path);
	#endif
	#ifdef USE_DIPSWITCH
		MyWritePrivateProfileInt(_T("Control"), _T("DipSwitch"), config.dipswitch, config_path);
	#endif
	#ifdef USE_DEVICE_TYPE
		MyWritePrivateProfileInt(_T("Control"), _T("DeviceType"), config.device_type, config_path);
	#endif
	#ifdef USE_DRIVE_TYPE
		MyWritePrivateProfileInt(_T("Control"), _T("DriveType"), config.drive_type, config_path);
	#endif
	#ifdef USE_KEYBOARD_TYPE
		MyWritePrivateProfileInt(_T("Control"), _T("KeyboardType"), config.keyboard_type, config_path);
	#endif
	#ifdef USE_MOUSE_TYPE
		MyWritePrivateProfileInt(_T("Control"), _T("MouseType"), config.mouse_type, config_path);
	#endif
	#ifdef USE_JOYSTICK_TYPE
		MyWritePrivateProfileInt(_T("Control"), _T("JoystickType"), config.joystick_type, config_path);
	#endif
	#ifdef USE_SOUND_TYPE
		MyWritePrivateProfileInt(_T("Control"), _T("SoundType"), config.sound_type, config_path);
	#endif
	#ifdef USE_MONITOR_TYPE
		MyWritePrivateProfileInt(_T("Control"), _T("MonitorType"), config.monitor_type, config_path);
	#endif
	#ifdef USE_SCANLINE
		MyWritePrivateProfileBool(_T("Control"), _T("ScanLine"), config.scan_line, config_path);
	#endif
	#ifdef USE_PRINTER
		MyWritePrivateProfileInt(_T("Control"), _T("PrinterType"), config.printer_type, config_path);
	#endif
	#ifdef USE_FLOPPY_DISK
		for(int drv = 0; drv < USE_FLOPPY_DISK; drv++) {
			MyWritePrivateProfileBool(_T("Control"), create_string(_T("CorrectDiskTiming%d"), drv + 1), config.correct_disk_timing[drv], config_path);
			MyWritePrivateProfileBool(_T("Control"), create_string(_T("IgnoreDiskCRC%d"), drv + 1), config.ignore_disk_crc[drv], config_path);
		}
	#endif
	#ifdef USE_TAPE
		for(int drv = 0; drv < USE_TAPE; drv++) {
			MyWritePrivateProfileBool(_T("Control"), create_string(_T("WaveShaper%d"), drv + 1), config.wave_shaper[drv], config_path);
			MyWritePrivateProfileBool(_T("Control"), create_string(_T("DirectLoadMZT%d"), drv + 1), config.direct_load_mzt[drv], config_path);
			MyWritePrivateProfileBool(_T("Control"), create_string(_T("BaudHigh%d"), drv + 1), config.baud_high[drv], config_path);
		}
	#endif
	MyWritePrivateProfileBool(_T("Control"), _T("CompressState"), config.compress_state, config_path);

	// recent files
	#ifdef USE_CART
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialCartDir"), config.initial_cart_dir, config_path);
		for(int drv = 0; drv < USE_CART; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentCartPath%d_%d"), drv + 1, i + 1), config.recent_cart_path[drv][i], config_path);
			}
		}
	#endif
	#ifdef USE_FLOPPY_DISK
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialDiskDir"), config.initial_floppy_disk_dir, config_path);
		for(int drv = 0; drv < USE_FLOPPY_DISK; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentDiskPath%d_%d"), drv + 1, i + 1), config.recent_floppy_disk_path[drv][i], config_path);
			}
		}
	#endif
	#ifdef USE_QUICK_DISK
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialQuickDiskDir"), config.initial_quick_disk_dir, config_path);
		for(int drv = 0; drv < USE_QUICK_DISK; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentQuickDiskPath%d_%d"), drv + 1, i + 1), config.recent_quick_disk_path[drv][i], config_path);
			}
		}
	#endif
	#ifdef USE_HARD_DISK
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialHardDiskDir"), config.initial_hard_disk_dir, config_path);
		for(int drv = 0; drv < USE_HARD_DISK; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentHardDiskPath%d_%d"), drv + 1, i + 1), config.recent_hard_disk_path[drv][i], config_path);
			}
			MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("LastHardDiskPath%d"), drv + 1), config.last_hard_disk_path[drv], config_path);
		}
	#endif
	#ifdef USE_TAPE
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialTapeDir"), config.initial_tape_dir, config_path);
		for(int drv = 0; drv < USE_TAPE; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentTapePath%d_%d"), drv + 1, i + 1), config.recent_tape_path[drv][i], config_path);
			}
		}
	#endif
	#ifdef USE_COMPACT_DISC
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialCompactDiscDir"), config.initial_compact_disc_dir, config_path);
		for(int drv = 0; drv < USE_COMPACT_DISC; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentCompactDiscPath%d_%d"), drv + 1, i + 1), config.recent_compact_disc_path[drv][i], config_path);
			}
		}
	#endif
	#ifdef USE_LASER_DISC
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialLaserDiscDir"), config.initial_laser_disc_dir, config_path);
		for(int drv = 0; drv < USE_LASER_DISC; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentLaserDiscPath%d_%d"), drv + 1, i + 1), config.recent_laser_disc_path[drv][i], config_path);
			}
		}
	#endif
	#ifdef USE_BINARY_FILE
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialBinaryDir"), config.initial_binary_dir, config_path);
		for(int drv = 0; drv < USE_BINARY_FILE; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentBinaryPath%d_%d"), drv + 1, i + 1), config.recent_binary_path[drv][i], config_path);
			}
		}
	#endif
	#ifdef USE_BUBBLE
		MyWritePrivateProfileString(_T("RecentFiles"), _T("InitialBubbleDir"), config.initial_bubble_casette_dir, config_path);
		for(int drv = 0; drv < USE_BUBBLE; drv++) {
			for(int i = 0; i < MAX_HISTORY; i++) {
				MyWritePrivateProfileString(_T("RecentFiles"), create_string(_T("RecentBubblePath%d_%d"), drv + 1, i + 1), config.recent_bubble_casette_path[drv][i], config_path);
			}
		}
	#endif

	// screen
	#ifndef ONE_BOARD_MICRO_COMPUTER
		MyWritePrivateProfileInt(_T("Screen"), _T("WindowMode"), config.window_mode, config_path);
		MyWritePrivateProfileInt(_T("Screen"), _T("WindowStretchType"), config.window_stretch_type, config_path);
		MyWritePrivateProfileInt(_T("Screen"), _T("FullScreenStretchType"), config.fullscreen_stretch_type, config_path);
//		#ifdef USE_SCREEN_ROTATE
			MyWritePrivateProfileInt(_T("Screen"), _T("RotateType"), config.rotate_type, config_path);
//		#endif
	#endif

	// filter
	#ifdef USE_SCREEN_FILTER
		MyWritePrivateProfileInt(_T("Screen"), _T("FilterType"), config.filter_type, config_path);
	#endif

	// sound
	MyWritePrivateProfileInt(_T("Sound"), _T("Frequency"), config.sound_frequency, config_path);
	MyWritePrivateProfileInt(_T("Sound"), _T("Latency"), config.sound_latency, config_path);
	MyWritePrivateProfileBool(_T("Sound"), _T("StrictRendering"), config.sound_strict_rendering, config_path);
	#ifdef USE_FLOPPY_DISK
		MyWritePrivateProfileBool(_T("Sound"), _T("NoiseFDD"), config.sound_noise_fdd, config_path);
	#endif
	#ifdef USE_TAPE
		MyWritePrivateProfileBool(_T("Sound"), _T("NoiseCMT"), config.sound_noise_cmt, config_path);
		MyWritePrivateProfileBool(_T("Sound"), _T("PlayTape"), config.sound_play_tape, config_path);
	#endif
	#ifdef USE_SOUND_VOLUME
		for(int i = 0; i < USE_SOUND_VOLUME; i++) {
			MyWritePrivateProfileInt(_T("Sound"), create_string(_T("VolumeLeft%d"), i + 1), config.sound_volume_l[i], config_path);
			MyWritePrivateProfileInt(_T("Sound"), create_string(_T("VolumeRight%d"), i + 1), config.sound_volume_r[i], config_path);
		}
	#endif
	#ifdef _WIN32
		MyWritePrivateProfileString(_T("Sound"), _T("YM2151GenDll"), config.mame2151_dll_path, config_path);
		MyWritePrivateProfileString(_T("Sound"), _T("YM2608GenDll"), config.mame2608_dll_path, config_path);
	#endif

	// input
	#ifdef USE_JOYSTICK
		for(int i = 0; i < 8; i++) {
			for(int j = 0; j < 16; j++) {
				MyWritePrivateProfileInt(_T("Input"), create_string(_T("JoyButtonsEx%d_%d"), i + 1, j + 1), config.joy_buttons[i][j], config_path);
			}
		}
		MyWritePrivateProfileBool(_T("Input"), _T("UseJoyToKey"), config.use_joy_to_key, config_path);
		MyWritePrivateProfileInt(_T("Input"), _T("JoyToKeyType"), config.joy_to_key_type, config_path);
		MyWritePrivateProfileBool(_T("Input"), _T("JoyToKeyNumPad5"), config.joy_to_key_numpad5, config_path);
		for(int i = 0; i < 16; i++) {
			MyWritePrivateProfileInt(_T("Input"), create_string(_T("JoyToKeyButtons%d"), i + 1), config.joy_to_key_buttons[i], config_path);
		}
	#endif

	// win32
	#ifdef _WIN32
		#ifndef ONE_BOARD_MICRO_COMPUTER
			MyWritePrivateProfileBool(_T("Win32"), _T("UseDirect3D9"), config.use_d3d9, config_path);
			MyWritePrivateProfileBool(_T("Win32"), _T("WaitVSync"), config.wait_vsync, config_path);
		#endif
		MyWritePrivateProfileBool(_T("Win32"), _T("UseDirectInput"), config.use_dinput, config_path);
		MyWritePrivateProfileBool(_T("Win32"), _T("DisableDwm"), config.disable_dwm, config_path);
		MyWritePrivateProfileBool(_T("Win32"), _T("ShowStatusBar"), config.show_status_bar, config_path);
	#endif

	// qt
	#ifdef _USE_QT
		MyWritePrivateProfileBool(_T("Qt"), _T("UseOpenGLScanLine"), config.use_opengl_scanline, config_path);
		MyWritePrivateProfileBool(_T("Qt"), _T("OpenGLScanLineVert"), config.opengl_scanline_vert, config_path);;
		MyWritePrivateProfileBool(_T("Qt"), _T("OpenGLScanLineHoriz"), config.opengl_scanline_horiz, config_path);;
		MyWritePrivateProfileBool(_T("Qt"), _T("UseOpenGLFilters"), config.use_opengl_filters, config_path);
		MyWritePrivateProfileInt(_T("Qt"), _T("OpenGLFilterNum"), config.opengl_filter_num, config_path);
		MyWritePrivateProfileBool(_T("Qt"), _T("SwapKanjiPause"), config.swap_kanji_pause, config_path);
		MyWritePrivateProfileInt(_T("Qt"), _T("RenderPlatform"), config.render_platform, config_path);
		MyWritePrivateProfileInt(_T("Qt"), _T("RenderMajorVersion"), config.render_major_version, config_path);
		MyWritePrivateProfileInt(_T("Qt"), _T("RenderMinorVersion"), config.render_minor_version, config_path);
		MyWritePrivateProfileInt(_T("Qt"), _T("RenderType"), config.rendering_type, config_path);
	#endif
}

#define CONFIG_STATE_ID	100000006

void save_config_state(STATE* state)
{
	state->SetValue((uint32_t)CONFIG_STATE_ID);
/*
	#ifdef USE_BOOT_MODE
		state->StateValue(config.boot_mode);
	#endif
	#ifdef USE_CPU_TYPE
		state->StateValue(config.cpu_type);
	#endif
	#ifdef USE_DIPSWITCH
		state->StateValue(config.dipswitch);
	#endif
	#ifdef USE_DEVICE_TYPE
		state->StateValue(config.device_type);
	#endif
	#ifdef USE_DRIVE_TYPE
		state->StateValue(config.drive_type);
	#endif
	#ifdef USE_KEYBOARD_TYPE
		state->StateValue(config.keyboard_type);
	#endif
	#ifdef USE_MOUSE_TYPE
		state->StateValue(config.mouse_type);
	#endif
	#ifdef USE_JOYSTICK_TYPE
		state->StateValue(config.joystick_type);
	#endif
	#ifdef USE_SOUND_TYPE
		state->StateValue(config.sound_type);
	#endif
	#ifdef USE_MONITOR_TYPE
		state->StateValue(config.monitor_type);
	#endif
	#ifdef USE_PRINTER_TYPE
		state->StateValue(config.printer_type);
	#endif
	#ifdef USE_FLOPPY_DISK
		for(int drv = 0; drv < USE_FLOPPY_DISK; drv++) {
			state->StateValue(config.correct_disk_timing[drv]);
			state->StateValue(config.ignore_disk_crc[drv]);
		}
	#endif
*/
	state->SetValue(config.sound_frequency);
	state->SetValue(config.sound_latency);
}

bool load_config_state(STATE* state)
{
	if(!state->CheckValue((uint32_t)CONFIG_STATE_ID))
		return false;
	return true;
}

#if defined(_LIBRETRO)
void DLL_PREFIX apply_display_config()
{
	switch(config.scv_console)
	{
		case SETTING_CONSOLE_EPOCH_VAL:
		case SETTING_CONSOLE_YENO_VAL:
		case SETTING_CONSOLE_EPOCHLADY_VAL:
			config.window_console = config.scv_console;
			break;
		case SETTING_CONSOLE_AUTO_VAL:
		default:
			config.window_console = SETTING_CONSOLE_EPOCH_VAL;
			break;
	}
	
	switch(config.scv_display)
	{
		case SETTING_DISPLAY_EMUSCV_VAL:
		case SETTING_DISPLAY_EPOCH_VAL:
		case SETTING_DISPLAY_YENO_VAL:
			config.screen_display = config.scv_display;
			break;
		case SETTING_DISPLAY_AUTO_VAL:
		default:
			switch(config.scv_console)
			{
				case SETTING_CONSOLE_EPOCH_VAL:
				case SETTING_CONSOLE_EPOCHLADY_VAL:
					config.screen_display = SETTING_DISPLAY_EPOCH_VAL;
					break;
				case SETTING_CONSOLE_YENO_VAL:
					config.screen_display = SETTING_DISPLAY_YENO_VAL;
					break;
				case SETTING_CONSOLE_AUTO_VAL:
				default:
					config.screen_display = SETTING_DISPLAY_EMUSCV_VAL;
					break;
			}
			break;
	}
	
	switch(config.scv_pixelaspect)
	{
		case SETTING_PIXELASPECT_RECTANGULAR_VAL:
		case SETTING_PIXELASPECT_SQUARE_VAL:
			config.window_pixelaspect = config.scv_pixelaspect;
			break;
		case SETTING_PIXELASPECT_AUTO_VAL:
		default:
			config.window_pixelaspect = SETTING_PIXELASPECT_RECTANGULAR_VAL;
			break;
	}
	
	switch(config.scv_resolution)
	{
		case SETTING_RESOLUTION_HIGH_VAL:
		case SETTING_RESOLUTION_MEDIUM_VAL:
		case SETTING_RESOLUTION_LOW_VAL:
			config.window_resolution = config.scv_resolution;
			break;
		case SETTING_RESOLUTION_AUTO_VAL:
		default:
#if defined(_RESOLUTION_AUTO_LOW)
			config.window_resolution = SETTING_RESOLUTION_LOW_VAL;
#elif defined(_RESOLUTION_AUTO_MEDIUM)
			config.window_resolution = SETTING_RESOLUTION_MEDIUM_VAL;
#else
			config.window_resolution = SETTING_RESOLUTION_HIGH_VAL;
#endif
			break;
	}

	switch(config.scv_palette)
	{
		case SETTING_PALETTE_STANDARD_VAL:
		case SETTING_PALETTE_OLDNTSC_VAL:
			config.window_palette = config.scv_palette;
			break;
		case SETTING_PALETTE_AUTO_VAL:
		default:
			config.window_palette = SETTING_PALETTE_STANDARD_VAL;
			break;
	}

	if(config.scv_displayfullmemory == SETTING_DISPLAYFULLMEMORY_YES_VAL)
	{
		config.window_displayfullmemory = SETTING_DISPLAYFULLMEMORY_YES_VAL;
		config.draw_x = DRAW_X_FULL;
		config.draw_y = DRAW_Y_FULL;
		config.draw_width = DRAW_WIDTH_FULL;
		config.draw_height = DRAW_HEIGHT_FULL;
		config.mix_x_min = MIX_X_MIN_FULL;
		config.mix_x_max = MIX_X_MAX_FULL;
		config.mix_y_min = MIX_Y_MIN_FULL;
		config.mix_y_max = MIX_Y_MAX_FULL;
		config.text_x_min = TEXT_X_MIN_FULL;
		config.text_x_max = TEXT_X_MAX_FULL;
		config.text_y_min = TEXT_Y_MIN_FULL;
		config.text_y_max = TEXT_Y_MAX_FULL;
		config.sprite_x_min = SPRITE_X_MIN_FULL;
		config.sprite_x_max = SPRITE_X_MAX_FULL;
		config.sprite_y_min = SPRITE_Y_MIN_FULL;
		config.sprite_y_max = SPRITE_Y_MAX_FULL;
	}
	else
	{
		config.window_displayfullmemory = SETTING_DISPLAYFULLMEMORY_NO_VAL;
		switch(config.screen_display)
		{
			case SETTING_DISPLAY_EPOCH_VAL:
					config.draw_x = DRAW_X_EPOCH;
					config.draw_y = DRAW_Y_EPOCH;
					config.draw_width = DRAW_WIDTH_EPOCH;
					config.draw_height = DRAW_HEIGHT_EPOCH;
					config.mix_x_min = MIX_X_MIN_EPOCH;
					config.mix_x_max = MIX_X_MAX_EPOCH;
					config.mix_y_min = MIX_Y_MIN_EPOCH;
					config.mix_y_max = MIX_Y_MAX_EPOCH;
					config.text_x_min = TEXT_X_MIN_EPOCH;
					config.text_x_max = TEXT_X_MAX_EPOCH;
					config.text_y_min = TEXT_Y_MIN_EPOCH;
					config.text_y_max = TEXT_Y_MAX_EPOCH;
					config.sprite_x_min = SPRITE_X_MIN_EPOCH;
					config.sprite_x_max = SPRITE_X_MAX_EPOCH;
					config.sprite_y_min = SPRITE_Y_MIN_EPOCH;
					config.sprite_y_max = SPRITE_Y_MAX_EPOCH;
				break;
			case SETTING_DISPLAY_YENO_VAL:
				config.draw_x = DRAW_X_YENO;
				config.draw_y = DRAW_Y_YENO;
				config.draw_width = DRAW_WIDTH_YENO;
				config.draw_height = DRAW_HEIGHT_YENO;
				config.mix_x_min = MIX_X_MIN_YENO;
				config.mix_x_max = MIX_X_MAX_YENO;
				config.mix_y_min = MIX_Y_MIN_YENO;
				config.mix_y_max = MIX_Y_MAX_YENO;
				config.text_x_min = TEXT_X_MIN_YENO;
				config.text_x_max = TEXT_X_MAX_YENO;
				config.text_y_min = TEXT_Y_MIN_YENO;
				config.text_y_max = TEXT_Y_MAX_YENO;
				config.sprite_x_min = SPRITE_X_MIN_YENO;
				config.sprite_x_max = SPRITE_X_MAX_YENO;
				config.sprite_y_min = SPRITE_Y_MIN_YENO;
				config.sprite_y_max = SPRITE_Y_MAX_YENO;
				break;
			case SETTING_DISPLAY_EMUSCV_VAL:
			case SETTING_DISPLAY_AUTO_VAL:
			default:
				config.draw_x = DRAW_X_EMUSCV;
				config.draw_y = DRAW_Y_EMUSCV;
				config.draw_width = DRAW_WIDTH_EMUSCV;
				config.draw_height = DRAW_HEIGHT_EMUSCV;
				config.mix_x_min = MIX_X_MIN_EMUSCV;
				config.mix_x_max = MIX_X_MAX_EMUSCV;
				config.mix_y_min = MIX_Y_MIN_EMUSCV;
				config.mix_y_max = MIX_Y_MAX_EMUSCV;
				config.text_x_min = TEXT_X_MIN_EMUSCV;
				config.text_x_max = TEXT_X_MAX_EMUSCV;
				config.text_y_min = TEXT_Y_MIN_EMUSCV;
				config.text_y_max = TEXT_Y_MAX_EMUSCV;
				config.sprite_x_min = SPRITE_X_MIN_EMUSCV;
				config.sprite_x_max = SPRITE_X_MAX_EMUSCV;
				config.sprite_y_min = SPRITE_Y_MIN_EMUSCV;
				config.sprite_y_max = SPRITE_Y_MAX_EMUSCV;
				break;
		}
	}

	config.window_height = config.draw_height;
	if(config.window_pixelaspect == SETTING_PIXELASPECT_SQUARE_VAL)
		config.window_width = config.draw_width;
	else
		config.window_width = config.draw_height*4/3;

	switch(config.window_resolution)
	{
		case SETTING_RESOLUTION_HIGH_VAL:
			config.window_width *= 4;
			config.window_height *= 4;
			config.window_space = 4;
			break;
		case SETTING_RESOLUTION_MEDIUM_VAL:
			config.window_width *= 2;
			config.window_height *= 2;
			config.window_space = 2;
			break;
		default:
			config.window_space = 1;
			break;
	}

	config.window_aspect_ratio = (float)(config.window_width) / (float)(config.window_height);

	switch(config.scv_fps)
	{
		case SETTING_FPS_EPOCH60_VAL:
			config.window_fps = WINDOW_FPS_EPOCH;
			break;
		case SETTING_FPS_YENO50_VAL:
			config.window_fps = WINDOW_FPS_YENO;
			break;
		case SETTING_FPS_AUTO_VAL:
		default:
			switch(config.scv_console)
			{
				case SETTING_CONSOLE_YENO_VAL:
					config.window_fps = WINDOW_FPS_YENO;
					break;
				case SETTING_CONSOLE_EPOCH_VAL:
				case SETTING_CONSOLE_EPOCHLADY_VAL:
				case SETTING_CONSOLE_AUTO_VAL:
				default:
					config.window_fps = WINDOW_FPS_EPOCH;
					break;
			}
			break;
	}

	config.window_clear = true;
}
#endif
