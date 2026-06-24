#ifndef HID_TOUCHPAD_DEFS_H
#define HID_TOUCHPAD_DEFS_H


// ============================================================
//                       usage Page: UP_
// ============================================================

#define UP_GENERIC_DESKTOP 0x01
#define UP_BUTTON 0x09
#define UP_DIGITIZER 0x0D

// ============================================================
//                          usage: U_
// ============================================================

// Button Page (0x09)
#define U_BUTTON 0x01


// Digitizer Page (0x0D)
#define U_DIGITIZER_TIP_SWITCH 0x42    // Finger touching
#define U_DIGITIZER_CONFIDENCE 0x47    // Valid contact
#define U_DIGITIZER_CONTACT_ID 0x51    // Finger tracking ID
#define U_DIGITIZER_CONTACT_COUNT 0x54 // Active fingers
#define U_DIGITIZER_CONTACT_MAX 0x56   // Max supported fingers


// Generic Desktop Page (0x01)
#define U_GENERIC_X 0x30 // X coordinate
#define U_GENERIC_Y 0x31 // Y coordinate


// ============================================================
//                     Abstraction Layer
// ============================================================

#define U_FINGER_TIP U_DIGITIZER_TIP_SWITCH
#define U_FINGER_CONFIDENCE U_DIGITIZER_CONFIDENCE
#define U_FINGER_ID U_DIGITIZER_CONTACT_ID
#define U_FINGER_X U_GENERIC_X
#define U_FINGER_Y U_GENERIC_Y


#endif
