/*
 * Copyright 2023 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_DEMO_ACTIONS_DSMT_H_
#define DSMT_DEMO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

/* Global actions for all languages */

enum _wake_word_action
{
    kWakeWord_Detected = 0,
    kWakeWord_ActionInvalid
};

enum _change_demo_action
{
    kChangeDemo_Elevator = 0,
    kChangeDemo_WashingMachine,
    kChangeDemo_SmartHome,
    kChangeDemo_ActionInvalid
};

enum _elevator_action
{
    kElevator_FirstFloor = 0,
    kElevator_SecondFloor,
    kElevator_ThirdFloor,
    kElevator_FourthFloor,
    kElevator_FifthFloor,
    kElevator_MainLobby,
    kElevator_GroundFloor,
    kElevator_BasementFloor,
    kElevator_OpenDoor,
    kElevator_CloseDoor,
    kElevator_ChangeDemo,
    kElevator_ActionInvalid
};

enum _washing_machine_action
{
    kWashingMachine_Delicate = 0,
    kWashingMachine_Normal,
    kWashingMachine_HeavyDuty,
    kWashingMachine_Whites,
    kWashingMachine_Start,
    kWashingMachine_Cancel,
    kWashingMachine_ChangeDemo,
    kWashingMachine_ActionInvalid
};

enum _smart_home_action
{
    kSmartHome_TurnOnTheLights = 0,
    kSmartHome_TurnOffTheLights,
    kSmartHome_TemperatureHigher,
    kSmartHome_TemperatureLower,
    kSmartHome_OpenTheWindow,
    kSmartHome_CloseTheWindow,
    kSmartHome_MakeItBrighter,
    kSmartHome_MakeItDarker,
    kSmartHome_ChangeDemo,
    kSmartHome_ActionInvalid
};
#endif /* ENABLE_DSMT_ASR */

#endif /* DSMT_DEMO_ACTIONS_DSMT_H_ */
