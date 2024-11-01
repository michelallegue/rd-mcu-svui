/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIT_EN_EN_STRINGS_TO_PROMPTS_VIT_H_
#define VIT_EN_EN_STRINGS_TO_PROMPTS_VIT_H_

#if ENABLE_VIT_ASR

#include "sln_flash_files.h"
#include "stddef.h"

const void * const prompts_ww_en[] = {
    AUDIO_WW_DETECTED, // "Hey NXP"
};

const void * const prompts_music_playback_en[] = {
    NULL, // "Next song"
    NULL, // "Previous song"
    NULL, // "Start music"
    NULL, // "Pause music"
    NULL, // "Stop music"
    NULL, // "Volume up"
    NULL, // "Volume down"
    NULL, // "Volume maximum"
    NULL, // "Volume minimum"
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_EN_EN_STRINGS_TO_PROMPTS_VIT_H_ */
