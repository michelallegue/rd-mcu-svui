/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_CN_CN_STRINGS_DSMT_H_
#define DSMT_CN_CN_STRINGS_DSMT_H_

#if ENABLE_DSMT_ASR

/* Chinese demos strings below.
 * These must match the DSMTv2 groups */

const char * const ww_cn[] = {
        "你好， 恩智浦",
        "你好， 恩智浦 ^1",
        "你好， 恩智浦 ^2",
        "你好， 恩智浦 ^3",
        "你好， 恩智浦 ^4",
        "你好， 恩智浦 ^5",
        "你好， 恩智浦 ^6",
        "你好， 恩智浦 ^7",
};

const char * const cmd_change_demo_cn[] = {
        "电梯",
        "洗衣机",
        "智能家居",
};

const char * const cmd_elevator_cn[] = {
        "到一楼",
        "到二楼",
        "到三楼",
        "到四楼",
        "到五楼",
        "大堂",
        "负一楼",
        "地下室",
        "开门",
        "关门",
        "选择应用",
};

const char * const cmd_washing_machine_cn[] = {
        "精致模式",
        "正常模式",
        "强力模式",
        "洗白模式",
        "洗白模式 ^1",
        "开始",
        "取消",
        "选择应用",
};

const char * const cmd_smart_home_cn[] = {
        "打开灯",
        "关闭灯",
        "升高温度",
        "降低温度",
        "打开窗帘",
        "关闭窗帘",
        "亮一点",
        "暗一点",
        "选择应用",
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_CN_CN_STRINGS_DSMT_H_ */
