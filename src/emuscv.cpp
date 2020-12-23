#include "emuscv.h"

#include "config.h"
#include "emu.h"
#include "vm/vm.h"
#include "vm/scv/vdp_colors.h"
/*
extern void retro_audio_cb(void);
static uint8_t color_index = 0;
*/

static uint32_t test_video_buffer[WINDOW_WIDTH_EMUSCV*WINDOW_HEIGHT_EMUSCV];

// Not used in Libretro
//#ifdef USE_CART
//void open_cart_dialog(HWND hWnd, int drv);
//void open_recent_cart(int drv, int index);
//#endif

// Not used in Libretro
//#if defined(USE_CART) || defined(USE_FLOPPY_DISK) || defined(USE_HARD_DISK) || defined(USE_TAPE) || defined(USE_COMPACT_DISC) || defined(USE_LASER_DISC) || defined(USE_BINARY_FILE) || defined(USE_BUBBLE)
//#define SUPPORT_DRAG_DROP
//#endif

// dialog
//#ifdef USE_SOUND_VOLUME
// TODO_MM
//BOOL CALLBACK VolumeWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
//#endif
/*
#ifdef USE_JOYSTICK
// TODO_MM
//BOOL CALLBACK JoyWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK JoyToKeyWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
#endif

#ifdef USE_STATE
// TODO_MM
//const _TCHAR *state_file_path(int num)
//{
//	return create_local_path(_T("%s.sta%d"), _T(CONFIG_NAME), num);
//}
#endif
*/
// ----------------------------------------------------------------------------
// window main
// ----------------------------------------------------------------------------
/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow)
{


	// load config
	load_config(create_local_path(_T("%s.ini"), _T(CONFIG_NAME)));

	// create window
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = (WNDPROC)WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
//	wndclass.hCursor = 0;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wndclass.lpszClassName = _T("CWINDOW");
	RegisterClass(&wndclass);

	// get window position
	RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, TRUE);
	HDC hdcScr = GetDC(NULL);
	desktop_width = GetDeviceCaps(hdcScr, HORZRES);
	desktop_height = GetDeviceCaps(hdcScr, VERTRES);
	desktop_bpp = GetDeviceCaps(hdcScr, BITSPIXEL);
	ReleaseDC(NULL, hdcScr);
	int dest_x = (int)((desktop_width - (rect.right - rect.left)) / 2);
	int dest_y = (int)((desktop_height - (rect.bottom - rect.top)) / 2);
	//dest_x = (dest_x < 0) ? 0 : dest_x;
	dest_y = (dest_y < 0) ? 0 : dest_y;

	// show window
	HWND hWnd = CreateWindow(_T("CWINDOW"), _T(DEVICE_NAME), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
	                         dest_x, dest_y, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	// show menu bar
	show_menu_bar(hWnd);

	// show status bar
	if(config.show_status_bar) {
		show_status_bar(hWnd);
	}

	int status_bar_height = get_status_bar_height();
	RECT rect_tmp;
	GetClientRect(hWnd, &rect_tmp);
	if(rect_tmp.bottom != WINDOW_HEIGHT + status_bar_height) {
		rect.bottom += WINDOW_HEIGHT + status_bar_height - rect_tmp.bottom;
		dest_y = (int)((desktop_height - (rect.bottom - rect.top)) / 2);
		dest_y = (dest_y < 0) ? 0 : dest_y;
		SetWindowPos(hWnd, NULL, dest_x, dest_y, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
	}

	// enumerate screen mode
	enum_screen_mode();

	// restore screen mode
	if(config.window_mode >= 0 && config.window_mode < MAX_WINDOW) {
		PostMessage(hWnd, WM_COMMAND, ID_SCREEN_WINDOW + config.window_mode, 0L);
	} else if(config.window_mode >= MAX_WINDOW && config.window_mode < screen_mode_count + MAX_WINDOW) {
		PostMessage(hWnd, WM_COMMAND, ID_SCREEN_FULLSCREEN + config.window_mode - MAX_WINDOW, 0L);
	} else {
		config.window_mode = 0;
		PostMessage(hWnd, WM_COMMAND, ID_SCREEN_WINDOW, 0L);
	}

	// accelerator
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// initialize emulation core
	emu = new EMU(hWnd, hInstance);
	emu->set_host_window_size(WINDOW_WIDTH, WINDOW_HEIGHT, true);

#ifdef SUPPORT_DRAG_DROP
	// open command line path
	if(szCmdLine[0]) {
		if(szCmdLine[0] == _T('"')) {
			int len = _tcslen(szCmdLine);
			szCmdLine[len - 1] = _T('\0');
			szCmdLine++;
		}
		_TCHAR path[_MAX_PATH];
		get_long_full_path_name(szCmdLine, path, _MAX_PATH);
		open_any_file(path);
	}
#endif

	// set priority
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	// main loop
	int run_frames = 0, draw_frames = 0, skip_frames = 0;
	DWORD next_time = 0;
	bool prev_skip = false;
	DWORD update_fps_time = 0;
	DWORD update_status_bar_time = 0;
	DWORD disable_screen_saver_time = 0;
	MSG msg;

	while(1) {
		// check window message
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if(!GetMessage(&msg, NULL, 0, 0)) {
#ifdef _DEBUG
				_CrtDumpMemoryLeaks();
#endif
				ExitProcess(0);	// trick
				return msg.wParam;
			}
			if(!TranslateAccelerator(hWnd, hAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else if(emu) {
			// drive machine
			int run_frames = emu->run();
			run_frames += run_frames;

			// timing controls
			int sleep_period = 0;
			bool now_skip = (config.full_speed || emu->is_frame_skippable()) && !emu->is_video_recording() && !emu->is_sound_recording();

			if((prev_skip && !now_skip) || next_time == 0) {
				next_time = timeGetTime();
			}
			if(!now_skip) {
				static int accum = 0;
				accum += emu->get_frame_interval();
				int interval = accum >> 10;
				accum -= interval << 10;
				next_time += interval;
			}
			prev_skip = now_skip;

			if(next_time > timeGetTime()) {
				// update window if enough time
				draw_frames += emu->draw_screen();
				skip_frames = 0;

				// sleep 1 frame priod if need
				DWORD current_time = timeGetTime();
				if((int)(next_time - current_time) >= 10) {
					sleep_period = next_time - current_time;
				}
			} else if(++skip_frames > (int)emu->get_frame_rate()) {
				// update window at least once per 1 sec in virtual machine time
				draw_frames += emu->draw_screen();
				skip_frames = 0;
				next_time = timeGetTime();
			}
			Sleep(sleep_period);

			// calc frame rate
			DWORD current_time = timeGetTime();
			if(update_fps_time <= current_time) {
				if(update_fps_time != 0) {
					if(emu->message_count > 0) {
						SetWindowText(hWnd, create_string(_T("%s - %s"), _T(DEVICE_NAME), emu->message));
						emu->message_count--;
					} else if(now_skip) {
						int ratio = (int)(100.0 * (double)run_frames / emu->get_frame_rate() + 0.5);
						SetWindowText(hWnd, create_string(_T("%s - Skip Frames (%d %%)"), _T(DEVICE_NAME), ratio));
					} else {
						int ratio = (int)(100.0 * (double)draw_frames / (double)run_frames + 0.5);
						SetWindowText(hWnd, create_string(_T("%s - %d fps (%d %%)"), _T(DEVICE_NAME), draw_frames, ratio));
					}
					update_fps_time += 1000;
					run_frames = draw_frames = 0;
				}
				update_fps_time = current_time + 1000;
			}

			// update status bar
			if(update_status_bar_time <= current_time) {
				if(hStatus != NULL && status_bar_visible) {
					if(get_status_bar_updated()) {
						SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | SBT_OWNERDRAW, (LPARAM)NULL);
//						InvalidateRect(hStatus, NULL, FALSE);
					}
				}
				update_status_bar_time = current_time + 200;
			}

			// disable screen saver
			if(disable_screen_saver_time <= current_time) {
				SetThreadExecutionState(ES_DISPLAY_REQUIRED);
				INPUT input[1];
				ZeroMemory(input, sizeof(INPUT));
				input[0].type = INPUT_MOUSE;
				input[0].mi.dwFlags = MOUSEEVENTF_MOVE;
				input[0].mi.dx = 0;
				input[0].mi.dy = 0;
				SendInput(1, input, sizeof(INPUT));
				disable_screen_saver_time = current_time + 30000;
			}
		}
	}
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
*/
// ----------------------------------------------------------------------------
// window procedure
// ----------------------------------------------------------------------------
/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	static HIMC himcPrev = 0;

	switch(iMsg) {
	case WM_CREATE:
		if(config.disable_dwm && win8_or_later) {
			disable_dwm();
		}
#ifdef ONE_BOARD_MICRO_COMPUTER
		create_buttons(hWnd);
#endif
#ifdef SUPPORT_DRAG_DROP
		DragAcceptFiles(hWnd, TRUE);
#endif
		hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		timeBeginPeriod(1);
		break;
	case WM_CLOSE:
#ifdef USE_NOTIFY_POWER_OFF
		// notify power off
		if(emu) {
			static bool notified = false;
			if(!notified) {
				emu->notify_power_off();
				notified = true;
				return 0;
			}
		}
#endif
		// release window
		if(now_fullscreen) {
			ChangeDisplaySettings(NULL, 0);
		}
		now_fullscreen = false;
		if(hMenu != NULL && IsMenu(hMenu)) {
			DestroyMenu(hMenu);
			hMenu = NULL;
		}
		if(hStatus != NULL) {
			DestroyWindow(hStatus);
			hStatus = NULL;
		}
		DestroyWindow(hWnd);
		// release emulation core
		if(emu) {
			delete emu;
			emu = NULL;
		}
		save_config(create_local_path(_T("%s.ini"), _T(CONFIG_NAME)));
		timeEndPeriod(1);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_ACTIVATE:
		// thanks PC8801MA��
		if(LOWORD(wParam) != WA_INACTIVE) {
			himcPrev = ImmAssociateContext(hWnd, 0);
		} else {
			ImmAssociateContext(hWnd, himcPrev);
		}
		break;
	case WM_SIZE:
		if(hStatus != NULL) {
			SendMessage(hStatus, WM_SIZE, wParam, lParam);
		}
#ifdef ONE_BOARD_MICRO_COMPUTER
		if(emu) {
			emu->reload_bitmap();
		}
#endif
		break;
	case WM_KILLFOCUS:
		if(emu) {
			emu->key_lost_focus();
		}
		break;
	case WM_PAINT:
		if(emu) {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
#ifdef ONE_BOARD_MICRO_COMPUTER
			// check if self invalidate or not
			int left = 0, top = 0, right = 0, bottom = 0;
			emu->get_invalidated_rect(&left, &top, &right, &bottom);
			if(ps.rcPaint.left != left || ps.rcPaint.top != top || ps.rcPaint.right != right || ps.rcPaint.bottom != bottom) {
				emu->reload_bitmap();
			}
#endif
			emu->update_screen(hdc);
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
			if(lpDrawItem->CtlID == ID_STATUS) {
				if(emu) {
					update_status_bar(hInstance, lpDrawItem);
				}
			} else {
				#ifdef ONE_BOARD_MICRO_COMPUTER
//					if(lpDrawItem->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) {
						draw_button(lpDrawItem->hDC, (UINT)wParam - ID_BUTTON, lpDrawItem->itemState & ODS_SELECTED);
//					}
				#endif
			}
		}
		return TRUE;
	case WM_MOVING:
		if(emu) {
			emu->suspend();
		}
		break;
	case WM_KEYDOWN:
		if(emu) {
			bool extended = ((HIWORD(lParam) & 0x100) != 0);
			bool repeat = ((HIWORD(lParam) & 0x4000) != 0);
			emu->key_down(LOBYTE(wParam), extended, repeat);
		}
		break;
	case WM_KEYUP:
		if(emu) {
			bool extended = ((HIWORD(lParam) & 0x100) != 0);
			emu->key_up(LOBYTE(wParam), extended);
		}#define FRAMES_PER_SEC				60

		break;
	case WM_SYSKEYDOWN:
		if(emu) {
			bool extended = ((HIWORD(lParam) & 0x100) != 0);
			bool repeat = ((HIWORD(lParam) & 0x4000) != 0);
			emu->key_down(LOBYTE(wParam), extended, repeat);
		}
		return 0;	// not activate menu when hit ALT/F10
	case WM_SYSKEYUP:
		if(emu) {
			bool extended = ((HIWORD(lParam) & 0x100) != 0);
			emu->key_up(LOBYTE(wParam), extended);
		}
		return 0;	// not activate menu when hit ALT/F10
	case WM_CHAR:
		if(emu) {
			emu->key_char(LOBYTE(wParam));
		}
		break;
	case WM_SYSCHAR:
		return 0;	// not activate menu when hit ALT/F10
	case WM_INITMENUPOPUP:
		if(emu) {
			emu->suspend();
		}
		update_menu(hWnd, (HMENU)wParam);
		break;
	case WM_ENTERMENULOOP:
		now_menuloop = true;
		break;
	case WM_EXITMENULOOP:
		if(now_fullscreen && now_menuloop) {
			hide_menu_bar(hWnd);
		}
		now_menuloop = false;
		break;
	case WM_MOUSEMOVE:
		if(now_fullscreen && !now_menuloop) {
			POINTS p = MAKEPOINTS(lParam);
			if(p.y == 0) {
				show_menu_bar(hWnd);
			} else if(p.y > 32) {
				hide_menu_bar(hWnd);
			}
		}
		break;
	case WM_RESIZE:
		if(emu) {
			if(now_fullscreen) {
				emu->set_host_window_size(-1, -1, false);
			} else {
				set_window(hWnd, config.window_mode);
			}
		}
		break;
#ifdef SUPPORT_DRAG_DROP
	case WM_DROPFILES:
		if(emu) {
			open_dropped_file((HDROP)wParam);
		}
		break;
#endif
#ifdef USE_SOCKET
	case WM_SOCKET0: case WM_SOCKET1: case WM_SOCKET2: case WM_SOCKET3:
		if(emu) {
			update_socket(iMsg - WM_SOCKET0, wParam, lParam);
		}
		break;
#endif
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case ID_RESET:
			if(emu) {
				emu->reset();
			}
			break;
#ifdef USE_SPECIAL_RESET
		case ID_SPECIAL_RESET:
			if(emu) {
				emu->special_reset();
			}
			break;
#endif
		case ID_CPU_POWER0: case ID_CPU_POWER1: case ID_CPU_POWER2: case ID_CPU_POWER3: case ID_CPU_POWER4:
			config.cpu_power = LOWORD(wParam) - ID_CPU_POWER0;
			if(emu) {
				emu->update_config();
			}
			break;
		case ID_FULL_SPEED:
			config.full_speed = !config.full_speed;
			break;
#ifdef USE_AUTO_KEY
		case ID_AUTOKEY_START:
			if(emu) {
				start_auto_key();
			}
			break;
		case ID_AUTOKEY_STOP:
			if(emu) {
				emu->stop_auto_key();
			}
			break;
		case ID_ROMAJI_TO_KANA:
			if(emu) {
				if(!config.romaji_to_kana) {
					emu->set_auto_key_char(1); // start
					config.romaji_to_kana = true;
				} else {
					emu->set_auto_key_char(0); // end
					config.romaji_to_kana = false;
				}
			}
			break;
#endif
#ifdef USE_DEBUGGER
		case ID_OPEN_DEBUGGER0: case ID_OPEN_DEBUGGER1: case ID_OPEN_DEBUGGER2: case ID_OPEN_DEBUGGER3:
		case ID_OPEN_DEBUGGER4: case ID_OPEN_DEBUGGER5: case ID_OPEN_DEBUGGER6: case ID_OPEN_DEBUGGER7:
			if(emu) {
				e#define FRAMES_PER_SEC				60
mu->open_debugger(LOWORD(wParam) - ID_OPEN_DEBUGGER0);
			}
			break;
		case ID_CLOSE_DEBUGGER:
			if(emu) {
				emu->close_debugger();
			}
			break;
#endif
#ifdef USE_STATE
		case ID_SAVE_STATE0: case ID_SAVE_STATE1: case ID_SAVE_STATE2: case ID_SAVE_STATE3: case ID_SAVE_STATE4:
		case ID_SAVE_STATE5: case ID_SAVE_STATE6: case ID_SAVE_STATE7: case ID_SAVE_STATE8: case ID_SAVE_STATE9:
			if(emu) {
				emu->save_state(state_file_path(LOWORD(wParam) - ID_SAVE_STATE0));
			}
			break;
		case ID_LOAD_STATE0: case ID_LOAD_STATE1: case ID_LOAD_STATE2: case ID_LOAD_STATE3: case ID_LOAD_STATE4:
		case ID_LOAD_STATE5: case ID_LOAD_STATE6: case ID_LOAD_STATE7: case ID_LOAD_STATE8: case ID_LOAD_STATE9:
			if(emu) {
				emu->load_state(state_file_path(LOWORD(wParam) - ID_LOAD_STATE0));
			}
			break;
#endif
		case ID_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0L);
			break;
#ifdef USE_BOOT_MODE
		case ID_VM_BOOT_MODE0: case ID_VM_BOOT_MODE1: case ID_VM_BOOT_MODE2: case ID_VM_BOOT_MODE3:
		case ID_VM_BOOT_MODE4: case ID_VM_BOOT_MODE5: case ID_VM_BOOT_MODE6: case ID_VM_BOOT_MODE7:
			config.boot_mode = LOWORD(wParam) - ID_VM_BOOT_MODE0;
			if(emu) {
				emu->update_config();
			}
			break;
#endif
#ifdef USE_CPU_TYPE
		case ID_VM_CPU_TYPE0: case ID_VM_CPU_TYPE1: case ID_VM_CPU_TYPE2: case ID_VM_CPU_TYPE3:
		case ID_VM_CPU_TYPE4: case ID_VM_CPU_TYPE5: case ID_VM_CPU_TYPE6: case ID_VM_CPU_TYPE7:
			config.cpu_type = LOWORD(wParam) - ID_VM_CPU_TYPE0;
			// need to recreate vm class instance
//			if(emu) {
//				emu->update_config();
//			}
			break;
#endif
#ifdef USE_DIPSWITCH
		case ID_VM_DIPSWITCH0:  case ID_VM_DIPSWITCH1:  case ID_VM_DIPSWITCH2:  case ID_VM_DIPSWITCH3:
		case ID_VM_DIPSWITCH4:  case ID_VM_DIPSWITCH5:  case ID_VM_DIPSWITCH6:  case ID_VM_DIPSWITCH7:
		case ID_VM_DIPSWITCH8:  case ID_VM_DIPSWITCH9:  case ID_VM_DIPSWITCH10: case ID_VM_DIPSWITCH11:
		case ID_VM_DIPSWITCH12: case ID_VM_DIPSWITCH13: case ID_VM_DIPSWITCH14: case ID_VM_DIPSWITCH15:
		case ID_VM_DIPSWITCH16: case ID_VM_DIPSWITCH17: case ID_VM_DIPSWITCH18: case ID_VM_DIPSWITCH19:
		case ID_VM_DIPSWITCH20: case ID_VM_DIPSWITCH21: case ID_VM_DIPSWITCH22: case ID_VM_DIPSWITCH23:
		case ID_VM_DIPSWITCH24: case ID_VM_DIPSWITCH25: case ID_VM_DIPSWITCH26: case ID_VM_DIPSWITCH27:
		case ID_VM_DIPSWITCH28: case ID_VM_DIPSWITCH29: case ID_VM_DIPSWITCH30: case ID_VM_DIPSWITCH31:
			config.dipswitch ^= (1 << (LOWORD(wParam) - ID_VM_DIPSWITCH0));
			break;
#endif
#ifdef USE_DEVICE_TYPE
		case ID_VM_DEVICE_TYPE0: case ID_VM_DEVICE_TYPE1: case ID_VM_DEVICE_TYPE2: case ID_VM_DEVICE_TYPE3:
		case ID_VM_DEVICE_TYPE4: case ID_VM_DEVICE_TYPE5: case ID_VM_DEVICE_TYPE6: case ID_VM_DEVICE_TYPE7:
			config.device_type = LOWORD(wParam) - ID_VM_DEVICE_TYPE0;
			break;
#endif
#ifdef USE_DRIVE_TYPE
		case ID_VM_DRIVE_TYPE0: case ID_VM_DRIVE_TYPE1: case ID_VM_DRIVE_TYPE2: case ID_VM_DRIVE_TYPE3:
		case ID_VM_DRIVE_TYPE4: case ID_VM_DRIVE_TYPE5: case ID_VM_DRIVE_TYPE6: case ID_VM_DRIVE_TYPE7:
			config.drive_type = LOWORD(wParam) - ID_VM_DRIVE_TYPE0;
			if(emu) {
				emu->update_config();
			}
			break;
#endif
#ifdef USE_KEYBOARD_TYPE
		case ID_VM_KEYBOARD_TYPE0: case ID_VM_KEYBOARD_TYPE1: case ID_VM_KEYBOARD_TYPE2: case ID_VM_KEYBOARD_TYPE3:
		case ID_VM_KEYBOARD_TYPE4: case ID_VM_KEYBOARD_TYPE5: case ID_VM_KEYBOARD_TYPE6: case ID_VM_KEYBOARD_TYPE7:
			config.keyboard_type = LOWORD(wParam) - ID_VM_KEYBOARD_TYPE0;
			if(emu) {
				emu->update_config();
			}
			break;
#endif
#ifdef USE_MOUSE_TYPE
		case ID_VM_MOUSE_TYPE0: case ID_VM_MOUSE_TYPE1: case ID_VM_MOUSE_TYPE2: case ID_VM_MOUSE_TYPE3:
		case ID_VM_MOUSE_TYPE4: case ID_VM_MOUSE_TYPE5: case ID_VM_MOUSE_TYPE6: case ID_VM_MOUSE_TYPE7:
			config.mouse_type = LOWORD(wParam) - ID_VM_MOUSE_TYPE0;
			if(emu) {
				emu->update_config();
			}
			break;
#endif
#ifdef USE_JOYSTICK_TYPE
		case ID_VM_JOYSTICK_TYPE0: case ID_VM_JOYSTICK_TYPE1: case ID_VM_JOYSTICK_TYPE2: case ID_VM_JOYSTICK_TYPE3:
		case ID_VM_JOYSTICK_TYPE4: case ID_VM_JOYSTICK_TYPE5: case ID_VM_JOYSTICK_TYPE6: case ID_VM_JOYSTICK_TYPE7:
			config.joystick_type = LOWORD(wParam) - ID_VM_JOYSTICK_TYPE0;
			if(emu) {
				emu->update_config();
			}
			break;
#endif
#ifdef USE_SOUND_TYPE
		case ID_VM_SOUND_TYPE0: case ID_VM_SOUND_TYPE1: case ID_VM_SOUND_TYPE2: case ID_VM_SOUND_TYPE3:
		case ID_VM_SOUND_TYPE4: case ID_VM_SOUND_TYPE5: case ID_VM_SOUND_TYPE6: case ID_VM_SOUND_TYPE7:
			config.sound_type = LOWORD(wParam) - ID_VM_SOUND_TYPE0;
			//if(emu) {
			//	emu->update_config();
			//}
			break;
#endif
#ifdef USE_FLOPPY_DISK
		case ID_VM_SOUND_NOISE_FDD:
			config.sound_noise_fdd = !config.sound_noise_fdd;
			if(emu) {
				emu->update_config();
			}
			break;
#endif
#ifdef USE_TAPE
		case ID_VM_SOUND_NOISE_CMT:
			config.sound_noise_cmt = !config.sound_noise_cmt;
			if(emu) {
				emu->update_config();
			}
			break;
		case ID_VM_SOUND_PLAY_TAPE:
			config.sound_play_tape = !config.sound_play_tape;
			break;
#endif
#ifdef USE_MONITOR_TYPE
		case ID_VM_MONITOR_TYPE0: case ID_VM_MONITOR_TYPE1: case ID_VM_MONITOR_TYPE2: case ID_VM_MONITOR_TYPE3:
		case ID_VM_MONITOR_TYPE4: case ID_VM_MONITOR_TYPE5: case ID_VM_MONITOR_TYPE6: case ID_VM_MONITOR_TYPE7:
			config.monitor_type = LOWORD(wParam) - ID_VM_MONITOR_TYPE0;
			if(emu) {
#ifdef ONE_BOARD_MICRO_COMPUTER
				emu->reload_bitmap();
				emu->invalidate_screen();
#endif
				emu->update_config();
			}
			break;
#endif
#ifdef USE_SCANLINE
		case ID_VM_MONITOR_SCANLINE:
			config.scan_line = !config.scan_line;
			if(emu) {
				emu->update_config();
			}
			break;
#endif
#ifdef USE_PRINTER_TYPE
		case ID_VM_PRINTER_TYPE0: case ID_VM_PRINTER_TYPE1: case ID_VM_PRINTER_TYPE2: case ID_VM_PRINTER_TYPE3:
		case ID_VM_PRINTER_TYPE4: case ID_VM_PRINTER_TYPE5: case ID_VM_PRINTER_TYPE6: case ID_VM_PRINTER_TYPE7:
			config.printer_type = LOWORD(wParam) - ID_VM_PRINTER_TYPE0;
			break;
#endif
		case ID_HOST_REC_MOVIE_60FPS: case ID_HOST_REC_MOVIE_30FPS: case ID_HOST_REC_MOVIE_15FPS:
			if(emu) {
				static const int fps[3] = {60, 30, 15};
				emu->start_record_sound();
				if(!emu->start_record_video(fps[LOWORD(wParam) - ID_HOST_REC_MOVIE_60FPS])) {
					emu->stop_record_sound();
				}
			}
			break;
		case ID_HOST_REC_SOUND:
			if(emu) {
				emu->start_record_sound();
			}
			break;
		case ID_HOST_REC_STOP:
			if(emu) {
				emu->stop_record_video();
				emu->stop_record_sound();
			}
			break;
		case ID_HOST_CAPTURE_SCREEN:
			if(emu) {
				emu->capture_screen();
			}
			break;
		case ID_HOST_USE_D3D9:
			config.use_d3d9 = !config.use_d3d9;
			if(emu) {
				emu->set_host_window_size(-1, -1, !now_fullscreen);
			}
			break;
		case ID_HOST_WAIT_VSYNC:
			config.wait_vsync = !config.wait_vsync;
			if(emu) {
				emu->set_host_window_size(-1, -1, !now_fullscreen);
			}
			break;
		case ID_HOST_USE_DINPUT:
			config.use_dinput = !config.use_dinput;
			break;
		case ID_HOST_DISABLE_DWM:
			config.disable_dwm = !config.disable_dwm;
			break;
		case ID_HOST_SHOW_STATUS_BAR:
			config.show_status_bar = !config.show_status_bar;
			if(emu) {
				if(!now_fullscreen) {
					set_window(hWnd, prev_window_mode);
				}
			}
			break;
		case ID_SCREEN_WINDOW + 0: case ID_SCREEN_WINDOW + 1: case ID_SCREEN_WINDOW + 2: case ID_SCREEN_WINDOW + 3: case ID_SCREEN_WINDOW + 4:
		case ID_SCREEN_WINDOW + 5: case ID_SCREEN_WINDOW + 6: case ID_SCREEN_WINDOW + 7: case ID_SCREEN_WINDOW + 8: case ID_SCREEN_WINDOW + 9:
			if(emu) {
				set_window(hWnd, LOWORD(wParam) - ID_SCREEN_WINDOW);
			}
			break;
		case ID_SCREEN_FULLSCREEN +  0: case ID_SCREEN_FULLSCREEN +  1: case ID_SCREEN_FULLSCREEN +  2: case ID_SCREEN_FULLSCREEN +  3: case ID_SCREEN_FULLSCREEN +  4:
		case ID_SCREEN_FULLSCREEN +  5: case ID_SCREEN_FULLSCREEN +  6: case ID_SCREEN_FULLSCREEN +  7: case ID_SCREEN_FULLSCREEN +  8: case ID_SCREEN_FULLSCREEN +  9:
		case ID_SCREEN_FULLSCREEN + 10: case ID_SCREEN_FULLSCREEN + 11: case ID_SCREEN_FULLSCREEN + 12: case ID_SCREEN_FULLSCREEN + 13: case ID_SCREEN_FULLSCREEN + 14:
		case ID_SCREEN_FULLSCREEN + 15: case ID_SCREEN_FULLSCREEN + 16: case ID_SCREEN_FULLSCREEN + 17: case ID_SCREEN_FULLSCREEN + 18: case ID_SCREEN_FULLSCREEN + 19:
		case ID_SCREEN_FULLSCREEN + 20: case ID_SCREEN_FULLSCREEN + 21: case ID_SCREEN_FULLSCREEN + 22: case ID_SCREEN_FULLSCREEN + 23: case ID_SCREEN_FULLSCREEN + 24:
		case ID_SCREEN_FULLSCREEN + 25: case ID_SCREEN_FULLSCREEN + 26: case ID_SCREEN_FULLSCREEN + 27: case ID_SCREEN_FULLSCREEN + 28: case ID_SCREEN_FULLSCREEN + 29:
		case ID_SCREEN_FULLSCREEN + 30: case ID_SCREEN_FULLSCREEN + 31: case ID_SCREEN_FULLSCREEN + 32: case ID_SCREEN_FULLSCREEN + 33: case ID_SCREEN_FULLSCREEN + 34:
		case ID_SCREEN_FULLSCREEN + 35: case ID_SCREEN_FULLSCREEN + 36: case ID_SCREEN_FULLSCREEN + 37: case ID_SCREEN_FULLSCREEN + 38: case ID_SCREEN_FULLSCREEN + 39:
		case ID_SCREEN_FULLSCREEN + 40: case ID_SCREEN_FULLSCREEN + 41: case ID_SCREEN_FULLSCREEN + 42: case ID_SCREEN_FULLSCREEN + 43: case ID_SCREEN_FULLSCREEN + 44:
		case ID_SCREEN_FULLSCREEN + 45: case ID_SCREEN_FULLSCREEN + 46: case ID_SCREEN_FULLSCREEN + 47: case ID_SCREEN_FULLSCREEN + 48: case ID_SCREEN_FULLSCREEN + 49:
			if(emu && !now_fullscreen) {
				set_window(hWnd, LOWORD(wParam) - ID_SCREEN_FULLSCREEN + MAX_WINDOW);
			}
			break;
		case ID_SCREEN_WINDOW_STRETCH:
		case ID_SCREEN_WINDOW_ASPECT:
			config.window_stretch_type = LOWORD(wParam) - ID_SCREEN_WINDOW_STRETCH;
			if(emu) {
				if(!now_fullscreen) {
					set_window(hWnd, prev_window_mode);
				}
			}
			break;
		case ID_SCREEN_FULLSCREEN_DOTBYDOT:
		case ID_SCREEN_FULLSCREEN_STRETCH:
		case ID_SCREEN_FULLSCREEN_ASPECT:
		case ID_SCREEN_FULLSCREEN_FILL:
			config.fullscreen_stretch_type = LOWORD(wParam) - ID_SCREEN_FULLSCREEN_DOTBYDOT;
			if(emu) {
				if(now_fullscreen) {
					emu->set_host_window_size(-1, -1, false);
				}
			}
			break;
//#ifdef USE_SCREEN_ROTATE
		case ID_SCREEN_ROTATE_0: case ID_SCREEN_ROTATE_90: case ID_SCREEN_ROTATE_180: case ID_SCREEN_ROTATE_270:
			config.rotate_type = LOWORD(wParam) - ID_SCREEN_ROTATE_0;
			if(emu) {
				if(now_fullscreen) {
					emu->set_host_window_size(-1, -1, false);
				} else {
					set_window(hWnd, prev_window_mode);
				}
			}
			break;
//#endif
#ifdef USE_SCREEN_FILTER
		case ID_FILTER_RGB:
			config.filter_type = SCREEN_FILTER_RGB;
			break;
		case ID_FILTER_RF:
			config.filter_type = SCREEN_FILTER_RF;
			break;
		case ID_FILTER_NONE:
			config.filter_type = SCREEN_FILTER_NONE;
			break;
#endif
		case ID_SOUND_FREQ0: case ID_SOUND_FREQ1: case ID_SOUND_FREQ2: case ID_SOUND_FREQ3:
		case ID_SOUND_FREQ4: case ID_SOUND_FREQ5: case ID_SOUND_FREQ6: case ID_SOUND_FREQ7:
			config.sound_frequency = LOWORD(wParam) - ID_SOUND_FREQ0;
			if(emu) {
				emu->update_config();
			}
			break;
		case ID_SOUND_LATE0: case ID_SOUND_LATE1: case ID_SOUND_LATE2: case ID_SOUND_LATE3: case ID_SOUND_LATE4:
			config.sound_latency = LOWORD(wParam) - ID_SOUND_LATE0;
			if(emu) {
				emu->update_config();
			}
			break;
		case ID_SOUND_STRICT_RENDER: case ID_SOUND_LIGHT_RENDER:
			config.sound_strict_rendering = (LOWORD(wParam) == ID_SOUND_STRICT_RENDER);
			if(emu) {
				emu->update_config();
			}
			break;
#ifdef USE_SOUND_VOLUME
		case ID_SOUND_VOLUME:
			DialogBoxParam((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_VOLUME), hWnd, VolumeWndProc, 0);
			break;
#endif
#ifdef USE_JOYSTICK
		case ID_INPUT_JOYSTICK0: case ID_INPUT_JOYSTICK1: case ID_INPUT_JOYSTICK2: case ID_INPUT_JOYSTICK3:
		case ID_INPUT_JOYSTICK4: case ID_INPUT_JOYSTICK5: case ID_INPUT_JOYSTICK6: case ID_INPUT_JOYSTICK7:
			{
				LONG index = LOWORD(wParam) - ID_INPUT_JOYSTICK0;
				DialogBoxParam((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_JOYSTICK), hWnd, JoyWndProc, (LPARAM)&index);
			}
			break;
		case ID_INPUT_JOYTOKEY:
			{
				LONG index = 0;
				DialogBoxParam((HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDD_JOYTOKEY), hWnd, JoyToKeyWndProc, (LPARAM)&index);
			}
			break;
#endif
#ifdef USE_VIDEO_CAPTURE
		case ID_CAPTURE_FILTER:
			if(emu) {
				emu->show_capture_dev_filter();
			}
			break;
		case ID_CAPTURE_PIN:
			if(emu) {
				emu->show_capture_dev_pin();
			}
			break;
		case ID_CAPTURE_SOURCE:
			if(emu) {
				emu->show_capture_dev_source();
			}
			break;
		case ID_CAPTURE_CLOSE:
			if(emu) {
				emu->close_capture_dev();
			}
			break;
		case ID_CAPTURE_DEVICE + 0: case ID_CAPTURE_DEVICE + 1: case ID_CAPTURE_DEVICE + 2: case ID_CAPTURE_DEVICE + 3:
		case ID_CAPTURE_DEVICE + 4: case ID_CAPTURE_DEVICE + 5: case ID_CAPTURE_DEVICE + 6: case ID_CAPTURE_DEVICE + 7:
			if(emu) {
				emu->open_capture_dev(LOWORD(wParam) - ID_CAPTURE_DEVICE, false);
			}
			break;
#endif
#ifdef USE_CART
	#if USE_CART >= 1
		#define CART_MENU_ITEMS(drv, ID_OPEN_CART, ID_CLOSE_CART, ID_RECENT_CART) \
		case ID_OPEN_CART: \
			if(emu) { \
				open_cart_dialog(hWnd, drv); \
			} \
			break; \
		case ID_CLOSE_CART: \
			if(emu) { \
				emu->close_cart(drv); \
			} \
			break; \
		case ID_RECENT_CART + 0: case ID_RECENT_CART + 1: case ID_RECENT_CART + 2: case ID_RECENT_CART + 3: \
		case ID_RECENT_CART + 4: case ID_RECENT_CART + 5: case ID_RECENT_CART + 6: case ID_RECENT_CART + 7: \
			if(emu) { \
				open_recent_cart(drv, LOWORD(wParam) - ID_RECENT_CART); \
			} \
			break;
		CART_MENU_ITEMS(0, ID_OPEN_CART1, ID_CLOSE_CART1, ID_RECENT_CART1)
	#endif
	#if USE_CART >= 2
		CART_MENU_ITEMS(1, ID_OPEN_CART2, ID_CLOSE_CART2, ID_RECENT_CART2)
	#endif
#endif
#ifdef USE_FLOPPY_DISK
	#if USE_FLOPPY_DISK >= 1
		#define FD_MENU_ITEMS(drv, ID_OPEN_FD, ID_CLOSE_FD, ID_OPEN_BLANK_2D_FD, ID_OPEN_BLANK_2DD_FD, ID_OPEN_BLANK_2HD_FD, ID_WRITE_PROTECT_FD, ID_CORRECT_TIMING_FD, ID_IGNORE_CRC_FD, ID_RECENT_FD, ID_SELECT_D88_BANK, ID_EJECT_D88_BANK) \
		case ID_OPEN_FD: \
			if(emu) { \
				open_floppy_disk_dialog(hWnd, drv); \
			} \
			break; \
		case ID_CLOSE_FD: \
			if(emu) { \
				close_floppy_disk(drv); \
			} \
			break; \
		case ID_OPEN_BLANK_2D_FD: \
			if(emu) { \
				open_blank_floppy_disk_dialog(hWnd, drv, 0x00); \
			} \
			break; \
		case ID_OPEN_BLANK_2DD_FD: \
			if(emu) { \
				open_blank_floppy_disk_dialog(hWnd, drv, 0x10); \
			} \
			break; \
		case ID_OPEN_BLANK_2HD_FD: \
			if(emu) { \
				open_blank_floppy_disk_dialog(hWnd, drv, 0x20); \
			} \
			break; \
		case ID_WRITE_PROTECT_FD: \
			if(emu) { \
				emu->is_floppy_disk_protected(drv, !emu->is_floppy_disk_protected(drv)); \
			} \
			break; \
		case ID_CORRECT_TIMING_FD: \
			config.correct_disk_timing[drv] = !config.correct_disk_timing[drv]; \
			break; \
		case ID_IGNORE_CRC_FD: \
			config.ignore_disk_crc[drv] = !config.ignore_disk_crc[drv]; \
			break; \
		case ID_RECENT_FD + 0: case ID_RECENT_FD + 1: case ID_RECENT_FD + 2: case ID_RECENT_FD + 3: \
		case ID_RECENT_FD + 4: case ID_RECENT_FD + 5: case ID_RECENT_FD + 6: case ID_RECENT_FD + 7: \
			if(emu) { \
				open_recent_floppy_disk(drv, LOWORD(wParam) - ID_RECENT_FD); \
			} \
			break; \
		case ID_SELECT_D88_BANK +  0: case ID_SELECT_D88_BANK +  1: case ID_SELECT_D88_BANK +  2: case ID_SELECT_D88_BANK +  3: \
		case ID_SELECT_D88_BANK +  4: case ID_SELECT_D88_BANK +  5: case ID_SELECT_D88_BANK +  6: case ID_SELECT_D88_BANK +  7: \
		case ID_SELECT_D88_BANK +  8: case ID_SELECT_D88_BANK +  9: case ID_SELECT_D88_BANK + 10: case ID_SELECT_D88_BANK + 11: \
		case ID_SELECT_D88_BANK + 12: case ID_SELECT_D88_BANK + 13: case ID_SELECT_D88_BANK + 14: case ID_SELECT_D88_BANK + 15: \
		case ID_SELECT_D88_BANK + 16: case ID_SELECT_D88_BANK + 17: case ID_SELECT_D88_BANK + 18: case ID_SELECT_D88_BANK + 19: \
		case ID_SELECT_D88_BANK + 20: case ID_SELECT_D88_BANK + 21: case ID_SELECT_D88_BANK + 22: case ID_SELECT_D88_BANK + 23: \
		case ID_SELECT_D88_BANK + 24: case ID_SELECT_D88_BANK + 25: case ID_SELECT_D88_BANK + 26: case ID_SELECT_D88_BANK + 27: \
		case ID_SELECT_D88_BANK + 28: case ID_SELECT_D88_BANK + 29: case ID_SELECT_D88_BANK + 30: case ID_SELECT_D88_BANK + 31: \
		case ID_SELECT_D88_BANK + 32: case ID_SELECT_D88_BANK + 33: case ID_SELECT_D88_BANK + 34: case ID_SELECT_D88_BANK + 35: \
		case ID_SELECT_D88_BANK + 36: case ID_SELECT_D88_BANK + 37: case ID_SELECT_D88_BANK + 38: case ID_SELECT_D88_BANK + 39: \
		case ID_SELECT_D88_BANK + 40: case ID_SELECT_D88_BANK + 41: case ID_SELECT_D88_BANK + 42: case ID_SELECT_D88_BANK + 43: \
		case ID_SELECT_D88_BANK + 44: case ID_SELECT_D88_BANK + 45: case ID_SELECT_D88_BANK + 46: case ID_SELECT_D88_BANK + 47: \
		case ID_SELECT_D88_BANK + 48: case ID_SELECT_D88_BANK + 49: case ID_SELECT_D88_BANK + 50: case ID_SELECT_D88_BANK + 51: \
		case ID_SELECT_D88_BANK + 52: case ID_SELECT_D88_BANK + 53: case ID_SELECT_D88_BANK + 54: case ID_SELECT_D88_BANK + 55: \
		case ID_SELECT_D88_BANK + 56: case ID_SELECT_D88_BANK + 57: case ID_SELECT_D88_BANK + 58: case ID_SELECT_D88_BANK + 59: \
		case ID_SELECT_D88_BANK + 60: case ID_SELECT_D88_BANK + 61: case ID_SELECT_D88_BANK + 62: case ID_SELECT_D88_BANK + 63: \
			if(emu) { \
				select_d88_bank(drv, LOWORD(wParam) - ID_SELECT_D88_BANK); \
			} \
			break; \
		case ID_EJECT_D88_BANK: \
			if(emu) { \
				select_d88_bank(drv, -1); \
			} \
			break;
		FD_MENU_ITEMS(0, ID_OPEN_FD1, ID_CLOSE_FD1, ID_OPEN_BLANK_2D_FD1, ID_OPEN_BLANK_2DD_FD1, ID_OPEN_BLANK_2HD_FD1, ID_WRITE_PROTECT_FD1, ID_CORRECT_TIMING_FD1, ID_IGNORE_CRC_FD1, ID_RECENT_FD1, ID_SELECT_D88_BANK1, ID_EJECT_D88_BANK1)
	#endif
	#if USE_FLOPPY_DISK >= 2
		FD_MENU_ITEMS(1, ID_OPEN_FD2, ID_CLOSE_FD2, ID_OPEN_BLANK_2D_FD2, ID_OPEN_BLANK_2DD_FD2, ID_OPEN_BLANK_2HD_FD2, ID_WRITE_PROTECT_FD2, ID_CORRECT_TIMING_FD2, ID_IGNORE_CRC_FD2, ID_RECENT_FD2, ID_SELECT_D88_BANK2, ID_EJECT_D88_BANK2)
	#endif
	#if USE_FLOPPY_DISK >= 3
		FD_MENU_ITEMS(2, ID_OPEN_FD3, ID_CLOSE_FD3, ID_OPEN_BLANK_2D_FD3, ID_OPEN_BLANK_2DD_FD3, ID_OPEN_BLANK_2HD_FD3, ID_WRITE_PROTECT_FD3, ID_CORRECT_TIMING_FD3, ID_IGNORE_CRC_FD3, ID_RECENT_FD3, ID_SELECT_D88_BANK3, ID_EJECT_D88_BANK3)
	#endif
	#if USE_FLOPPY_DISK >= 4
		FD_MENU_ITEMS(3, ID_OPEN_FD4, ID_CLOSE_FD4, ID_OPEN_BLANK_2D_FD4, ID_OPEN_BLANK_2DD_FD4, ID_OPEN_BLANK_2HD_FD4, ID_WRITE_PROTECT_FD4, ID_CORRECT_TIMING_FD4, ID_IGNORE_CRC_FD4, ID_RECENT_FD4, ID_SELECT_D88_BANK4, ID_EJECT_D88_BANK4)
	#endif
	#if USE_FLOPPY_DISK >= 5
		FD_MENU_ITEMS(4, ID_OPEN_FD5, ID_CLOSE_FD5, ID_OPEN_BLANK_2D_FD5, ID_OPEN_BLANK_2DD_FD5, ID_OPEN_BLANK_2HD_FD5, ID_WRITE_PROTECT_FD5, ID_CORRECT_TIMING_FD5, ID_IGNORE_CRC_FD5, ID_RECENT_FD5, ID_SELECT_D88_BANK5, ID_EJECT_D88_BANK5)
	#endif
	#if USE_FLOPPY_DISK >= 6
		FD_MENU_ITEMS(5, ID_OPEN_FD6, ID_CLOSE_FD6, ID_OPEN_BLANK_2D_FD6, ID_OPEN_BLANK_2DD_FD6, ID_OPEN_BLANK_2HD_FD6, ID_WRITE_PROTECT_FD6, ID_CORRECT_TIMING_FD6, ID_IGNORE_CRC_FD6, ID_RECENT_FD6, ID_SELECT_D88_BANK6, ID_EJECT_D88_BANK6)
	#endif
	#if USE_FLOPPY_DISK >= 7
		FD_MENU_ITEMS(6, ID_OPEN_FD7, ID_CLOSE_FD7, ID_OPEN_BLANK_2D_FD7, ID_OPEN_BLANK_2DD_FD7, ID_OPEN_BLANK_2HD_FD7, ID_WRITE_PROTECT_FD7, ID_CORRECT_TIMING_FD7, ID_IGNORE_CRC_FD7, ID_RECENT_FD7, ID_SELECT_D88_BANK7, ID_EJECT_D88_BANK7)
	#endif
	#if USE_FLOPPY_DISK >= 8
		FD_MENU_ITEMS(7, ID_OPEN_FD8, ID_CLOSE_FD8, ID_OPEN_BLANK_2D_FD8, ID_OPEN_BLANK_2DD_FD8, ID_OPEN_BLANK_2HD_FD8, ID_WRITE_PROTECT_FD8, ID_CORRECT_TIMING_FD8, ID_IGNORE_CRC_FD8, ID_RECENT_FD8, ID_SELECT_D88_BANK8, ID_EJECT_D88_BANK8)
	#endif
#endif
#ifdef USE_QUICK_DISK
	#if USE_QUICK_DISK >= 1
		#define QD_MENU_ITEMS(drv, ID_OPEN_QD, ID_CLOSE_QD, ID_RECENT_QD) \
		case ID_OPEN_QD: \
			if(emu) { \
				open_quick_disk_dialog(hWnd, drv); \
			} \
			break; \
		case ID_CLOSE_QD: \
			if(emu) { \
				emu->close_quick_disk(drv); \
			} \
			break; \
		case ID_RECENT_QD + 0: case ID_RECENT_QD + 1: case ID_RECENT_QD + 2: case ID_RECENT_QD + 3: \
		case ID_RECENT_QD + 4: case ID_RECENT_QD + 5: case ID_RECENT_QD + 6: case ID_RECENT_QD + 7: \
			if(emu) { \
				open_recent_quick_disk(drv, LOWORD(wParam) - ID_RECENT_QD); \
			} \
			break;
		QD_MENU_ITEMS(0, ID_OPEN_QD1, ID_CLOSE_QD1, ID_RECENT_QD1)
	#endif
	#if USE_QUICK_DISK >= 2
		QD_MENU_ITEMS(1, ID_OPEN_QD2, ID_CLOSE_QD2, ID_RECENT_QD2)
	#endif
#endif
#ifdef USE_HARD_DISK
	#if USE_HARD_DISK >= 1
		#define HD_MENU_ITEMS(drv, ID_OPEN_HD, ID_CLOSE_HD, ID_RECENT_HD) \
		case ID_OPEN_HD: \
			if(emu) { \
				open_hard_disk_dialog(hWnd, drv); \
			} \
			break; \
		case ID_CLOSE_HD: \
			if(emu) { \
				emu->close_hard_disk(drv); \
			} \
			break; \
		case ID_RECENT_HD + 0: case ID_RECENT_HD + 1: case ID_RECENT_HD + 2: case ID_RECENT_HD + 3: \
		case ID_RECENT_HD + 4: case ID_RECENT_HD + 5: case ID_RECENT_HD + 6: case ID_RECENT_HD + 7: \
			if(emu) { \
				open_recent_hard_disk(drv, LOWORD(wParam) - ID_RECENT_HD); \
			} \
			break;
		HD_MENU_ITEMS(0, ID_OPEN_HD1, ID_CLOSE_HD1, ID_RECENT_HD1)
	#endif
	#if USE_HARD_DISK >= 2
		HD_MENU_ITEMS(1, ID_OPEN_HD2, ID_CLOSE_HD2, ID_RECENT_HD2)
	#endif
	#if USE_HARD_DISK >= 3
		HD_MENU_ITEMS(2, ID_OPEN_HD3, ID_CLOSE_HD3, ID_RECENT_HD3)
	#endif
	#if USE_HARD_DISK >= 4
		HD_MENU_ITEMS(3, ID_OPEN_HD4, ID_CLOSE_HD4, ID_RECENT_HD4)
	#endif
	#if USE_HARD_DISK >= 5
		HD_MENU_ITEMS(4, ID_OPEN_HD5, ID_CLOSE_HD5, ID_RECENT_HD5)
	#endif
	#if USE_HARD_DISK >= 6
		HD_MENU_ITEMS(5, ID_OPEN_HD6, ID_CLOSE_HD6, ID_RECENT_HD6)
	#endif
	#if USE_HARD_DISK >= 7
		HD_MENU_ITEMS(6, ID_OPEN_HD7, ID_CLOSE_HD7, ID_RECENT_HD7)
	#endif
	#if USE_HARD_DISK >= 8
		HD_MENU_ITEMS(7, ID_OPEN_HD8, ID_CLOSE_HD8, ID_RECENT_HD8)
	#endif
#endif
#ifdef USE_TAPE
	#if USE_TAPE >= 1
		#define TAPE_MENU_ITEMS(drv, ID_PLAY_TAPE, ID_REC_TAPE, ID_CLOSE_TAPE, ID_PLAY_BUTTON, ID_STOP_BUTTON, ID_FAST_FORWARD, ID_FAST_REWIND, ID_APSS_FORWARD, ID_APSS_REWIND, ID_USE_WAVE_SHAPER, ID_DIRECT_LOAD_MZT, ID_TAPE_BAUD_LOW, ID_TAPE_BAUD_HIGH, ID_RECENT_TAPE) \
		case ID_PLAY_TAPE: \
			if(emu) { \
				open_tape_dialog(hWnd, drv, true); \
			} \
			break; \
		case ID_REC_TAPE: \
			if(emu) { \
				open_tape_dialog(hWnd, drv, false); \
			} \
			break; \
		case ID_CLOSE_TAPE: \
			if(emu) { \
				emu->close_tape(drv); \
			} \
			break; \
		case ID_PLAY_BUTTON: \
			if(emu) { \
				emu->push_play(drv); \
			} \
			break; \
		case ID_STOP_BUTTON: \
			if(emu) { \
				emu->push_stop(drv); \
			} \
			break; \
		case ID_FAST_FORWARD: \
			if(emu) { \
				emu->push_fast_forward(drv); \
			} \
			break; \
		case ID_FAST_REWIND: \
			if(emu) { \
				emu->push_fast_rewind(drv); \
			} \
			break; \
		case ID_APSS_FORWARD: \
			if(emu) { \
				emu->push_apss_forward(drv); \
			} \
			break; \
		case ID_APSS_REWIND: \
			if(emu) { \
				emu->push_apss_rewind(drv); \
			} \
			break; \
		case ID_USE_WAVE_SHAPER: \
			config.wave_shaper[drv] = !config.wave_shaper[drv]; \
			break; \
		case ID_DIRECT_LOAD_MZT: \
			config.direct_load_mzt[drv] = !config.direct_load_mzt[drv]; \
			break; \
		case ID_TAPE_BAUD_LOW: \
			config.baud_high[drv] = false; \
			break; \
		case ID_TAPE_BAUD_HIGH: \
			config.baud_high[drv] = true; \
			break; \
		case ID_RECENT_TAPE + 0: case ID_RECENT_TAPE + 1: case ID_RECENT_TAPE + 2: case ID_RECENT_TAPE + 3: \
		case ID_RECENT_TAPE + 4: case ID_RECENT_TAPE + 5: case ID_RECENT_TAPE + 6: case ID_RECENT_TAPE + 7: \
			if(emu) { \
				open_recent_tape(drv, LOWORD(wParam) - ID_RECENT_TAPE); \
			} \
			break;
		TAPE_MENU_ITEMS(0, ID_PLAY_TAPE1, ID_REC_TAPE1, ID_CLOSE_TAPE1, ID_PLAY_BUTTON1, ID_STOP_BUTTON1, ID_FAST_FORWARD1, ID_FAST_REWIND1, ID_APSS_FORWARD1, ID_APSS_REWIND1, ID_USE_WAVE_SHAPER1, ID_DIRECT_LOAD_MZT1, ID_TAPE_BAUD_LOW1, ID_TAPE_BAUD_HIGH1, ID_RECENT_TAPE1)
	#endif
	#if USE_TAPE >= 2
		TAPE_MENU_ITEMS(1, ID_PLAY_TAPE2, ID_REC_TAPE2, ID_CLOSE_TAPE2, ID_PLAY_BUTTON2, ID_STOP_BUTTON2, ID_FAST_FORWARD2, ID_FAST_REWIND2, ID_APSS_FORWARD2, ID_APSS_REWIND2, ID_USE_WAVE_SHAPER2, ID_DIRECT_LOAD_MZT2, ID_TAPE_BAUD_LOW2, ID_TAPE_BAUD_HIGH2, ID_RECENT_TAPE2)
	#endif
#endif
#ifdef USE_COMPACT_DISC
	#if USE_COMPACT_DISC >= 1
		#define COMPACT_DISC_MENU_ITEMS(drv, ID_OPEN_COMPACT_DISC, ID_CLOSE_COMPACT_DISC, ID_RECENT_COMPACT_DISC) \
		case ID_OPEN_COMPACT_DISC: \
			if(emu) { \
				open_compact_disc_dialog(hWnd, drv); \
			} \
			break; \
		case ID_CLOSE_COMPACT_DISC: \
			if(emu) { \
				emu->close_compact_disc(drv); \
			} \
			break; \
		case ID_RECENT_COMPACT_DISC + 0: case ID_RECENT_COMPACT_DISC + 1: case ID_RECENT_COMPACT_DISC + 2: case ID_RECENT_COMPACT_DISC + 3: \
		case ID_RECENT_COMPACT_DISC + 4: case ID_RECENT_COMPACT_DISC + 5: case ID_RECENT_COMPACT_DISC + 6: case ID_RECENT_COMPACT_DISC + 7: \
			if(emu) { \
				open_recent_compact_disc(drv, LOWORD(wParam) - ID_RECENT_COMPACT_DISC); \
			} \
			break;
		COMPACT_DISC_MENU_ITEMS(0, ID_OPEN_COMPACT_DISC1, ID_CLOSE_COMPACT_DISC1, ID_RECENT_COMPACT_DISC1)
	#endif
	#if USE_COMPACT_DISC >= 2
		COMPACT_DISC_MENU_ITEMS(1, ID_OPEN_COMPACT_DISC2, ID_CLOSE_COMPACT_DISC2, ID_RECENT_COMPACT_DISC2)
	#endif
#endif
#ifdef USE_LASER_DISC
	#if USE_LASER_DISC >= 1
		#define LASER_DISC_MENU_ITEMS(drv, ID_OPEN_LASER_DISC, ID_CLOSE_LASER_DISC, ID_RECENT_LASER_DISC) \
		case ID_OPEN_LASER_DISC: \
			if(emu) { \
				open_laser_disc_dialog(hWnd, drv); \
			} \
			break; \
		case ID_CLOSE_LASER_DISC: \
			if(emu) { \
				emu->close_laser_disc(drv); \
			} \
			break; \
		case ID_RECENT_LASER_DISC + 0: case ID_RECENT_LASER_DISC + 1: case ID_RECENT_LASER_DISC + 2: case ID_RECENT_LASER_DISC + 3: \
		case ID_RECENT_LASER_DISC + 4: case ID_RECENT_LASER_DISC + 5: case ID_RECENT_LASER_DISC + 6: case ID_RECENT_LASER_DISC + 7: \
			if(emu) { \
				open_recent_laser_disc(drv, LOWORD(wParam) - ID_RECENT_LASER_DISC); \
			} \
			break;
		LASER_DISC_MENU_ITEMS(0, ID_OPEN_LASER_DISC1, ID_CLOSE_LASER_DISC1, ID_RECENT_LASER_DISC1)
	#endif
	#if USE_LASER_DISC >= 2
		LASER_DISC_MENU_ITEMS(1, ID_OPEN_LASER_DISC2, ID_CLOSE_LASER_DISC2, ID_RECENT_LASER_DISC2)
	#endif
#endif
#ifdef USE_BINARY_FILE
	#if USE_BINARY_FILE >= 1
		#define BINARY_MENU_ITEMS(drv, ID_LOAD_BINARY, ID_SAVE_BINARY, ID_RECENT_BINARY) \
		case ID_LOAD_BINARY: \
			if(emu) { \
				open_binary_dialog(hWnd, drv, true); \
			} \
			break; \
		case ID_SAVE_BINARY: \
			if(emu) { \
				open_binary_dialog(hWnd, drv, false); \
			} \
			break; \
		case ID_RECENT_BINARY + 0: case ID_RECENT_BINARY + 1: case ID_RECENT_BINARY + 2: case ID_RECENT_BINARY + 3: \
		case ID_RECENT_BINARY + 4: case ID_RECENT_BINARY + 5: case ID_RECENT_BINARY + 6: case ID_RECENT_BINARY + 7: \
			if(emu) { \
				open_recent_binary(drv, LOWORD(wParam) - ID_RECENT_BINARY); \
			} \
			break;
		BINARY_MENU_ITEMS(0, ID_LOAD_BINARY1, ID_SAVE_BINARY1, ID_RECENT_BINARY1)
	#endif
	#if USE_BINARY_FILE >= 2
		BINARY_MENU_ITEMS(1, ID_LOAD_BINARY2, ID_SAVE_BINARY2, ID_RECENT_BINARY2)
	#endif
#endif
#ifdef USE_BUBBLE
	#if USE_BUBBLE >= 1
		#define BUBBLE_CASETTE_MENU_ITEMS(drv, ID_OPEN_BUBBLE, ID_CLOSE_BUBBLE, ID_RECENT_BUBBLE) \
		case ID_OPEN_BUBBLE: \
			if(emu) { \
				open_bubble_casette_dialog(hWnd, drv); \
			} \
			break; \
		case ID_CLOSE_BUBBLE: \
			if(emu) { \
				emu->close_bubble_casette(drv); \
			} \
			break; \
		case ID_RECENT_BUBBLE + 0: case ID_RECENT_BUBBLE + 1: case ID_RECENT_BUBBLE + 2: case ID_RECENT_BUBBLE + 3: \
		case ID_RECENT_BUBBLE + 4: case ID_RECENT_BUBBLE + 5: case ID_RECENT_BUBBLE + 6: case ID_RECENT_BUBBLE + 7: \
			if(emu) { \
				open_recent_bubble_casette(drv, LOWORD(wParam) - ID_RECENT_BUBBLE); \
			} \
			break;
		BUBBLE_CASETTE_MENU_ITEMS(0, ID_OPEN_BUBBLE1, ID_CLOSE_BUBBLE1, ID_RECENT_BUBBLE1)
	#endif
	#if USE_BUBBLE >= 2
		BUBBLE_CASETTE_MENU_ITEMS(1, ID_OPEN_BUBBLE2, ID_CLOSE_BUBBLE2, ID_RECENT_BUBBLE2)
	#endif
#endif
		case ID_ACCEL_SCREEN:
			if(emu) {
				emu->suspend();
				set_window(hWnd, now_fullscreen ? prev_window_mode : -1);
			}
			break;
#ifdef USE_MOUSE
		case ID_ACCEL_MOUSE:
			if(emu) {
				emu->toggle_mouse();
			}
			break;
#endif
		case ID_ACCEL_SPEED:
			config.full_speed = !config.full_speed;
			break;
#ifdef USE_AUTO_KEY
		case ID_ACCEL_ROMAJI:
			if(emu) {
				if(!config.romaji_to_kana) {
					emu->set_auto_key_char(1); // start
					config.romaji_to_kana = true;
				} else {
					emu->set_auto_key_char(0); // end
					config.romaji_to_kana = false;
				}
			}
			break;
#endif
#ifdef ONE_BOARD_MICRO_COMPUTER
		case ID_BUTTON +  0: case ID_BUTTON +  1: case ID_BUTTON +  2: case ID_BUTTON +  3:
		case ID_BUTTON +  4: case ID_BUTTON +  5: case ID_BUTTON +  6: case ID_BUTTON +  7:
		case ID_BUTTON +  8: case ID_BUTTON +  9: case ID_BUTTON + 10: case ID_BUTTON + 11:
		case ID_BUTTON + 12: case ID_BUTTON + 13: case ID_BUTTON + 14: case ID_BUTTON + 15:
		case ID_BUTTON + 16: case ID_BUTTON + 17: case ID_BUTTON + 18: case ID_BUTTON + 19:
		case ID_BUTTON + 20: case ID_BUTTON + 21: case ID_BUTTON + 22: case ID_BUTTON + 23:
		case ID_BUTTON + 24: case ID_BUTTON + 25: case ID_BUTTON + 26: case ID_BUTTON + 27:
		case ID_BUTTON + 28: case ID_BUTTON + 29: case ID_BUTTON + 30: case ID_BUTTON + 31:
			if(emu) {
				emu->press_button(LOWORD(wParam) - ID_BUTTON);
			}
			break;
#endif
		}
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}
*/


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
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::cEmuSCV()\n", EMUSCV_NAME);

	// Init variables
	retro_core_initialized		= false;
/*
	retro_use_audio_cb			= false;
*/
	retro_audio_enable			= false;
/*
	retro_audio_phase			= 0;
*/
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

	frame_surface = NULL;
	frame_renderer = NULL;
	noise_surface = NULL;
	noise_renderer = NULL;

	run_frames				= 0;
	draw_frames					= 0;
//	skip_frames					= 0;
//	next_time.tv_sec			= 0;
//	next_time.tv_nsec			= 0;
//	prev_skip					= false;
//	update_fps_time.tv_sec		= 0;
//	update_fps_time.tv_nsec		= 0;
//	update_status_bar_time		= 0;
//	disable_screen_saver_time	= 0;

	config.window_width = WINDOW_WIDTH_EMUSCV;
	config.window_height = WINDOW_HEIGHT_EMUSCV;
	config.window_aspect_ratio = WINDOW_ASPECT_RATIO_EMUSCV;
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

	start_up_counter_power = 0;
	start_up_counter_logo = 0;
}

//
// EmuSCV class destructor.
//
cEmuSCV::~cEmuSCV()
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::~cEmuSCV()\n", EMUSCV_NAME);

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
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetEnvironment()\n", EMUSCV_NAME);

	// Set the Libretro environment callback
	RetroEnvironment = cb;
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Environment callback set\n", EMUSCV_NAME);

	// Set the log callback
	static struct retro_log_callback log_callback;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log_callback))
	{
		RetroLogPrintf = log_callback.log;
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Log callback set to LibRetro\n", EMUSCV_NAME);
	}
	else
	{
		RetroLogPrintf = EmuSCV_RetroLogFallback;
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Log callback set to EmuSCV\n", EMUSCV_NAME);
	}

	// The core can be run without ROM (to show display test if no game loaded)
	static bool support_no_game = TRUE;
	RetroEnvironment(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &support_no_game);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] support_no_game set\n", EMUSCV_NAME);

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
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Controller infos set\n", EMUSCV_NAME);

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
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Input descriptors set\n", EMUSCV_NAME);

	// Set config variables
	struct retro_variable variable[] =
	{
		{ SETTING_CONSOLE_KEY, "CONSOLE; AUTO|EPOCH|YENO|EPOCHLADY" },
		{ SETTING_DISPLAY_KEY, "DISPLAY; AUTO|EMUSCV|EPOCH|YENO" },
		{ SETTING_DISPLAYFPS_KEY, "DISPLAY FPS; AUTO|EPOCH_60|YENO_50" },
		{ SETTING_DISPLAYFULL_KEY, "DISPLAY FULL; NO|YES" },
		{ SETTING_DISPLAYINPUTS_KEY, "DISPLAY INPUTS; NO|YES" },
		{ SETTING_LANGAGE_KEY, "LANGAGE; AUTO|JP|EN|FR" },
		{ SETTING_CHECKBIOS_KEY, "CHECKBIOS; YES|NO" },
		{ NULL, NULL }
	};
	RetroEnvironment(RETRO_ENVIRONMENT_SET_VARIABLES, variable);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Variables set\n", EMUSCV_NAME);
}

// 
// Libretro: set the video refresh callback
// 
void cEmuSCV::RetroSetVideoRefresh(retro_video_refresh_t cb)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetVideoRefresh()\n", EMUSCV_NAME);

	// Set the Libretro video callback
	RetroVideoRefresh = cb;
}

// 
// Libretro: set the audio sample callback
// 
void cEmuSCV::RetroSetAudioSample(retro_audio_sample_t cb)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetAudioSample()\n", EMUSCV_NAME);

	// Set the Libretro audio sample callback
	RetroAudioSample = cb;
}

// 
// Libretro: set the audio batch callback
// 
void cEmuSCV::RetroSetAudioSampleBatch(retro_audio_sample_batch_t cb)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetAudioSampleBatch()\n", EMUSCV_NAME);

	// Set the Libretro audio sample batch callback
	RetroAudioSampleBatch = cb;
}

// 
// Libretro: set the input poll callback
// 
void cEmuSCV::RetroSetInputPoll(retro_input_poll_t cb)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetInputPoll()\n", EMUSCV_NAME);

	RetroInputPoll = cb;
}

// 
// Libretro: set the input state callback
// 
void cEmuSCV::RetroSetInputState(retro_input_state_t cb)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetInputState()\n", EMUSCV_NAME);

	RetroInputState = cb;
}

//
// Libretro: return the version used by the core for compatibility check with the frontend
//
unsigned cEmuSCV::RetroGetApiVersion(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroApiVersion() => %d\n", EMUSCV_NAME, RETRO_API_VERSION);

	return RETRO_API_VERSION;
}

//
// Libretro: return the video standard used
//
unsigned cEmuSCV::RetroGetVideoRegion(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroGetVideoRegion() => RETRO_REGION_PAL\n", EMUSCV_NAME);
//	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroGetVideoRegion() => RETRO_REGION_NTSC\n", EMUSCV_NAME);

	return RETRO_REGION_PAL;
//	return RETRO_REGION_NTSC;
}

// 
// Libretro: get the system infos
// 
void cEmuSCV::RetroGetSystemInfo(struct retro_system_info *info)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroGetSystemInfo()\n     library_name     = %s\n     library_version  = %s\n     valid_extensions = %s\n     need_fullpath    = false\n     block_extract    = true\n", EMUSCV_NAME, EMUSCV_NAME, EMUSCV_VERSION, EMUSCV_EXTENSIONS);

	memset(info, 0, sizeof(*info));
	info->library_name		= EMUSCV_NAME;
	info->library_version	= EMUSCV_VERSION;
	info->valid_extensions	= EMUSCV_EXTENSIONS;
	info->need_fullpath		= false;
	info->block_extract		= true;
}

// 
// Libretro: get the audio/video infos
// 
void cEmuSCV::RetroGetAudioVideoInfo(struct retro_system_av_info *info)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV:RetroGetAudioVideoInfo()\n     timing.fps            = %f\n     timing.sample_rate    = 44100\n     geometry.base_width   = %d\n     geometry.base_height  = %d\n     geometry.max_width    = %d\n     geometry.max_height   = %d\n     geometry.aspect_ratio = %f\n", EMUSCV_NAME, config.window_fps, config.window_width, config.window_height, config.window_width * WINDOW_ZOOM_MAX, config.window_height * WINDOW_ZOOM_MAX, config.window_aspect_ratio);

	memset(info, 0, sizeof(*info));
	info->timing.fps            = config.window_fps;
	info->timing.sample_rate    = 44100;
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
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroInit()\n", EMUSCV_NAME);

	if (retro_core_initialized == TRUE)
	{
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Initialisaztions already done\n", EMUSCV_NAME);
		return;
	}
/*
	// Set audio callbacks
	struct retro_audio_callback audio_callback = { RetroAudioCb, RetroAudioSetStateCb };
	retro_use_audio_cb = RetroEnvironment(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &audio_callback);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Audio callback set\n", EMUSCV_NAME);
//retro_use_audio_cb = false;
*/
	// Set frame time callback
	struct retro_frame_time_callback frame_time_callback;
	frame_time_callback.callback  = RetroFrameTimeCb;
	frame_time_callback.reference = 1000000 / FRAMES_PER_SEC;
	frame_time_callback.callback(frame_time_callback.reference);
	RetroEnvironment(RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK, &frame_time_callback);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Frame time callback set\n", EMUSCV_NAME);

	// Retrieve base directory
	const char *dir = NULL;
	memset(retro_base_directory, 0, sizeof(retro_base_directory));
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
	{
		snprintf(retro_base_directory, sizeof(retro_base_directory), "%s", dir);
		set_libretro_system_directory(dir);
		RetroLogPrintf(RETRO_LOG_INFO, "[%s] Base directory: %s\n", EMUSCV_NAME, retro_base_directory);
	}
	else
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Base directory not set\n", EMUSCV_NAME);

	// Retrieve save directory	retro_base_path
	memset(retro_save_directory, 0, sizeof(retro_save_directory));
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &dir) && dir)
	{
		snprintf(retro_save_directory, sizeof(retro_save_directory), "%s", dir);
		set_libretro_save_directory(dir);
		RetroLogPrintf(RETRO_LOG_INFO, "[%s] Save directory: %s\n", EMUSCV_NAME, retro_save_directory);
	}
	else
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Save directory not set\n", EMUSCV_NAME);

	// Init core variables
	is_power_on					= false;
	retro_core_initialized 		= false;
	retro_frame_time			= 0;
	retro_game_loaded			= false;
	retro_audio_enable			= false;
/*
	retro_audio_phase			= 0;
*/

	retro_input_support_bitmask	= RetroEnvironment(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL);
	if(retro_input_support_bitmask)
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] retro_input_support_bitmask set (true)\n", EMUSCV_NAME);
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] retro_input_support_bitmask set (false)\n", EMUSCV_NAME);
/*
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL initialization failed! %s\n", EMUSCV_NAME, SDL_GetError());
		return;	// Fatal error
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL initialization ok\n", EMUSCV_NAME);
*/
	// Initialize eSCV settings
	initialize_config();
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] config initialized\n", EMUSCV_NAME);

	// Load EmuSCV settings
	RetroLoadSettings();
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] settings (variables) loaded\n", EMUSCV_NAME);

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

	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] All initialisations done\n", EMUSCV_NAME);
}

//
// Libretro: deinitialize the core
//
void cEmuSCV::RetroDeinit(void)
{
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroDeinit()\n", EMUSCV_NAME);

	if (!retro_core_initialized)
	{
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] All deinitialisaztions already done\n", EMUSCV_NAME);
		return;
	}

	if (retro_game_loaded)
	{
		RetroUnloadGame();
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game unloaded\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game not loaded, nothing to unload\n", EMUSCV_NAME);	

	// Destroy eSCV
	if (escv_emu)
	{
#ifdef USE_NOTIFY_POWER_OFF
		// notify power off
		escv_emu->notify_power_off();

#endif
		delete(escv_emu);
		escv_emu = NULL;
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu deleted\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu don't exists, nothing to delete\n", EMUSCV_NAME);
/*	
	// Deinit SDL
	SDL_Quit();
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL quitted\n", EMUSCV_NAME);
*/
	// Reinit core variables
	retro_frame_time			= 0;
	retro_game_loaded			= FALSE;
    retro_input_support_bitmask	= FALSE;
	retro_audio_enable			= FALSE;
/*
	retro_audio_phase			= 0;
*/
	retro_core_initialized 		= FALSE;

	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] All deinitialisaztions done\n", EMUSCV_NAME);
}

// 
// Libretro: set controller port device
// 
void cEmuSCV::RetroSetControllerPortDevice(unsigned port, unsigned device)
{
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSetControllerPortDevice()\n          Set device %d into port %d\n", EMUSCV_NAME, device, port);
}

/*
// 
// Libretro: audio callback
// 
void cEmuSCV::RetroAudioCb(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroAudioCb()\n", EMUSCV_NAME);
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

		for (uint16_t i = 0; i < 44100; i++)
			RetroAudioSample(val, val);

//	}
	retro_audio_phase %= 44100;//AUDIO_SAMPLING_RATE;
//RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] cEmuSCV::RetroAudioCb() => RetroAudioSample()\n", EMUSCV_NAME);
//int16_t val = 0;
//for (uint32_t i = 0; i < 44100; i++)
//	RetroAudioSample(val, val);
}
*/

// 
// Libretro: audio set state enable/disable callback
// 
void cEmuSCV::RetroAudioSetStateCb(bool enable)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroAudioSetStateCb()\n          Set audio state = %s\n", EMUSCV_NAME, (enable ? "ON" : "OFF"));

	// Set audio state
	retro_audio_enable = enable;
}

// 
// Libretro: frame time callback
// 
void cEmuSCV::RetroFrameTimeCb(retro_usec_t usec)
{

	int64_t usec_corrected = usec*FRAMES_PER_SEC/config.window_fps;
//int64_t usec_corrected = usec*FRAMES_PER_SEC/WINDOW_FPS_EPOCH;

	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] cEmuSCV::RetroFrameTimeCb()\n          Frame time = %d microseconds\n", EMUSCV_NAME, usec_corrected);
	
	// Memorise current frame time
	retro_frame_time = usec_corrected;
}

// 
// Libretro: load game
// 
bool cEmuSCV::RetroLoadGame(const struct retro_game_info *info)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroLoadGame()\n", EMUSCV_NAME);

	if(retro_game_loaded)
	{
		RetroUnloadGame();
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Previous game unloded\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No previous game, nothing to unload\n", EMUSCV_NAME);
	
	retro_game_loaded = false;

	// Set the performance level is guide to Libretro to give an idea of how intensive this core is to run for the game
	// It should be set in retro_load_game()
	static unsigned performance_level = 4;
	RetroEnvironment(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &performance_level);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Performance level set (4)\n", EMUSCV_NAME);

    enum retro_pixel_format pixel_format = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!RetroEnvironment(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format))
    {
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Pixel format not supported! (XRGB8888)\n", EMUSCV_NAME);
        return FALSE;
    }
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Pixel format set (XRGB8888)\n", EMUSCV_NAME);

	bool updated = false;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
	{
		RetroLoadSettings();
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Settings (variable) loaded\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Settings not updated, already loaded\n", EMUSCV_NAME);

	// Create SDL main frame surface
    frame_surface = SDL_CreateRGBSurface(0, config.window_width, config.window_height, 8*sizeof(uint32_t), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
//frame_surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH_EMUSCV, WINDOW_HEIGHT_EMUSCV, 8*sizeof(uint32_t), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    if (frame_surface == NULL)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
        return FALSE;
	}
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface created\n", EMUSCV_NAME);

	// Create SDL main frame renderer
    frame_renderer = SDL_CreateSoftwareRenderer(frame_surface);
    if (frame_renderer == NULL)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface renderer creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
        return FALSE;
    }
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface renderer created\n", EMUSCV_NAME);

	// create SDL TV static noise surface
    noise_surface = SDL_CreateRGBSurface(0, config.window_width >> 2, config.window_height >> 2, 8*sizeof(uint32_t), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    if (noise_surface == NULL)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
        return FALSE;
	}
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface created\n", EMUSCV_NAME);

	// Create SDL TV static noise renderer
    noise_renderer = SDL_CreateSoftwareRenderer(noise_surface);
    if (noise_renderer == NULL)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface renderer creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
        return FALSE;
    }
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer created\n", EMUSCV_NAME);

	// Paint it black
	SDL_SetRenderDrawColor(frame_renderer, 0, 0, 0, 255);
	SDL_RenderClear(frame_renderer);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface cleared\n", EMUSCV_NAME);

	// reset the frame counter
	retro_frame_counter = 0;

	// Get game rom path
	memset(retro_game_path, 0, sizeof(retro_game_path));
    if (info && info->data)
	{
		snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);
		RetroLogPrintf(RETRO_LOG_INFO, "[%s] load game: %s\n", EMUSCV_NAME, retro_game_path);
/*
		unzFile zipfile = unzOpen(retro_game_path);
		if (zipfile == NULL)
		{
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] game ZIP archive not found!\n", EMUSCV_NAME);
			return FALSE;
		}
		RetroLogPrintf(RETRO_LOG_INFO, "[%s] game ZIP archive found\n", EMUSCV_NAME);

		// Get info about the zip file
		unz_global_info global_info;
		if (unzGetGlobalInfo( zipfile, &global_info ) != UNZ_OK)
		{
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] can't read game ZIP archive!\n", EMUSCV_NAME);
			unzClose(zipfile);
			return FALSE;
		}
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] number of entries %d\n", EMUSCV_NAME, global_info.number_entry);

		// Buffer to hold data read from the zip file.
		uint8_t *read_buffer = NULL;

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
				RetroLogPrintf(RETRO_LOG_ERROR, "[%s] could not read file info\n", EMUSCV_NAME);
				unzClose(zipfile);
				return FALSE;
			}
//			RetroLogPrintf(RETRO_LOG_INFO, "[%s] file info read\n", EMUSCV_NAME);

			// Check if this entry is a directory or file.
//			const size_t filename_length = strlen( filename );
//			if ( filename[filename_length-1] == dir_delimiter )
//			{
//				// Entry is a directory, so create it.
//				RetroLogPrintf(RETRO_LOG_INFO, "[%s] dir: %s\n", EMUSCV_NAME, filename);
//				mkdir( filename );
//			}
//			else
//			if (unzStringFileNameCompare(filename, "cartfrfront.png", 2) == 0)
//			{
				// Entry is a file, so extract it.
				RetroLogPrintf(RETRO_LOG_INFO, "[%s] file: %s\n", EMUSCV_NAME, filename);
				RetroLogPrintf(RETRO_LOG_INFO, "[%s] uncompressed size: %u\n", EMUSCV_NAME, file_info.uncompressed_size);
				RetroLogPrintf(RETRO_LOG_INFO, "[%s] crc: %u\n", EMUSCV_NAME, file_info.crc);

				if ( unzOpenCurrentFile( zipfile ) != UNZ_OK )
				{
					log_printf_cb(RETRO_LOG_ERROR, "[%s] can't open file\n", EMUSCV_NAME);
					unzClose( zipfile );
					return FALSE;
				}
//				// Open a file to write out the data.
//				FILE *out = fopen( filename, "wb" );
//				if ( out == NULL )
//				{
//					printf( "could not open destination file\n" );
//					unzCloseCurrentFile( zipfile );
//					unzClose( zipfile );
//					return -1;
//				}
				read_buffer = (uint8_t*)malloc(file_info.uncompressed_size);
				if ( read_buffer == NULL )
				{
					RetroLogPrintf(RETRO_LOG_ERROR, "[%s] can't allocate memory for the image!\n", EMUSCV_NAME);
					unzCloseCurrentFile( zipfile );
					unzClose( zipfile );
					return FALSE;
				}
				RetroLogPrintf(RETRO_LOG_INFO, "[%s] Memory allocated for PNG image!\n", EMUSCV_NAME);

				int error = UNZ_OK;
				do
				{
					error = unzReadCurrentFile( zipfile, read_buffer, file_info.uncompressed_size );
					if ( error < 0 )
					{
						RetroLogPrintf(RETRO_LOG_ERROR, "[%s] error %d\n", EMUSCV_NAME, error);
						unzCloseCurrentFile( zipfile );
						unzClose( zipfile );
						return FALSE;
					}
					RetroLogPrintf(RETRO_LOG_INFO, "[%s] PNG image loaded into memory\n", EMUSCV_NAME);
//					// Write data to file.
//					if ( error > 0 )
//					{
//						fwrite( read_buffer, error, 1, out ); // You should check return of fwrite...
//					}
				} while ( error > 0 );
//				fclose( out );
//			}
			unzCloseCurrentFile( zipfile );
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] ZIP file closed\n", EMUSCV_NAME);

//			// Go the the next entry listed in the zip file.
//			if ( ( i+1 ) < global_info.number_entry )
//			{
//				if ( unzGoToNextFile( zipfile ) != UNZ_OK )
//				{
//					RetroLogPrintf(RETRO_LOG_ERROR, "[%s] cound not read next file\n", EMUSCV_NAME);
//					unzClose( zipfile );
//					return false;
//				}
//			}
		}
		unzClose(zipfile);


		if (read_buffer != NULL)
		{
//			uint8_t *read_buffer_sig[9];
//			memset(read_buffer_sig,0,9);
//			memcpy(read_buffer_sig, read_buffer, 8);
//			RetroLogPrintf(RETRO_LOG_INFO, "[%s] PNG signature = %s\n", EMUSCV_NAME, read_buffer_sig);
			if(png_check_sig(read_buffer,8))
			{
				RetroLogPrintf(RETRO_LOG_INFO, "[%s] PNG signature OK\n", EMUSCV_NAME);

				// get PNG file info struct (memory is allocated by libpng)
				png_structp png_ptr = NULL;
				png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

				if(png_ptr != NULL)
				{
					log_printf_cb(RETRO_LOG_INFO, "[%s] PNG read struct created\n", EMUSCV_NAME);

					// get PNG image data info struct (memory is allocated by libpng)
					png_infop info_ptr = NULL;
					info_ptr = png_create_info_struct(png_ptr);

					if(info_ptr != NULL)
					{
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] PNG info struct created\n", EMUSCV_NAME);
						PngReadDataHandle a = {{read_buffer,file_info.uncompressed_size},0};
						png_set_read_fn(png_ptr, &a, png_read_data_from_inputstream);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] PNG read fn setted\n", EMUSCV_NAME);
						png_read_info(png_ptr, info_ptr);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] png_read_info()\n", EMUSCV_NAME);
						image_width = png_get_image_width(png_ptr, info_ptr);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] image_width = %u\n", EMUSCV_NAME, image_width);
						image_height = png_get_image_height(png_ptr, info_ptr);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] image_height = %u\n", EMUSCV_NAME, image_height);
						image_bit_depth = png_get_bit_depth(png_ptr, info_ptr);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] image_bit_depth = %u\n", EMUSCV_NAME, image_bit_depth);
						image_color_type = png_get_color_type(png_ptr, info_ptr);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] image_color_type = %u\n", EMUSCV_NAME, image_color_type);

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
							RetroLogPrintf(RETRO_LOG_ERROR, "[%s] error allocating image buffer\n", EMUSCV_NAME);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] image buffer allocated\n", EMUSCV_NAME);

						// allocate row pointers:
						image_row_data = (png_bytep *)malloc((size_t)(image_height * sizeof(png_bytep)));
						if (image_row_data == NULL)
							RetroLogPrintf(RETRO_LOG_ERROR, "[%s] error allocating row pointers\n", EMUSCV_NAME);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] row pointers allocated\n", EMUSCV_NAME);

						// set the row pointers and read the RGBA image data:
						for (png_uint_32 row = 0; row < image_height; row++)
							image_row_data[row] = image_data + (image_height - (row + 1)) * (image_width * (4));
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] row pointers set\n", EMUSCV_NAME);

						png_read_image(png_ptr, image_row_data);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] png_read_image()\n", EMUSCV_NAME);

						// libpng and dynamic resource unwinding:
						png_read_end(png_ptr, NULL);
						RetroLogPrintf(RETRO_LOG_INFO, "[%s] png_read_end()\n", EMUSCV_NAME);

//						png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);
//						log_printf_cb(RETRO_LOG_INFO, "[%s] png_get_IHDR()\n", EMUSCV_NAME);
//						if(retval != 1)
//						{
//							RetroLogPrintf(RETRO_LOG_INFO, "[%s] PNG width:%u \n", EMUSCV_NAME, width);
//							RetroLogPrintf(RETRO_LOG_INFO, "[%s] PNG height:%u \n", EMUSCV_NAME, height);
//							RetroLogPrintf(RETRO_LOG_INFO, "[%s] TEST TEST TEST\n", EMUSCV_NAME);
//						}
//						else
//						{
//							RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can't read PNG header!\n", EMUSCV_NAME);
//						}
//						png_destroy_info_struct(&png_ptr, NULL, NULL);
					}
					else
					{
						RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can't create PNG info struct!\n", EMUSCV_NAME);
					}
					png_destroy_read_struct(&png_ptr, NULL, NULL);
					RetroLogPrintf(RETRO_LOG_INFO, "[%s] png_destroy_read_struct()\n", EMUSCV_NAME);
				}
				else
				{
					RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can't create PNG read struct!\n", EMUSCV_NAME);
				}
			}
			else
			{
				RetroLogPrintf(RETRO_LOG_WARN, "[%s] PNG signature KO\n", EMUSCV_NAME);
			}



			free(read_buffer);
			read_buffer = NULL;
		}
*/
		// Insert cart
/*
		if(escv_emu && escv_emu->is_bios_present() && strcmp(retro_game_path, "") != 0)
		{
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Try to load %s\n", EMUSCV_NAME, retro_game_path);
			escv_emu->open_cart(0,retro_game_path);
			if(escv_emu->is_cart_inserted(0))
			{
				retro_game_loaded = true;
				RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game loaded\n", EMUSCV_NAME);
			}
			else
			{
				RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Game not loaded\n", EMUSCV_NAME);
*/
FILEIO *fio = new FILEIO();
if(fio->IsFileExisting(retro_game_path))
	RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Game file exists\n", EMUSCV_NAME);
else
	RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Game file don't exist\n", EMUSCV_NAME);
if(fio->IsFileProtected(retro_game_path))
	RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Game file is protected\n", EMUSCV_NAME);
else
	RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Game file is not protected\n", EMUSCV_NAME);
if(!fio->Fopen(retro_game_path, FILEIO_READ_BINARY))
{
	RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can open game file in READ_BINARY mode\n", EMUSCV_NAME);
	uint8_t raw[0x20000];
	if(fio->Fread(&raw, sizeof(raw), 1))
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can read game file in READ_BINARY mode\n", EMUSCV_NAME);
	else
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can't read game file in READ_BINARY mode\n", EMUSCV_NAME);
	fio->Fclose();
}
else
	RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can't open game file in READ_BINARY mode\n", EMUSCV_NAME);
if(!fio->Fopen(retro_game_path, FILEIO_READ_ASCII))
{
	RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can open game file in FILEIO_READ_ASCII mode\n", EMUSCV_NAME);
	uint8_t raw[0x20000];
	if(fio->Fread(&raw, sizeof(raw), 1))
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can read game file in FILEIO_READ_ASCII mode\n", EMUSCV_NAME);
	else
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can't read game file in FILEIO_READ_ASCII mode\n", EMUSCV_NAME);
	fio->Fclose();
}
else
	RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Can't open game file in FILEIO_READ_ASCII mode\n", EMUSCV_NAME);
delete(fio);
/*
			}
		}
		else
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] Bios not present or empty game path\n", EMUSCV_NAME);
//RetroLogPrintf(RETRO_LOG_INFO, "[%s] WE DON'T TRY TO LOAD GAME\n", EMUSCV_NAME);
*/	}
	else
		RetroLogPrintf(RETRO_LOG_INFO, "[%s] no game\n", EMUSCV_NAME);

	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game loading done\n", EMUSCV_NAME);

	return true;
}

// 
// Libretro: unload game
// 
void cEmuSCV::RetroUnloadGame(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroUnloadGame()\n", EMUSCV_NAME);

	// Free eSCV
	if(escv_emu && escv_emu->is_cart_inserted(0))
	{
		escv_emu->close_cart(0);
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game unloaded\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No game loaded, nothing to unload\n", EMUSCV_NAME);

	// Free SDL TV static noise renderer
	if (noise_renderer != NULL)
	{
		SDL_DestroyRenderer(noise_renderer);
		noise_renderer = NULL;
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer destroyed\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface renderer, nothing to destroy\n", EMUSCV_NAME);

	// Free SDL TV static noise surface
	if (noise_surface != NULL)
	{
		SDL_FreeSurface(noise_surface);
		noise_surface = NULL;
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface destroyed\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface, nothing to destroy\n", EMUSCV_NAME);

	// Free SDL main frame renderer
	if (frame_renderer != NULL)
	{
		SDL_DestroyRenderer(frame_renderer);
		frame_renderer = NULL;
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface renderer destroyed\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL main surface renderer, nothing to destroy\n", EMUSCV_NAME);

	// Free SDL main frame surface
	if (frame_surface != NULL)
	{
		SDL_FreeSurface(frame_surface);
		frame_surface = NULL;
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface destroyed\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL main surface, nothing to destroy\n", EMUSCV_NAME);

	retro_game_loaded = FALSE;
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Game unloading done\n", EMUSCV_NAME);
}

//
// Libretro: run for only one frame
//
void cEmuSCV::RetroRun(void)
{
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] cEmuSCV::RetroRun()\n", EMUSCV_NAME);

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

	int16_t joyposx			= 0;
	int16_t joyposy			= 0;
	uint32_t color			= 0xFF000000;

	bool config_changed = false;


	// SDL surfaces and renderers must exists
	if (!frame_surface)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface not created!\n", EMUSCV_NAME);
		return;
	}
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface exists\n", EMUSCV_NAME);

	if (!frame_renderer)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface renderer not created!\n", EMUSCV_NAME);
		return;
	}
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface exists\n", EMUSCV_NAME);

	if (!noise_surface)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface not created!\n", EMUSCV_NAME);
		return;
	}
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface exists\n", EMUSCV_NAME);

	if (!noise_renderer)
	{
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface renderer not created!\n", EMUSCV_NAME);
		return;
	}
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface exists\n", EMUSCV_NAME);

	// Recreate SDL surfaces and renderers if the size change
	if(frame_surface->w != config.window_width || frame_surface->h != config.window_height)
	{
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Resolution change detected\n", EMUSCV_NAME);

		// Free SDL TV static noise renderer
		if (noise_renderer != NULL)
		{
			SDL_DestroyRenderer(noise_renderer);
			noise_renderer = NULL;
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface renderer destroyed\n", EMUSCV_NAME);
		}
		else
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface renderer, nothing to destroy\n", EMUSCV_NAME);

		// Free SDL TV static noise surface
		if (noise_surface != NULL)
		{
			SDL_FreeSurface(noise_surface);
			noise_surface = NULL;
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface destroyed\n", EMUSCV_NAME);
		}
		else
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL secondary surface, nothing to destroy\n", EMUSCV_NAME);

		// Free SDL main frame renderer
		if (frame_renderer != NULL)
		{
			SDL_DestroyRenderer(frame_renderer);
			frame_renderer = NULL;
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface renderer destroyed\n", EMUSCV_NAME);
		}
		else
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL main surface renderer, nothing to destroy\n", EMUSCV_NAME);

		// Free SDL main frame surface
		if (frame_surface != NULL)
		{
			SDL_FreeSurface(frame_surface);
			frame_surface = NULL;
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface destroyed\n", EMUSCV_NAME);
		}
		else
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] No SDL main surface, nothing to destroy\n", EMUSCV_NAME);

		// Create SDL main frame surface
		frame_surface = SDL_CreateRGBSurface(0, config.window_width, config.window_height, 8*sizeof(uint32_t), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
		if (frame_surface == NULL)
		{
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
			return;
		}
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface created\n", EMUSCV_NAME);

		// Create SDL main frame renderer
		frame_renderer = SDL_CreateSoftwareRenderer(frame_surface);
		if (frame_renderer == NULL)
		{
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface renderer creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
			return;
		}
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface renderer created\n", EMUSCV_NAME);

		// Create SDL TV static noise surface
		noise_surface = SDL_CreateRGBSurface(0, config.window_width >> 2, config.window_height >> 2, 8*sizeof(uint32_t), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
		if (noise_surface == NULL)
		{
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
			return;
		}
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface created\n", EMUSCV_NAME);

		// Create SDL TV static noise renderer
		noise_renderer = SDL_CreateSoftwareRenderer(noise_surface);
		if (noise_renderer == NULL)
		{
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface renderer creation failed! %s\n", EMUSCV_NAME, SDL_GetError());
			return;
		}
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL secondary surface created\n", EMUSCV_NAME);
	}

	// Get inputs
	RetroInputPoll();
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Imputs polled\n", EMUSCV_NAME);

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
	keyPower = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_i);
	keyReset = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_r);
	keyPause = RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_LCTRL)
            || RetroInputState(port0, RETRO_DEVICE_KEYBOARD, 0, RETROK_RCTRL);
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Imputs parsed\n", EMUSCV_NAME);

	bool updated = false;

	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
	{
		RetroLoadSettings();
		config_changed = true;
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Settings (variables) loaded\n", EMUSCV_NAME);
	}
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Settings (variables) unchanged, already loaded\n", EMUSCV_NAME);

	if(escv_emu)
	{
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu exists\n", EMUSCV_NAME);

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
	else
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu don't exists\n", EMUSCV_NAME);	

	// Button SELECT
	// open configuration
	if ((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)))
	{
	}

	// Other buttons
	// open the console keyboard overlay
	if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L3)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))
	|| (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L3)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R3)))
	{
	}

	if(start_up_counter_power < 150)
	{
		// Auto power ON after 0,5 seconds
		start_up_counter_power += 300/config.window_fps;
		is_power_on = (start_up_counter_power >= 150);
	}
	else
	{
		// Key POWER ON/OFF
		if(keyPower != 0)
		{
			if (!key_pressed_power)
			{
				is_power_on = !is_power_on;
				if(is_power_on)
					RetroReset();
			}
			key_pressed_power = true;
		}
		else if(key_pressed_power)
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
	if (keyReset != 0)
	{
		key_pressed_reset = true;
		RetroReset();
	}
	else if(key_pressed_reset)
		key_pressed_reset = false;

    // Key PAUSE
	if (keyPause != 0)
	{
		if(escv_emu)
			escv_emu->key_down(VK_SPACE, false, key_pressed_pause);
		key_pressed_pause = true;
	}
	else if(key_pressed_pause)
	{
		if(escv_emu)
			escv_emu->key_up(VK_SPACE, false);
		key_pressed_pause = false;
	}

	// Key 0
	if (key0 != 0)
	{
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD0, false, key_pressed_0);
		key_pressed_0 = true;
	}
	else if(key_pressed_0)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD0, false);
		key_pressed_0 = false;
	}

	// Key 1
	if (key1 != 0)
	{
//		if(!key_pressed_1)
//		{
//			config.scv_display = SETTING_DISPLAY_EMUSCV_VAL;
//			config_changed = true;
//		}
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD1, false, key_pressed_1);
		key_pressed_1 = true;
	}
	else if(key_pressed_1)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD1, false);
		key_pressed_1 = false;
	}

	// Key 2
	if (key2 != 0)
	{
//		config.scv_display = SETTING_DISPLAY_EPOCH_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD2, false, key_pressed_2);
		key_pressed_2 = true;
	}
	else if(key_pressed_2)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD2, false);
		key_pressed_2 = false;
	}

	// Key 3
	if (key3 != 0)
	{
//		config.scv_display = SETTING_DISPLAY_YENO_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD3, false, key_pressed_3);
		key_pressed_3 = true;
	}
	else if(key_pressed_3)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD3, false);
		key_pressed_3 = false;
	}

	// Key 4
	if (key4 != 0)
	{
//		config.scv_displayfull = SETTING_DISPLAYFULL_NO_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD4, false, key_pressed_4);
		key_pressed_4 = true;
	}
	else if(key_pressed_4)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD4, false);
		key_pressed_4 = false;
	}

	// Key 5
	if (key5 != 0)
	{
//		config.scv_displayfull = SETTING_DISPLAYFULL_YES_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD5, false, key_pressed_5);
		key_pressed_5 = true;
	}
	else if(key_pressed_5)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD5, false);
		key_pressed_5 = false;
	}

	// Key 6
	if (key6 != 0)
	{
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD6, false, key_pressed_6);
		key_pressed_6 = true;
	}
	else if(key_pressed_6)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD6, false);
		key_pressed_6 = false;
	}

	// Key 7
	if (key7 != 0)
	{
//		config.scv_displayfps = SETTING_DISPLAYFPS_EPOCH60_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD7, false, key_pressed_7);
		key_pressed_7 = true;
	}
	else if(key_pressed_7)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD7, false);
		key_pressed_7 = false;
	}

	// Key 8
	if (key8 != 0)
	{
//		config.scv_displayfps = SETTING_DISPLAYFPS_YENO50_VAL;
//		config_changed = true;
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD8, false, key_pressed_8);
		key_pressed_8 = true;
	}
	else if(key_pressed_8)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD8, false);
		key_pressed_8 = false;
	}

	// Key 9
	if (key9 != 0)
	{
		if(escv_emu)
			escv_emu->key_down(VK_NUMPAD9, false, key_pressed_9);
		key_pressed_9 = true;
	}
	else if(key_pressed_9)
	{
		if(escv_emu)
			escv_emu->key_up(VK_NUMPAD9, false);
		key_pressed_9 = false;
	}

	// Key CL
	if (keyClear != 0)
	{
		if(escv_emu)
			escv_emu->key_down(VK_BACK, false, key_pressed_cl);
		key_pressed_cl = true;
	}
	else if(key_pressed_cl)
	{
		if(escv_emu)
			escv_emu->key_up(VK_BACK, false);
		key_pressed_cl = false;
	}

	// Key EN
	// Controllers 1 & 2, Button START
	if (keyEnter != 0 || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)))
	{
		if(escv_emu)
			escv_emu->key_down(VK_RETURN, false, key_pressed_en);
		key_pressed_en = true;
	}
	else if(key_pressed_en)
	{
		if(escv_emu)
			escv_emu->key_up(VK_RETURN, false);
		key_pressed_en = false;
	}

	if(config_changed)
	{
		apply_display_config();
		RetroSaveSettings();
	}

	if (config.window_width != window_width_old || config.window_height != window_height_old || config.window_fps != window_fps_old)
	{
		struct retro_system_av_info av_info;
		retro_get_system_av_info(&av_info);
		av_info.timing.fps = config.window_fps;
		RetroEnvironment(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &av_info);

		window_width_old = config.window_width;
		window_height_old = config.window_height;
		window_fps_old = config.window_fps;

		// Paint it black
		SDL_SetRenderDrawColor(frame_renderer, 0, 0, 0, 255);
		SDL_RenderClear(frame_renderer);
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL main surface cleared\n", EMUSCV_NAME);
	}

	// Increment the frame counter
	retro_frame_counter++;

	if(is_power_on && escv_emu)
	{
		// drive machine
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu->run()\n", EMUSCV_NAME);
		run_frames += escv_emu->run();

		// update window if enough time
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] escv_emu->draw_screen()\n", EMUSCV_NAME);
		draw_frames += escv_emu->draw_screen();

		// Copy screen
		SDL_Rect RectSrc;
		RectSrc.x = config.draw_x;
		RectSrc.y = config.draw_y;
		RectSrc.w = config.draw_width;
		RectSrc.h = config.draw_height;
		SDL_SetSurfaceBlendMode(escv_emu->get_osd()->get_vm_screen_buffer()->frame_surface, SDL_BLENDMODE_NONE);
		if(SDL_BlitScaled(escv_emu->get_osd()->get_vm_screen_buffer()->frame_surface, &RectSrc, frame_surface, NULL) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled emu_scv->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
		else
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled emu_scv->main ok\n", EMUSCV_NAME);
	}
	else
	{
		SDL_SetRenderDrawColor(frame_renderer, 0, 0, 0, 255);
		SDL_RenderClear(frame_renderer);
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL main surface cleared\n", EMUSCV_NAME);

		for(int y = 0; y < noise_surface->h; y++)
			for(int x = 0; x < noise_surface->w; x++)
			{
				uint8_t noise_color = rand() % 255;
			    pixelRGBA(noise_renderer, x, y, noise_color, noise_color, noise_color, 255);
			}
		RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL secondary surface drawn\n", EMUSCV_NAME);
		SDL_SetSurfaceBlendMode(frame_surface, SDL_BLENDMODE_NONE);
		if(SDL_BlitScaled(noise_surface, NULL, frame_surface, NULL) != 0)
			RetroLogPrintf(RETRO_LOG_ERROR, "[%s] SDL blit scaled secondary->main failed! %s\n", EMUSCV_NAME, SDL_GetError());
		else
			RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] SDL blit scaled secondary->main ok\n", EMUSCV_NAME);
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
	if(start_up_counter_logo < 1200)
	{
		start_up_counter_logo += 300/config.window_fps;
		if(start_up_counter_logo > 1200)
			start_up_counter_logo = 1200;
	}
	if(start_up_counter_logo < 1200)
	{
		// Draw an embedded binary image (64x97 pixels in ARGB8888 format)
		posx = config.window_width - 75;
		posy = config.window_height - 80;
		sizx = 64;
		sizy = 64;
		uint8_t alpha = (start_up_counter_logo < 1200-100 ? 255: 255*(1200-start_up_counter_logo)/100);
		for (uint32_t y = 0; y < sizy; y++)
			for (uint32_t x = 0; x < sizx; x++)
				pixelRGBA(frame_renderer, posx+x, posy+y, src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)], src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)+1], src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)+2], alpha*src_res_emuscv64x64xrgba8888_data[4*(x+y*sizx)+3]/255);
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Logo drawn\n", EMUSCV_NAME);
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

	// Display controls over picture?
	if(config.scv_displayinputs)
	{
		// FIRST CONTROLLER (LEFT SIDE, ORANGE)
		posx = 10;
		posy = config.window_height-247-10;
		// Contour
		color = palette_pc[1];	// Black
		rectangleRGBA(frame_renderer, posx+29, posy, posx+60, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// L3
		rectangleRGBA(frame_renderer, posx+29, posy+32, posx+60, posy+63, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// L2
		rectangleRGBA(frame_renderer, posx+29, posy+64, posx+60, posy+95, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// L1
		rectangleRGBA(frame_renderer, posx+29, posy+96, posx+60, posy+126, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// UP
		rectangleRGBA(frame_renderer, posx, posy+125, posx+30, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// LEFT
		rectangleRGBA(frame_renderer, posx+59, posy+125, posx+89, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT
		rectangleRGBA(frame_renderer, posx+29, posy+155, posx+60, posy+185, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// DOWN
		rectangleRGBA(frame_renderer, posx+14, posy+186, posx+75, posy+247, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT STICK
		rectangleRGBA(frame_renderer, posx+90, posy+125, posx+121, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// SELECT
		rectangleRGBA(frame_renderer, posx+122, posy+125, posx+153, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// START
		rectangleRGBA(frame_renderer, posx+154, posy+125, posx+185, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 4
		rectangleRGBA(frame_renderer, posx+212, posy+125, posx+243, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 1
		rectangleRGBA(frame_renderer, posx+168, posy+186, posx+229, posy+247, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT STICK
		rectangleRGBA(frame_renderer, posx+183, posy+154, posx+214, posy+185, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 2
		rectangleRGBA(frame_renderer, posx+183, posy+96, posx+214, posy+127, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 3
		rectangleRGBA(frame_renderer, posx+183, posy+64, posx+214, posy+95, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R1
		rectangleRGBA(frame_renderer, posx+183, posy+32, posx+214, posy+63, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R2
		rectangleRGBA(frame_renderer, posx+183, posy, posx+214, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// R3
		color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+30, posy+126, posx+58, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// Cross center
		boxRGBA(frame_renderer, posx+15, posy+187, posx+73, posy+245, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// LEFT STICK
		boxRGBA(frame_renderer, posx+169, posy+187, posx+227, posy+245, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// RIGHT STICK
		color = palette_pc[1];	// Black
		lineRGBA(frame_renderer, posx+44-15, posy+216, posx+44+15, posy+216, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT STICK
		lineRGBA(frame_renderer, posx+44, posy+216-15, posx+44, posy+216+15, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT STICK
		lineRGBA(frame_renderer, posx+198-15, posy+216, posx+198+15, posy+216, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT STICK
		lineRGBA(frame_renderer, posx+198, posy+216-15, posx+198, posy+216+15, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT STICK
		// Button L3
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L3))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[6];	// Cyan
		boxRGBA(frame_renderer, posx+30, posy+1, posx+58, posy+29, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button L2
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[2];	// Blue Light
		boxRGBA(frame_renderer, posx+30, posy+33, posx+58, posy+61, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button L1
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[0];	// Blue medium
		boxRGBA(frame_renderer, posx+30, posy+65, posx+58, posy+93, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button UP
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
			color = palette_pc[4];	// Green light
		else if(analoglefty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty0 <= EMUSCV_AXIS_NEUTRAL_MIN)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+30, posy+97, posx+58, posy+125, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button LEFT
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
			color = palette_pc[4];	// Green light
		else if(analogleftx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx0 <= EMUSCV_AXIS_NEUTRAL_MIN)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+126, posx+29, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button RIGHT
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
			color = palette_pc[4];	// Green light
		else if(analogleftx0 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrightx0 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+59, posy+126, posx+87, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button DOWN
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
			color = palette_pc[4];	// Green light
		else if(analoglefty0 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrighty0 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+30, posy+155, posx+58, posy+183, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Left analog stick
		if(analogleftx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogleftx0 >= EMUSCV_AXIS_NEUTRAL_MAX || analoglefty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analoglefty0 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[15];	// White
		joyposx = (analogleftx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogleftx0 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogleftx0 : 0);
		joyposy = (analoglefty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analoglefty0 >= EMUSCV_AXIS_NEUTRAL_MAX ? analoglefty0 : 0);
		boxRGBA(frame_renderer, posx+44-3+26*joyposx/INT16_MAX, posy+216-3+26*joyposy/INT16_MAX, posx+44+3+26*joyposx/INT16_MAX, posy+216+3+26*joyposy/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button SELECT
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[13];	// Green dark
		boxRGBA(frame_renderer, posx+91, posy+126, posx+119, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button START
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[5];	// Green soft
		boxRGBA(frame_renderer, posx+123, posy+126, posx+151, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 4
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[2];	// Blue light
		boxRGBA(frame_renderer, posx+155, posy+126, posx+183, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 1
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[8];	// Red
		boxRGBA(frame_renderer, posx+213, posy+126, posx+241, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Right analog stick
		if(analogrightx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx0 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrighty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty0 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[15];	// White
		joyposx = (analogrightx0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx0 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogrightx0 : 0);
		joyposy = (analogrighty0 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty0 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogrighty0 : 0);
		boxRGBA(frame_renderer, posx+198-3+26*joyposx/INT16_MAX, posy+216-3+26*joyposy/INT16_MAX, posx+198+3+26*joyposx/INT16_MAX, posy+216+3+26*joyposy/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 2
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_B))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[7];	// Green medium
		boxRGBA(frame_renderer, posx+184, posy+155, posx+212, posy+183, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 3
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[9];	// Orange
		boxRGBA(frame_renderer, posx+184, posy+97, posx+212, posy+125, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R1
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_A)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[3];	// Purple
		boxRGBA(frame_renderer, posx+184, posy+65, posx+212, posy+93, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R2
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R2))
			color = palette_pc[4];	// Green light
		else if((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[10];	// Fuschia
		boxRGBA(frame_renderer, posx+184, posy+33, posx+212, posy+61, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R3
		if(ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[11];	// Pink
		boxRGBA(frame_renderer, posx+184, posy+1, posx+212, posy+29, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);


		// SECOND CONTROLLER (RIGHT SIDE, BLUE)
		posx = config.window_width-243-10;
		posy = config.window_height-247-10;
		// Contour
		color = palette_pc[1];	// Black
		rectangleRGBA(frame_renderer, posx+29, posy, posx+60, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// L3
		rectangleRGBA(frame_renderer, posx+29, posy+32, posx+60, posy+63, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// L2
		rectangleRGBA(frame_renderer, posx+29, posy+64, posx+60, posy+95, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// L1
		rectangleRGBA(frame_renderer, posx+29, posy+96, posx+60, posy+126, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// UP
		rectangleRGBA(frame_renderer, posx, posy+125, posx+30, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// LEFT
		rectangleRGBA(frame_renderer, posx+59, posy+125, posx+89, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT
		rectangleRGBA(frame_renderer, posx+29, posy+155, posx+60, posy+185, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// DOWN
		rectangleRGBA(frame_renderer, posx+14, posy+186, posx+75, posy+247, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT STICK
		rectangleRGBA(frame_renderer, posx+90, posy+125, posx+121, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// SELECT
		rectangleRGBA(frame_renderer, posx+122, posy+125, posx+153, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// START
		rectangleRGBA(frame_renderer, posx+154, posy+125, posx+185, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 4
		rectangleRGBA(frame_renderer, posx+212, posy+125, posx+243, posy+156, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 1
		rectangleRGBA(frame_renderer, posx+168, posy+186, posx+229, posy+247, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT STICK
		rectangleRGBA(frame_renderer, posx+183, posy+154, posx+214, posy+185, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 2
		rectangleRGBA(frame_renderer, posx+183, posy+96, posx+214, posy+127, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// BUTTON 3
		rectangleRGBA(frame_renderer, posx+183, posy+64, posx+214, posy+95, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R1
		rectangleRGBA(frame_renderer, posx+183, posy+32, posx+214, posy+63, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// R2
		rectangleRGBA(frame_renderer, posx+183, posy, posx+214, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// R3
		color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+30, posy+126, posx+58, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// Cross center
		boxRGBA(frame_renderer, posx+15, posy+187, posx+73, posy+245, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);			// LEFT STICK
		boxRGBA(frame_renderer, posx+169, posy+187, posx+227, posy+245, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);		// RIGHT STICK
		color = palette_pc[1];	// Black
		lineRGBA(frame_renderer, posx+44-15, posy+216, posx+44+15, posy+216, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT STICK
		lineRGBA(frame_renderer, posx+44, posy+216-15, posx+44, posy+216+15, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// LEFT STICK
		lineRGBA(frame_renderer, posx+198-15, posy+216, posx+198+15, posy+216, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT STICK
		lineRGBA(frame_renderer, posx+198, posy+216-15, posx+198, posy+216+15, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);	// RIGHT STICK
		// Button L3
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L3))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[6];	// Cyan
		boxRGBA(frame_renderer, posx+30, posy+1, posx+58, posy+29, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button L2
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[2];	// Blue Light
		boxRGBA(frame_renderer, posx+30, posy+33, posx+58, posy+61, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button L1
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[0];	// Blue medium
		boxRGBA(frame_renderer, posx+30, posy+65, posx+58, posy+93, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button UP
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
			color = palette_pc[4];	// Green light
		else if(analoglefty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty1 <= EMUSCV_AXIS_NEUTRAL_MIN)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+30, posy+97, posx+58, posy+125, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button LEFT
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
			color = palette_pc[4];	// Green light
		else if(analogleftx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx1 <= EMUSCV_AXIS_NEUTRAL_MIN)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+126, posx+29, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button RIGHT
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
			color = palette_pc[4];	// Green light
		else if(analogleftx1 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrightx1 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+59, posy+126, posx+87, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button DOWN
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
			color = palette_pc[4];	// Green light
		else if(analoglefty1 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrighty1 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+30, posy+155, posx+58, posy+183, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Left analog stick
		if (analogleftx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogleftx1 >= EMUSCV_AXIS_NEUTRAL_MAX || analoglefty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analoglefty1 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[15];	// White
		joyposx = (analogleftx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogleftx1 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogleftx1 : 0);
		joyposy = (analoglefty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analoglefty1 >= EMUSCV_AXIS_NEUTRAL_MAX ? analoglefty1 : 0);
		boxRGBA(frame_renderer, posx+44-3+26*joyposx/INT16_MAX, posy+216-3+26*joyposy/INT16_MAX, posx+44+3+26*joyposx/INT16_MAX, posy+216+3+26*joyposy/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button SELECT
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[13];	// Green dark
		boxRGBA(frame_renderer, posx+91, posy+126, posx+119, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button START
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[5];	// Green soft
		boxRGBA(frame_renderer, posx+123, posy+126, posx+151, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 4
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[2];	// Blue light
		boxRGBA(frame_renderer, posx+155, posy+126, posx+183, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 1
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[8];	// Red
		boxRGBA(frame_renderer, posx+213, posy+126, posx+241, posy+154, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Right analog stick
		if (analogrightx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx1 >= EMUSCV_AXIS_NEUTRAL_MAX || analogrighty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty1 >= EMUSCV_AXIS_NEUTRAL_MAX)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[15];	// White
		joyposx = (analogrightx1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrightx1 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogrightx1 : 0);
		joyposy = (analogrighty1 <= EMUSCV_AXIS_NEUTRAL_MIN || analogrighty1 >= EMUSCV_AXIS_NEUTRAL_MAX ? analogrighty1 : 0);
		boxRGBA(frame_renderer, posx+198-3+26*joyposx/INT16_MAX, posy+216-3+26*joyposy/INT16_MAX, posx+198+3+26*joyposx/INT16_MAX, posy+216+3+26*joyposy/INT16_MAX, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 2
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_B))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[7];	// Green medium
		boxRGBA(frame_renderer, posx+184, posy+155, posx+212, posy+183, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button 3
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[9];	// Orange
		boxRGBA(frame_renderer, posx+184, posy+97, posx+212, posy+125, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R1
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_A)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[3];	// Purple
		boxRGBA(frame_renderer, posx+184, posy+65, posx+212, posy+93, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R2
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R2))
			color = palette_pc[4];	// Green light
		else if((ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_X)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_L2)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[10];	// Fuschia
		boxRGBA(frame_renderer, posx+184, posy+33, posx+212, posy+61, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Button R3
		if (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_R3))
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[11];	// Pink
		boxRGBA(frame_renderer, posx+184, posy+1, posx+212, posy+29, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);


		// KEYBOARD
		posx = config.window_width-(74+64+31)-10;
		posy = 10;
		// Contour
		color = palette_pc[1];	// Black
		rectangleRGBA(frame_renderer, posx, posy, posx+73, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);                      // POWER ON/OFF BUTTON
		rectangleRGBA(frame_renderer, posx, posy+48, posx+73, posy+48+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);                // RESET BUTTON
		rectangleRGBA(frame_renderer, posx, posy+96, posx+73, posy+96+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);                // PAUSE BUTTON
		rectangleRGBA(frame_renderer, posx+74, posy, posx+74+31, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74+32, posy, posx+74+32+31, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74+64, posy, posx+74+64+31, posy+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74, posy+32, posx+74+31, posy+32+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74+32, posy+32, posx+74+32+31, posy+32+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74+64, posy+32, posx+74+64+31, posy+32+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74, posy+64, posx+74+31, posy+64+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74+32, posy+64, posx+74+32+31, posy+64+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74+64, posy+64, posx+74+64+31, posy+64+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74, posy+96, posx+74+31, posy+96+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74+32, posy+96, posx+74+32+31, posy+96+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		rectangleRGBA(frame_renderer, posx+74+64, posy+96, posx+74+64+31, posy+96+31, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key POWER ON/OFF
		if (keyPower != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+1, posx+1+70, posy+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		color = palette_pc[1];	// Black
		if (is_power_on)
			rectangleRGBA(frame_renderer, posx+43, posy+1, posx+72, posy+30, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		else
			rectangleRGBA(frame_renderer, posx+1, posy+1, posx+30, posy+30, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key RESET
		if (keyReset != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+48+1, posx+1+70, posy+48+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key PAUSE
		if (keyPause != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+1, posy+97, posx+1+70, posy+97+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 7
		if (key7 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+1, posy+1, posx+74+1+28, posy+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 8
		if (key8 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+33, posy+1, posx+74+33+28, posy+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 9
		if (key9 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+65, posy+1, posx+74+65+28, posy+1+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 4
		if (key4 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+1, posy+33, posx+74+1+28, posy+33+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 5
		if (key5 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+33, posy+33, posx+74+33+28, posy+33+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 6
		if (key6 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+65, posy+33, posx+74+65+28, posy+33+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 1
		if (key1 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+1, posy+65, posx+74+1+28, posy+65+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 2
		if (key2 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+33, posy+65, posx+74+33+28, posy+65+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 3
		if (key3 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+65, posy+65, posx+74+65+28, posy+65+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key 0
		if (key0 != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+1, posy+97, posx+74+1+28, posy+97+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key CL
		if (keyClear != 0)
			color = palette_pc[4];	// Green light
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+33, posy+97, posx+74+33+28, posy+97+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);
		// Key EN
		if (keyEnter != 0)
			color = palette_pc[4];	// Green light
		else if ((ret0 & (1 << RETRO_DEVICE_ID_JOYPAD_START)) || (ret1 & (1 << RETRO_DEVICE_ID_JOYPAD_START)))
			color = palette_pc[0xc];	// Yellow
		else
			color = palette_pc[14];	// Gray
		boxRGBA(frame_renderer, posx+74+65, posy+97, posx+74+65+28, posy+97+28, R_OF_COLOR(color), G_OF_COLOR(color), B_OF_COLOR(color), alpha);

		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] Inputs drawn\n", EMUSCV_NAME);
	}

/*
	// Call audio callback manually if not set
	if (retro_use_audio_cb == false)
	{
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] retro_use_audio_cb == false -> RetroAudioCb()\n", EMUSCV_NAME);
		RetroAudioCb();
	}
	else
	{
		RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] retro_use_audio_cb == true => nothing to do\n", EMUSCV_NAME);
*/
int16_t sound_buffer[2*44100/WINDOW_FPS_EPOCH];
for (uint32_t i = 0; i < 2*44100/WINDOW_FPS_EPOCH; i++)
	sound_buffer[i] = 0;
RetroAudioSampleBatch(sound_buffer, sizeof(sound_buffer));
RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] => RetroAudioSampleBatch()\n", EMUSCV_NAME);
/*
	}
*/
	// Call video callback
	RetroVideoRefresh(frame_surface->pixels, config.window_width, config.window_height,  config.window_width*sizeof(uint32_t));
//RetroVideoRefresh(test_video_buffer, WINDOW_WIDTH_EMUSCV, WINDOW_HEIGHT_EMUSCV,  WINDOW_WIDTH_EMUSCV*sizeof(uint32_t));
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] RetroVideoRefresh()\n", EMUSCV_NAME);
}

//
// Libretro: load core settings
//
void cEmuSCV::RetroLoadSettings(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroLoadSettings()\n", EMUSCV_NAME);

	struct retro_variable var;
	
	// CONSOLE
	config.scv_console	= SETTING_CONSOLE_AUTO_VAL;
	var.key		= SETTING_CONSOLE_KEY;
	var.value	= NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
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
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Show console as EPOCH\n", EMUSCV_NAME);
			break;
		case SETTING_CONSOLE_YENO_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Show console as YENO\n", EMUSCV_NAME);
			break;
		case SETTING_CONSOLE_EPOCHLADY_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Show console as EPOCH LADY\n", EMUSCV_NAME);
			break;
		case SETTING_CONSOLE_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Show console as AUTO (EPOCH)\n", EMUSCV_NAME);
	}

	// DISPLAY
	config.scv_display	= SETTING_DISPLAY_AUTO_VAL;
	var.key		= SETTING_DISPLAY_KEY;
	var.value	= NULL;
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
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display picture as EmuSCV (custom)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAY_EPOCH_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display picture as EPOCH (genuine)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAY_YENO_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display picture as YENO (genuine)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAY_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display picture as AUTO (EmuSCV)\n", EMUSCV_NAME);
	}

	// DISPLAY FPS
	config.scv_displayfps	= SETTING_DISPLAYFPS_AUTO_VAL;
	var.key		= SETTING_DISPLAYFPS_KEY;
	var.value	= NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_DISPLAYFPS_EPOCH60_KEY)) == 0)
			config.scv_displayfps = SETTING_DISPLAYFPS_EPOCH60_VAL;
		else if(strcmp(str, _T(SETTING_DISPLAYFPS_YENO50_KEY)) == 0)
			config.scv_displayfps = SETTING_DISPLAYFPS_YENO50_VAL;
	}
	switch(config.scv_displayfps)
	{
		case SETTING_DISPLAYFPS_EPOCH60_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display set as 60Hz / 60 FPS (EPOCH)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAYFPS_YENO50_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display set as 50Hz / 50 FPS (YENO)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAYFPS_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display Frequency / FPS set as AUTO (depending of display)\n", EMUSCV_NAME);
	}

	// DISPLAY FULL
	config.scv_displayfull	= SETTING_DISPLAYFULL_NO_VAL;
	var.key		= SETTING_DISPLAYFULL_KEY;
	var.value	= NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_DISPLAYFULL_YES_KEY)) == 0)
			config.scv_displayfull = SETTING_DISPLAYFULL_YES_VAL;
	}
	switch(config.scv_displayfull)
	{
		case SETTING_DISPLAYFULL_YES_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display entire video memory (developer mode)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAYFULL_NO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Normal display (user mode)\n", EMUSCV_NAME);
	}

	// DISPLAY INPUTS
	config.scv_displayinputs	= SETTING_DISPLAYINPUTS_NO_VAL;
	var.key		= SETTING_DISPLAYINPUTS_KEY;
	var.value	= NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_DISPLAYINPUTS_YES_KEY)) == 0)
			config.scv_displayinputs = SETTING_DISPLAYINPUTS_YES_VAL;
	}
	switch(config.scv_displayinputs)
	{
		case SETTING_DISPLAYINPUTS_YES_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Display inputs over picture (developer mode)\n", EMUSCV_NAME);
			break;
		case SETTING_DISPLAYINPUTS_NO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Don't display inputs (user mode)\n", EMUSCV_NAME);
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
		else if(strcmp(str, _T(SETTING_LANGAGE_EN_KEY)) == 0)
			config.scv_langage = SETTING_LANGAGE_EN_VAL;
		else if(strcmp(str, _T(SETTING_LANGAGE_FR_KEY)) == 0)
			config.scv_langage = SETTING_LANGAGE_FR_VAL;
	}
	switch(config.scv_langage)
	{
		case SETTING_LANGAGE_JP_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Langage set as JP (EPOCH)\n", EMUSCV_NAME);
			break;
		case SETTING_LANGAGE_EN_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Langage set as EN (YENO)\n", EMUSCV_NAME);
			break;
		case SETTING_LANGAGE_FR_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Langage set as FR\n", EMUSCV_NAME);
			break;
		case SETTING_LANGAGE_AUTO_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Langage set as AUTO (depending of console)\n", EMUSCV_NAME);
	}

	// CHECK BIOS
	config.scv_checkbios	= SETTING_CHECKBIOS_YES_VAL;
	var.key		= SETTING_CHECKBIOS_KEY;
	var.value	= NULL;
	if (RetroEnvironment(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		_TCHAR str[sizeof(var.value)];
		memset(str, 0, sizeof(str));
		strncpy(str, var.value, sizeof(str));
		if(strcmp(str, _T(SETTING_CHECKBIOS_NO_KEY)) == 0)
			config.scv_checkbios = SETTING_CHECKBIOS_NO_VAL;
	}
	switch(config.scv_checkbios)
	{
		case SETTING_CHECKBIOS_NO_VAL:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Don't check BIOS\n", EMUSCV_NAME);
			break;
		case SETTING_CHECKBIOS_YES_VAL:
		default:
			RetroLogPrintf(RETRO_LOG_INFO, "[%s] Check BIOS\n", EMUSCV_NAME);
	}

	apply_display_config();
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] apply_display_config()\n", EMUSCV_NAME);
}

//
// Libretro: save core settings
//
void cEmuSCV::RetroSaveSettings(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSaveSettings()\n", EMUSCV_NAME);

	struct retro_variable variable[8]; 

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
	
	variable[2].key = SETTING_DISPLAYFPS_KEY;
	switch(config.scv_displayfps)
	{
		case SETTING_DISPLAYFPS_EPOCH60_VAL:
			variable[2].value = "DISPLAY FPS; EPOCH_60";
			break;
		case SETTING_DISPLAYFPS_YENO50_VAL:
			variable[2].value = "DISPLAY FPS; YENO_50";
			break;
		case SETTING_DISPLAYFPS_AUTO_VAL:
		default:
			variable[2].value = "DISPLAY FPS; AUTO";
			break;
	}

	variable[3].key = SETTING_DISPLAYFULL_KEY;
	if(config.scv_displayfull == SETTING_DISPLAYFULL_YES_VAL)
		variable[3].value = "DISPLAY FULL; YES";
	else
		variable[3].value = "DISPLAY FULL; NO";
	
	variable[4].key = SETTING_DISPLAYINPUTS_KEY;
	if(config.scv_displayinputs == SETTING_DISPLAYINPUTS_YES_VAL)
		variable[4].value = "DISPLAY INPUTS; YES";
	else
		variable[4].value = "DISPLAY INPUTS; NO";
	
	variable[5].key = SETTING_LANGAGE_KEY;
	switch(config.scv_langage)
	{
		case SETTING_LANGAGE_JP_VAL:
			variable[5].value = "LANGAGE; JP";
			break;
		case SETTING_LANGAGE_EN_VAL:
			variable[5].value = "LANGAGE; EN";
			break;
		case SETTING_LANGAGE_FR_VAL:
			variable[5].value = "LANGAGE; FR";
			break;
		case SETTING_LANGAGE_AUTO_VAL:
		default:
			variable[5].value = "LANGAGE; AUTO";
			break;
	}
	
	variable[6].key = SETTING_CHECKBIOS_KEY;
	if(config.scv_checkbios == SETTING_CHECKBIOS_NO_VAL)
		variable[6].value = "CHECK BIOS; NO";
	else
		variable[6].value = "CHECK BIOS; YES";

	variable[7].key = NULL;
	variable[7].value = NULL;

	RetroEnvironment(RETRO_ENVIRONMENT_SET_VARIABLES, variable);

	// Set possible values
	variable[0].value = "CONSOLE; AUTO|EPOCH|YENO|EPOCHLADY";
	variable[1].value = "DISPLAY; AUTO|EMUSCV|EPOCH|YENO";
	variable[2].value = "DISPLAY FPS; AUTO|EPOCH_60|YENO_50";
	variable[3].value = "DISPLAY FULL; NO|YES";
	variable[4].value = "DISPLAY INPUTS; NO|YES";
	variable[5].value = "LANGAGE; AUTO|JP|EN|FR";
	variable[6].value = "CHECK BIOS; YES|NO";
	RetroEnvironment(RETRO_ENVIRONMENT_SET_VARIABLES, variable);
}

//
// Libretro: reset the Libretro core
//
void cEmuSCV::RetroReset(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroReset()\n", EMUSCV_NAME);
	if(escv_emu)
		escv_emu->reset();
}

// 
// Libretro: return save state size
// 
size_t cEmuSCV::RetroSaveStateSize(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSaveStateSize()\n", EMUSCV_NAME);

	return 0;
}

// 
// Libretro: return save state data pointer
// 
void *cEmuSCV::RetroSaveStateData(void)
{
	// Log
	RetroLogPrintf(RETRO_LOG_DEBUG, "[%s] ================================================================================\n", EMUSCV_NAME);
	RetroLogPrintf(RETRO_LOG_INFO, "[%s] cEmuSCV::RetroSaveStateData()\n", EMUSCV_NAME);

	return NULL;
}
