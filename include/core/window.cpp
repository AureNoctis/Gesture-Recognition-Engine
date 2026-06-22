#ifndef _WINDOW_
#define _WINDOW_

#include <Windows.h>
#include <cstring>
#include "utils/declaration.h"
#include "utils/usage.h"



window_dimension getWindowDimensions(HWND window) {
    window_dimension dimension;

    RECT clientRect;
    GetClientRect(window, &clientRect);						// area in window where you can draw
    dimension.width = clientRect.right - clientRect.left;
    dimension.height = clientRect.bottom - clientRect.top;
    return dimension;
}

void* _init_window_state(HWND window, Window_state w_state){
    void* data = malloc(sizeof(Window_state));
    memcpy(data, &w_state, sizeof(w_state));

    SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)data);

    return data;
}

LRESULT CALLBACK mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {

    Window_state* w_state = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);

    switch (message) {
    case WM_PAINT: {

        PAINTSTRUCT paint;
        HDC deviceContext = BeginPaint(window, &paint);

        window_dimension dimension = getWindowDimensions(window);
        updateWindow(deviceContext, dimension.width, dimension.height, &w_state->back_buffer);
        
        EndPaint(window, &paint);
        return 0;
    }break;

    case WM_DESTROY:
    {
        PostQuitMessage(0); // put quit message in message loop
        return 0;
    } break;

    case WM_INPUT: {
        if (getRawData(window, lParam)) {
            w_state->input_report_info.deviceHandle = w_state->raw_input->header.hDevice;
            getInputReportInfo(&w_state->input_report_info);
        }
        getFingerData(window);

        if(w_state->gesture_start == true) w_state->gesture_start_counter++;

        printTouchpadData(window);
        return 0;
    } break;

    default: {
        return DefWindowProc(window, message, wParam, lParam);
    }break;
    }
}


#endif
