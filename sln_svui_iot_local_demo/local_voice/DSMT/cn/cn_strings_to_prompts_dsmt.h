/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_CN_CN_STRINGS_TO_PROMPTS_DSMT_H_
#define DSMT_CN_CN_STRINGS_TO_PROMPTS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_flash_files.h"

const void * const prompts_ww_cn[] = {
    AUDIO_WW_DETECTED, // "你好， 恩智浦"
    AUDIO_WW_DETECTED, // "你好， 恩智浦 ^1"
    AUDIO_WW_DETECTED, // "你好， 恩智浦 ^2"
    AUDIO_WW_DETECTED, // "你好， 恩智浦 ^3"
    AUDIO_WW_DETECTED, // "你好， 恩智浦 ^4"
    AUDIO_WW_DETECTED, // "你好， 恩智浦 ^5"
    AUDIO_WW_DETECTED, // "你好， 恩智浦 ^6"
    AUDIO_WW_DETECTED, // "你好， 恩智浦 ^7"
};

const void * const prompts_change_demo_cn[] = {
    AUDIO_OK_CN, // "电梯"
    AUDIO_OK_CN, // "洗衣机"
    AUDIO_OK_CN  // "智能家居"
};

const void * const prompts_elevator_cn[] = {
    AUDIO_FIRST_FLOOR_CN,    // "到一楼"
    AUDIO_SECOND_FLOOR_CN,   // "到二楼"
    AUDIO_THIRD_FLOOR_CN,    // "到三楼"
    AUDIO_FOURTH_FLOOR_CN,   // "到四楼"
    AUDIO_FIFTH_FLOOR_CN,    // "到五楼"
    AUDIO_MAIN_LOBBY_CN,     // "大堂"
    AUDIO_GROUND_FLOOR_CN,   // "负一楼"
    AUDIO_BASEMENT_FLOOR_CN, // "地下室"
    AUDIO_OPEN_DOOR_CN,      // "开门"
    AUDIO_CLOSE_DOOR_CN,     // "关门"
    AUDIO_DEMO_NAME_TEST_CN, // "选择应用"
};

const void * const prompts_washing_machine_cn[] = {
    AUDIO_DELICATE_CN,       // "精致模式"
    AUDIO_NORMAL_CN,         // "正常模式"
    AUDIO_HEAVY_DUTY_CN,     // "强力模式"
    AUDIO_WHITES_CN,         // "洗白模式"
    AUDIO_WHITES_CN,         // "洗白模式 ^1"
    AUDIO_START_CN,          // "开始"
    AUDIO_CANCEL_CN,         // "取消"
    AUDIO_DEMO_NAME_TEST_CN, // "选择应用"
};

const void * const prompts_smart_home_cn[] = {
    AUDIO_TURN_ON_THE_LIGHTS_CN,  // "打开灯"
    AUDIO_TURN_OFF_THE_LIGHTS_CN, // "关闭灯"
    AUDIO_TEMPERATURE_HIGHER_CN,  // "升高温度"
    AUDIO_TEMPERATURE_LOWER_CN,   // "降低温度"
    AUDIO_OPEN_THE_WINDOW_CN,     // "打开窗帘"
    AUDIO_CLOSE_THE_WINDOW_CN,    // "关闭窗帘"
    AUDIO_MAKE_IT_BRIGHTER_CN,    // "亮一点"
    AUDIO_MAKE_IT_DARKER_CN,      // "暗一点"
    AUDIO_DEMO_NAME_TEST_CN,      // "选择应用"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_CN_CN_STRINGS_TO_PROMPTS_DSMT_H_ */
