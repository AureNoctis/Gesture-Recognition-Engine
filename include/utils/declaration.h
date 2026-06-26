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

#define GRE_GA_DELTA_READY (WM_APP + 1)
#define GRE_GESTURE_END (WM_APP + 2)

#define COMBINE_64(low, high) ((((u64)(high)) << 32) | ((u32)(low)))
#define SPLIT_64(group, low_ptr, high_ptr)                                                                                                                \
	do {                                                                                                                                                  \
		*(high_ptr) = (u32)((group) >> 32);                                                                                                               \
		*(low_ptr)	= (u32)((group) & 0xFFFFFFFFULL);                                                                                                     \
	} while (0)

#define SET_BIT(host_ptr, bit) (*(host_ptr) |= (1ULL << bit))

#define delta_t 70 // manually got this data for touchpad :) -> 70 us
#define frequency 143.85

// i will ignore the values with 0

#define swipe_min_travel 300
#define swipe_min_time 0
#define swipe_max_time 3000

#define move_min_travel 50
#define move_min_time 0
#define move_max_time 0

#define tap_max_travel 5
#define tap_min_time 100
#define tap_max_time 4999

#define hold_max_travel 10
#define hold_min_time 5000
#define hold_max_time 20000

#define _do_
#define _then_

// ===================  typedefs  ==================

typedef int8_t	 i8;
typedef int16_t	 i16;
typedef int32_t	 i32;
typedef int64_t	 i64;
typedef uint8_t	 u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;


// ===================  enum  ===================
enum Finger_index : u8 {
	F1 = 0,
	F2,
	F3,
	F4,
	F5,
	TOTAL_FINGERS
};

enum Gesture_type : u8 {
	IDLE = 0,
	SWIPE,
	MOVE,
	TAP,
	HOLD,
	LONG_HOLD, // TODO: add the threshold values
	UNCLEAR
};

enum Contact_state : u8 {
	UP = 0,
	DOWN
};
// ===================  structs  ==================
struct InputReportInfo {
	PHIDP_PREPARSED_DATA	   ptrPreparsedData;
	HIDP_CAPS*				   pCaps;
	HIDP_VALUE_CAPS*		   pValCaps;
	HIDP_BUTTON_CAPS*		   pButtonCaps;
	HIDP_LINK_COLLECTION_NODE* pLinkCollection;

	HANDLE deviceHandle;
};

struct offscreen_buffer {
	BITMAPINFO info;   // This is you telling Windows the "Rules" of your DIB
	void*	   memory; // A raw pointer for you to touch the pixels directly
	int		   width;
	int		   height;
	int		   bytesPerPixel = 4;
	int		   pitch; // gap between tow rows
};

struct window_dimension {
	int width;
	int height;
};

struct Finger {
	u32 x;
	u32 y;
	u8	id;
	u8	tip_switch;
	u8	confidence;
};
struct TouchPad_state {
	u16 scanTime;
	u8	contactCount;
	u8	touchPadButton;
};

struct FingerDeltaData {
	u16 startTime; // x10^-4 sec
	u16 deltaTime;

	u32 xi, yi;
	u32 xf, yf;
	i32 xd, yd;
	u32 distance_traveled;

	u8			  confidence;
	Contact_state contact_state;
};

// TODO : order this according to size and alignment
struct Window_state {
	offscreen_buffer back_buffer;
	InputReportInfo	 input_report_info;
	RAWINPUT*		 raw_input;
	Finger			 finger_data[5];
	FingerDeltaData	 finger_delta[5];
	TouchPad_state	 t_state;
	bool			 gesture_start;
	bool			 gesture_end;
	bool			 running;
	int				 gesture_start_counter;
};


// ===================  function declaration  ==================

// --- helper ---
void summonConsole();
void getUsageValue_status(NTSTATUS status);

// --- window specific ---
LRESULT CALLBACK mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
window_dimension getWindowDimensions(HWND window);
void			 renderWeirdGradiant(offscreen_buffer* buffer, int blueOffset, int greenOffset);
void			 resizeDIBSection(offscreen_buffer* buffer, int width, int height);
void			 updateWindow(HDC deviceContext, int width, int height, offscreen_buffer* buffer);
void*			 _init_window_state(HWND window, Window_state w_state);
#define init_window_state(window, ...) _init_window_state(window, (Window_state){__VA_ARGS__})

// --- hid_data extraction and consumption ---
void getInputReportInfo(InputReportInfo* info);
int	 getRawData(HWND window, LPARAM lParam);
void getFingerData(HWND window);
void getFingerDeltaData(HWND window);

// --- export hid_data/hid_info ---
void			  printTouchpadData(HWND window);
force_Inline void getTouchPadInfoFile(InputReportInfo* info);
void			  printFingerDeltaData(HWND window);

// --- holder ---
FingerDeltaData* create_holder(u32 data_size, u32 data_count);
u32*			 get_maxContactCount(FingerDeltaData* holder);
void			 free_holder(FingerDeltaData* holder);


#endif
