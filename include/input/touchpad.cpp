#ifndef _TOUCHPAD_TEST_
#define _TOUCHPAD_TEST_

#include "utils/declaration.h"
#include "utils/usage.h"
#include "math.h"
#include <winuser.h>


[[maybe_unused]]
void printTouchpadData(HWND window) {

    Window_state* w_state = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);
    TouchPad_state t_state = w_state->t_state;
    Finger* finger_data = w_state->finger_data;

    printf(
        "---------------------------------------------------\n"
        "contact count  :  %hhu \n"
        "scan time      :  %hu x10^-4\n"
        "button         :  %hhu \n",
        t_state.contactCount,
        t_state.scanTime,
        t_state.touchPadButton
    );

    printf("Finger        Tip        Confidence        ID        X              Y\n");

    for(i32 i = 0; i < 5; i++){
        printf("F%-2i          %-3hu        %-10hu        %-3hhu       %-10u     %-10u\n",
                i+1,
                finger_data[i].tip_switch,
                finger_data[i].confidence,
                finger_data[i].id,
                finger_data[i].x,
                finger_data[i].y);
    }
}

void getFingerData(HWND window){

    Window_state* w_state = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);
    PHIDP_PREPARSED_DATA preparsedData = w_state->input_report_info.ptrPreparsedData;
    Finger* finger_data = w_state->finger_data;
    TouchPad_state t_state = w_state->t_state;

    RAWINPUT* raw = w_state->raw_input;
    i8* report = (i8*)raw->data.hid.bRawData;
    u32 reportLen = raw->data.hid.dwSizeHid;

    memset(finger_data, 0, sizeof(Finger)*5);
    memset(&t_state, 0, sizeof(TouchPad_state));

    HIDP_DATA data[32];
    u32 data_length = 32;
    HidP_GetData(HidP_Input, data, (unsigned long*)&data_length, preparsedData, (char*)report, reportLen);


    for(u32 i = 0; i < data_length; i++){
        switch(data[i].DataIndex) {
        // touchpad
        case 0:  { t_state.touchPadButton = data[i].On;       } break;
        case 1:  { t_state.contactCount   = data[i].RawValue; } break;
        case 2:  { t_state.scanTime       = data[i].RawValue; } break;


        // {F1 F2 ...} are realtive to touchpad:
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

        default: {
            fprintf(stderr, "unknown data index: %d found while filling data\n", data[i].DataIndex);
        } break;

        }
    }

    w_state->gesture_end = !(bool)((finger_data[0].tip_switch | finger_data[1].tip_switch | finger_data[2].tip_switch |
                            finger_data[3].tip_switch | finger_data[4].tip_switch));
    w_state->gesture_start = !w_state->gesture_end;

}

void getFingerDeltaData(HWND window, FingerDeltaData* holder){
    Window_state* w_state = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);

    static Finger gesture_start_data[5];
    static bool got_gesture_start_data[5];

    static u32 gesture_start_time[5];
    static bool got_start_time[5];
    // static Finger gesture_end_data[5]; 
    // |--> this data is already present in finger_data while the gesture_end is true
    static u8 prev_contact_count;


    getFingerData(window);
    
    




    

    prev_contact_count = w_state->t_state.contactCount;
//     if(t_state.contactCount > *get_maxContactCount(holder)){
//         *get_maxContactCount(holder) = t_state.contactCount;
//     }
//
//     for(int i = 0; i < 5; i++){
//         if(finger_data[i].confidence != 0){
//             if(got_start_time[finger_data[i].id] != true){
//                 gesture_start_time[finger_data[i].id] = t_state.scanTime;
//                 got_start_time[finger_data[i].id] = true;
//             }            
//             continue;
//         }
//         break;
//     }
//
//
//     if(gesture_start_counter == 2){
//         memcpy(gesture_start_data, finger_data, 5 * sizeof(Finger));
//         return;
//     }
//     if(gesture_end){
//         for(int i = 0; i < 5; i++){
//             holder[i].xi = gesture_start_data[i].x;
//             holder[i].yi = gesture_start_data[i].y;
//             holder[i].xf = finger_data[i].x;
//             holder[i].yf = finger_data[i].y; 
//             holder[i].xd = holder[i].xf - holder[i].xi;             
//             holder[i].yd = holder[i].yf - holder[i].yi;
//
//             holder[i].confidence = gesture_start_data[i].confidence;
//             holder[i].tip_switch = gesture_start_data[i].tip_switch;
//             holder[i].startTime = gesture_start_time[i];
//
// #define Short_max 65536
//             holder[i].deltaTime = (Short_max + t_state.scanTime - gesture_start_time[i]) % Short_max;
// #undef Short_max
//
//             holder[i].distance_traveled = (u32)hypot(holder[i].xd, holder[i].yd);
//         }
//         memset(gesture_start_data, 0, sizeof(gesture_start_data));
//         memset(gesture_start_time, 0, sizeof(gesture_start_time));
//         memset(got_start_time, 0, sizeof(got_start_time));
//     }
//
}

#endif
