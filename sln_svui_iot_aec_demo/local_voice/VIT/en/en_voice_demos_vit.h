/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIT_EN_EN_VOICE_DEMOS_VIT_H_
#define VIT_EN_EN_VOICE_DEMOS_VIT_H_

#if ENABLE_VIT_ASR

#include "sln_voice_demo.h"
#include "en_strings_vit.h"
#include "en_strings_to_actions_vit.h"
#include "en_strings_to_prompts_vit.h"
#include "demo_definitions_vit.h"
#include "stddef.h"

#include "PL_platformTypes_CortexM.h"
#include "VIT_Model_en_MusicPlayback.h"

const sln_voice_demo_t demo_music_playback_en =
{
        ww_en,
        cmd_music_playback_en,
        actions_ww_en,
        actions_music_playback_en,
        prompts_ww_en,
        prompts_music_playback_en,
        AUDIO_MUSIC_PLAYBACK_DEMO_EN,
        NUM_ELEMENTS(ww_en),
        NUM_ELEMENTS(cmd_music_playback_en),
        (void *)VIT_Model_en_musicplayback,
        ASR_ENGLISH,
        ASR_CMD_MUSIC_PLAYBACK,
        LANG_STR_EN,
        DEMO_STR_MUSIC_PLAYBACK,
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_EN_EN_VOICE_DEMOS_VIT_H_ */
