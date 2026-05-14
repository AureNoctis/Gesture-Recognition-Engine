#ifndef _WINDOW_
#define _WINDOW_



static Win32_window_dimension win32_getWindowDimensions(HWND window) {
    Win32_window_dimension dimension;

    RECT clientRect;
    GetClientRect(window, &clientRect);						// area in window where you can draw
    dimension.width = clientRect.right - clientRect.left;
    dimension.height = clientRect.bottom - clientRect.top;
    return dimension;
}


LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {

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

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        u32 VKCode = (u32)wParam;
        bool wasPressed = (lParam & (1 << 30)) != 0;
        bool isPressed = (lParam & (1 << 31)) == 0;
        return DefWindowProc(window, message, wParam, lParam);
    }break;

    case WM_INPUT: {
        if (Win32_getRawData(lParam)) {
            globalInputReportInfo.deviceHandle = globalRawInput->header.hDevice;
            Win32_getInputReportInfo(&globalInputReportInfo);
        }

        win32_printTouchpadData(globalInputReportInfo.ptrPreparsedData, globalRawInput);


        return DefWindowProc(window, message, wParam, lParam);
    } break;

    default: {
        return DefWindowProc(window, message, wParam, lParam);
    }break;
    }
}


#endif