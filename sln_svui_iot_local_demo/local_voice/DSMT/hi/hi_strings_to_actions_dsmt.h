/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_HI_HI_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_HI_HI_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_hi[] = {
    kWakeWord_Detected, // "नमस्ते एनएक्सपी"
    kWakeWord_Detected, // "नमस्ते एनएक्सपी ^1"
};

const int16_t actions_change_demo_hi[] = {
    kChangeDemo_Elevator,       // "एलीवेटर"
    kChangeDemo_WashingMachine, // "वॉशिंग मशीन"
    kChangeDemo_SmartHome,      // "स्मार्टहोम"
};

const int16_t actions_elevator_hi[] = {
    kElevator_FirstFloor,    // "पहली मंजिल"
    kElevator_SecondFloor,   // "दूसरी मंजिल"
    kElevator_ThirdFloor,    // "तीसरी मंजिल"
    kElevator_FourthFloor,   // "चौथी मंजिल"
    kElevator_FifthFloor,    // "पांचवीं मंजिल"
    kElevator_MainLobby,     // "मेन लॉबी"
    kElevator_GroundFloor,   // "भूतल"
    kElevator_BasementFloor, // "बेसमेंट फ्लोर"
    kElevator_OpenDoor,      // "दरवाज़ा खोलें"
    kElevator_CloseDoor,     // "दरवाज़ा बंद करें"
    kElevator_ChangeDemo     // "डेमो बदलो"
};

const int16_t actions_washing_machine_hi[] = {
    kWashingMachine_Delicate,  // "डेलिकेट"
    kWashingMachine_Normal,    // "नॉर्मल"
    kWashingMachine_HeavyDuty, // "हैवी ड्यूटी"
    kWashingMachine_Whites,    // "व्हाइट्स"
    kWashingMachine_Start,     // "शुरू करें"
    kWashingMachine_Cancel,    // "रद्द करें"
    kWashingMachine_ChangeDemo // "डेमो बदलो"
};

const int16_t actions_smart_home_hi[] = {
    kSmartHome_TurnOnTheLights,   // "लाइट चालू करें"
    kSmartHome_TurnOffTheLights,  // "लाइट बंद करें"
    kSmartHome_TemperatureHigher, // "तापमान बढ़ाओ"
    kSmartHome_TemperatureLower,  // "तापमान कम करें"
    kSmartHome_OpenTheWindow,     // "खिड़की खोलो"
    kSmartHome_CloseTheWindow,    // "खिड़की बंद करो"
    kSmartHome_MakeItBrighter,    // "उजाला बढ़ाओ"
    kSmartHome_MakeItDarker,      // "उजाला कम करें"
    kSmartHome_ChangeDemo         // "डेमो बदलो"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_HI_HI_STRINGS_TO_ACTIONS_DSMT_H_ */
