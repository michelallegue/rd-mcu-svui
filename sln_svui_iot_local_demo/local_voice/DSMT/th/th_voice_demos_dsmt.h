/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_TH_TH_VOICE_DEMOS_DSMT_H_
#define DSMT_TH_TH_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "th_strings_dsmt.h"
#include "th_strings_to_actions_dsmt.h"
#include "th_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int th_model_begin;

const sln_voice_demo_t demo_change_demo_th =
{
        ww_th,
        cmd_change_demo_th,
        actions_ww_th,
        actions_change_demo_th,
        prompts_ww_th,
        prompts_change_demo_th,
        AUDIO_DEMO_NAME_TEST_TH,
        NUM_ELEMENTS(ww_th),
        NUM_ELEMENTS(cmd_change_demo_th),
        &th_model_begin,
        ASR_THAI,
        ASR_CMD_CHANGE_DEMO,
        LANG_STR_TH,
        DEMO_STR_CHANGE_DEMO,
};

const sln_voice_demo_t demo_elevator_th =
{
        ww_th,
        cmd_elevator_th,
        actions_ww_th,
        actions_elevator_th,
        prompts_ww_th,
        prompts_elevator_th,
        AUDIO_ELEVATOR_MULTILINGUAL_TH,
        NUM_ELEMENTS(ww_th),
        NUM_ELEMENTS(cmd_elevator_th),
        &th_model_begin,
        ASR_THAI,
        ASR_CMD_ELEVATOR,
        LANG_STR_TH,
        DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_th =
{
        ww_th,
        cmd_washing_machine_th,
        actions_ww_th,
        actions_washing_machine_th,
        prompts_ww_th,
        prompts_washing_machine_th,
        AUDIO_WASHING_MACHINE_MULTILINGUAL_TH,
        NUM_ELEMENTS(ww_th),
        NUM_ELEMENTS(cmd_washing_machine_th),
        &th_model_begin,
        ASR_THAI,
        ASR_CMD_WASHING_MACHINE,
        LANG_STR_TH,
        DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_th =
{
        ww_th,
        cmd_smart_home_th,
        actions_ww_th,
        actions_smart_home_th,
        prompts_ww_th,
        prompts_smart_home_th,
        AUDIO_SMART_HOME_MULTILINGUAL_TH,
        NUM_ELEMENTS(ww_th),
        NUM_ELEMENTS(cmd_smart_home_th),
        &th_model_begin,
        ASR_THAI,
        ASR_CMD_SMART_HOME,
        LANG_STR_TH,
        DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_TH_TH_VOICE_DEMOS_DSMT_H_ */
