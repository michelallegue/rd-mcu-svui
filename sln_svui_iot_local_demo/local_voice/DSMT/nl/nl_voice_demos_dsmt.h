/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_NL_NL_VOICE_DEMOS_DSMT_H_
#define DSMT_NL_NL_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "nl_strings_dsmt.h"
#include "nl_strings_to_actions_dsmt.h"
#include "nl_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int nl_model_begin;

const sln_voice_demo_t demo_change_demo_nl =
{
        ww_nl,
        cmd_change_demo_nl,
        actions_ww_nl,
        actions_change_demo_nl,
        prompts_ww_nl,
        prompts_change_demo_nl,
        AUDIO_DEMO_NAME_TEST_NL,
        NUM_ELEMENTS(ww_nl),
        NUM_ELEMENTS(cmd_change_demo_nl),
        &nl_model_begin,
        ASR_DUTCH,
        ASR_CMD_CHANGE_DEMO,
        LANG_STR_NL,
        DEMO_STR_CHANGE_DEMO,
};

const sln_voice_demo_t demo_elevator_nl =
{
        ww_nl,
        cmd_elevator_nl,
        actions_ww_nl,
        actions_elevator_nl,
        prompts_ww_nl,
        prompts_elevator_nl,
        AUDIO_ELEVATOR_MULTILINGUAL_NL,
        NUM_ELEMENTS(ww_nl),
        NUM_ELEMENTS(cmd_elevator_nl),
        &nl_model_begin,
        ASR_DUTCH,
        ASR_CMD_ELEVATOR,
        LANG_STR_NL,
        DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_nl =
{
        ww_nl,
        cmd_washing_machine_nl,
        actions_ww_nl,
        actions_washing_machine_nl,
        prompts_ww_nl,
        prompts_washing_machine_nl,
        AUDIO_WASHING_MACHINE_MULTILINGUAL_NL,
        NUM_ELEMENTS(ww_nl),
        NUM_ELEMENTS(cmd_washing_machine_nl),
        &nl_model_begin,
        ASR_DUTCH,
        ASR_CMD_WASHING_MACHINE,
        LANG_STR_NL,
        DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_nl =
{
        ww_nl,
        cmd_smart_home_nl,
        actions_ww_nl,
        actions_smart_home_nl,
        prompts_ww_nl,
        prompts_smart_home_nl,
        AUDIO_SMART_HOME_MULTILINGUAL_NL,
        NUM_ELEMENTS(ww_nl),
        NUM_ELEMENTS(cmd_smart_home_nl),
        &nl_model_begin,
        ASR_DUTCH,
        ASR_CMD_SMART_HOME,
        LANG_STR_NL,
        DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_NL_NL_VOICE_DEMOS_DSMT_H_ */
