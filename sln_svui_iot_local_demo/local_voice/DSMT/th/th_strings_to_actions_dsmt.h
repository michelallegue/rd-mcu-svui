/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_TH_TH_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_TH_TH_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_th[] = {
    kWakeWord_Detected, // "สวัสดี เอ็นเอ็กซ์พี"
    kWakeWord_Detected, // "สวัสดี เอ็นเอ็กซ์พี ^1"
};

const int16_t actions_change_demo_th[] = {
    kChangeDemo_Elevator,       // "ลิฟต์"
    kChangeDemo_WashingMachine, // "เครื่องซักผ้า"
    kChangeDemo_SmartHome,      // "สมาร์ทโฮม"
};

const int16_t actions_elevator_th[] = {
    kElevator_FirstFloor,    // "ชั้นหนึ่ง"
    kElevator_SecondFloor,   // "ชั้นสอง"
    kElevator_ThirdFloor,    // "ชั้นที่สาม"
    kElevator_FourthFloor,   // "ชั้นสี่"
    kElevator_FifthFloor,    // "ชั้นห้า"
    kElevator_MainLobby,     // "ล็อบบี้หลัก"
    kElevator_GroundFloor,   // "ชั้นล่าง"
    kElevator_BasementFloor, // "ชั้นใต้ดิน"
    kElevator_OpenDoor,      // "เปิดประตู"
    kElevator_CloseDoor,     // "ปิดประตู"
    kElevator_ChangeDemo     // "เปลี่ยนการสาธิต"
};

const int16_t actions_washing_machine_th[] = {
    kWashingMachine_Delicate,  // "ผ้าบอบบาง"
    kWashingMachine_Normal,    // "ปกติ"
    kWashingMachine_HeavyDuty, // "ผ้าหนา"
    kWashingMachine_Whites,    // "ผ้าขาว"
    kWashingMachine_Start,     // "เริ่ม"
    kWashingMachine_Cancel,    // "ยกเลิก"
    kWashingMachine_ChangeDemo // "เปลี่ยนการสาธิต"
};

const int16_t actions_smart_home_th[] = {
    kSmartHome_TurnOnTheLights,   // "เปิดไฟ"
    kSmartHome_TurnOffTheLights,  // "ปิดไฟ"
    kSmartHome_TemperatureHigher, // "อุณหภูมิสูงขึ้น"
    kSmartHome_TemperatureLower,  // "อุณหภูมิต่ำลง"
    kSmartHome_OpenTheWindow,     // "เปิดหน้าต่าง"
    kSmartHome_CloseTheWindow,    // "ปิดหน้าต่าง"
    kSmartHome_MakeItBrighter,    // "ไฟสว่างขึ้น"
    kSmartHome_MakeItDarker,      // "หรี่ไฟลง"
    kSmartHome_ChangeDemo         // "เปลี่ยนการสาธิต"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_TH_TH_STRINGS_TO_ACTIONS_DSMT_H_ */
