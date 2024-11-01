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

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern app_asr_shell_commands_t appAsrShellCommands;
extern oob_demo_control_t oob_demo_control;
extern TaskHandle_t appTaskHandle;
extern VIT_Intent_st SpeechIntent;

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

static void APP_LAYER_LedChangeDemoCommand(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_ORANGE);
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

static void APP_LAYER_ProcessChangeDemoIntent(void)
{
    oob_demo_control.skipWW = 1;
    oob_demo_control.changeDemoFlow = 1;

    APP_LAYER_LedChangeDemoCommand();
}

static status_t APP_LAYER_SwitchToNewDemo()
{
    status_t status = kStatus_Success;
    VIT_Intent_st *pSpeechIntent = NULL;
    asr_inference_t newDemo      = UNDEFINED_INFERENCE;
    PL_INT16 tagsCnt;

    pSpeechIntent = &SpeechIntent;
    tagsCnt       = pSpeechIntent->Slot_Tag_count - 1;

    if(!strcmp("ChangeDemo", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("demo", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            if (!strcmp("hvac", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                configPRINTF(("Changing demo to Hvac\r\n"));
                newDemo = ASR_S2I_HVAC;
            }
            else if (!strcmp("oven", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                configPRINTF(("Changing demo to Oven\r\n"));
                newDemo = ASR_S2I_OVEN;
            }
            else if (!strcmp("smart", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                configPRINTF(("Changing demo to Home\r\n"));
                newDemo = ASR_S2I_HOME;
            }
        }
    }

    if (newDemo)
    {
        appAsrShellCommands.demo   = newDemo;
        appAsrShellCommands.status = WRITE_READY;
        appAsrShellCommands.asrCfg = ASR_CFG_CMD_INFERENCE_ENGINE_CHANGED;
        oob_demo_control.changeDemoFlow = 0;
    }
    else
    {
        oob_demo_control.skipWW = 1;
        oob_demo_control.changeDemoFlow = 1;
    }

    return status;
}

static void APP_LAYER_VIT_ParseHvacIntent(void)
{
    VIT_Intent_st *pSpeechIntent = NULL;
    PL_INT16 tagsCnt;
    char *prompt = NULL;

    pSpeechIntent = &SpeechIntent;
    tagsCnt = pSpeechIntent->Slot_Tag_count - 1;

    /* POWER *************************************************************************************/
    if (!strcmp("Power", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("on", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
            !strcmp("start", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_START_THE_AIRCON_HVAC;
        }
        else if (!strcmp("off", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                 !strcmp("stop", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_STOP_THE_AIRCON_HVAC;
        }
    }
    /* FAN SPEED**********************************************************************************/
    else if (!strcmp("FanSpeed", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("speed", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            if (!strcmp("low", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                prompt = AUDIO_OK_SET_FAN_SPEED_TO_LOW_HVAC;
            }
            else if (!strcmp("medium", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                prompt = AUDIO_OK_SET_FAN_SPEED_TO_MEDIUM_HVAC;
            }
            else if (!strcmp("high", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                prompt = AUDIO_OK_SET_FAN_SPEED_TO_HIGH_HVAC;
            }
            else if (!strcmp("auto", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                prompt = AUDIO_OK_SET_FAN_SPEED_TO_AUTO_HVAC;
            }
            else if (!strcmp("maximum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                prompt = AUDIO_OK_SET_FAN_SPEED_TO_MAXIMUM_HVAC;
            }
            else if (!strcmp("minimum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                prompt = AUDIO_OK_SET_FAN_SPEED_TO_MINIMUM_HVAC;
            }
        }
        else if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            if (!strcmp("increase", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
            {
                prompt = AUDIO_OK_INCREASE_FAN_SPEED_HVAC;
            }
            else if (!strcmp("decrease", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                     !strcmp("reduce", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                     !strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
            {
                prompt = AUDIO_OK_DECREASE_FAN_SPEED_HVAC;
            }
        }
    }
    /* MODE **************************************************************************************/
    else if (!strcmp("Mode", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("mode", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("cool", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_MODE_TO_COOL_HVAC;
                }
                else if (!strcmp("dry", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_MODE_TO_DRY_HVAC;
                }
                else if (!strcmp("fan", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_MODE_TO_FAN_HVAC;
                }
                else if (!strcmp("swing", TAG_VALUE(pSpeechIntent, tagsCnt, 0))
                         && pSpeechIntent->Slot_Tag_count<2)
                {
                    prompt = AUDIO_OK_SET_MODE_TO_SWING_HVAC;
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("mode", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("on", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                    !strcmp("activate", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SWING_ON_HVAC;
                }
                else if (!strcmp("off", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                         !strcmp("deactivate", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SWING_OFF_HVAC;
                }
            }
            else if (!strcmp("mode", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                     !strcmp("action", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("on", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                    !strcmp("activate", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    prompt = AUDIO_OK_SWING_ON_HVAC;
                }
                else if (!strcmp("off", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                         !strcmp("deactivate", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    prompt = AUDIO_OK_SWING_OFF_HVAC;
                }
            }
        }
    }
    /* CUSTOM ADJUST TEMP ************************************************************************/
    else if (!strcmp("CustomAdjustTemp", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("increase_by_two", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            prompt = AUDIO_OK_INCREASING_TEMPERATURE_HVAC;
        }
        else if (!strcmp("decrease_by_two", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            prompt = AUDIO_OK_DECREASING_TEMPERATURE_HVAC;
        }
    }
    /* TEMPERATURE *******************************************************************************/
    else if (!strcmp("Temperature", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        /* Set temperature to ... */
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("degree", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 1)
                {
                    if (!strcmp("fifteen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_15_DEGREES_HVAC;
                    }
                    else if (!strcmp("sixteen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_16_DEGREES_HVAC;
                    }
                    else if (!strcmp("seventeen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_17_DEGREES_HVAC;
                    }
                    else if (!strcmp("eighteen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_18_DEGREES_HVAC;
                    }
                    else if (!strcmp("nineteen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_19_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_20_DEGREES_HVAC;
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_30_DEGREES_HVAC;
                    }
                }
                else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 2)
                {
                    if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                        !strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_21_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_22_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("three", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_23_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("four", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_24_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_25_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("six", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_26_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("seven", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_27_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("eight", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_28_DEGREES_HVAC;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("nine", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_29_DEGREES_HVAC;
                    }
                }
            }
        }
        /* Increase/decrease temperture by ... */
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("degree", TAG_NAME(pSpeechIntent, (tagsCnt - 1))) &&
                !strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("increase", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                {
                    if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] == 1)
                    {
                        if (!strcmp("one", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_1_DEGREE_HVAC;
                        }
                        else if (!strcmp("two", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_2_DEGREES_HVAC;
                        }
                        else if (!strcmp("three", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_3_DEGREES_HVAC;
                        }
                        else if (!strcmp("four", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_4_DEGREES_HVAC;
                        }
                        else if (!strcmp("five", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_5_DEGREES_HVAC;
                        }
                    }
                }
                else if (!strcmp("decrease", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("reduce", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                {
                    if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] == 1)
                    {
                        if (!strcmp("one", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_1_DEGREE_HVAC;
                        }
                        else if (!strcmp("two", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_2_DEGREES_HVAC;
                        }
                        else if (!strcmp("three", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_3_DEGREES_HVAC;
                        }
                        else if (!strcmp("four", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_4_DEGREES_HVAC;
                        }
                        else if (!strcmp("five", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_5_DEGREES_HVAC;
                        }
                    }
                }
            }
        }
    }
    /* TIMER *************************************************************************************/
    else if (!strcmp("Timer", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("duration", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 2)
            {
                if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_HVAC;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_HVAC;
                }
                else if (!strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_10_MINUTES_HVAC;
                }
                else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_20_MINUTES_HVAC;
                }
                else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_30_MINUTES_HVAC;
                }
                else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_40_MINUTES_HVAC;
                }
                else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_50_MINUTES_HVAC;
                }
            }
            else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 5)
            {
                if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                    !strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_10_MINUTES_HVAC;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_20_MINUTES_HVAC;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_30_MINUTES_HVAC;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_40_MINUTES_HVAC;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_50_MINUTES_HVAC;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_10_MINUTES_HVAC;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_20_MINUTES_HVAC;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_30_MINUTES_HVAC;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_40_MINUTES_HVAC;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_50_MINUTES_HVAC;
                }
            }
        }
    }

#if ENABLE_STREAMER
    if (prompt != NULL)
    {
        APP_LAYER_PlayAudioFromFileSystem(prompt);
    }
    else
    {
        configPRINTF(("Intent processed successfully but no audio prompt associated to it found.\r\n"));
    }
#endif /* ENABLE_STREAMER */
}

static void APP_LAYER_VIT_ParseOvenIntent(void)
{
    VIT_Intent_st *pSpeechIntent = NULL;
    PL_INT16 tagsCnt;
    char *prompt = NULL;

    pSpeechIntent = &SpeechIntent;
    tagsCnt = pSpeechIntent->Slot_Tag_count - 1;

    /* COOKING MODE ******************************************************************************/
    if (!strcmp("CookingMode", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("convection", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_SET_CONVECTION_MODE_OVEN;
        }
        else if (!strcmp("fan", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_SET_FAN_FORCED_MODE_OVEN;
        }
        else if (!strcmp("grill", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_SET_GRILL_MODE_OVEN;
        }
        else if (!strcmp("defrost", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_SET_DEFROST_MODE_OVEN;
        }
    }
    /* DOOR **************************************************************************************/
    else if (!strcmp("Door", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("open", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_OPEN_DOOR_OVEN;
        }
        else if (!strcmp("close", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_CLOSE_DOOR_OVEN;
        }
    }
    /* LIGHT *************************************************************************************/
    else if (!strcmp("Light", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("on", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_LIGHT_ON_OVEN;
        }
        else if (!strcmp("off", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_LIGHT_OFF_OVEN;
        }
    }
    /* STOP **************************************************************************************/
    else if (!strcmp("State", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("start", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
            !strcmp("on", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_START_THE_OVEN_OVEN;
        }
        else if (!strcmp("stop", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                 !strcmp("off", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
        {
            prompt = AUDIO_OK_STOP_THE_OVEN_OVEN;
        }
    }
    /* TEMPERATURE *******************************************************************************/
    else if (!strcmp("Temperature", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        /* Set temperature to ... */
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("degree", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 1)
                {
                    if (!strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_40_DEGREES_OVEN;
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_50_DEGREES_OVEN;
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_60_DEGREES_OVEN;
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_70_DEGREES_OVEN;
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_80_DEGREES_OVEN;
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_90_DEGREES_OVEN;
                    }
                }
                else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 2)
                {
                    if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_100_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_200_DEGREES_OVEN;
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_45_DEGREES_OVEN;
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_55_DEGREES_OVEN;
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_65_DEGREES_OVEN;
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_75_DEGREES_OVEN;
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_85_DEGREES_OVEN;
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_95_DEGREES_OVEN;
                    }
                }
                else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 3)
                {
                    if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                        !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_105_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_110_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("fifteen", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_115_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_120_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_130_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_140_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_150_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("sixty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_160_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("seventy", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_170_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("eighty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_180_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("ninety", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_190_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_205_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_210_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("fifteen", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_215_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_220_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_230_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_240_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_250_DEGREES_OVEN;
                    }
                }
                else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 4)
                {
                    if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                        !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_125_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_135_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_145_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_155_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("sixty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_165_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("seventy", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_175_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("eighty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_185_DEGREES_OVEN;
                    }
                    else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("ninety", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_195_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_225_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_235_DEGREES_OVEN;
                    }
                    else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                             !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 2)))
                    {
                        prompt = AUDIO_OK_SET_TEMPERATURE_TO_245_DEGREES_OVEN;
                    }
                }
            }
        }
        /* Increase/decrease temperture by ... */
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("degree", TAG_NAME(pSpeechIntent, (tagsCnt - 1))) &&
                !strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("increase", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                {
                    if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] == 1)
                    {
                        if (!strcmp("five", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_5_DEGREES_OVEN;
                        }
                        else if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_10_DEGREES_OVEN;
                        }
                        else if (!strcmp("fifteen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_15_DEGREES_OVEN;
                        }
                        else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_20_DEGREES_OVEN;
                        }
                        else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_30_DEGREES_OVEN;
                        }
                    }
                    else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] == 2)
                    {
                        if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) &&
                            !strcmp("five", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 1)))
                        {
                            prompt = AUDIO_OK_INCREASE_TEMPERATURE_BY_25_DEGREES_OVEN;
                        }
                    }
                }
                else if (!strcmp("decrease", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("reduce", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                {
                    if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] == 1)
                    {
                        if (!strcmp("five", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_5_DEGREES_OVEN;
                        }
                        else if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_10_DEGREES_OVEN;
                        }
                        else if (!strcmp("fifteen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_15_DEGREES_OVEN;
                        }
                        else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_20_DEGREES_OVEN;
                        }
                        else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_30_DEGREES_OVEN;
                        }
                    }
                    else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] == 2)
                    {
                        if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) &&
                            !strcmp("five", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 1)))
                        {
                            prompt = AUDIO_OK_DECREASE_TEMPERATURE_BY_25_DEGREES_OVEN;
                        }
                    }
                }
            }
        }
    }
    /* TIMER *************************************************************************************/
    else if (!strcmp("Timer", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("duration", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 2)
            {
                if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_OVEN;
                }
                else if (!strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_5_MINUTES_OVEN;
                }
                else if (!strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_10_MINUTES_OVEN;
                }
                else if (!strcmp("fifteen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_15_MINUTES_OVEN;
                }
                else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_20_MINUTES_OVEN;
                }
                else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_30_MINUTES_OVEN;
                }
                else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_40_MINUTES_OVEN;
                }
                else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_50_MINUTES_OVEN;
                }
            }
            else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 3)
            {
                if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                    !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_25_MINUTES_OVEN;
                }
                else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_35_MINUTES_OVEN;
                }
                else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_45_MINUTES_OVEN;
                }
                else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_55_MINUTES_OVEN;
                }
            }
            else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 5)
            {
                if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                    !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_5_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_10_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("fifteen", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_15_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_20_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_30_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_40_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_50_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("five", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_5_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("ten", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_10_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("fifteen", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_15_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_20_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_30_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_40_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_50_MINUTES_OVEN;
                }
            }
            else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt] == 6)
            {
                if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                    !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_25_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_35_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_45_MINUTES_OVEN;
                }
                else if (!strcmp("one", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_ONE_HOUR_AND_55_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("twenty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_25_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("thirty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_35_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("forty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_45_MINUTES_OVEN;
                }
                else if (!strcmp("two", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) &&
                         !strcmp("fifty", TAG_VALUE(pSpeechIntent, tagsCnt, 3)))
                {
                    prompt = AUDIO_OK_SET_A_TIMER_FOR_TWO_HOURS_AND_55_MINUTES_OVEN;
                }
            }
        }
    }

#if ENABLE_STREAMER
    if (prompt != NULL)
    {
        APP_LAYER_PlayAudioFromFileSystem(prompt);
    }
    else
    {
        configPRINTF(("Intent processed successfully but no audio prompt associated to it found.\r\n"));
    }
#endif /* ENABLE_STREAMER */
}

static void APP_LAYER_VIT_ParseHomeIntent(void)
{
    VIT_Intent_st *pSpeechIntent = NULL;
    PL_INT16 tagsCnt;
    char *prompt = NULL;

    pSpeechIntent = &SpeechIntent;
    tagsCnt = pSpeechIntent->Slot_Tag_count - 1;

    /* LIGHT BRIGHTNESS */
    if (!strcmp("LightBrightness", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("level", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("maximum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("max", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_HOME;
                }
                else if (!strcmp("minimum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("min", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_HOME;
                }
            }
            else if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("dim", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("darker", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("less", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_DECREASING_BRIGHTNESS_HOME;
                }
                else if (!strcmp("more", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("brighter", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_INCREASING_BRIGHTNESS_HOME;
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("level", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("location", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("maximum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("max", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("minimum", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("min", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_IN_THE_LIVING_ROOM_HOME;
                    }
                }
            }
            else if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                     !strcmp("location", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("more", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("brighter", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("dim", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("darker", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("less", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
            }
            else if (!strcmp("location", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                     !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("maximum", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                    !strcmp("max", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MAXIMUM_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("minimum", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                         !strcmp("min", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_SET_BRIGHTNESS_TO_MINIMUM_IN_THE_LIVING_ROOM_HOME;
                    }
                }
            }
            else if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                     !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("increase", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("more", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("brighter", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_HOME;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_HOME;
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_HOME;
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_HOME;
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_HOME;
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_HOME;
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_HOME;
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_HOME;
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_HOME;
                    }
                }
                else if (!strcmp("decrease", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                         !strcmp("reduce", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("darker", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("dim", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_HOME;
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_HOME;
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_HOME;
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_HOME;
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_HOME;
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_HOME;
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_HOME;
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_HOME;
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_HOME;
                    }
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 3)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 1))) &&
                !strcmp("location", TAG_NAME(pSpeechIntent, (tagsCnt - 2))))
            {
                if (!strcmp("increase", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("more", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("brighter", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                }
                else if (!strcmp("decrease", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                         !strcmp("reduce", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("darker", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("dim", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                }
            }
            else if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                     !strcmp("location", TAG_NAME(pSpeechIntent, (tagsCnt - 1))) &&
                     !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 2))))
            {
                if (!strcmp("increase", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("more", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("brighter", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_10_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_20_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_30_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_40_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_50_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_60_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_70_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_80_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_INCREASE_BRIGHTNESS_BY_90_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                }
                else if (!strcmp("decrease", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 1)) ||
                         !strcmp("reduce", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("darker", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("dim", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("ten", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_10_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("twenty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_20_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("thirty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_30_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("forty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_40_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("fifty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_50_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("sixty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_60_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("seventy", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_70_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("eighty", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_80_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                    else if (!strcmp("ninety", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_IN_THE_KITCHEN_HOME;
                        }
                        else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_IN_THE_BEDROOM_HOME;
                        }
                        else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_IN_THE_BATHROOM_HOME;
                        }
                        else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_DECREASE_BRIGHTNESS_BY_90_IN_THE_LIVING_ROOM_HOME;
                        }
                    }
                }
            }
        }
    }
    /* LIGHT COLOR */
    else if (!strcmp("LightColor", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("color", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("blue", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_BLUE_LIGHTS_HOME;
                }
                else if (!strcmp("red", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_RED_LIGHTS_HOME;
                }
                else if (!strcmp("pink", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_PINK_LIGHTS_HOME;
                }
                else if (!strcmp("green", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_GREEN_LIGHTS_HOME;
                }
                else if (!strcmp("purple", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_PURPLE_LIGHTS_HOME;
                }
                else if (!strcmp("yellow", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_YELLOW_LIGHTS_HOME;
                }
                else if (!strcmp("orange", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_ORANGE_LIGHTS_HOME;
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("color", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("location", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("blue", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_BLUE_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_BLUE_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_BLUE_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_BLUE_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("red", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RED_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RED_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RED_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RED_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("pink", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_PINK_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_PINK_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_PINK_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_PINK_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("green", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_GREEN_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_GREEN_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_GREEN_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_GREEN_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("purple", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_PURPLE_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_PURPLE_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_PURPLE_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_PURPLE_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("yellow", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_YELLOW_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_YELLOW_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_YELLOW_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_YELLOW_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("orange", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_ORANGE_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_ORANGE_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_ORANGE_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_ORANGE_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
            }
            if (!strcmp("location", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("color", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("blue", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_BLUE_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_BLUE_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_BLUE_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_BLUE_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("red", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_RED_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_RED_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_RED_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_RED_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("pink", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_PINK_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_PINK_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_PINK_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_PINK_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("green", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_GREEN_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_GREEN_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_GREEN_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_GREEN_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("purple", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_PURPLE_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_PURPLE_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_PURPLE_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_PURPLE_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("yellow", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_YELLOW_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_YELLOW_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_YELLOW_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_YELLOW_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("orange", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_ORANGE_LIGHTS_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_ORANGE_LIGHTS_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_ORANGE_LIGHTS_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_ORANGE_LIGHTS_IN_THE_LIVING_ROOM_HOME;
                    }
                }
            }
        }
    }
    /* LIGHT POWER */
    else if (!strcmp("LightPower", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("on", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_LIGHTS_ON_HOME;
                }
                else if (!strcmp("off", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_LIGHTS_OFF_HOME;
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("location", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("on", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_ON_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_ON_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_ON_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_ON_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("off", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_OFF_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_OFF_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_OFF_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_OFF_IN_THE_LIVING_ROOM_HOME;
                    }
                }
            }
            else if (!strcmp("location", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                     !strcmp("action", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("on", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_ON_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_ON_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_ON_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_ON_IN_THE_LIVING_ROOM_HOME;
                    }
                }
                else if (!strcmp("off", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                {
                    if (!strcmp("kitchen", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_OFF_IN_THE_KITCHEN_HOME;
                    }
                    else if (!strcmp("bedroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_OFF_IN_THE_BEDROOM_HOME;
                    }
                    else if (!strcmp("bathroom", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_OFF_IN_THE_BATHROOM_HOME;
                    }
                    else if (!strcmp("living", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                    {
                        prompt = AUDIO_OK_LIGHTS_OFF_IN_THE_LIVING_ROOM_HOME;
                    }
                }
            }
        }
    }
    /* SHADES */
    else if (!strcmp("Shades", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (pSpeechIntent->Slot_Tag_count == 1)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("open", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("lift", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_RAISE_WINDOW_SHADES_HOME;
                }
                else if (!strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("close", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("shut", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    prompt = AUDIO_OK_LOWER_WINDOW_SHADES_HOME;
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 2)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("side", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("open", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("lift", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("left", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_HOME;
                    }
                    else if (!strcmp("right", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_HOME;
                    }
                    else if (!strcmp("middle", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_HOME;
                    }
                    else if (!strcmp("all", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_HOME;
                    }
                }
                else if (!strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("close", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("shut", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("left", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_HOME;
                    }
                    else if (!strcmp("right", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_HOME;
                    }
                    else if (!strcmp("middle", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_HOME;
                    }
                    else if (!strcmp("all", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_HOME;
                    }
                }
            }
            else if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                     !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 1))))
            {
                if (!strcmp("open", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("lift", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                        !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RAISE_WINDOW_SHADES_HALFWAY_HOME;
                    }
                    else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RAISE_WINDOW_SHADES_COMPLETELY_HOME;
                    }
                    else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_RAISE_WINDOW_SHADES_SLIGHTLY_HOME;
                    }
                    /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                    else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                    {
                        prompt = AUDIO_OK_RAISE_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                    }
                }
                else if (!strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("close", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("shut", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                        !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LOWER_WINDOW_SHADES_HALFWAY_HOME;
                    }
                    else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LOWER_WINDOW_SHADES_COMPLETELY_HOME;
                    }
                    else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        prompt = AUDIO_OK_LOWER_WINDOW_SHADES_SLIGHTLY_HOME;
                    }
                    /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                    else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                    {
                        prompt = AUDIO_OK_LOWER_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                    }
                }
            }
        }
        else if (pSpeechIntent->Slot_Tag_count == 3)
        {
            if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                !strcmp("side", TAG_NAME(pSpeechIntent, (tagsCnt - 1))) &&
                !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 2))))
            {
                if (!strcmp("open", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("lift", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("left", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 2] > 1)
                        {
                            prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("right", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 2] > 1)
                        {
                            prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("middle", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 2] > 1)
                        {
                            prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("all", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 2] > 1)
                        {
                            prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                }
                else if (!strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("close", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("shut", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("left", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 2] > 1)
                        {
                            prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("right", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 2] > 1)
                        {
                            prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("middle", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 2] > 1)
                        {
                            prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("all", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 2] > 1)
                        {
                            prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                }
            }
            else if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)) &&
                     !strcmp("level", TAG_NAME(pSpeechIntent, (tagsCnt - 1))) &&
                     !strcmp("side", TAG_NAME(pSpeechIntent, (tagsCnt - 2))))
            {

                if (!strcmp("open", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("up", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("lift", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("raise", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("left", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                        {
                            prompt = AUDIO_OK_RAISE_LEFT_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("right", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                        {
                            prompt = AUDIO_OK_RAISE_RIGHT_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("middle", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                        {
                            prompt = AUDIO_OK_RAISE_MIDDLE_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("all", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                        {
                            prompt = AUDIO_OK_RAISE_ALL_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                }
                else if (!strcmp("down", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("lower", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("close", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                         !strcmp("shut", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    if (!strcmp("left", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                        {
                            prompt = AUDIO_OK_LOWER_LEFT_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("right", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                        {
                            prompt = AUDIO_OK_LOWER_RIGHT_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("middle", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                        {
                            prompt = AUDIO_OK_LOWER_MIDDLE_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                    else if (!strcmp("all", TAG_VALUE(pSpeechIntent, (tagsCnt - 2), 0)))
                    {
                        if (!strcmp("halfway", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)) ||
                            !strcmp("partially", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_HALFWAY_HOME;
                        }
                        else if (!strcmp("completely", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_COMPLETELY_HOME;
                        }
                        else if (!strcmp("slightly", TAG_VALUE(pSpeechIntent, (tagsCnt - 1), 0)))
                        {
                            prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_SLIGHTLY_HOME;
                        }
                        /* A LITTLE, A LITTLE BIT, JUST A LITTLE BIT */
                        else if (pSpeechIntent->Slot_Tag_Value_count[tagsCnt - 1] > 1)
                        {
                            prompt = AUDIO_OK_LOWER_ALL_WINDOW_SHADES_A_LITTLE_BIT_HOME;
                        }
                    }
                }
            }
        }
    }
    /* CUSTOM ADJUST BRIGHTNESS*/
    else if (!strcmp("CustomAdjustBrightness", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("light_on", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            if (!strcmp("dark", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                prompt = AUDIO_OK_INCREASING_BRIGHTNESS_HOME;
            }
        }
        else if (!strcmp("light_off", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            if (!strcmp("bright", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
            {
                prompt = AUDIO_OK_DECREASING_BRIGHTNESS_HOME;
            }
        }
    }

#if ENABLE_STREAMER
    if (prompt != NULL)
    {
        APP_LAYER_PlayAudioFromFileSystem(prompt);
    }
    else
    {
        configPRINTF(("Intent processed successfully but no audio prompt associated to it found.\r\n"));
    }
#endif /* ENABLE_STREAMER */
}

static int APP_LAYER_VIT_ParseChangeDemoIntent(void)
{
    VIT_Intent_st *pSpeechIntent = NULL;
    PL_INT16 tagsCnt;
    int ret = 0;

    pSpeechIntent = &SpeechIntent;
    tagsCnt = pSpeechIntent->Slot_Tag_count - 1;

    if (!strcmp("ChangeDemo", INTENT_NAME(pSpeechIntent, tagsCnt)))
    {
        if (!strcmp("action", TAG_NAME(pSpeechIntent, tagsCnt)))
        {
            if (!strcmp("change", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                !strcmp("switch", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                !strcmp("try", TAG_VALUE(pSpeechIntent, tagsCnt, 1)))
            {
                ret = 1;
                APP_LAYER_ProcessChangeDemoIntent();
#if ENABLE_STREAMER
            APP_LAYER_PlayAudioFromFileSystem(AUDIO_PLEASE_SAY_A_DEMO_NAME);
#endif /* ENABLE_STREAMER */
            }
        }
    }

    return ret;
}

static void APP_LAYER_VIT_ParseSpeechIntent(void)
{
    if (!APP_LAYER_VIT_ParseChangeDemoIntent())
    {
        switch (appAsrShellCommands.demo)
        {
            case ASR_S2I_HVAC:
                APP_LAYER_VIT_ParseHvacIntent();
                break;
            case ASR_S2I_OVEN:
                APP_LAYER_VIT_ParseOvenIntent();
                break;
            case ASR_S2I_HOME:
                APP_LAYER_VIT_ParseHomeIntent();
                break;
            default:
                configPRINTF(("[ERROR] Invalid demo id!\r\n"));
                break;
        }
    }
}

/*******************************************************************************
 * API
 ******************************************************************************/

status_t APP_LAYER_ProcessWakeWord(oob_demo_control_t *commandConfig)
{
    status_t status = kStatus_Success;

#if ENABLE_STREAMER
    APP_LAYER_PlayAudioFromFileSystem(AUDIO_WW_DETECTED);
#endif /* ENABLE_STREAMER */

    if (status == kStatus_Success)
    {
        APP_LAYER_LedListening();
    }
    else
    {
        APP_LAYER_LedError();
    }

    return status;
}

status_t APP_LAYER_ProcessIntent()
{
    status_t status = kStatus_Success;

    if (!oob_demo_control.changeDemoFlow)
    {
        APP_LAYER_LedCommandDetected();
        APP_LAYER_VIT_ParseSpeechIntent();
    }
    else
    {
        APP_LAYER_SwitchToNewDemo();
    }

    return status;
}

bool APP_LAYER_FilterIntent()
{
    bool filterDetection = false;
    VIT_Intent_st *pSpeechIntent = NULL;
    PL_INT16 tagsCnt;

    pSpeechIntent = &SpeechIntent;
    tagsCnt       = pSpeechIntent->Slot_Tag_count - 1;

    if (oob_demo_control.changeDemoFlow)
    {
        if (strcmp("ChangeDemo", INTENT_NAME(pSpeechIntent, tagsCnt)))
        {
            filterDetection = true;
        }
    }
    else
    {
        if(!strcmp("ChangeDemo", INTENT_NAME(pSpeechIntent, tagsCnt)))
        {
            if (!strcmp("demo", TAG_NAME(pSpeechIntent, tagsCnt)))
            {
                if (!strcmp("hvac", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("oven", TAG_VALUE(pSpeechIntent, tagsCnt, 0)) ||
                    !strcmp("smart", TAG_VALUE(pSpeechIntent, tagsCnt, 0)))
                {
                    filterDetection = true;
                }
            }
        }
    }

    return filterDetection;
}

status_t APP_LAYER_ProcessTimeout(oob_demo_control_t *commandConfig)
{
    status_t status = kStatus_Success;

    if (commandConfig != NULL)
    {
        /* appAsrShellCommands.demo contains current demo. */
        switch (appAsrShellCommands.demo)
        {
            case ASR_CMD_CHANGE_DEMO:
            {
                appAsrShellCommands.demo   = DEFAULT_ASR_CMD_DEMO;
                appAsrShellCommands.status = WRITE_READY;
                xTaskNotify(appTaskHandle, kAsrModelChanged, eSetBits);
                break;
            }

            case ASR_S2I_HVAC:
            case ASR_S2I_OVEN:
            case ASR_S2I_HOME:
            {
                if (oob_demo_control.changeDemoFlow ||
                    oob_demo_control.changeLanguageFlow)
                {
                    /* appTask plays the demo prompt when notified that the model changed
                     * reuse that behavior here to play the demo prompt after a timeout
                     * in this way the user will be reminded the demo and the language that are active */
                    xTaskNotify(appTaskHandle, kAsrModelChanged, eSetBits);
                }
                break;
            }

            default:
            {
                configPRINTF(("Error: Unsupported command set %d\r\n", commandConfig->commandSet));
                status = kStatus_Fail;
                break;
            }
        }
    }
    else
    {
        configPRINTF(("Error, commandConfig is NULL\r\n"));
        status = kStatus_InvalidArgument;
    }

    if (oob_demo_control.changeLanguageFlow)
    {
        oob_demo_control.changeLanguageFlow = 0;
    }

    if (oob_demo_control.changeDemoFlow)
    {
        oob_demo_control.changeDemoFlow = 0;
    }

    if (oob_demo_control.skipWW)
    {
        oob_demo_control.skipWW = 0;
    }

    if (status == kStatus_Success)
    {
#if ENABLE_STREAMER
        APP_LAYER_PlayAudioFromFileSystem(AUDIO_TONE_TIMEOUT);
#endif /* ENABLE_STREAMER */
        APP_LAYER_LedTimeout();
    }
    else
    {
        APP_LAYER_LedError();
    }

    return status;
}

void APP_LAYER_SwitchToNextDemo(void)
{
    if (!(appAsrShellCommands.asrCfg & ASR_CFG_CMD_INFERENCE_ENGINE_CHANGED) &&
        !(appAsrShellCommands.asrCfg & ASR_CFG_DEMO_LANGUAGE_CHANGED))
    {
        switch (appAsrShellCommands.demo)
        {
            case ASR_S2I_HVAC:
                appAsrShellCommands.demo = ASR_S2I_OVEN;
                break;
            case ASR_S2I_OVEN:
                appAsrShellCommands.demo = ASR_S2I_HOME;
                break;
            case ASR_S2I_HOME:
                appAsrShellCommands.demo = ASR_S2I_HVAC;
                break;
            default:
                configPRINTF(("Error: unsupported demo %d\r\n", appAsrShellCommands.demo));
                appAsrShellCommands.demo = ASR_S2I_HVAC;
                break;
        }

        appAsrShellCommands.status = WRITE_READY;
        appAsrShellCommands.asrCfg |= ASR_CFG_CMD_INFERENCE_ENGINE_CHANGED;

        /* In case no voice control interaction with the board yet, just use what we have saved in flash */
        if (UNDEFINED_LANGUAGE == oob_demo_control.language)
        {
            oob_demo_control.language   = appAsrShellCommands.activeLanguage;
        }
    }
}

void APP_LAYER_HandleFirstBoardBoot(void)
{
    if ((appAsrShellCommands.demo == BOOT_ASR_CMD_DEMO) &&
        (BOOT_ASR_CMD_DEMO != DEFAULT_ASR_CMD_DEMO))
    {
        APP_LAYER_ProcessChangeDemoIntent();
    }
}

#endif /* ENABLE_S2I_ASR */
#endif /* ENABLE_NXP_OOBE */
