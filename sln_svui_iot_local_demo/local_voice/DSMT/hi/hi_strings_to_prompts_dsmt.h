/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_HI_HI_STRINGS_TO_PROMPTS_DSMT_H_
#define DSMT_HI_HI_STRINGS_TO_PROMPTS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_flash_files.h"

const void * const prompts_ww_hi[] = {
    AUDIO_WW_DETECTED, // "नमस्ते एनएक्सपी"
    AUDIO_WW_DETECTED, // "नमस्ते एनएक्सपी ^1"
};

const void * const prompts_change_demo_hi[] = {
    AUDIO_OK_HI, // "एलीवेटर"
    AUDIO_OK_HI, // "वॉशिंग मशीन"
    AUDIO_OK_HI, // "स्मार्टहोम"
};

const void * const prompts_elevator_hi[] = {
    AUDIO_FIRST_FLOOR_HI,    // "पहली मंजिल"
    AUDIO_SECOND_FLOOR_HI,   // "दूसरी मंजिल"
    AUDIO_THIRD_FLOOR_HI,    // "तीसरी मंजिल"
    AUDIO_FOURTH_FLOOR_HI,   // "चौथी मंजिल"
    AUDIO_FIFTH_FLOOR_HI,    // "पांचवीं मंजिल"
    AUDIO_MAIN_LOBBY_HI,     // "मेन लॉबी"
    AUDIO_GROUND_FLOOR_HI,   // "भूतल"
    AUDIO_BASEMENT_FLOOR_HI, // "बेसमेंट फ्लोर"
    AUDIO_OPEN_DOOR_HI,      // "दरवाज़ा खोलें"
    AUDIO_CLOSE_DOOR_HI,     // "दरवाज़ा बंद करें"
    AUDIO_DEMO_NAME_TEST_HI  // "डेमो बदलो"
};

const void * const prompts_washing_machine_hi[] = {
    AUDIO_DELICATE_HI,      // "डेलिकेट"
    AUDIO_NORMAL_HI,        // "नॉर्मल"
    AUDIO_HEAVY_DUTY_HI,    // "हैवी ड्यूटी"
    AUDIO_WHITES_HI,        // "व्हाइट्स",
    AUDIO_START_HI,         // "शुरू करें"
    AUDIO_CANCEL_HI,        // "रद्द करें"
    AUDIO_DEMO_NAME_TEST_HI // "डेमो बदलो"
};

const void * const prompts_smart_home_hi[] = {
    AUDIO_TURN_ON_THE_LIGHTS_HI,  // "लाइट चालू करें"
    AUDIO_TURN_OFF_THE_LIGHTS_HI, // "लाइट बंद करें"
    AUDIO_TEMPERATURE_HIGHER_HI,  // "तापमान बढ़ाओ"
    AUDIO_TEMPERATURE_LOWER_HI,   // "तापमान कम करें"
    AUDIO_OPEN_THE_WINDOW_HI,     // "खिड़की खोलो"
    AUDIO_CLOSE_THE_WINDOW_HI,    // "खिड़की बंद करो"
    AUDIO_MAKE_IT_BRIGHTER_HI,    // "उजाला बढ़ाओ"
    AUDIO_MAKE_IT_DARKER_HI,      // "उजाला कम करें"
    AUDIO_DEMO_NAME_TEST_HI       // "डेमो बदलो"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_HI_HI_STRINGS_TO_PROMPTS_DSMT_H_ */
