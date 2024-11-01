/*
 * Copyright 2023 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_FR_FR_VOICE_DEMOS_DSMT_H_
#define DSMT_FR_FR_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "fr_strings_dsmt.h"
#include "fr_strings_to_actions_dsmt.h"
#include "fr_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int fr_model_begin;

const sln_voice_demo_t demo_change_demo_fr =
{
		ww_fr,
		cmd_change_demo_fr,
		actions_ww_fr,
		actions_change_demo_fr,
		prompts_ww_fr,
		prompts_change_demo_fr,
		AUDIO_DEMO_NAME_TEST_FR,
		NUM_ELEMENTS(ww_fr),
		NUM_ELEMENTS(cmd_change_demo_fr),
		&fr_model_begin,
		ASR_FRENCH,
		ASR_CMD_CHANGE_DEMO,
		LANG_STR_FR,
		DEMO_STR_CHANGE_DEMO,
};

const sln_voice_demo_t demo_elevator_fr =
{
		ww_fr,
		cmd_elevator_fr,
		actions_ww_fr,
		actions_elevator_fr,
		prompts_ww_fr,
		prompts_elevator_fr,
		AUDIO_ELEVATOR_MULTILINGUAL_FR,
		NUM_ELEMENTS(ww_fr),
		NUM_ELEMENTS(cmd_elevator_fr),
		&fr_model_begin,
		ASR_FRENCH,
		ASR_CMD_ELEVATOR,
		LANG_STR_FR,
		DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_fr =
{
		ww_fr,
		cmd_washing_machine_fr,
		actions_ww_fr,
		actions_washing_machine_fr,
		prompts_ww_fr,
		prompts_washing_machine_fr,
		AUDIO_WASHING_MACHINE_MULTILINGUAL_FR,
		NUM_ELEMENTS(ww_fr),
		NUM_ELEMENTS(cmd_washing_machine_fr),
		&fr_model_begin,
		ASR_FRENCH,
		ASR_CMD_WASHING_MACHINE,
		LANG_STR_FR,
		DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_fr =
{
		ww_fr,
		cmd_smart_home_fr,
		actions_ww_fr,
		actions_smart_home_fr,
		prompts_ww_fr,
		prompts_smart_home_fr,
		AUDIO_SMART_HOME_MULTILINGUAL_FR,
		NUM_ELEMENTS(ww_fr),
		NUM_ELEMENTS(cmd_smart_home_fr),
		&fr_model_begin,
		ASR_FRENCH,
		ASR_CMD_SMART_HOME,
		LANG_STR_FR,
		DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_FR_FR_VOICE_DEMOS_DSMT_H_ */
