/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_VN_VN_STRINGS_TO_PROMPTS_DSMT_H_
#define DSMT_VN_VN_STRINGS_TO_PROMPTS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_flash_files.h"

const void * const prompts_ww_vn[] = {
    AUDIO_WW_DETECTED, // "Xin chào NXP"
    AUDIO_WW_DETECTED, // "Xin chào NXP ^1"
    AUDIO_WW_DETECTED, // "Xin chào NXP ^2"
    AUDIO_WW_DETECTED, // "Xin chào NXP ^3"
};

const void * const prompts_change_demo_vn[] = {
    AUDIO_OK_VN, // "Thang máy"
    AUDIO_OK_VN, // "Máy giặt"
    AUDIO_OK_VN, // "Nhà thông minh"
};

const void * const prompts_elevator_vn[] = {
    AUDIO_FIRST_FLOOR_VN,    // "Tầng một"
    AUDIO_SECOND_FLOOR_VN,   // "Tầng hai"
    AUDIO_THIRD_FLOOR_VN,    // "Tầng ba"
    AUDIO_FOURTH_FLOOR_VN,   // "Tầng bốn"
    AUDIO_FIFTH_FLOOR_VN,    // "Tầng năm"
    AUDIO_MAIN_LOBBY_VN,     // "Hành lang chính"
    AUDIO_GROUND_FLOOR_VN,   // "Tầng trệt"
    AUDIO_BASEMENT_FLOOR_VN, // "Tầng hầm"
    AUDIO_OPEN_DOOR_VN,      // "Mở cửa"
    AUDIO_CLOSE_DOOR_VN,     // "Đóng cửa"
    AUDIO_DEMO_NAME_TEST_VN  // "Thay đổi bản thử nghiệm"
};

const void * const prompts_washing_machine_vn[] = {
    AUDIO_DELICATE_VN,      // "Đồ mỏng nhẹ"
    AUDIO_NORMAL_VN,        // "Thông thường"
    AUDIO_HEAVY_DUTY_VN,    // "Chế độ giặt mạnh"
    AUDIO_WHITES_VN,        // "Đồ màu trắng",
    AUDIO_START_VN,         // "Bắt đầu"
    AUDIO_CANCEL_VN,        // "Hủy bỏ"
    AUDIO_DEMO_NAME_TEST_VN // "Thay đổi bản thử nghiệm"
};

const void * const prompts_smart_home_vn[] = {
    AUDIO_TURN_ON_THE_LIGHTS_VN,  // "Bật đèn"
    AUDIO_TURN_OFF_THE_LIGHTS_VN, // "Tắt đèn"
    AUDIO_TEMPERATURE_HIGHER_VN,  // "Nhiệt độ cao hơn"
    AUDIO_TEMPERATURE_LOWER_VN,   // "Nhiệt độ thấp hơn"
    AUDIO_OPEN_THE_WINDOW_VN,     // "Mở cửa sổ"
    AUDIO_CLOSE_THE_WINDOW_VN,    // "Đóng cửa sổ"
    AUDIO_MAKE_IT_BRIGHTER_VN,    // "Tăng độ sáng của đèn"
    AUDIO_MAKE_IT_DARKER_VN,      // "Giảm độ sáng của đèn"
    AUDIO_DEMO_NAME_TEST_VN       // "Thay đổi bản thử nghiệm"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_VN_VN_STRINGS_TO_PROMPTS_DSMT_H_ */
