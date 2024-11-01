/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_DE_DE_STRINGS_TO_PROMPTS_DSMT_H_
#define DSMT_DE_DE_STRINGS_TO_PROMPTS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_flash_files.h"

const void * const prompts_ww_de[] = {
    AUDIO_WW_DETECTED, // "Hallo NXP"
    AUDIO_WW_DETECTED, // "Hallo NXP ^1"
};

const void * const prompts_change_demo_de[] = {
    AUDIO_OK_DE, // "Aufzug"
    AUDIO_OK_DE, // "Waschmaschine"
    AUDIO_OK_DE  // "Heimautomatisierung"
};

const void * const prompts_elevator_de[] = {
    AUDIO_FIRST_FLOOR_DE,    // "Erste etage"
    AUDIO_SECOND_FLOOR_DE,   // "Zweite etage"
    AUDIO_THIRD_FLOOR_DE,    // "Dritte etage"
    AUDIO_FOURTH_FLOOR_DE,   // "Vierte etage"
    AUDIO_FIFTH_FLOOR_DE,    // "Fünfte etage"
    AUDIO_MAIN_LOBBY_DE,     // "Hauptlobby"
    AUDIO_GROUND_FLOOR_DE,   // "Erdgeschoss"
    AUDIO_BASEMENT_FLOOR_DE, // "Untergeschoss"
    AUDIO_OPEN_DOOR_DE,      // "Öffne die tür"
    AUDIO_CLOSE_DOOR_DE,     // "Schließe die tür"
    AUDIO_DEMO_NAME_TEST_DE, // "Anwendung wechseln"
};

const void * const prompts_washing_machine_de[] = {
    AUDIO_DELICATE_DE,      // "Feinwäsche"
    AUDIO_NORMAL_DE,        // "Normaler"
    AUDIO_HEAVY_DUTY_DE,    // "Stark verschmutze"
    AUDIO_WHITES_DE,        // "Weiß"
    AUDIO_START_DE,         // "Starten"
    AUDIO_CANCEL_DE,        // "Abbrechen"
    AUDIO_DEMO_NAME_TEST_DE // "Anwendung wechseln"
};

const void * const prompts_smart_home_de[] = {
    AUDIO_TURN_ON_THE_LIGHTS_DE,  // "Licht einschalten"
    AUDIO_TURN_OFF_THE_LIGHTS_DE, // "Licht ausschalten"
    AUDIO_TEMPERATURE_HIGHER_DE,  // "Temperatur erhöhen"
    AUDIO_TEMPERATURE_LOWER_DE,   // "Temperatur verringern"
    AUDIO_OPEN_THE_WINDOW_DE,     // "Fenster hoch"
    AUDIO_CLOSE_THE_WINDOW_DE,    // "Fenster runter"
    AUDIO_MAKE_IT_BRIGHTER_DE,    // "Heller"
    AUDIO_MAKE_IT_DARKER_DE,      // "Dunkler"
    AUDIO_DEMO_NAME_TEST_DE,      // "Anwendung wechseln"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_DE_DE_STRINGS_TO_PROMPTS_DSMT_H_ */
