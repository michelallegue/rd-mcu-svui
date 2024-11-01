/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIT_EN_EN_STRINGS_TO_ACTIONS_VIT_H_
#define VIT_EN_EN_STRINGS_TO_ACTIONS_VIT_H_

#if ENABLE_VIT_ASR

#include "demo_actions_vit.h"
#include "stdint.h"

const int16_t actions_ww_en[] = {
    kWakeWord_Detected, // "Hey NXP"
};

const int16_t actions_music_playback_en[] = {
    kMusicPlayback_NextSong,       // "Next song"
    kMusicPlayback_PreviousSong,   // "Previous song"
    kMusicPlayback_StartMusic,     // "Start music"
    kMusicPlayback_PauseMusic,     // "Pause music"
    kMusicPlayback_StopMusic,      // "Stop music"
    kMusicPlayback_VolumeUp,       // "Volume up"
    kMusicPlayback_VolumeDown,     // "Volume down"
    kMusicPlayback_VolumeMaximum,  // "Volume maximum"
    kMusicPlayback_VolumeMinimum,  // "Volume minimum"
};


#endif /* ENABLE_VIT_ASR */
#endif /* VIT_EN_EN_STRINGS_TO_ACTIONS_VIT_H_ */
