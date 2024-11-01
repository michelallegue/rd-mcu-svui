/*
 * Copyright 2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_NXP_OOBE
#if ENABLE_S2I_ASR

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

#define INTENT_NAME(x, i)   (x->pIntent[i])
#define TAG_NAME(x, i)      (x->pSlot_Tag[i])
#define TAG_VALUE_CNT(x, i) (x->Slot_Tag_Value_count[i] - 1)
#define TAG_VALUE(x, i, j)  (x->pSlot_Tag_Value[(i * MAX_NUMBER_WORDS_PER_SLOT_TAG_VALUE) + \
                             TAG_VALUE_CNT(x, i) - j])

#define NUM_SONGS 5

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern app_asr_shell_commands_t appAsrShellCommands;
extern oob_demo_control_t oob_demo_control;
extern TaskHandle_t appTaskHandle;
extern VIT_Intent_st SpeechIntent;

static char* s_songsList[NUM_SONGS] = {AUDIO_ROMANTIC_PIANO, AUDIO_RISE_UP_HIGH, AUDIO_SLOW_BLUES, AUDIO_ROCK, AUDIO_MOONLIT_BLUES};
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

static void APP_LAYER_LedError(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_RED);
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

static void APP_LAYER_VIT_ParseAudioPlayerIntent(void)
{
    VIT_Intent_st *pSpeechIntent = NULL;
    PL_INT16 tagsCnt;

    pSpeechIntent = &SpeechIntent;
    tagsCnt = pSpeechIntent->Slot_Tag_count - 1;

    /* VOLUME */
    if (!strcmp("Volume", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("level", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("maximum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("max", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 100;
                }
                else if (!strcmp("minimum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("min", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 10;
                }
                else if (!strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 10;
                }
                else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 20;
                }
                else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 30;
                }
                else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 40;
                }
                else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 50;
                }
                else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 60;
                }
                else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 70;
                }
                else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 80;
                }
                else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    appAsrShellCommands.volume = 90;
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("increase", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 10;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 20;
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 30;
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 40;
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 50;
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 60;
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 70;
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 80;
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume + 90;
                    }
                    else if (!strcmp("maximum", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                             !strcmp("max", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                             !strcmp("one", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = 100;
                    }
                }
                else if (!strcmp("decrease", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                         !strcmp("reduce", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 10;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 20;
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 30;
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 40;
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 50;
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 60;
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 70;
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 80;
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = appAsrShellCommands.volume - 90;
                    }
                    else if (!strcmp("minimum", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                             !strcmp("min", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        appAsrShellCommands.volume = 10;
                    }
                }
            }
        }

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

    /* MUSIC */
    else if (!strcmp("Music", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("start", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
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
                }
                else if (!strcmp("stop", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (s_musicStarted == 1)
                    {
                        s_musicStarted = 0;

                        LOCAL_SOUNDS_StopPlaying();
                    }
                }
                else if (!strcmp("pause", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (s_musicStarted == 1)
                    {
                        s_musicStarted = 0;

                        LOCAL_SOUNDS_PausePlaying();
                    }
                }
                else if (!strcmp("play", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
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
                }
                else if (!strcmp("next", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (LOCAL_SOUNDS_isPaused())
                    {
                        s_musicStarted = 1;

                        LOCAL_SOUNDS_StopPlaying();
                        s_currentSong = (s_currentSong + 1) % NUM_SONGS;
                        APP_LAYER_PlayAudioFromFileSystem(s_songsList[s_currentSong]);
                    }
                }
                else if (!strcmp("previous", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (LOCAL_SOUNDS_isPaused())
                    {
                        s_musicStarted = 1;

                        LOCAL_SOUNDS_StopPlaying();
                        s_currentSong = (s_currentSong == 0) ? NUM_SONGS - 1 : s_currentSong - 1;
                        APP_LAYER_PlayAudioFromFileSystem(s_songsList[s_currentSong]);
                    }
                }
            }
        }
    }
}

static void APP_LAYER_VIT_ParseSpeechIntent(void)
{
    switch (appAsrShellCommands.demo)
    {
        case ASR_S2I_AUDIO_PLAYER:
            APP_LAYER_VIT_ParseAudioPlayerIntent();
            break;
        default:
            configPRINTF(("[ERROR] Invalid demo id!\r\n"));
            break;
    }
}

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

status_t APP_LAYER_ProcessIntent()
{
    status_t status = kStatus_Success;

    if (!oob_demo_control.changeDemoFlow)
    {
        APP_LAYER_LedCommandDetected();
        APP_LAYER_VIT_ParseSpeechIntent();
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

#endif /* ENABLE_S2I_ASR */
#endif /* ENABLE_NXP_OOBE */
