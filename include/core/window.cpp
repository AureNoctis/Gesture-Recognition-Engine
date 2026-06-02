#ifndef _WINDOW_
#define _WINDOW_


#include "utils/declaration.h"
#include "utils/usage.h"

extern Win32_offscrean_buffer globalBackBuffer;
extern Win32_InputReportInfo globalInputReportInfo;
extern RAWINPUT* globalRawInput;

extern Finger finger_data[5];
extern TouchPad_state t_state;

extern bool gesture_start;
extern bool gesture_end;


static Win32_window_dimension win32_getWindowDimensions(HWND window) {
    Win32_window_dimension dimension;

    RECT clientRect;
    GetClientRect(window, &clientRect);						// area in window where you can draw
    dimension.width = clientRect.right - clientRect.left;
    dimension.height = clientRect.bottom - clientRect.top;
    return dimension;
}


static LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {

        PAINTSTRUCT paint;
        HDC deviceContext = BeginPaint(window, &paint);

        Win32_window_dimension dimension = win32_getWindowDimensions(window);
        win32_updateWindow(deviceContext, dimension.width, dimension.height, &globalBackBuffer);

        EndPaint(window, &paint);
        return 0;
    }break;

    case WM_DESTROY:
    {
        PostQuitMessage(0); // put quit message in message loop
        return 0;
    } break;

    case WM_INPUT: {
        if (Win32_getRawData(lParam)) {
            globalInputReportInfo.deviceHandle = globalRawInput->header.hDevice;
            Win32_getInputReportInfo(&globalInputReportInfo);
        }

        // fill_payload()
        // fill_events_in_event_buffer(find_gesture(payload))  --> for all fingers connected

        win32_getFingerData(globalInputReportInfo.ptrPreparsedData, globalRawInput, finger_data, &t_state);

        if(gesture_start == true)
            printf("start\n");
        if(gesture_end == true)
            printf("end\n");


        // win32_printTouchpadData(finger_data, t_state);
        return DefWindowProc(window, message, wParam, lParam);
    } break;

    default: {
        return DefWindowProc(window, message, wParam, lParam);
    }break;
    }
}


#endif