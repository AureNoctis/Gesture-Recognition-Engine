//! ===================================================================    TODO    =============================================================
/*
    * 3) find a proper way to store data

    * 4) function to know type of device( dynamic, static, hybride)
    * 5) function to update all the data

*/
//! ============================================================================================================================================

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
#include "usage.h"

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
    PHIDP_PREPARSED_DATA ptrPreparsedData;
    HIDP_CAPS* pCaps;
    HIDP_VALUE_CAPS* pValCaps;
    HIDP_BUTTON_CAPS* pButtopnCaps;
    HIDP_LINK_COLLECTION_NODE* pLinkCollection;

    HANDLE deviceHandle;
};


// ======= global_variables ========

bool globalRunning = false;
static Win32_offscrean_buffer globalBackBuffer;
static Win32_InputReportInfo globalInputReportInfo;

static RAWINPUT* globalRawInput;
static UINT prevRawInputSize = 0;

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

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);	    // specifing the rules
    buffer->info.bmiHeader.biWidth = buffer->width;						// ...
    buffer->info.bmiHeader.biHeight = -buffer->height;					// ...
    buffer->info.bmiHeader.biPlanes = 1;								// ...
    buffer->info.bmiHeader.biBitCount = 32;								// 4 bytes = 3(rgb) + 1(padding: for proper aligment) : size of each pixle
    buffer->info.bmiHeader.biCompression = BI_RGB;					    // ...

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
    // allocat buffer if not allocated else reuse that

    unsigned int pdataSize;
    GetRawInputDeviceInfo(info->deviceHandle, RIDI_PREPARSEDDATA, NULL, &pdataSize);
    if (pdataSize > 0) {
        if (info->ptrPreparsedData == nullptr) {
            info->ptrPreparsedData = (PHIDP_PREPARSED_DATA)malloc(pdataSize);
        }
        GetRawInputDeviceInfo(info->deviceHandle, RIDI_PREPARSEDDATA, info->ptrPreparsedData, &pdataSize);

        if (info->pCaps == nullptr) {
            info->pCaps = (HIDP_CAPS*)malloc(sizeof(HIDP_CAPS));
        }
        HidP_GetCaps(info->ptrPreparsedData, info->pCaps);

        USHORT pValCapsLen;
        if (info->pValCaps == nullptr) {
            pValCapsLen = info->pCaps->NumberInputValueCaps; // 17
            info->pValCaps = (HIDP_VALUE_CAPS*)malloc(sizeof(HIDP_VALUE_CAPS) * pValCapsLen);
        }
        HidP_GetValueCaps(HidP_Input, info->pValCaps, &pValCapsLen, info->ptrPreparsedData);

        USHORT pButtopnCapsLen;
        if (info->pButtopnCaps == nullptr) {
            pButtopnCapsLen = info->pCaps->NumberInputButtonCaps; // 11
            info->pButtopnCaps = (HIDP_BUTTON_CAPS*)malloc(sizeof(HIDP_BUTTON_CAPS) * pButtopnCapsLen);
        }
        HidP_GetButtonCaps(HidP_Input, info->pButtopnCaps, &pButtopnCapsLen, info->ptrPreparsedData);

        ULONG linkNodeNumber;
        if (info->pLinkCollection == nullptr) {
            linkNodeNumber = info->pCaps->NumberLinkCollectionNodes; // 6
            info->pLinkCollection = (HIDP_LINK_COLLECTION_NODE*)malloc(sizeof(HIDP_LINK_COLLECTION_NODE) * linkNodeNumber);
        }
        HidP_GetLinkCollectionNodes(info->pLinkCollection, &linkNodeNumber, info->ptrPreparsedData);
    }
}

int Win32_getRawData(LPARAM lParam){
    // allocat buffer if not allocated else reuse that

    UINT size;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
    char return_value = 0;

    if (size > prevRawInputSize ) { // just in case size of raw input changed (for dynamic touch pad)
        RAWINPUT* new_globalRawInput = (RAWINPUT*)realloc(globalRawInput, size);
        if(new_globalRawInput != nullptr)
            globalRawInput = new_globalRawInput;
        return_value = 1;
        prevRawInputSize = size;
    }

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (BYTE*)globalRawInput, &size, sizeof(RAWINPUTHEADER));
    return return_value;
}

void win32_getUsageValue_staus(NTSTATUS status) {
    printf("\033[1;31m");
    switch (status) {
    case HIDP_STATUS_SUCCESS: printf("SUCCESS"); break;
    case HIDP_STATUS_INVALID_REPORT_LENGTH: printf("INVALID_REPORT_LENGTH"); break;
    case HIDP_STATUS_INVALID_REPORT_TYPE: printf("INVALID_REPORT_TYPE"); break;
    case HIDP_STATUS_INCOMPATIBLE_REPORT_ID: printf("INCOMPATIBLE_REPORT_ID"); break;
    case HIDP_STATUS_INVALID_PREPARSED_DATA: printf("INVALID_PREPARSED_DATA"); break;
    case HIDP_STATUS_USAGE_NOT_FOUND: printf("USAGE_NOT_FOUND"); break;
    }
    printf("\033[0m \n");
}

void win32_printTouchpadData(PHIDP_PREPARSED_DATA preparsedData, RAWINPUT* raw) {

    char* report = (char*)raw->data.hid.bRawData;
    ULONG reportLen = raw->data.hid.dwSizeHid;

    // Get Contact Count
    ULONG contactCount = 0;
    HidP_GetUsageValue(
        HidP_Input,
        UP_DIGITIZER,
        0,
        U_DIGITIZER_CONTACT_COUNT,
        &contactCount,
        preparsedData,
        report,
        reportLen
    );

    USAGE buttonUsageList[1];
    ULONG buttonUsageLength = 1;
    USHORT button_flag = 9999;
    HidP_GetUsages(
        HidP_Input,
        UP_BUTTON,          // usage page 0x09
        0,                  // link collection 0
        buttonUsageList,
        &buttonUsageLength,
        preparsedData,
        report,
        reportLen
    );

    button_flag = (buttonUsageLength > 0 && buttonUsageList[0] == U_BUTTON) ? 1 : 0;

    printf("-------------------------------------------------\n");
    printf("Contact Count : %lu\n", contactCount);
    printf("button flag : %hu\n\n", button_flag);


    // Header with large spacing
    printf("Finger        Tip        Confidence        ID        X              Y\n");

    // Iterate Fingers
    for (ULONG link = 1; link <= globalInputReportInfo.pCaps->NumberLinkCollectionNodes - 1; link++)
    {
        USHORT tip = 0;
        USHORT conf = 0;
        ULONG id = 9999;
        ULONG x = 9999;
        ULONG y = 9999;

        USAGE usageList[2];
        ULONG usageLength = 2;

        // for buttons:
        HidP_GetUsages(HidP_Input, UP_DIGITIZER, link, usageList, &usageLength,
             preparsedData, report, reportLen);

        if (usageList[0] == U_FINGER_TIP)        tip = 1;
        if (usageList[1] == U_FINGER_CONFIDENCE) conf = 1;

        // for usage values:
        HidP_GetUsageValue(HidP_Input, UP_DIGITIZER, link, U_FINGER_ID,
            &id, preparsedData, report, reportLen);

        HidP_GetUsageValue(HidP_Input, UP_GENERIC_DESKTOP, link, U_FINGER_X,
            &x, preparsedData, report, reportLen);

        HidP_GetUsageValue(HidP_Input, UP_GENERIC_DESKTOP, link, U_FINGER_Y,
            &y, preparsedData, report, reportLen);

        printf("F%-2lu          %-3hu        %-10hu        %-3lu       %-10lu     %-10lu\n",
            link, tip, conf, id, x, y);
    }
}


LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){

    switch (message) {
        case WM_SIZE: {
            return DefWindowProc(window, message, wParam, lParam);
        }break;

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
            if (Win32_getRawData(lParam) && globalInputReportInfo.deviceHandle == nullptr) {
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

        // OUTPUT MODE (colors etc.)
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD outMode = 0;
        GetConsoleMode(hOut, &outMode);
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, outMode);

        // INPUT MODE (FIX FREEZE)
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        DWORD inMode = 0;
        GetConsoleMode(hIn, &inMode);

        inMode |= ENABLE_EXTENDED_FLAGS;      // required
        inMode &= ~ENABLE_QUICK_EDIT_MODE;    // disable freeze
        inMode &= ~ENABLE_INSERT_MODE;        // optional

        SetConsoleMode(hIn, inMode);
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
                while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
                    if(message.message == WM_QUIT)
                        globalRunning = false;

                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                //! WM_QUIT:
                // It is NOT sent to your window procedure
                // It is NOT dispatched via DispatchMessage
                // It is only seen by GetMessage / PeekMessage




                Win32_window_dimension dimension = win32_getWindowDimensions(window);
                win32_updateWindow(deviceContext, dimension.width, dimension.height, &globalBackBuffer);
            }
            ReleaseDC(window, deviceContext);
        }else{}
    }else{}


    free(globalRawInput);
    free(globalInputReportInfo.ptrPreparsedData);
    free(globalInputReportInfo.pLinkCollection);
    free(globalInputReportInfo.pCaps);
    free(globalInputReportInfo.pValCaps);
    free(globalInputReportInfo.pButtopnCaps);
}