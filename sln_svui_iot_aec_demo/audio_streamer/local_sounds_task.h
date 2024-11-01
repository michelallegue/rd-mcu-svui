/*
 * Copyright 2022-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LOCAL_SOUNDS_TASK_H_
#define LOCAL_SOUNDS_TASK_H_

#include "fsl_common.h"
#include "stdint.h"

#if ENABLE_STREAMER

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize the audio streamer.
 *
 * @return kStatus_Success if success.
 */
status_t LOCAL_SOUNDS_InitStreamer(void);

/**
 * @brief Set and play an offline audio.
 *        If there is another audio already playing, do nothing.
 *
 * @param audioFileName Path to the audio file in the filesystem.
 * @param volume Volume of the audio.
 *
 *  @return kStatus_Success if success.
 */
status_t LOCAL_SOUNDS_PlayAudioFile(char *audioFileName, int32_t volume);

/**
 * @brief Resume playback
 *
 * @param volume Volume of the audio.
 *
 * @return status of the operation
 */
status_t LOCAL_SOUNDS_ResumePlaying(int32_t volume);

/**
 * @brief Start playback
 *
 * @param volume Volume of the audio.
 *
 * @return status of the operation
 */
status_t LOCAL_SOUNDS_StartPlaying(int32_t volume);

/**
 * @brief Stop playback
 *
 * @return status of the operation
 */
status_t LOCAL_SOUNDS_StopPlaying(void);

/**
 * @brief Pause playback. If playback will be started afterwards,
 *        it will be resumed from where it was left.
 *
 * @return status of the operation
 */
status_t LOCAL_SOUNDS_PausePlaying(void);

/**
 * @brief Check if the audio streamer is playing a file
 *
 * @return true if it is playing.
 */
bool LOCAL_SOUNDS_isPlaying(void);

/**
 * @brief Check if the audio streamer is paused
 *
 * @return true if it is paused.
 */
bool LOCAL_SOUNDS_isPaused(void);

/**
 * @brief Check if the audio streamer reached EOS
 *
 * @return true if EOS was reached.
 */
bool LOCAL_SOUNDS_isEOS(void);

#endif /* ENABLE_STREAMER */
#endif /* LOCAL_SOUNDS_TASK_H_ */
