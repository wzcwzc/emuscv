#include "emuscv.h"

// emulation core
EMU* emu;

#ifdef USE_CART
/*
// TODO_MM
void open_cart_dialog(HWND hWnd, int drv);
void open_recent_cart(int drv, int index);
*/
#endif

#if defined(USE_CART) || defined(USE_FLOPPY_DISK) || defined(USE_HARD_DISK) || defined(USE_TAPE) || defined(USE_COMPACT_DISC) || defined(USE_LASER_DISC) || defined(USE_BINARY_FILE) || defined(USE_BUBBLE)
#define SUPPORT_DRAG_DROP
#endif

// dialog
#ifdef USE_SOUND_VOLUME
/*
// TODO_MM
BOOL CALLBACK VolumeWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
*/
#endif
#ifdef USE_JOYSTICK
/*
// TODO_MM
BOOL CALLBACK JoyWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK JoyToKeyWndProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
*/
#endif

#ifdef USE_STATE
const _TCHAR *state_file_path(int num)
{
	return create_local_path(_T("%s.sta%d"), _T(CONFIG_NAME), num);
}
#endif

// ----------------------------------------------------------------------------
// window main
// ----------------------------------------------------------------------------
/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow)
{

	// get os version
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO*)&osvi);
	win8_or_later = (osvi.dwPlatformId == 2 && (osvi.dwMajorVersion > 6 || (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 2)));

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
	int total_frames = 0, draw_frames = 0, skip_frames = 0;
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
			total_frames += run_frames;

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
						int ratio = (int)(100.0 * (double)total_frames / emu->get_frame_rate() + 0.5);
						SetWindowText(hWnd, create_string(_T("%s - Skip Frames (%d %%)"), _T(DEVICE_NAME), ratio));
					} else {
						int ratio = (int)(100.0 * (double)draw_frames / (double)total_frames + 0.5);
						SetWindowText(hWnd, create_string(_T("%s - %d fps (%d %%)"), _T(DEVICE_NAME), draw_frames, ratio));
					}
					update_fps_time += 1000;
					total_frames = draw_frames = 0;
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
		// thanks PC8801MA‰ü
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
		}
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
				emu->open_debugger(LOWORD(wParam) - ID_OPEN_DEBUGGER0);
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








cEmuSCV::cEmuSCV()
{
	int total_frames	= 0;
	int draw_frames		= 0;
	int skip_frames		= 0;
	DWORD next_time = 0;
	bool prev_skip = false;
	DWORD update_fps_time = 0;
	DWORD update_status_bar_time = 0;
	DWORD disable_screen_saver_time = 0;
//	MSG msg;




//	emu = new EMU(hWnd, hInstance);
	emu = new EMU();
	emu->set_host_window_size(WINDOW_WIDTH, WINDOW_HEIGHT, true);

	// main loop
	if(emu)
	{
/*
		// drive machine
		int run_frames = emu->run();
		total_frames += run_frames;

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
//					SetWindowText(hWnd, create_string(_T("%s - %s"), _T(DEVICE_NAME), emu->message));
					emu->message_count--;
				} else if(now_skip) {
					int ratio = (int)(100.0 * (double)total_frames / emu->get_frame_rate() + 0.5);
//					SetWindowText(hWnd, create_string(_T("%s - Skip Frames (%d %%)"), _T(DEVICE_NAME), ratio));
				} else {
					int ratio = (int)(100.0 * (double)draw_frames / (double)total_frames + 0.5);
//					SetWindowText(hWnd, create_string(_T("%s - %d fps (%d %%)"), _T(DEVICE_NAME), draw_frames, ratio));
				}
				update_fps_time += 1000;
				total_frames = draw_frames = 0;
			}
			update_fps_time = current_time + 1000;
		}

		// update status bar
//		if(update_status_bar_time <= current_time) {
//			if(hStatus != NULL && status_bar_visible) {
//				if(get_status_bar_updated()) {
//					SendMessage(hStatus, SB_SETTEXT, (WPARAM)0 | SBT_OWNERDRAW, (LPARAM)NULL);
//						InvalidateRect(hStatus, NULL, FALSE);
//				}
//			}
//			update_status_bar_time = current_time + 200;
//		}

		// disable screen saver
//		if (disable_screen_saver_time <= current_time)
//		{
//			SetThreadExecutionState(ES_DISPLAY_REQUIRED);
//			INPUT input[1];
//			ZeroMemory(input, sizeof(INPUT));
//			input[0].type = INPUT_MOUSE;
//			input[0].mi.dwFlags = MOUSEEVENTF_MOVE;
//			input[0].mi.dx = 0;
//			input[0].mi.dy = 0;
//			SendInput(1, input, sizeof(INPUT));
//			disable_screen_saver_time = current_time + 30000;
//		}
*/
	}
}

cEmuSCV::~cEmuSCV()
{

}
