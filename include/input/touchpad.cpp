#ifndef _TOUCHPAD_TEST_
#define _TOUCHPAD_TEST_

#include "utils/declaration.h"
#include "utils/usage.h"


extern Win32_offscrean_buffer globalBackBuffer;
extern Win32_InputReportInfo globalInputReportInfo;
extern RAWINPUT* globalRawInput;


static void win32_printTouchpadData(Finger* finger_data, TouchPad_state t_state) {

    printf(
        "---------------------------------------------------\n"
        "contact count  :  %hhu \n"
        "scan time      :  %hu \n"
        "button         :  %hhu \n",
        t_state.contactCount,
        t_state.scanTime,
        t_state.touchPadButton
    );

    printf("Finger        Tip        Confidence        ID        X              Y\n");

    for(i32 i = 0; i < 5; i++){
        printf("F%-2i          %-3hu        %-10hu        %-3lu       %-10lu     %-10lu\n",
                i+1,
                finger_data[i].tip_switch,
                finger_data[i].confidence,
                finger_data[i].id,
                finger_data[i].x,
                finger_data[i].y);
    }
}

static void win32_getFingerData(PHIDP_PREPARSED_DATA preparsedData, RAWINPUT* raw,
                                Finger* finger_data, TouchPad_state* t_state)
{
    i8* report = (i8*)raw->data.hid.bRawData;
    u32 reportLen = raw->data.hid.dwSizeHid;

    memset(finger_data, 0, sizeof(Finger)*5);
    memset(t_state, 0, sizeof(TouchPad_state));

    HIDP_DATA data[32];
    u32 data_length = 32;
    HidP_GetData(HidP_Input, data, (unsigned long*)&data_length, preparsedData, (char*)report, reportLen);

    for(i32 i = 0; i < data_length; i++){
        switch(data[i].DataIndex) {
        // touchpad
        case 0:  { t_state[0].touchPadButton = data[i].On;       } break;
        case 1:  { t_state[0].contactCount   = data[i].RawValue; } break;
        case 2:  { t_state[0].scanTime       = data[i].RawValue; } break;

        // Finger 0
        case 3:  { finger_data[0].tip_switch = data[i].On;       } break;
        case 4:  { finger_data[0].confidence = data[i].On;       } break;
        case 5:  { finger_data[0].id         = data[i].RawValue; } break;
        case 6:  { finger_data[0].x          = data[i].RawValue; } break;
        case 7:  { finger_data[0].y          = data[i].RawValue; } break;

        // Finger 1
        case 8:  { finger_data[1].tip_switch = data[i].On;       } break;
        case 9:  { finger_data[1].confidence = data[i].On;       } break;
        case 10: { finger_data[1].id         = data[i].RawValue; } break;
        case 11: { finger_data[1].x          = data[i].RawValue; } break;
        case 12: { finger_data[1].y          = data[i].RawValue; } break;

        // Finger 2
        case 13: { finger_data[2].tip_switch = data[i].On;       } break;
        case 14: { finger_data[2].confidence = data[i].On;       } break;
        case 15: { finger_data[2].id         = data[i].RawValue; } break;
        case 16: { finger_data[2].x          = data[i].RawValue; } break;
        case 17: { finger_data[2].y          = data[i].RawValue; } break;

        // Finger 3
        case 18: { finger_data[3].tip_switch = data[i].On;       } break;
        case 19: { finger_data[3].confidence = data[i].On;       } break;
        case 20: { finger_data[3].id         = data[i].RawValue; } break;
        case 21: { finger_data[3].x          = data[i].RawValue; } break;
        case 22: { finger_data[3].y          = data[i].RawValue; } break;

        // Finger 4
        case 23: { finger_data[4].tip_switch = data[i].On;       } break;
        case 24: { finger_data[4].confidence = data[i].On;       } break;
        case 25: { finger_data[4].id         = data[i].RawValue; } break;
        case 26: { finger_data[4].x          = data[i].RawValue; } break;
        case 27: { finger_data[4].y          = data[i].RawValue; } break;

        default: {
            fprintf(stderr, "unknown data index: %d found while filling data\n", data[i].DataIndex);
        } break;

        }
    }
}

#endif