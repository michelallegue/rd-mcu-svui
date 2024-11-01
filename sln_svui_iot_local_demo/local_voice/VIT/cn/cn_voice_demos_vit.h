/*
 * Copyright 2023 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIT_CN_CN_VOICE_DEMOS_VIT_H_
#define VIT_CN_CN_VOICE_DEMOS_VIT_H_

#if ENABLE_VIT_ASR

#include "sln_voice_demo.h"
#include "cn_strings_vit.h"
#include "cn_strings_to_actions_vit.h"
#include "cn_strings_to_prompts_vit.h"
#include "demo_definitions_vit.h"
#include "stddef.h"

#include "PL_platformTypes_CortexM.h"
#include "VIT_Model_cn_Elevator.h"
#include "VIT_Model_cn_SmartHome.h"
#include "VIT_Model_cn_WashingMachine.h"

const sln_voice_demo_t demo_elevator_cn =
{
		ww_cn,
		cmd_elevator_cn,
		actions_ww_cn,
		actions_elevator_cn,
		prompts_ww_cn,
		prompts_elevator_cn,
		AUDIO_ELEVATOR_DEMO_CN,
		NUM_ELEMENTS(ww_cn),
		NUM_ELEMENTS(cmd_elevator_cn),
		(void *)VIT_Model_cn_elevator,
		ASR_CHINESE,
		ASR_CMD_ELEVATOR,
		LANG_STR_CN,
		DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_cn =
{
		ww_cn,
		cmd_washing_machine_cn,
		actions_ww_cn,
		actions_washing_machine_cn,
		prompts_ww_cn,
		prompts_washing_machine_cn,
		AUDIO_WASHING_MACHINE_DEMO_CN,
		NUM_ELEMENTS(ww_cn),
		NUM_ELEMENTS(cmd_washing_machine_cn),
		(void *)VIT_Model_cn_washingmachine,
		ASR_CHINESE,
		ASR_CMD_WASHING_MACHINE,
		LANG_STR_CN,
		DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_cn =
{
		ww_cn,
		cmd_smart_home_cn,
		actions_ww_cn,
		actions_smart_home_cn,
		prompts_ww_cn,
		prompts_smart_home_cn,
		AUDIO_SMART_HOME_DEMO_CN,
		NUM_ELEMENTS(ww_cn),
		NUM_ELEMENTS(cmd_smart_home_cn),
		(void *)VIT_Model_cn_smarthome,
		ASR_CHINESE,
		ASR_CMD_SMART_HOME,
		LANG_STR_CN,
		DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_CN_CN_VOICE_DEMOS_VIT_H_ */
