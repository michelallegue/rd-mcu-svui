/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_HI_HI_VOICE_DEMOS_DSMT_H_
#define DSMT_HI_HI_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "hi_strings_dsmt.h"
#include "hi_strings_to_actions_dsmt.h"
#include "hi_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int hi_model_begin;

const sln_voice_demo_t demo_change_demo_hi =
{
        ww_hi,
        cmd_change_demo_hi,
        actions_ww_hi,
        actions_change_demo_hi,
        prompts_ww_hi,
        prompts_change_demo_hi,
        AUDIO_DEMO_NAME_TEST_HI,
        NUM_ELEMENTS(ww_hi),
        NUM_ELEMENTS(cmd_change_demo_hi),
        &hi_model_begin,
        ASR_HINDI,
        ASR_CMD_CHANGE_DEMO,
        LANG_STR_HI,
        DEMO_STR_CHANGE_DEMO,
};

const sln_voice_demo_t demo_elevator_hi =
{
        ww_hi,
        cmd_elevator_hi,
        actions_ww_hi,
        actions_elevator_hi,
        prompts_ww_hi,
        prompts_elevator_hi,
        AUDIO_ELEVATOR_MULTILINGUAL_HI,
        NUM_ELEMENTS(ww_hi),
        NUM_ELEMENTS(cmd_elevator_hi),
        &hi_model_begin,
        ASR_HINDI,
        ASR_CMD_ELEVATOR,
        LANG_STR_HI,
        DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_hi =
{
        ww_hi,
        cmd_washing_machine_hi,
        actions_ww_hi,
        actions_washing_machine_hi,
        prompts_ww_hi,
        prompts_washing_machine_hi,
        AUDIO_WASHING_MACHINE_MULTILINGUAL_HI,
        NUM_ELEMENTS(ww_hi),
        NUM_ELEMENTS(cmd_washing_machine_hi),
        &hi_model_begin,
        ASR_HINDI,
        ASR_CMD_WASHING_MACHINE,
        LANG_STR_HI,
        DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_hi =
{
        ww_hi,
        cmd_smart_home_hi,
        actions_ww_hi,
        actions_smart_home_hi,
        prompts_ww_hi,
        prompts_smart_home_hi,
        AUDIO_SMART_HOME_MULTILINGUAL_HI,
        NUM_ELEMENTS(ww_hi),
        NUM_ELEMENTS(cmd_smart_home_hi),
        &hi_model_begin,
        ASR_HINDI,
        ASR_CMD_SMART_HOME,
        LANG_STR_HI,
        DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_HI_HI_VOICE_DEMOS_DSMT_H_ */
