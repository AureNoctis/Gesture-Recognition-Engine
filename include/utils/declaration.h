#ifndef _DECLARATION_H
#define _DECLARATION_H

// C/C++ Standard Library
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Windows Basics
#include <Windows.h>

// Fix for hidpi.h (It needs NTSTATUS defined)
typedef long NTSTATUS;
#include <hidusage.h>
#include <hidpi.h>
#include <hidsdi.h>
#include <SetupAPI.h>


// ===================  defines  ==================
#define force_Inline __forceinline


// ===================  typedefs  ==================

typedef int8_t         i8;
typedef int16_t        i16;
typedef int32_t        i32;
typedef int64_t        i64;
typedef uint8_t        u8;
typedef uint16_t       u16;
typedef uint32_t       u32;
typedef uint64_t       u64;

typedef float          f32;
typedef double         f64;


// ===================  structs  ==================

struct Win32_InputReportInfo {
    PHIDP_PREPARSED_DATA ptrPreparsedData;
    HIDP_CAPS* pCaps;
    HIDP_VALUE_CAPS* pValCaps;
    HIDP_BUTTON_CAPS* pButtonCaps;
    HIDP_LINK_COLLECTION_NODE* pLinkCollection;

    HANDLE deviceHandle;
};

struct Win32_offscrean_buffer {
    BITMAPINFO info;													// This is you telling Windows the "Rules" of your DIB
    void* memory;														// A raw pointer for you to touch the pixels directly
    int width;
    int height;
    int bytesPerPixel = 4;
    int pitch;															// gap between tow rows
};

struct Win32_window_dimension {
    int width;
    int height;
};

struct Finger{
    u32 x;
    u32 y;
    u8 id;
    u8 tip_switch;
    u8 confidence;
};
struct TouchPad_state{
    u16 scanTime;
    u8 contactCount;
    u8 touchPadButton;
};


// ===================  function declaration  ==================

static void win32_getFingerData(PHIDP_PREPARSED_DATA preparsedData, RAWINPUT* raw,
    Finger* finger_data, TouchPad_state* t_state);

static void win32_printTouchpadData(PHIDP_PREPARSED_DATA preparsedData, RAWINPUT* raw);

static LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

static Win32_window_dimension win32_getWindowDimensions(HWND window);

force_Inline static void win32_getTouchPadInfoFile(Win32_InputReportInfo* info);

static void win32_getUsageValue_status(NTSTATUS status);

static int Win32_getRawData(LPARAM lParam);

static void Win32_getInputReportInfo(Win32_InputReportInfo* info);

static void win32_renderWeirdGradiant(Win32_offscrean_buffer* buffer, int blueOffset, int greenOffset);

static void win32_resizeDIBSection(Win32_offscrean_buffer* buffer, int width, int height);

static void win32_updateWindow(HDC deviceContext, int width, int height, Win32_offscrean_buffer* buffer);



#endif