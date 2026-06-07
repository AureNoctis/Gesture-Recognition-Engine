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


// ====================  global variable  ====================
static bool globalRunning = false;
static offscrean_buffer globalBackBuffer;

static InputReportInfo globalInputReportInfo;

static RAWINPUT* globalRawInput;
static Finger finger_data[5];
static TouchPad_state t_state;

static bool gesture_start = false;
static bool gesture_end   = false;
static int gesture_start_counter = 0;


#include "utils/Holder.cpp"
#include "utils/utils.cpp"
#include "core/render.cpp"
#include "input/raw_input.cpp"
#include "input/touchpad.cpp"
#include "core/window.cpp"




int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow){

    (void)prevInstance;
    (void)cmdLine;
    (void)cmdShow;

    summonConsole();
    WNDCLASS windowClass = {};
    windowClass.style = CS_VREDRAW | CS_HREDRAW;
    windowClass.lpfnWndProc = mainWindowCallback;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = L"G.R.E";

    if (RegisterClass(&windowClass)) {
      HWND window = CreateWindowEx(
          0, windowClass.lpszClassName, L"gesture recognition engine",
          WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 200,
          200, 0, 0, instance, 0);
      if (window) {

        RAWINPUTDEVICE rid[1];

        rid[0].usUsagePage = 0x000D;
        rid[0].usUsage = 0x0005;
        rid[0].dwFlags = RIDEV_INPUTSINK;
        rid[0].hwndTarget = window;

        RegisterRawInputDevices(rid, 1, sizeof(rid[0]));

        HDC deviceContext = GetDC(window);
        resizeDIBSection(&globalBackBuffer, 200, 200);
        globalRunning = true;

        while (globalRunning) {
          renderWeirdGradiant(&globalBackBuffer, 0, 0);

          MSG message;
          while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT)
              globalRunning = false;

            TranslateMessage(&message);
            DispatchMessage(&message);
          }
          //! WM_QUIT:
          // It is NOT sent to your window procedure
          // It is NOT dispatched via DispatchMessage
          // It is only seen by GetMessage / PeekMessage

          window_dimension dimension = getWindowDimensions(window);
          updateWindow(deviceContext, dimension.width, dimension.height,
                             &globalBackBuffer);
        }
        ReleaseDC(window, deviceContext);
      } else {
      }
    }else{}


    free(globalRawInput);
    free(globalInputReportInfo.ptrPreparsedData);
    free(globalInputReportInfo.pLinkCollection);
    free(globalInputReportInfo.pCaps);
    free(globalInputReportInfo.pValCaps);
    free(globalInputReportInfo.pButtonCaps);
}

