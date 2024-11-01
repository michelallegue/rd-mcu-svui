/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_NL_NL_STRINGS_TO_PROMPTS_DSMT_H_
#define DSMT_NL_NL_STRINGS_TO_PROMPTS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_flash_files.h"

const void * const prompts_ww_nl[] = {
    AUDIO_WW_DETECTED, // "Goedendag NXP"
    AUDIO_WW_DETECTED, // "Goedendag NXP ^1"
};

const void * const prompts_change_demo_nl[] = {
    AUDIO_OK_NL, // "Lift"
    AUDIO_OK_NL, // "Wasmachine"
    AUDIO_OK_NL, // "Slimme woning"
};

const void * const prompts_elevator_nl[] = {
    AUDIO_FIRST_FLOOR_NL,    // "Eerste verdieping"
    AUDIO_SECOND_FLOOR_NL,   // "Tweede verdieping"
    AUDIO_THIRD_FLOOR_NL,    // "Derde verdieping"
    AUDIO_FOURTH_FLOOR_NL,   // "Vierde verdieping"
    AUDIO_FIFTH_FLOOR_NL,    // "Vijfde verdieping"
    AUDIO_MAIN_LOBBY_NL,     // "Hoofdlobby"
    AUDIO_GROUND_FLOOR_NL,   // "Begane grond"
    AUDIO_BASEMENT_FLOOR_NL, // "Kelder"
    AUDIO_OPEN_DOOR_NL,      // "Open de deur"
    AUDIO_CLOSE_DOOR_NL,     // "Doe de deur dicht"
    AUDIO_DEMO_NAME_TEST_NL  // "Verander de demo"
};

const void * const prompts_washing_machine_nl[] = {
    AUDIO_DELICATE_NL,      // "Delicaat"
    AUDIO_NORMAL_NL,        // "Normaal"
    AUDIO_HEAVY_DUTY_NL,    // "Intensieve was"
    AUDIO_WHITES_NL,        // "Witte was",
    AUDIO_START_NL,         // "Start"
    AUDIO_CANCEL_NL,        // "Annuleer"
    AUDIO_DEMO_NAME_TEST_NL // "Verander de demo"
};

const void * const prompts_smart_home_nl[] = {
    AUDIO_TURN_ON_THE_LIGHTS_NL,  // "Doe de lichten aan"
    AUDIO_TURN_OFF_THE_LIGHTS_NL, // "Doe de lichten uit"
    AUDIO_TEMPERATURE_HIGHER_NL,  // "Temperatuur hoger"
    AUDIO_TEMPERATURE_LOWER_NL,   // "Temperatuur lager"
    AUDIO_OPEN_THE_WINDOW_NL,     // "Open het raam"
    AUDIO_CLOSE_THE_WINDOW_NL,    // "Doe het raam dicht"
    AUDIO_MAKE_IT_BRIGHTER_NL,    // "Maak het helderder"
    AUDIO_MAKE_IT_DARKER_NL,      // "Maak het donkerder"
    AUDIO_DEMO_NAME_TEST_NL       // "Verander de demo"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_NL_NL_STRINGS_TO_PROMPTS_DSMT_H_ */
