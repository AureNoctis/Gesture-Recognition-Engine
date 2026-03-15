#include <stdio.h>
#include <Windows.h>
#include <stdint.h>
#include "mstd/mstd.c"
#include <windowsx.h>
#include <tpcshrd.h>

#pragma comment(lib, "User32")
#pragma comment(lib, "gdi32")


// ======== struct ==========
struct Win32_window_dimension {
    int width;
    int height;
};

struct Win32_offscrean_buffer {
    BITMAPINFO info;													// This is you telling Windows the "Rules" of your DIB
    void* memory;														// A raw pointer for you to touch the pixels directly
    int width;
    int height;
    int bytesPerPixel = 4;
    int pitch;															// gap between tow rows
};
// ==========================
// ======= global_variables ========

bool globalRunning = false;
static Win32_offscrean_buffer globalBackBuffer;
//==================================


static Win32_window_dimension win32_getWindowDimensions(HWND window) {
    Win32_window_dimension dimension;

    RECT clientRect;
    GetClientRect(window, &clientRect);						// area in window where you can draw
    dimension.width = clientRect.right - clientRect.left;
    dimension.height = clientRect.bottom - clientRect.top;
    return dimension;
}

static void win32_updateWindow(HDC deviceContext, int width, int height, Win32_offscrean_buffer* buffer) {

    StretchDIBits(deviceContext,
        0, 0, width, height,					// destination where we are bliting
        0, 0, buffer->width, buffer->height,	// source from where we are bliting
        buffer->memory, &buffer->info,
        DIB_RGB_COLORS, SRCCOPY);    			// SRCCOPY: what bitwise operation we want to do, we just want to copy
}

static void win32_resizeDIBSection(Win32_offscrean_buffer* buffer, int width, int height) {					// DIB: device independent buffer->


    if (buffer->memory) {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width;
    buffer->height = height;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);	// specifing the rules
    buffer->info.bmiHeader.biWidth = buffer->width;						// ...
    buffer->info.bmiHeader.biHeight = -buffer->height;					// ...
    buffer->info.bmiHeader.biPlanes = 1;									// ...
    buffer->info.bmiHeader.biBitCount = 32;								// 4 bytes = 3(rgb) + 1(padding: for proper aligment) : size of each pixle
    buffer->info.bmiHeader.biCompression = BI_RGB;						    // ...

    int bitMapmemorySize = (width * height) * buffer->bytesPerPixel;
    buffer->memory = VirtualAlloc(0, bitMapmemorySize, MEM_COMMIT, PAGE_READWRITE);
    buffer->pitch = width * buffer->bytesPerPixel;
}

static void win32_renderWeirdGradiant(Win32_offscrean_buffer* buffer, int blueOffset, int greenOffset) {

    u8* row = (u8*)buffer->memory;
    for (int y = 0; y < buffer->height; ++y) {
        u32* pixle = (u32*)row;
        for (int x = 0; x < buffer->width; ++x) {
            /*
                memory: little endian ( LSB first )
                memory   : BB GG RR xx
                registor : xx RR GG BB
            */

            u8 blue = (x + blueOffset);
            u8 green = (y + greenOffset);

            *pixle++ = (green << 8) | blue;

        }
        row += buffer->pitch;
    }

}

LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
    EnableMouseInPointer(true);
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

        case WM_PAINT: {

            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);

            Win32_window_dimension dimension = win32_getWindowDimensions(window);
            win32_updateWindow(deviceContext, dimension.width, dimension.height, &globalBackBuffer);

            EndPaint(window, &paint);
        }break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            // wparam : which key was pressed
            // lparam : additional info about the key press

            u32 VKCode = (u32)wParam;
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

                bool altKeyWasDown = (lParam & (1 << 29)) != 0;
                if (altKeyWasDown && (VKCode == VK_F4)) { DestroyWindow(window); }
            }
        }break;

        case WM_ACTIVATEAPP: {}break;

        //==========================================

        case WM_INPUT: {
            UINT size;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

            if (size > 0) {
                BYTE* data = new BYTE[size];
                if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, data, &size, sizeof(RAWINPUTHEADER)) == size) {
                    RAWINPUT* raw = (RAWINPUT*)data;

                    if (raw->header.dwType == RIM_TYPEHID) {
                        // To see raw hex data from your touchpad
                        printf("HID Data (Size %d): ", raw->data.hid.dwSizeHid);
                        // fflush(stdout);
                        for (u32 i = 0; i < raw->data.hid.dwSizeHid; ++i) {
                            printf("%02X ", raw->data.hid.bRawData[i]);
                            // fflush(stdout);
                        }
                        printf("\n");
                        // fflush(stdout);
                    }
                }
                delete[] data;
            }
        } break;


        //==========================================

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


    // ======== console ======

    os_init_state();
    os_attach_console_if_exists();

    // ============================

    WNDCLASS windowClass = {
        .style = CS_VREDRAW | CS_HREDRAW,
        .lpfnWndProc = win32_mainWindowCallback,
        .hInstance = instance,
        .lpszClassName = L"G.R.E"
    };

    if(RegisterClass(&windowClass)){
        HWND window = CreateWindowEx(0, windowClass.lpszClassName, L"gesture recognition engine", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, 0, 0, instance, 0);
        if(window){



            RAWINPUTDEVICE rid[1];

            rid[0].usUsagePage = 0x000D;
            rid[0].usUsage = 0x0005;
            rid[0].dwFlags = RIDEV_INPUTSINK;
            rid[0].hwndTarget = window;

            if(RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == false){
                // '''''
            }





            HDC deviceContext = GetDC(window);
            win32_resizeDIBSection(&globalBackBuffer, 1280, 720);
            globalRunning = true;

            while(globalRunning){
                win32_renderWeirdGradiant(&globalBackBuffer, 0, 0);

                MSG message;
                while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
                    if (message.message == WM_QUIT) {
                        globalRunning = false;
                    }
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                }

                Win32_window_dimension dimension = win32_getWindowDimensions(window);
                win32_updateWindow(deviceContext, dimension.width, dimension.height, &globalBackBuffer);
            }
            ReleaseDC(window, deviceContext);
        }else{}
    }else{}
}