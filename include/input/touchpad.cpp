#ifndef _TOUCHPAD_TEST_
#define _TOUCHPAD_TEST_

#include "utils/declaration.h"
#include "math.h"
#include <cstring>
#include <Windows.h>


[[maybe_unused]]
void printTouchpadData(HWND window) {
	Window_state*  w_state	   = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);
	TouchPad_state t_state	   = w_state->t_state;
	Finger*		   finger_data = w_state->finger_data;

	printf("---------------------------------------------------\n"
		   "contact count  :  %hhu \n"
		   "scan time      :  %hu x10^-4\n"
		   "button         :  %hhu \n",
		   t_state.contactCount, t_state.scanTime, t_state.touchPadButton);

	printf("Finger        Tip        Confidence        ID        X              Y\n");

	for (i32 i = 0; i < 5; i++) {
		printf("F%-2i          %-3hu        %-10hu        %-3hhu       %-10u     %-10u\n", i + 1, finger_data[i].tip_switch, finger_data[i].confidence,
			   finger_data[i].id, finger_data[i].x, finger_data[i].y);
	}
}

[[maybe_unused]]
void printFingerDeltaData(HWND window) {
	Window_state* w_state = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);

	FingerDeltaData* delta_data = w_state->finger_delta;

	printf("-----------------------------------------------------------------------------------------------------------------------\n");
	printf("Finger  State  Conf  StartT    DeltaT    Xi       Yi       Xf       Yf       Xd       Yd       Dist\n");
	printf("-----------------------------------------------------------------------------------------------------------------------\n");

	for (i32 i = 0; i < 5; i++) {
		printf("F%-2i     %-5d  %-4hhu  %-8hu  %-8hu  %-8u %-8u %-8u %-8u %-8d %-8d %-8u\n", i + 1, (int)delta_data[i].contact_state,
			   delta_data[i].confidence, delta_data[i].startTime, delta_data[i].deltaTime, delta_data[i].xi, delta_data[i].yi, delta_data[i].xf,
			   delta_data[i].yf, delta_data[i].xd, delta_data[i].yd, delta_data[i].distance_traveled);
	}
	printf("\n");
}

void getFingerData(HWND window) {
	Window_state*		 w_state	   = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);
	PHIDP_PREPARSED_DATA preparsedData = w_state->input_report_info.ptrPreparsedData;
	Finger*				 finger_data   = w_state->finger_data;
	TouchPad_state*		 t_state	   = &(w_state->t_state);

	RAWINPUT* raw		= w_state->raw_input;
	i8*		  report	= (i8*)raw->data.hid.bRawData;
	u32		  reportLen = raw->data.hid.dwSizeHid;

	memset(finger_data, 0, sizeof(Finger) * 5);
	memset(t_state, 0, sizeof(TouchPad_state));

	HIDP_DATA data[32];
	u32		  data_length = 32;
	HidP_GetData(HidP_Input, data, (unsigned long*)&data_length, preparsedData, (char*)report, reportLen);

	for (u32 i = 0; i < data_length; i++) {
		switch (data[i].DataIndex) {
				// clang-format off

			// touchpad
			case 0: { (*t_state).touchPadButton = data[i].On;       } break;
			case 1: { (*t_state).contactCount   = data[i].RawValue; } break;
			case 2: { (*t_state).scanTime       = data[i].RawValue; } break;

			// {F1 F2 ...} are relative to touchpad:
			// index0
			case 3:  { finger_data[F1].tip_switch = data[i].On;       } break;
			case 4:  { finger_data[F1].confidence = data[i].On;       } break;
			case 5:  { finger_data[F1].id         = data[i].RawValue; } break;
			case 6:  { finger_data[F1].x          = data[i].RawValue; } break;
			case 7:  { finger_data[F1].y          = data[i].RawValue; } break;

			// index1
			case 8:  { finger_data[F2].tip_switch = data[i].On;       } break;
			case 9:  { finger_data[F2].confidence = data[i].On;       } break;
			case 10: { finger_data[F2].id         = data[i].RawValue; } break;
			case 11: { finger_data[F2].x          = data[i].RawValue; } break;
			case 12: { finger_data[F2].y          = data[i].RawValue; } break;

			// index2
			case 13: { finger_data[F3].tip_switch = data[i].On;       } break;
			case 14: { finger_data[F3].confidence = data[i].On;       } break;
			case 15: { finger_data[F3].id         = data[i].RawValue; } break;
			case 16: { finger_data[F3].x          = data[i].RawValue; } break;
			case 17: { finger_data[F3].y          = data[i].RawValue; } break;

			// index3
			case 18: { finger_data[F4].tip_switch = data[i].On;       } break;
			case 19: { finger_data[F4].confidence = data[i].On;       } break;
			case 20: { finger_data[F4].id         = data[i].RawValue; } break;
			case 21: { finger_data[F4].x          = data[i].RawValue; } break;
			case 22: { finger_data[F4].y          = data[i].RawValue; } break;

			// index4
			case 23: { finger_data[F5].tip_switch = data[i].On;       } break;
			case 24: { finger_data[F5].confidence = data[i].On;       } break;
			case 25: { finger_data[F5].id         = data[i].RawValue; } break;
			case 26: { finger_data[F5].x          = data[i].RawValue; } break;
			case 27: { finger_data[F5].y          = data[i].RawValue; } break;

			default: { fprintf(stderr, "unknown data index: %d found while filling data\n", data[i].DataIndex); } break;

				// clang-format on
		}
	}

	w_state->gesture_end = !(
		bool)((finger_data[0].tip_switch | finger_data[1].tip_switch | finger_data[2].tip_switch | finger_data[3].tip_switch | finger_data[4].tip_switch));
	w_state->gesture_start = !w_state->gesture_end;
}


u8 fillDeltaStruct(Finger* ga_pf_start_data, Finger* ga_pf_end_data, u16* ga_pf_start_time, u16 ga_end_time, FingerDeltaData* ga_pf_delta_data) {
#define Short_max 65536
	memset(ga_pf_delta_data, 0, 5 * sizeof(FingerDeltaData));
	u8 finger_state = 0;

	static vec2f prev_pos[5] = {};

	for (int i = 0; i < 5; i++) {
		if (ga_pf_start_data[i].confidence == 1) {

			ga_pf_delta_data[i].xi			  = ga_pf_start_data[i].x;
			ga_pf_delta_data[i].yi			  = ga_pf_start_data[i].y;
			ga_pf_delta_data[i].confidence	  = ga_pf_start_data[i].confidence;
			ga_pf_delta_data[i].startTime	  = ga_pf_start_time[i];
			ga_pf_delta_data[i].deltaTime	  = (Short_max + ga_end_time - ga_pf_start_time[i]) % Short_max;
			ga_pf_delta_data[i].contact_state = (Contact_state)ga_pf_end_data[i].tip_switch;

			FILL_BIT(&finger_state, i, ga_pf_end_data[i].tip_switch);

			ga_pf_delta_data[i].xf				  = ga_pf_end_data[i].x;
			ga_pf_delta_data[i].yf				  = ga_pf_end_data[i].y;
			ga_pf_delta_data[i].xd				  = ga_pf_end_data[i].x - ga_pf_start_data[i].x;
			ga_pf_delta_data[i].yd				  = ga_pf_end_data[i].y - ga_pf_start_data[i].y;
			ga_pf_delta_data[i].distance_traveled = (u32)hypot(ga_pf_delta_data[i].xd, ga_pf_delta_data[i].yd);
		}
	}
	return finger_state;
#undef Short_max
}

void getFingerDeltaData(HWND window) {
	Window_state* w_state = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);

	/*
	 naming convention:
	 g  -> gesture
	 a  -> atom
	 pf -> per finger

	 geture = collection of ga(s)
	 */

	//	static u16 g_start_time = 0;

	static Finger ga_pf_start_data[5]	  = {};
	static u16	  ga_pf_start_time[5]	  = {};
	static u8	  got_ga_pf_start_data[5] = {}; // for both time and data
	Finger		  ga_pf_current_data[5]	  = {};
	// |-> this contain the data before lifting finger
	// not using finger_data as the end data because the the filling order is different finger_data : 0 to count-1 : (bottom(0) to up)
	// but i wanted filling to be done on the basis of finger index :)

	getFingerData(window);

	Finger*		   finger_data = w_state->finger_data;
	TouchPad_state t_state	   = w_state->t_state;

	// if(w_state->gesture_start == true && gesture_start_time == 0 )
	int finger_ID;
	for (int i = 0; i < 5; i++) {
		if (finger_data[i].confidence == 1) {
			finger_ID = finger_data[i].id;
			// filling current data
			memcpy(ga_pf_current_data + finger_ID, finger_data + i, sizeof(Finger));

			// filling start data
			if (got_ga_pf_start_data[finger_ID] == 0) {
				got_ga_pf_start_data[finger_ID] = 1;
				memcpy(ga_pf_start_data + finger_ID, finger_data + i, sizeof(Finger));
				ga_pf_start_time[finger_ID] = t_state.scanTime;
			}
			continue;
		}
		break;
	}

	u8 finger_state = fillDeltaStruct(ga_pf_start_data, ga_pf_current_data, ga_pf_start_time, t_state.scanTime, w_state->finger_delta);
	PostMessageW(window, GRE_GA_DELTA_READY, 0, 0);

	for (int i = 0; i < 5; i++) {
		if ((got_ga_pf_start_data[i] &= ((finger_state >> i) & 1)) == 0) {
			// to remove ghost finger:
			// 3 finger --> 2 finger --> 1 finger
			// the data of 1st removed finger is still there(confidence = 1), so it will again be filled in delta struct:
			memset(ga_pf_start_data + i, 0, sizeof(Finger));
			ga_pf_start_time[i] = 0;
		}
	}

	if (w_state->gesture_end) {
		PostMessageW(window, GRE_GESTURE_END, 0, 0);

		memset(ga_pf_start_data, 0, sizeof(ga_pf_start_data));
		memset(ga_pf_start_time, 0, sizeof(ga_pf_start_time));
		memset(got_ga_pf_start_data, 0, sizeof(got_ga_pf_start_data));
	}
}


void _getFingerDeltaData(HWND window) {
	Window_state* w_state = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);

	/*
	 naming convention:
	 g  -> gesture
	 a  -> atom
	 pf -> per finger

	 geture = collection of ga(s)
	 */

	//	static u16 g_start_time = 0;

	static Finger ga_pf_start_data[5]	   = {};
	static u16	  ga_pf_start_time[5]	   = {};
	static u8	  got_ga_pf_start_data[5]  = {}; // for both time and data
	static Finger ga_pf_end_data[5]		   = {};
	// |-> this contain the data before lifting finger
	// not using finger_data as the end data because the the filling order is different finger_data : 0 to count-1 : (bottom(0) to up)
	// but i wanted filling to be done on the basis of finger index :)
	static u8	  ga_prev_contact_count	   = 0;
	static u8	  ga_current_contact_count = 0;

	getFingerData(window);

	Finger*		   finger_data = w_state->finger_data;
	TouchPad_state t_state	   = w_state->t_state;
	ga_current_contact_count   = t_state.contactCount;


	if (ga_current_contact_count < ga_prev_contact_count || w_state->gesture_end) {
		if (w_state->gesture_end == true) {
			for (int i = 0; i < ga_current_contact_count; i++)
				memcpy(ga_pf_end_data + finger_data[i].id, finger_data + i, sizeof(Finger));
		}
		u8 finger_state = fillDeltaStruct(ga_pf_start_data, ga_pf_end_data, ga_pf_start_time, t_state.scanTime, w_state->finger_delta);
		for (int i = 0; i < 5; i++) {
			if ((got_ga_pf_start_data[i] &= ((finger_state >> i) & 1)) == 0) {
				// to remove ghost finger:
				// 3 finger --> 2 finger --> 1 finger
				// the data of 1st removed finger is still there(confidence = 1), so it will again be filled in delta struct:
				memset(ga_pf_start_data + i, 0, sizeof(Finger));
				memset(ga_pf_end_data + i, 0, sizeof(Finger));
				ga_pf_start_time[i] = 0;
			}
		}


		PostMessageW(window, GRE_GA_DELTA_READY, ga_prev_contact_count, 0);

		if (w_state->gesture_end) {
			PostMessageW(window, GRE_GESTURE_END, 0, 0);

			memset(ga_pf_start_data, 0, sizeof(ga_pf_start_data));
			memset(ga_pf_start_time, 0, sizeof(ga_pf_start_time));
			memset(got_ga_pf_start_data, 0, sizeof(got_ga_pf_start_data));
			memset(ga_pf_end_data, 0, sizeof(ga_pf_end_data));

			ga_prev_contact_count	 = 0;
			ga_current_contact_count = 0;
			return;
		}
	}

	// if(w_state->gesture_start == true && gesture_start_time == 0 )
	int finger_ID;
	for (int i = 0; i < 5; i++) {
		if (finger_data[i].confidence == 1) {
			finger_ID = finger_data[i].id;
			if (got_ga_pf_start_data[finger_ID] == 0) {
				got_ga_pf_start_data[finger_ID] = 1;
				memcpy(ga_pf_start_data + finger_ID, finger_data + i, sizeof(Finger));
				ga_pf_start_time[finger_ID] = t_state.scanTime;
			}
			memcpy(ga_pf_end_data + finger_ID, finger_data + i, sizeof(Finger));
			continue;
		}
		break;
	}

	ga_prev_contact_count = ga_current_contact_count;
}

#endif
