/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIT_DEMO_ACTIONS_VIT_H_
#define VIT_DEMO_ACTIONS_VIT_H_

#if ENABLE_VIT_ASR

/* Global actions for all languages */

enum _wake_word_action
{
    kWakeWord_Detected = 0,
    kWakeWord_ActionInvalid
};

enum _music_playback_action
{
    kMusicPlayback_NextSong = 0,
    kMusicPlayback_PreviousSong,
    kMusicPlayback_StartMusic,
    kMusicPlayback_PauseMusic,
    kMusicPlayback_StopMusic,
    kMusicPlayback_VolumeUp,
    kMusicPlayback_VolumeDown,
    kMusicPlayback_VolumeMaximum,
    kMusicPlayback_VolumeMinimum,
    kMusicPlayback_ActionInvalid
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_DEMO_ACTIONS_VIT_H_ */
