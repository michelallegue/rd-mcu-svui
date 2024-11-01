/*
 * Copyright 2023 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_DE_DE_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_DE_DE_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_de[] = {
    kWakeWord_Detected, // "Hallo NXP"
    kWakeWord_Detected, // "Hallo NXP ^1"
};

const int16_t actions_change_demo_de[] = {
    kChangeDemo_Elevator,       // "Aufzug"
    kChangeDemo_WashingMachine, // "Waschmaschine"
    kChangeDemo_SmartHome       // "Heimautomatisierung"
};

const int16_t actions_elevator_de[] = {
    kElevator_FirstFloor,    // "Erste etage"
    kElevator_SecondFloor,   // "Zweite etage"
    kElevator_ThirdFloor,    // "Dritte etage"
    kElevator_FourthFloor,   // "Vierte etage"
    kElevator_FifthFloor,    // "Fünfte etage"
    kElevator_MainLobby,     // "Hauptlobby"
    kElevator_GroundFloor,   // "Erdgeschoss"
    kElevator_BasementFloor, // "Untergeschoss"
    kElevator_OpenDoor,      // "Öffne die tür"
    kElevator_CloseDoor,     // "Schließe die tür"
    kElevator_ChangeDemo,    // "Anwendung wechseln"
};

const int16_t actions_washing_machine_de[] = {
    kWashingMachine_Delicate,  // "Feinwäsche"
    kWashingMachine_Normal,    // "Normaler"
    kWashingMachine_HeavyDuty, // "Stark verschmutze"
    kWashingMachine_Whites,    // "Weiß"
    kWashingMachine_Start,     // "Starten"
    kWashingMachine_Cancel,    // "Abbrechen"
    kWashingMachine_ChangeDemo // "Anwendung wechseln"
};

const int16_t actions_smart_home_de[] = {
    kSmartHome_TurnOnTheLights,   // "Licht einschalten"
    kSmartHome_TurnOffTheLights,  // "Licht ausschalten"
    kSmartHome_TemperatureHigher, // "Temperatur erhöhen"
    kSmartHome_TemperatureLower,  // "Temperatur verringern"
    kSmartHome_OpenTheWindow,     // "Fenster hoch"
    kSmartHome_CloseTheWindow,    // "Fenster runter"
    kSmartHome_MakeItBrighter,    // "Heller"
    kSmartHome_MakeItDarker,      // "Dunkler"
    kSmartHome_ChangeDemo,        // "Anwendung wechseln"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_DE_DE_STRINGS_TO_ACTIONS_DSMT_H_ */
