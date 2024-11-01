/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_CN_CN_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_CN_CN_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_cn[] = {
    kWakeWord_Detected, // "你好， 恩智浦"
    kWakeWord_Detected, // "你好， 恩智浦 ^1"
    kWakeWord_Detected, // "你好， 恩智浦 ^2"
    kWakeWord_Detected, // "你好， 恩智浦 ^3"
    kWakeWord_Detected, // "你好， 恩智浦 ^4"
    kWakeWord_Detected, // "你好， 恩智浦 ^5"
    kWakeWord_Detected, // "你好， 恩智浦 ^6"
    kWakeWord_Detected, // "你好， 恩智浦 ^7"
};

const int16_t actions_change_demo_cn[] = {
    kChangeDemo_Elevator,       // "电梯"
    kChangeDemo_WashingMachine, // "洗衣机"
    kChangeDemo_SmartHome       // "智能家居"
};

const int16_t actions_elevator_cn[] = {
    kElevator_FirstFloor,    // "到一楼"
    kElevator_SecondFloor,   // "到二楼"
    kElevator_ThirdFloor,    // "到三楼"
    kElevator_FourthFloor,   // "到四楼"
    kElevator_FifthFloor,    // "到五楼"
    kElevator_MainLobby,     // "大堂"
    kElevator_GroundFloor,   // "负一楼"
    kElevator_BasementFloor, // "地下室"
    kElevator_OpenDoor,      // "开门"
    kElevator_CloseDoor,     // "关门"
    kElevator_ChangeDemo,    // "选择应用"
};

const int16_t actions_washing_machine_cn[] = {
    kWashingMachine_Delicate,   // "精致模式"
    kWashingMachine_Normal,     // "正常模式"
    kWashingMachine_HeavyDuty,  // "强力模式"
    kWashingMachine_Whites,     // "洗白模式"
    kWashingMachine_Whites,     // "洗白模式 ^1"
    kWashingMachine_Start,      // "开始"
    kWashingMachine_Cancel,     // "取消"
    kWashingMachine_ChangeDemo, // "选择应用"
};

const int16_t actions_smart_home_cn[] = {
    kSmartHome_TurnOnTheLights,   // "打开灯"
    kSmartHome_TurnOffTheLights,  // "关闭灯"
    kSmartHome_TemperatureHigher, // "升高温度"
    kSmartHome_TemperatureLower,  // "降低温度"
    kSmartHome_OpenTheWindow,     // "打开窗帘"
    kSmartHome_CloseTheWindow,    // "关闭窗帘"
    kSmartHome_MakeItBrighter,    // "亮一点"
    kSmartHome_MakeItDarker,      // "暗一点"
    kSmartHome_ChangeDemo,        // "选择应用"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_CN_CN_STRINGS_TO_ACTIONS_DSMT_H_ */
