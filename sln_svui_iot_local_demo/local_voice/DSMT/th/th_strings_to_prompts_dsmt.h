/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_TH_TH_STRINGS_TO_PROMPTS_DSMT_H_
#define DSMT_TH_TH_STRINGS_TO_PROMPTS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_flash_files.h"

const void * const prompts_ww_th[] = {
    AUDIO_WW_DETECTED, // "สวัสดี เอ็นเอ็กซ์พี"
    AUDIO_WW_DETECTED, // "สวัสดี เอ็นเอ็กซ์พี ^1"
};

const void * const prompts_change_demo_th[] = {
    AUDIO_OK_TH, // "ลิฟต์"
    AUDIO_OK_TH, // "เครื่องซักผ้า"
    AUDIO_OK_TH, // "สมาร์ทโฮม"
};

const void * const prompts_elevator_th[] = {
    AUDIO_FIRST_FLOOR_TH,    // "ชั้นหนึ่ง"
    AUDIO_SECOND_FLOOR_TH,   // "ชั้นสอง"
    AUDIO_THIRD_FLOOR_TH,    // "ชั้นที่สาม"
    AUDIO_FOURTH_FLOOR_TH,   // "ชั้นสี่"
    AUDIO_FIFTH_FLOOR_TH,    // "ชั้นห้า"
    AUDIO_MAIN_LOBBY_TH,     // "ล็อบบี้หลัก"
    AUDIO_GROUND_FLOOR_TH,   // "ชั้นล่าง"
    AUDIO_BASEMENT_FLOOR_TH, // "ชั้นใต้ดิน"
    AUDIO_OPEN_DOOR_TH,      // "เปิดประตู"
    AUDIO_CLOSE_DOOR_TH,     // "ปิดประตู"
    AUDIO_DEMO_NAME_TEST_TH  // "เปลี่ยนการสาธิต"
};

const void * const prompts_washing_machine_th[] = {
    AUDIO_DELICATE_TH,      // "ผ้าบอบบาง"
    AUDIO_NORMAL_TH,        // "ปกติ"
    AUDIO_HEAVY_DUTY_TH,    // "ผ้าหนา"
    AUDIO_WHITES_TH,        // "ผ้าขาว",
    AUDIO_START_TH,         // "เริ่ม"
    AUDIO_CANCEL_TH,        // "ยกเลิก"
    AUDIO_DEMO_NAME_TEST_TH // "เปลี่ยนการสาธิต"
};

const void * const prompts_smart_home_th[] = {
    AUDIO_TURN_ON_THE_LIGHTS_TH,  // "เปิดไฟ"
    AUDIO_TURN_OFF_THE_LIGHTS_TH, // "ปิดไฟ"
    AUDIO_TEMPERATURE_HIGHER_TH,  // "อุณหภูมิสูงขึ้น"
    AUDIO_TEMPERATURE_LOWER_TH,   // "อุณหภูมิต่ำลง"
    AUDIO_OPEN_THE_WINDOW_TH,     // "เปิดหน้าต่าง"
    AUDIO_CLOSE_THE_WINDOW_TH,    // "ปิดหน้าต่าง"
    AUDIO_MAKE_IT_BRIGHTER_TH,    // "ไฟสว่างขึ้น"
    AUDIO_MAKE_IT_DARKER_TH,      // "หรี่ไฟลง"
    AUDIO_DEMO_NAME_TEST_TH       // "เปลี่ยนการสาธิต"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_TH_TH_STRINGS_TO_PROMPTS_DSMT_H_ */
