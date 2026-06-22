#ifndef _RAW_INPUT_
#define _RAW_INPUT_

#include "utils/declaration.h"
#include "utils/usage.h"
#include <winuser.h>


void getInputReportInfo(InputReportInfo* info) {
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

        USHORT pButtonCapsLen;
        pButtonCapsLen = info->pCaps->NumberInputButtonCaps;
        info->pButtonCaps = (HIDP_BUTTON_CAPS*)malloc(sizeof(HIDP_BUTTON_CAPS) * pButtonCapsLen);
        HidP_GetButtonCaps(HidP_Input, info->pButtonCaps, &pButtonCapsLen, info->ptrPreparsedData);

        ULONG linkNodeNumber;
        linkNodeNumber = info->pCaps->NumberLinkCollectionNodes;
        info->pLinkCollection = (HIDP_LINK_COLLECTION_NODE*)malloc(sizeof(HIDP_LINK_COLLECTION_NODE) * linkNodeNumber);
        HidP_GetLinkCollectionNodes(info->pLinkCollection, &linkNodeNumber, info->ptrPreparsedData);
    }else{
        printf("size of preparsed data is invalid\n");
    }
}


int getRawData(HWND window, LPARAM lParam) {
    Window_state* w_state = (Window_state*)GetWindowLongPtrW(window, GWLP_USERDATA);
    // allocat buffer if not allocated else reuse that
    static u32 prevRawInputSize = 0;

    u32 size;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
    u32 return_value = 0;

    if (size > prevRawInputSize) { // just in case size of raw input changed
        RAWINPUT* new_raw_input = (RAWINPUT*)realloc(w_state->raw_input, size);
        if (new_raw_input != nullptr)
            w_state->raw_input = new_raw_input;
        return_value = 1;
        prevRawInputSize = size;
    }

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (u8*)w_state->raw_input, &size, sizeof(RAWINPUTHEADER));
    return return_value;
    // return_value( gets feeded to if condtion ) will decide if getInputReportInfo should be called or not
}

[[maybe_unused]]
void getUsageValue_status(NTSTATUS status) {
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


void __getTouchPadInfoFile(InputReportInfo* info){
    FILE* file = fopen("touchpad_data.txt", "w");
    if(!file){
        printf("file can't be opened\n");
    }

    HIDP_CAPS* pCaps = info->pCaps;
    HIDP_VALUE_CAPS* pValueCaps = info->pValCaps;
    HIDP_BUTTON_CAPS* pButtonCaps = info->pButtonCaps;
    HIDP_LINK_COLLECTION_NODE* pLinkCollection = info->pLinkCollection;

    int numValCaps= pCaps->NumberInputValueCaps;
    int numButtonCaps = pCaps->NumberInputButtonCaps;
    int numLinkCollection = pCaps->NumberLinkCollectionNodes;


    fputs("\t\t\t\t\t****************  Caps  ****************\n\n", file);
    fprintf(file,
        "Usage : 0x%X\n"
        "Usage Page : 0x%X\n"
        "InputReportByteLength : %d\n"
        "NumberLinkCollectionNodes : %d\n"
        "NumberInputButtonCaps : %d\n"
        "NumberInputValueCaps : %d\n"
        "NumberInputDataIndices : %d\n\n",
        pCaps->Usage,
        pCaps->UsagePage,
        pCaps->InputReportByteLength,
        pCaps->NumberLinkCollectionNodes,
        pCaps->NumberInputButtonCaps,
        pCaps->NumberInputValueCaps,
        pCaps->NumberInputDataIndices);


    fputs("\n\n\t\t\t\t\t****************  link Collection  ****************\n\n", file);
    fprintf(file, "%-25s", "Field");
    for (int i = 0; i < numLinkCollection; i++) {
        char nodeHeader[16];
        sprintf(nodeHeader, "Node %d", i);
        fprintf(file, "%-12s", nodeHeader);
    }
    fprintf(file, "\n");

    fprintf(file, "--------------------------------------------------------------------------------\n");

    fprintf(file, "%-25s", "LinkUsage");
    for (int i = 0; i < numLinkCollection; i++) fprintf(file, "0x%-10X", pLinkCollection[i].LinkUsage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LinkUsagePage");
    for (int i = 0; i < numLinkCollection; i++) fprintf(file, "0x%-10X", pLinkCollection[i].LinkUsagePage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Parent");
    for (int i = 0; i < numLinkCollection; i++) fprintf(file, "%-12d", pLinkCollection[i].Parent);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NumberOfChildren");
    for (int i = 0; i < numLinkCollection; i++) fprintf(file, "%-12d", pLinkCollection[i].NumberOfChildren);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NextSibling");
    for (int i = 0; i < numLinkCollection; i++) fprintf(file, "%-12d", pLinkCollection[i].NextSibling);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "FirstChild");
    for (int i = 0; i < numLinkCollection; i++) fprintf(file, "%-12d", pLinkCollection[i].FirstChild);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "CollectionType");
    for (int i = 0; i < numLinkCollection; i++) fprintf(file, "0x%-10X", pLinkCollection[i].CollectionType);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsAlias");
    for (int i = 0; i < numLinkCollection; i++) fprintf(file, "%-12s", pLinkCollection[i].IsAlias ? "TRUE" : "FALSE");
    fprintf(file, "\n\n");





    fputs("\n\n\t\t\t\t\t****************  Value Caps  ****************\n\n", file);

    fprintf(file, "%-25s", "Field");
    for (int i = 0; i < numValCaps; i++) {
        char capHeader[16];
        sprintf(capHeader, "Cap %d", i);
        fprintf(file, "%-12s", capHeader);
    }
    fprintf(file, "\n");

    fprintf(file, "--------------------------------------------------------------------------------\n");

    fprintf(file, "%-25s", "UsagePage");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "0x%-10X", pValueCaps[i].UsagePage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "ReportID");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].ReportID);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsAlias");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12s", pValueCaps[i].IsAlias ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "BitField");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "0x%-10X", pValueCaps[i].BitField);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LinkCollection");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].LinkCollection);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LinkUsage");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "0x%-10X", pValueCaps[i].LinkUsage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LinkUsagePage");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "0x%-10X", pValueCaps[i].LinkUsagePage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsRange");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12s", pValueCaps[i].IsRange ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsStringRange");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12s", pValueCaps[i].IsStringRange ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsDesignatorRange");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12s", pValueCaps[i].IsDesignatorRange ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsAbsolute");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12s", pValueCaps[i].IsAbsolute ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "HasNull");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12s", pValueCaps[i].HasNull ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "BitSize");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].BitSize);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "ReportCount");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].ReportCount);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "UnitsExp");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12lu", pValueCaps[i].UnitsExp);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Units");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "0x%-10lX", pValueCaps[i].Units);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LogicalMin");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12ld", pValueCaps[i].LogicalMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LogicalMax");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12ld", pValueCaps[i].LogicalMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "PhysicalMin");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12ld", pValueCaps[i].PhysicalMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "PhysicalMax");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12ld", pValueCaps[i].PhysicalMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.UsageMin");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "0x%-10X", pValueCaps[i].Range.UsageMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.UsageMax");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "0x%-10X", pValueCaps[i].Range.UsageMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.StringMin");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].Range.StringMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.StringMax");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].Range.StringMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.DesignatorMin");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].Range.DesignatorMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.DesignatorMax");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].Range.DesignatorMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.DataIndexMin");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].Range.DataIndexMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.DataIndexMax");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].Range.DataIndexMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NotRange.Usage");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "0x%-10X", pValueCaps[i].NotRange.Usage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NotRange.StringIndex");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].NotRange.StringIndex);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NotRange.DesignatorIndex");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].NotRange.DesignatorIndex);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NotRange.DataIndex");
    for (int i = 0; i < numValCaps; i++) fprintf(file, "%-12d", pValueCaps[i].NotRange.DataIndex);
    fprintf(file, "\n\n");




    fputs("\n\n\t\t\t\t\t****************  Button Caps  ****************\n\n", file);
    fprintf(file, "%-25s", "Field");
    for (int i = 0; i < numButtonCaps; i++) {
        char capHeader[16];
        sprintf(capHeader, "Cap %d", i);
        fprintf(file, "%-12s", capHeader);
    }
    fprintf(file, "\n");

    fprintf(file, "--------------------------------------------------------------------------------\n");

    fprintf(file, "%-25s", "UsagePage");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "0x%-10X", pButtonCaps[i].UsagePage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "ReportID");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].ReportID);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsAlias");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12s", pButtonCaps[i].IsAlias ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "BitField");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "0x%-10X", pButtonCaps[i].BitField);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LinkCollection");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].LinkCollection);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LinkUsage");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "0x%-10X", pButtonCaps[i].LinkUsage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "LinkUsagePage");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "0x%-10X", pButtonCaps[i].LinkUsagePage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsRange");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12s", pButtonCaps[i].IsRange ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsStringRange");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12s", pButtonCaps[i].IsStringRange ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsDesignatorRange");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12s", pButtonCaps[i].IsDesignatorRange ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "IsAbsolute");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12s", pButtonCaps[i].IsAbsolute ? "TRUE" : "FALSE");
    fprintf(file, "\n");

    fprintf(file, "%-25s", "ReportCount");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].ReportCount);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.UsageMin");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "0x%-10X", pButtonCaps[i].Range.UsageMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.UsageMax");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "0x%-10X", pButtonCaps[i].Range.UsageMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.StringMin");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].Range.StringMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.StringMax");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].Range.StringMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.DesignatorMin");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].Range.DesignatorMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.DesignatorMax");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].Range.DesignatorMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.DataIndexMin");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].Range.DataIndexMin);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "Range.DataIndexMax");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].Range.DataIndexMax);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NotRange.Usage");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "0x%-10X", pButtonCaps[i].NotRange.Usage);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NotRange.StringIndex");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].NotRange.StringIndex);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NotRange.DesignatorIndex");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].NotRange.DesignatorIndex);
    fprintf(file, "\n");

    fprintf(file, "%-25s", "NotRange.DataIndex");
    for (int i = 0; i < numButtonCaps; i++) fprintf(file, "%-12d", pButtonCaps[i].NotRange.DataIndex);
    fprintf(file, "\n\n");
    fclose(file);
}

force_Inline static void getTouchPadInfoFile(InputReportInfo* info){
    static int flag = 1;
    if(flag){
        __getTouchPadInfoFile(info);
        flag = 0;
    }
}

#endif
