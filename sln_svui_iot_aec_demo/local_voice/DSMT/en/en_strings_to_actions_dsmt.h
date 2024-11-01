/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_EN_EN_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_EN_EN_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_en[] = {
    kWakeWord_Detected, // "Hey NXP"
    kWakeWord_Detected, // "Hey NXP ^1"
    kWakeWord_Detected, // "Hey NXP ^2"
    kWakeWord_Detected, // "Hey NXP ^3"
    kWakeWord_Detected, // "Hey NXP ^4"
};

const int16_t actions_music_playback_en[] = {
    kMusicPlayback_NextSong,      // "Next song"
    kMusicPlayback_NextSong,      // "Next song ^1"
    kMusicPlayback_PreviousSong,  // "Previous song"
    kMusicPlayback_StartMusic,    // "Start music"
    kMusicPlayback_StartMusic,    // "Start music ^1"
    kMusicPlayback_StartMusic,    // "Start music ^2"
    kMusicPlayback_StartMusic,    // "Start music ^3"
    kMusicPlayback_PauseMusic,    // "Pause music"
    kMusicPlayback_PauseMusic,    // "Pause music ^1"
    kMusicPlayback_PauseMusic,    // "Pause music ^2"
    kMusicPlayback_PauseMusic,    // "Pause music ^3"
    kMusicPlayback_StopMusic,     // "Stop music"
    kMusicPlayback_StopMusic,     // "Stop music ^1"
    kMusicPlayback_StopMusic,     // "Stop music ^2"
    kMusicPlayback_StopMusic,     // "Stop music ^3"
    kMusicPlayback_VolumeUp,      // "Volume up"
    kMusicPlayback_VolumeDown,    // "Volume down"
    kMusicPlayback_VolumeMaximum, // "Volume maximum"
    kMusicPlayback_VolumeMinimum  // "Volume minimum"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_EN_EN_STRINGS_TO_ACTIONS_DSMT_H_ */
