//! ===================================================================    TODO    =============================================================
/*

*/
//! ============================================================================================================================================
// C/C++ Standard Library
#include <stdint.h>
#include <stdlib.h>

// Windows Basics
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Fix for hidpi.h (It needs NTSTATUS defined)
typedef long NTSTATUS;
#include <hidusage.h>
#include <hidpi.h>
#include <hidsdi.h>
#include <SetupAPI.h>

// Custom headers
#include "utils/declaration.h"


#pragma comment(lib, "hid.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Setupapi.lib")


// ============================================================

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow) {
	(void)prevInstance;
	(void)cmdLine;
	(void)cmdShow;


	summonConsole();
	WNDCLASS windowClass	  = {};
	windowClass.style		  = CS_VREDRAW | CS_HREDRAW;
	windowClass.lpfnWndProc	  = mainWindowCallback;
	windowClass.hInstance	  = instance;
	windowClass.lpszClassName = L"G.R.E";

	if (RegisterClass(&windowClass)) {
		HWND window = CreateWindowEx(0, windowClass.lpszClassName, L"gesture recognition engine", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
									 CW_USEDEFAULT, 200, 200, 0, 0, instance, 0);

		if (window) {
			Window_state* w_state = (Window_state*)init_window_state(window,/* .gesture_start = false, 
                                                                         .gesture_start_counter = 0 */);

			bool running = w_state->running;
			running		 = true;

			RAWINPUTDEVICE rid[1];
			rid[0].usUsagePage = 0x000D;
			rid[0].usUsage	   = 0x0005;
			rid[0].dwFlags	   = RIDEV_INPUTSINK;
			rid[0].hwndTarget  = window;

			RegisterRawInputDevices(rid, 1, sizeof(rid[0]));

			HDC deviceContext = GetDC(window);
			resizeDIBSection(&w_state->back_buffer, 200, 200);

			while (running) {
				renderWeirdGradiant(&w_state->back_buffer, 0, 0);

				MSG message;
				while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
					if (message.message == WM_QUIT)
						running = false;

					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				//! WM_QUIT:
				// It is NOT sent to your window procedure
				// It is NOT dispatched via DispatchMessage
				// It is only seen by GetMessage / PeekMessage

				window_dimension dimension = getWindowDimensions(window);
				updateWindow(deviceContext, dimension.width, dimension.height, &w_state->back_buffer);
			}
			ReleaseDC(window, deviceContext);
		} else {
		}
	} else {
	}
}


#include "utils/Holder.cpp"
#include "utils/utils.cpp"
#include "core/render.cpp"
#include "input/raw_input.cpp"
#include "input/touchpad.cpp"
#include "core/window.cpp"
