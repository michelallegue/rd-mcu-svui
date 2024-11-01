/*
 * Copyright 2022-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_NXP_OOBE
#if ENABLE_VIT_ASR || ENABLE_DSMT_ASR

#include "FreeRTOS.h"
#include "task.h"

#include "stdint.h"
#include "fsl_common.h"
#include "FreeRTOSConfig.h"
#include "sln_amplifier.h"
#include "sln_local_voice_common.h"
#include "sln_rgb_led_driver.h"
#include "local_sounds_task.h"
#include "sln_flash_files.h"

#include "demo_actions.h"
#include "IndexCommands.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define NUM_SONGS 5

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern app_asr_shell_commands_t appAsrShellCommands;
extern oob_demo_control_t oob_demo_control;
extern TaskHandle_t appTaskHandle;

static char* s_songsList[NUM_SONGS] = {AUDIO_ROCK, AUDIO_MOONLIT_BLUES, AUDIO_ROMANTIC_PIANO, AUDIO_RISE_UP_HIGH, AUDIO_SLOW_BLUES};
static uint8_t s_currentSong = 0;
static uint8_t s_musicStarted = 0;
static uint8_t s_musicInterrupted = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void APP_LAYER_LedCommandDetected(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_GREEN);
    vTaskDelay(200);
    RGB_LED_SetColor(LED_COLOR_OFF);
    if ((appAsrShellCommands.asrMode == ASR_MODE_WW_AND_MUL_CMD) || (appAsrShellCommands.asrMode == ASR_MODE_CMD_ONLY))
    {
        vTaskDelay(200);
        RGB_LED_SetColor(LED_COLOR_BLUE);
    }

    if (appAsrShellCommands.asrMode == ASR_MODE_PTT)
    {
        vTaskDelay(200);
        /* show the user that device will wake up only on SW3 press */
        RGB_LED_SetColor(LED_COLOR_CYAN);
    }
}

static void APP_LAYER_LedListening(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_BLUE);
    if (appAsrShellCommands.asrMode == ASR_MODE_WW_ONLY)
    {
        vTaskDelay(500);
        RGB_LED_SetColor(LED_COLOR_OFF);
    }
}

static void APP_LAYER_LedTimeout(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_PURPLE);
    vTaskDelay(200);
    RGB_LED_SetColor(LED_COLOR_OFF);
}

#if ENABLE_STREAMER
static status_t APP_LAYER_PlayAudioFromFileSystem(char *filename)
{
    status_t status = kStatus_Success;

    /* Make sure that speaker is not currently playing another audio. */
    while (LOCAL_SOUNDS_isPlaying())
    {
        vTaskDelay(10);
    }

    LOCAL_SOUNDS_PlayAudioFile(filename, appAsrShellCommands.volume);

    return status;
}
#endif /* ENABLE_STREAMER */

/*******************************************************************************
 * API
 ******************************************************************************/

status_t APP_LAYER_ProcessWakeWord(oob_demo_control_t *commandConfig)
{
#if ENABLE_STREAMER
    if (LOCAL_SOUNDS_isPlaying())
    {
        s_musicInterrupted = 1;
        LOCAL_SOUNDS_PausePlaying();
    }
    else
    {
        s_musicInterrupted = 0;
    }
    APP_LAYER_PlayAudioFromFileSystem(AUDIO_WW_DETECTED);
#endif /* ENABLE_STREAMER */

    APP_LAYER_LedListening();

#if ENABLE_STREAMER
    if (s_musicInterrupted && (appAsrShellCommands.asrMode == ASR_MODE_WW_ONLY))
    {
        s_musicStarted = 1;
        LOCAL_SOUNDS_ResumePlaying(appAsrShellCommands.volume);
    }
#endif /* ENABLE_STREAMER */

    return kStatus_Success;
}

status_t APP_LAYER_ProcessVoiceCommand(oob_demo_control_t *commandConfig)
{
    status_t status = kStatus_Success;
    uint16_t action = 0;

    if (commandConfig != NULL)
    {
        APP_LAYER_LedCommandDetected();

        action = get_action_from_keyword(commandConfig->language, commandConfig->commandSet, commandConfig->commandId);

#if ENABLE_STREAMER
        void *prompt = NULL;

        /* Play prompt for the active language */
        prompt = get_prompt_from_keyword(commandConfig->language, commandConfig->commandSet, commandConfig->commandId);

        if (NULL != prompt)
        {
            APP_LAYER_PlayAudioFromFileSystem((char*)prompt);
        }
#endif /* ENABLE_STREAMER */

        switch (action)
        {
            case kMusicPlayback_NextSong:
            {
                if (LOCAL_SOUNDS_isPaused())
                {
                    s_musicStarted = 1;

                    LOCAL_SOUNDS_StopPlaying();
                    s_currentSong = (s_currentSong + 1) % NUM_SONGS;
                    APP_LAYER_PlayAudioFromFileSystem(s_songsList[s_currentSong]);
                }
                break;
            }
            case kMusicPlayback_PreviousSong:
            {
                if (LOCAL_SOUNDS_isPaused())
                {
                    s_musicStarted = 1;

                    LOCAL_SOUNDS_StopPlaying();
                    s_currentSong = (s_currentSong == 0) ? NUM_SONGS - 1 : s_currentSong - 1;
                    APP_LAYER_PlayAudioFromFileSystem(s_songsList[s_currentSong]);
                }
                break;
            }
            case kMusicPlayback_StartMusic:
            {
                s_musicStarted = 1;

                if (LOCAL_SOUNDS_isPaused())
                {
                    LOCAL_SOUNDS_ResumePlaying(appAsrShellCommands.volume);
                }
                else
                {
                    APP_LAYER_PlayAudioFromFileSystem(s_songsList[s_currentSong]);
                }
                break;
            }
            case kMusicPlayback_PauseMusic:
            {
                if (s_musicStarted == 1)
                {
                    s_musicStarted = 0;

                    LOCAL_SOUNDS_PausePlaying();
                }
                break;
            }
            case kMusicPlayback_StopMusic:
            {
                if (s_musicStarted == 1)
                {
                    s_musicStarted = 0;

                    LOCAL_SOUNDS_StopPlaying();
                }
                break;
            }
            case kMusicPlayback_VolumeUp:
            {
                appAsrShellCommands.volume += 10;
                break;
            }
            case kMusicPlayback_VolumeDown:
            {
                appAsrShellCommands.volume -= 10;
                break;
            }
            case kMusicPlayback_VolumeMinimum:
            {
                appAsrShellCommands.volume = 10;
                break;
            }
            case kMusicPlayback_VolumeMaximum:
            {
                appAsrShellCommands.volume = 100;
                break;
            }
            default:
            {
                configPRINTF(("Invalid command action received"));
            }
        }
    }

    if ((action == kMusicPlayback_VolumeUp) ||
        (action == kMusicPlayback_VolumeDown) ||
        (action == kMusicPlayback_VolumeMaximum) ||
        (action == kMusicPlayback_VolumeMinimum))
    {
        // Make sure that volume is in range 10 - 100
        if (appAsrShellCommands.volume < 10)
        {
            appAsrShellCommands.volume = 10;
        }
        else if (appAsrShellCommands.volume > 100)
        {
            appAsrShellCommands.volume = 100;
        }

        // Notify app task that volume has changed
        xTaskNotify(appTaskHandle, kVolumeUpdate, eSetBits);

        // Give some time for volume update to be processed
        vTaskDelay(50);

        // Resume playback if needed
        if (LOCAL_SOUNDS_isPaused() && s_musicStarted)
        {
            LOCAL_SOUNDS_ResumePlaying(appAsrShellCommands.volume);
        }
    }

    return status;
}

void APP_LAYER_PeriodicCheck(void)
{
    if (LOCAL_SOUNDS_isEOS() && !LOCAL_SOUNDS_isPaused() && s_musicStarted)
    {
        s_currentSong = (s_currentSong + 1) % NUM_SONGS;
        APP_LAYER_PlayAudioFromFileSystem(s_songsList[s_currentSong]);
    }
}

status_t APP_LAYER_ProcessTimeout(oob_demo_control_t *commandConfig)
{
#if ENABLE_STREAMER
    APP_LAYER_PlayAudioFromFileSystem(AUDIO_TONE_TIMEOUT);
#endif /* ENABLE_STREAMER */

    APP_LAYER_LedTimeout();

#if ENABLE_STREAMER
    /* Wait for timeout chime to be played */
    while (LOCAL_SOUNDS_isPlaying())
    {
        vTaskDelay(10);
    }

    if (s_musicInterrupted)
    {
        s_musicStarted = 1;
        LOCAL_SOUNDS_ResumePlaying(appAsrShellCommands.volume);
    }
#endif /* ENABLE_STREAMER */

    return kStatus_Success;
}

#endif /* ENABLE_VIT_ASR || ENABLE_DSMT_ASR */
#endif /* ENABLE_NXP_OOBE */
