/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_NL_NL_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_NL_NL_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_nl[] = {
    kWakeWord_Detected, // "Goedendag NXP"
    kWakeWord_Detected, // "Goedendag NXP ^1"
};

const int16_t actions_change_demo_nl[] = {
    kChangeDemo_Elevator,       // "Lift"
    kChangeDemo_WashingMachine, // "Wasmachine"
    kChangeDemo_SmartHome,      // "Slimme woning"
};

const int16_t actions_elevator_nl[] = {
    kElevator_FirstFloor,    // "Eerste verdieping"
    kElevator_SecondFloor,   // "Tweede verdieping"
    kElevator_ThirdFloor,    // "Derde verdieping"
    kElevator_FourthFloor,   // "Vierde verdieping"
    kElevator_FifthFloor,    // "Vijfde verdieping"
    kElevator_MainLobby,     // "Hoofdlobby"
    kElevator_GroundFloor,   // "Begane grond"
    kElevator_BasementFloor, // "Kelder"
    kElevator_OpenDoor,      // "Open de deur"
    kElevator_CloseDoor,     // "Doe de deur dicht"
    kElevator_ChangeDemo     // "Verander de demo"
};

const int16_t actions_washing_machine_nl[] = {
    kWashingMachine_Delicate,  // "Delicaat"
    kWashingMachine_Normal,    // "Normaal"
    kWashingMachine_HeavyDuty, // "Intensieve was"
    kWashingMachine_Whites,    // "Witte was"
    kWashingMachine_Start,     // "Start"
    kWashingMachine_Cancel,    // "Annuleer"
    kWashingMachine_ChangeDemo // "Verander de demo"
};

const int16_t actions_smart_home_nl[] = {
    kSmartHome_TurnOnTheLights,   // "Doe de lichten aan"
    kSmartHome_TurnOffTheLights,  // "Doe de lichten uit"
    kSmartHome_TemperatureHigher, // "Temperatuur hoger"
    kSmartHome_TemperatureLower,  // "Temperatuur lager"
    kSmartHome_OpenTheWindow,     // "Open het raam"
    kSmartHome_CloseTheWindow,    // "Doe het raam dicht"
    kSmartHome_MakeItBrighter,    // "Maak het helderder"
    kSmartHome_MakeItDarker,      // "Maak het donkerder"
    kSmartHome_ChangeDemo         // "Verander de demo"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_NL_NL_STRINGS_TO_ACTIONS_DSMT_H_ */
