#include <stdio.h>
#include <Windows.h>
#include <stdint.h>

#pragma comment(lib, "User32")
#pragma comment(lib, "gdi32")


typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int32_t bool32;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

// global_variables

bool globalRunning = false;

//======

LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
    LRESULT result = 0;

    switch (message) {
        case WM_SIZE: {}break;

        case WM_CLOSE: {
            DestroyWindow(window);
        }break;

        case WM_DESTROY: {
            PostQuitMessage(0);
            globalRunning = false;
        }break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            // wparam : which key was pressed
            // lparam : additional info about the key press

            uint32 VKCode = (uint32)wParam;
            bool wasPressed = (lParam & (1 << 30)) != 0;
            bool isPressed = (lParam & (1 << 31)) == 0;

            if (wasPressed != isPressed) {
                if (VKCode == 'W') {}
                else if (VKCode == 'A') {}
                else if (VKCode == 'S') {}
                else if (VKCode == 'D') {}
                else if (VKCode == 'Q') {}
                else if (VKCode == 'E') {}
                else if (VKCode == VK_UP) {}
                else if (VKCode == VK_DOWN) {}
                else if (VKCode == VK_RIGHT) {}
                else if (VKCode == VK_LEFT) {}
                else if (VKCode == VK_ESCAPE) {}
                else if (VKCode == VK_SPACE) {}

                bool32 altKeyWasDown = (lParam & (1 << 29)) != 0;
                if (altKeyWasDown && (VKCode == VK_F4)) { globalRunning = false; }
            }
        }break;

        case WM_ACTIVATEAPP: {}break;

        default: {
            result = DefWindowProc(window, message, wParam, lParam);
        }break;
    }
    return result;
}


int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow){

    (void)prevInstance;
    (void)cmdLine;
    (void)cmdShow;

    WNDCLASS windowClass = {
        .style = CS_VREDRAW | CS_HREDRAW,
        .lpfnWndProc = win32_mainWindowCallback,
        .hInstance = instance,
        .lpszClassName = L"G.R.E"
    };

    if(RegisterClass(&windowClass)){
        HWND window = CreateWindowEx(0, windowClass.lpszClassName, L"G.R.E", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, 0, 0, instance, 0);
        if(window){
            globalRunning = true;

            while(globalRunning){
                MSG message;
                while(GetMessageA(&message, 0, 0, 0)){
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                }
                globalRunning = false; // GetMessageA got WM_QUIT message and thus returned 0
            }
        }else{}
    }else{}
}