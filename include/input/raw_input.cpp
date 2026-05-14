#ifndef _RAW_INPUT_
#define _RAW_INPUT_


void Win32_getInputReportInfo(Win32_InputReportInfo* info) {
    // allocat buffer if not allocated else reuse that

    u32 pdataSize;
    GetRawInputDeviceInfo(info->deviceHandle, RIDI_PREPARSEDDATA, NULL, &pdataSize);

    if (pdataSize > 0) {
        info->ptrPreparsedData = (PHIDP_PREPARSED_DATA)malloc(pdataSize);
        GetRawInputDeviceInfo(info->deviceHandle, RIDI_PREPARSEDDATA, info->ptrPreparsedData, &pdataSize);

        info->pCaps = (HIDP_CAPS*)malloc(sizeof(HIDP_CAPS));
        HidP_GetCaps(info->ptrPreparsedData, info->pCaps);

        USHORT pValCapsLen;
        pValCapsLen = info->pCaps->NumberInputValueCaps;
        info->pValCaps = (HIDP_VALUE_CAPS*)malloc(sizeof(HIDP_VALUE_CAPS) * pValCapsLen);
        HidP_GetValueCaps(HidP_Input, info->pValCaps, &pValCapsLen, info->ptrPreparsedData);

        USHORT pButtopnCapsLen;
        pButtopnCapsLen = info->pCaps->NumberInputButtonCaps;
        info->pButtopnCaps = (HIDP_BUTTON_CAPS*)malloc(sizeof(HIDP_BUTTON_CAPS) * pButtopnCapsLen);
        HidP_GetButtonCaps(HidP_Input, info->pButtopnCaps, &pButtopnCapsLen, info->ptrPreparsedData);

        ULONG linkNodeNumber;
        linkNodeNumber = info->pCaps->NumberLinkCollectionNodes;
        info->pLinkCollection = (HIDP_LINK_COLLECTION_NODE*)malloc(sizeof(HIDP_LINK_COLLECTION_NODE) * linkNodeNumber);
        HidP_GetLinkCollectionNodes(info->pLinkCollection, &linkNodeNumber, info->ptrPreparsedData);
    }else{
        printf("size of preparsed data is invalid\n");
    }
}


int Win32_getRawData(LPARAM lParam) {
    // allocat buffer if not allocated else reuse that
    static u32 prevRawInputSize = 0;

    u32 size;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
    u32 return_value = 0;

    if (size > prevRawInputSize) { // just in case size of raw input changed
        RAWINPUT* new_globalRawInput = (RAWINPUT*)realloc(globalRawInput, size);
        if (new_globalRawInput != nullptr)
            globalRawInput = new_globalRawInput;
        return_value = 1;
        prevRawInputSize = size;
    }

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (u8*)globalRawInput, &size, sizeof(RAWINPUTHEADER));
    return return_value;
    // return_value( gets feeded to if condtion ) will decide if win32_getInputReportInfo should be called or not
}


void win32_getUsageValue_status(NTSTATUS status) {
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

#endif