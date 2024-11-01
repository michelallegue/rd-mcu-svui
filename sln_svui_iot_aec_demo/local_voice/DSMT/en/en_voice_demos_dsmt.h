/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_EN_EN_VOICE_DEMOS_DSMT_H_
#define DSMT_EN_EN_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "en_strings_dsmt.h"
#include "en_strings_to_actions_dsmt.h"
#include "en_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int en_model_begin;

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
        &en_model_begin,
        ASR_ENGLISH,
        ASR_CMD_MUSIC_PLAYBACK,
        LANG_STR_EN,
        DEMO_STR_MUSIC_PLAYBACK,
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_EN_EN_VOICE_DEMOS_DSMT_H_ */
