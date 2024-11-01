/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_VN_VN_VOICE_DEMOS_DSMT_H_
#define DSMT_VN_VN_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "vn_strings_dsmt.h"
#include "vn_strings_to_actions_dsmt.h"
#include "vn_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int vn_model_begin;

const sln_voice_demo_t demo_change_demo_vn =
{
        ww_vn,
        cmd_change_demo_vn,
        actions_ww_vn,
        actions_change_demo_vn,
        prompts_ww_vn,
        prompts_change_demo_vn,
        AUDIO_DEMO_NAME_TEST_VN,
        NUM_ELEMENTS(ww_vn),
        NUM_ELEMENTS(cmd_change_demo_vn),
        &vn_model_begin,
        ASR_VIETNAMESE,
        ASR_CMD_CHANGE_DEMO,
        LANG_STR_VN,
        DEMO_STR_CHANGE_DEMO,
};

const sln_voice_demo_t demo_elevator_vn =
{
        ww_vn,
        cmd_elevator_vn,
        actions_ww_vn,
        actions_elevator_vn,
        prompts_ww_vn,
        prompts_elevator_vn,
        AUDIO_ELEVATOR_MULTILINGUAL_VN,
        NUM_ELEMENTS(ww_vn),
        NUM_ELEMENTS(cmd_elevator_vn),
        &vn_model_begin,
        ASR_VIETNAMESE,
        ASR_CMD_ELEVATOR,
        LANG_STR_VN,
        DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_vn =
{
        ww_vn,
        cmd_washing_machine_vn,
        actions_ww_vn,
        actions_washing_machine_vn,
        prompts_ww_vn,
        prompts_washing_machine_vn,
        AUDIO_WASHING_MACHINE_MULTILINGUAL_VN,
        NUM_ELEMENTS(ww_vn),
        NUM_ELEMENTS(cmd_washing_machine_vn),
        &vn_model_begin,
        ASR_VIETNAMESE,
        ASR_CMD_WASHING_MACHINE,
        LANG_STR_VN,
        DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_vn =
{
        ww_vn,
        cmd_smart_home_vn,
        actions_ww_vn,
        actions_smart_home_vn,
        prompts_ww_vn,
        prompts_smart_home_vn,
        AUDIO_SMART_HOME_MULTILINGUAL_VN,
        NUM_ELEMENTS(ww_vn),
        NUM_ELEMENTS(cmd_smart_home_vn),
        &vn_model_begin,
        ASR_VIETNAMESE,
        ASR_CMD_SMART_HOME,
        LANG_STR_VN,
        DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_VN_VN_VOICE_DEMOS_DSMT_H_ */
