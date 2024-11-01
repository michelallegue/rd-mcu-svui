/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_VN_VN_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_VN_VN_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_vn[] = {
    kWakeWord_Detected, // "Xin chào NXP"
    kWakeWord_Detected, // "Xin chào NXP ^1"
    kWakeWord_Detected, // "Xin chào NXP ^2"
    kWakeWord_Detected, // "Xin chào NXP ^3"
};

const int16_t actions_change_demo_vn[] = {
    kChangeDemo_Elevator,       // "Thang máy"
    kChangeDemo_WashingMachine, // "Máy giặt"
    kChangeDemo_SmartHome,      // "Nhà thông minh"
};

const int16_t actions_elevator_vn[] = {
    kElevator_FirstFloor,    // "Tầng một"
    kElevator_SecondFloor,   // "Tầng hai"
    kElevator_ThirdFloor,    // "Tầng ba"
    kElevator_FourthFloor,   // "Tầng bốn"
    kElevator_FifthFloor,    // "Tầng năm"
    kElevator_MainLobby,     // "Hành lang chính"
    kElevator_GroundFloor,   // "Tầng trệt"
    kElevator_BasementFloor, // "Tầng hầm"
    kElevator_OpenDoor,      // "Mở cửa"
    kElevator_CloseDoor,     // "Đóng cửa"
    kElevator_ChangeDemo     // "Thay đổi bản thử nghiệm"
};

const int16_t actions_washing_machine_vn[] = {
    kWashingMachine_Delicate,  // "Đồ mỏng nhẹ"
    kWashingMachine_Normal,    // "Thông thường"
    kWashingMachine_HeavyDuty, // "Chế độ giặt mạnh"
    kWashingMachine_Whites,    // "Đồ màu trắng"
    kWashingMachine_Start,     // "Bắt đầu"
    kWashingMachine_Cancel,    // "Hủy bỏ"
    kWashingMachine_ChangeDemo // "Thay đổi bản thử nghiệm"
};

const int16_t actions_smart_home_vn[] = {
    kSmartHome_TurnOnTheLights,   // "Bật đèn"
    kSmartHome_TurnOffTheLights,  // "Tắt đèn"
    kSmartHome_TemperatureHigher, // "Nhiệt độ cao hơn"
    kSmartHome_TemperatureLower,  // "Nhiệt độ thấp hơn"
    kSmartHome_OpenTheWindow,     // "Mở cửa sổ"
    kSmartHome_CloseTheWindow,    // "Đóng cửa sổ"
    kSmartHome_MakeItBrighter,    // "Tăng độ sáng của đèn"
    kSmartHome_MakeItDarker,      // "Giảm độ sáng của đèn"
    kSmartHome_ChangeDemo         // "Thay đổi bản thử nghiệm"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_VN_VN_STRINGS_TO_ACTIONS_DSMT_H_ */
