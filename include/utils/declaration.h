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


#define delta_t 70          // manually got this data for touchpad :) -> 70 us
#define frequency 143.85

// i will ignore the values with 0

#define swipe_min_travel   300
#define swipe_min_time     0
#define swipe_max_time     3000

#define move_min_travel   50
#define move_min_time     0
#define move_max_time     0

#define tap_max_travel     5
#define tap_min_time       100
#define tap_max_time       4999

#define hold_max_travel    10
#define hold_min_time      5000
#define hold_max_time      20000

#define _do_
#define _then_

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


// ===================  enum  ===================
enum Finger_index{
    F1 = 0,
    F2,
    F3,
    F4,
    F5,
    TOTAL_FINGERS
};

enum Gesture_type : u8{
    SWIPE = 0,
    MOVE,
    TAP,
    HOLD
};
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

struct Gesture_report{
    Gesture_type type;
    u16 time_elapsed;
    union{
        struct{
            u16 x;
            u16 y;
        };
        struct{
            u16 speed;
            u16 distance_traveled;
        };
    };
};

// struct TouchHistory{
//     Finger *history[TOTAL_FINGERS];
//     u8 contact_count;
//     u8 index_array[TOTAL_FINGERS];
//     u8 data_filled[TOTAL_FINGERS];
// };

// /*
//     if data_filled[finger] == max_sample ----> long_gesture(~0.5 sec) (e.g. hold, ...)
//     data_filling index in heap buffer will be obtained from "FINGER.ID"

// */

// ===================  function declaration  ==================

static void win32_getFingerData(PHIDP_PREPARSED_DATA preparsedData,
                                RAWINPUT *raw, Finger *finger_data,
                                TouchPad_state *t_state);

static void win32_printTouchpadData(Finger *finger_data,
                                    TouchPad_state t_state);
static LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message,
                                                 WPARAM wParam, LPARAM lParam);

static Win32_window_dimension win32_getWindowDimensions(HWND window);

force_Inline static void win32_getTouchPadInfoFile(Win32_InputReportInfo *info);

static void win32_getUsageValue_status(NTSTATUS status);

static int Win32_getRawData(LPARAM lParam);

static void Win32_getInputReportInfo(Win32_InputReportInfo *info);

static void win32_renderWeirdGradiant(Win32_offscrean_buffer *buffer,
                                      int blueOffset, int greenOffset);

static void win32_resizeDIBSection(Win32_offscrean_buffer *buffer, int width,
                                   int height);

static void win32_updateWindow(HDC deviceContext, int width, int height,
                               Win32_offscrean_buffer *buffer);

#endif