/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_EN_EN_STRINGS_TO_PROMPTS_DSMT_H_
#define DSMT_EN_EN_STRINGS_TO_PROMPTS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_flash_files.h"
#include "stddef.h"

const void * const prompts_ww_en[] = {
    AUDIO_WW_DETECTED, // "Hey NXP"
    AUDIO_WW_DETECTED, // "Hey NXP ^1"
    AUDIO_WW_DETECTED, // "Hey NXP ^2"
    AUDIO_WW_DETECTED, // "Hey NXP ^3"
    AUDIO_WW_DETECTED, // "Hey NXP ^4"
};

const void * const prompts_music_playback_en[] = {
    NULL,     // "Next song"
    NULL,     // "Next song ^1"
    NULL,     // "Previous song"
    NULL,     // "Start music"
    NULL,     // "Start music ^1"
    NULL,     // "Start music ^2"
    NULL,     // "Start music ^3"
    NULL,     // "Pause music"
    NULL,     // "Pause music ^1"
    NULL,     // "Pause music ^2"
    NULL,     // "Pause music ^3"
    NULL,     // "Stop music"
    NULL,     // "Stop music ^1"
    NULL,     // "Stop music ^2"
    NULL,     // "Stop music ^3"
    NULL,     // "Volume up"
    NULL,     // "Volume down"
    NULL,     // "Volume maximum"
    NULL      // "Volume minimum"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_EN_EN_STRINGS_TO_PROMPTS_DSMT_H_ */
