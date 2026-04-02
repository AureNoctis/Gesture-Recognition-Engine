#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// 1. C++ Standard Library
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdint.h>

// 2. Windows Basics
#include <Windows.h>

// 3. Fix for hidpi.h (It needs NTSTATUS defined)
typedef long NTSTATUS;
#include <hidusage.h>
#include <hidpi.h>
#include <hidsdi.h>
#include <SetupAPI.h>

// 4. custom headers
// #include "mstd/mstd.c"

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Setupapi.lib")


// typedefs:
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;


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

struct Win32_InputReportInfo{
    PHIDP_PREPARSED_DATA pPreparsedData;
    HIDP_CAPS* caps;
    HIDP_VALUE_CAPS* valCaps;
    HIDP_BUTTON_CAPS* buttonCaps;
    HIDP_LINK_COLLECTION_NODE* linkNodes;

    HANDLE deviceHandle;
};

// ==========================
// ======= global_variables ========

bool globalRunning = false;
static Win32_offscrean_buffer globalBackBuffer;
static Win32_InputReportInfo globalInputReportInfo;
static RAWINPUT* globalRawInput;
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

void Win32_getInputReportInfo(Win32_InputReportInfo* info){
    unsigned int pdataSize;
    GetRawInputDeviceInfo(info->deviceHandle, RIDI_PREPARSEDDATA, NULL, &pdataSize);
    if (pdataSize > 0) {

        if (info->pPreparsedData == nullptr) {
            info->pPreparsedData = (PHIDP_PREPARSED_DATA)malloc(pdataSize);
        }
        GetRawInputDeviceInfo(info->deviceHandle, RIDI_PREPARSEDDATA, info->pPreparsedData, &pdataSize);


        if (info->caps == nullptr) {
            info->caps = (HIDP_CAPS*)malloc(sizeof(HIDP_CAPS));
        }
        HidP_GetCaps(info->pPreparsedData, info->caps);


        USHORT valCapsLen;
        if (info->valCaps == nullptr) {
            valCapsLen = info->caps->NumberInputValueCaps; // 17
            info->valCaps = (HIDP_VALUE_CAPS*)malloc(sizeof(HIDP_VALUE_CAPS) * valCapsLen);
        }
        HidP_GetValueCaps(HidP_Input, info->valCaps, &valCapsLen, info->pPreparsedData);


        USHORT buttonCapsLen;
        if (info->buttonCaps == nullptr) {
            buttonCapsLen = info->caps->NumberInputButtonCaps; // 11
            info->buttonCaps = (HIDP_BUTTON_CAPS*)malloc(sizeof(HIDP_BUTTON_CAPS) * buttonCapsLen);
        }
        HidP_GetButtonCaps(HidP_Input, info->buttonCaps, &buttonCapsLen, info->pPreparsedData);


        ULONG linkNodeNumber;
        if (info->linkNodes == nullptr) {
            linkNodeNumber = info->caps->NumberLinkCollectionNodes; // 6
            info->linkNodes = (HIDP_LINK_COLLECTION_NODE*)malloc(sizeof(HIDP_LINK_COLLECTION_NODE) * linkNodeNumber);
        }
        HidP_GetLinkCollectionNodes(info->linkNodes, &linkNodeNumber, info->pPreparsedData);


    }
}

// UINT size;
// GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

// if (size > 0) {
//     BYTE* data = (BYTE*)malloc(size);
//     if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, data, &size, sizeof(RAWINPUTHEADER)) == size) {
//         RAWINPUT* raw = (RAWINPUT*)data;

//         if (globalInputReportInfo.deviceHandle == nullptr) {
//             globalInputReportInfo.deviceHandle = raw->header.hDevice;
//             Win32_getInputReportInfo(&globalInputReportInfo);
//         }
//     }
//     free(data);
// }

//! =====================================================================================================================================
//!                                                              some error
//! =====================================================================================================================================
int Win32_getRawData(RAWINPUT* rawData, LPARAM lParam){
    UINT size;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

    BYTE* data;
    if(globalRawInput == nullptr){
        if (size > 0)
            data = (BYTE*)malloc(size);
    }
    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, data, &size, sizeof(RAWINPUTHEADER)) == size)
        globalRawInput = (RAWINPUT*)data;
    else{
        return 0;
    }
}
//! =====================================================================================================================================



LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
    LRESULT result = 0;

    switch (message) {
        case WM_SIZE: {}break;

        case WM_PAINT: {

            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);

            Win32_window_dimension dimension = win32_getWindowDimensions(window);
            win32_updateWindow(deviceContext, dimension.width, dimension.height, &globalBackBuffer);

            EndPaint(window, &paint);
        }break;

        case WM_DESTROY:
        {
            PostQuitMessage(0); // put quit message in message loop
        } break;

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

            // bool altKeyWasDown = (lParam & (1 << 29)) != 0;
            // if (altKeyWasDown && (VKCode == VK_F4)) { DestroyWindow(window); }
            result = DefWindowProc(window, message, wParam, lParam);
            //}
        }break;

        //==========================================

        case WM_INPUT: {
            if(Win32_getRawData(globalRawInput, lParam)){
                if (globalInputReportInfo.deviceHandle == nullptr) {
                    globalInputReportInfo.deviceHandle = globalRawInput->header.hDevice;
                    Win32_getInputReportInfo(&globalInputReportInfo);
                }
            }

            /*  mechanism  */

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
    if (AllocConsole()) {
        FILE* fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONIN$", "r", stdin);
    }

    // ============================

    WNDCLASS windowClass = {
        .style = CS_VREDRAW | CS_HREDRAW,
        .lpfnWndProc = win32_mainWindowCallback,
        .hInstance = instance,
        .lpszClassName = L"G.R.E"
    };

    if(RegisterClass(&windowClass)){
        HWND window = CreateWindowEx(0, windowClass.lpszClassName, L"gesture recognition engine", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT, CW_USEDEFAULT, 200,
                                     200, 0, 0, instance, 0);
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
            win32_resizeDIBSection(&globalBackBuffer, 200, 200);
            globalRunning = true;

            while(globalRunning){
                win32_renderWeirdGradiant(&globalBackBuffer, 0, 0);

                MSG message;
                while (GetMessage(&message, 0, 0, 0)) {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                //! WM_QUIT:
                // It is NOT sent to your window procedure
                // It is NOT dispatched via DispatchMessage
                // It is only seen by GetMessage / PeekMessage

                if(message.message == WM_QUIT)
                    globalRunning = false;



                Win32_window_dimension dimension = win32_getWindowDimensions(window);
                win32_updateWindow(deviceContext, dimension.width, dimension.height, &globalBackBuffer);
            }
            ReleaseDC(window, deviceContext);
        }else{}
    }else{}


    free(globalRawInput);
    free(globalInputReportInfo.pPreparsedData);
    free(globalInputReportInfo.linkNodes);
    free(globalInputReportInfo.caps);
    free(globalInputReportInfo.valCaps);
    free(globalInputReportInfo.buttonCaps);
}