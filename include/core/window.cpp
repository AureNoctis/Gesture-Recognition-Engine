#ifndef _WINDOW_
#define _WINDOW_


#include "utils/declaration.h"
#include "utils/usage.h"

extern offscrean_buffer globalBackBuffer;
extern InputReportInfo globalInputReportInfo;
extern RAWINPUT* globalRawInput;

extern Finger finger_data[5];
extern TouchPad_state t_state;

extern bool gesture_start;
extern bool gesture_end;
extern int gesture_start_counter;


static window_dimension getWindowDimensions(HWND window) {
    window_dimension dimension;

    RECT clientRect;
    GetClientRect(window, &clientRect);						// area in window where you can draw
    dimension.width = clientRect.right - clientRect.left;
    dimension.height = clientRect.bottom - clientRect.top;
    return dimension;
}


static LRESULT CALLBACK mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {

        PAINTSTRUCT paint;
        HDC deviceContext = BeginPaint(window, &paint);

        window_dimension dimension = getWindowDimensions(window);
        updateWindow(deviceContext, dimension.width, dimension.height, &globalBackBuffer);

        EndPaint(window, &paint);
        return 0;
    }break;

    case WM_DESTROY:
    {
        PostQuitMessage(0); // put quit message in message loop
        return 0;
    } break;

    case WM_INPUT: {
        if (getRawData(lParam)) {
            globalInputReportInfo.deviceHandle = globalRawInput->header.hDevice;
            getInputReportInfo(&globalInputReportInfo);
        }

        // fill_payload()
        // fill_events_in_event_buffer(find_gesture(payload))  --> for all fingers connected

        // getFingerData(globalInputReportInfo.ptrPreparsedData, globalRawInput, finger_data, &t_state);

        if(gesture_start == true) gesture_start_counter++;




        // printTouchpadData(finger_data, t_state);
        return DefWindowProc(window, message, wParam, lParam);
    } break;

    default: {
        return DefWindowProc(window, message, wParam, lParam);
    }break;
    }
}


#endif