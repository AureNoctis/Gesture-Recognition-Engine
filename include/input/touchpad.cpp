#ifndef _TOUCHPAD_TEST_
#define _TOUCHPAD_TEST_


void win32_printTouchpadData(PHIDP_PREPARSED_DATA preparsedData, RAWINPUT* raw) {

    i8* report = (i8*)raw->data.hid.bRawData;
    u32 reportLen = raw->data.hid.dwSizeHid;

    // Get Contact Count
    u32 contactCount = 0;
    HidP_GetUsageValue(
        HidP_Input,
        UP_DIGITIZER,
        0,
        U_DIGITIZER_CONTACT_COUNT,
        (unsigned long*)&contactCount,
        preparsedData,
        (char*)report,
        reportLen
    );

    u16 buttonUsageList[1];
    u32 buttonUsageLength = 1;
    u16 button_flag = 9999;
    HidP_GetUsages(
        HidP_Input,
        UP_BUTTON,          // usage page 0x09
        0,                  // link collection 0
        buttonUsageList,
        (unsigned long*)&buttonUsageLength,
        preparsedData,
        (char*)report,
        reportLen
    );

    button_flag = (buttonUsageLength > 0 && buttonUsageList[0] == U_BUTTON) ? 1 : 0;

    printf("-------------------------------------------------\n");
    printf("Contact Count : %lu\n", contactCount);
    printf("button flag : %hu\n\n", button_flag);


    // Header with large spacing
    printf("Finger        Tip        Confidence        ID        X              Y\n");

    // Iterate Fingers
    for (u32 link = 1; link <= globalInputReportInfo.pCaps->NumberLinkCollectionNodes - 1; link++)
    {
        u16 tip = 0;
        u16 conf = 0;
        u32 id = 9999;
        u32 x = 9999;
        u32 y = 9999;

        u16 usageList[2];
        u32 usageLength = 2;

        // for buttons:
        HidP_GetUsages(HidP_Input, UP_DIGITIZER, link, usageList, (unsigned long*)&usageLength,
            preparsedData, (char*)report, reportLen);

        if (usageList[0] == U_FINGER_TIP)        tip = 1;
        if (usageList[1] == U_FINGER_CONFIDENCE) conf = 1;

        // for usage values:
        HidP_GetUsageValue(HidP_Input, UP_DIGITIZER, link, U_FINGER_ID,
            (unsigned long*)&id, preparsedData, (char*)report, reportLen);

        HidP_GetUsageValue(HidP_Input, UP_GENERIC_DESKTOP, link, U_FINGER_X,
            (unsigned long*)&x, preparsedData, (char*)report, reportLen);

        HidP_GetUsageValue(HidP_Input, UP_GENERIC_DESKTOP, link, U_FINGER_Y,
            (unsigned long*)&y, preparsedData, (char*)report, reportLen);

        printf("F%-2lu          %-3hu        %-10hu        %-3lu       %-10lu     %-10lu\n",
            link, tip, conf, id, x, y);
    }
}


#endif