#ifndef _HOLDER_
#define _HOLDER_

#include "declaration.h"

/*
        max_contact_count
              |
structure : [-] [----][----][----][----][----]
                |___ fingers delta data ____|
*/


// assuming header = u32 (size of max_contact_count)
FingerDeltaData* create_holder(u32 data_size, u32 data_count) {
    char* ptr = (char*)malloc(sizeof(u32) + data_size * data_count);
    memset(ptr, 0, sizeof(u32) + data_size * data_count);
    return (FingerDeltaData*)(ptr + sizeof(u32));
}

u32* get_maxContactCount(FingerDeltaData* holder) {
    char* byte   = (char*)holder;
    u32*  header = (u32*)(byte - sizeof(u32));
    return header;
}

void free_holder(FingerDeltaData* holder) {
    char* byte = (char*)holder;
    free(byte - sizeof(u32));
}


#endif
