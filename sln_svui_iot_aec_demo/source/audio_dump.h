/*
 * Copyright 2022-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_USB_AUDIO_DUMP
#ifndef _AUDIO_DUMP_H_
#define _AUDIO_DUMP_H_

#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if ENABLE_AMPLIFIER
#define aec_align_sound_task_NAME              "AecAlignSound"
#define aec_align_sound_task_PRIORITY          4
#define aec_align_sound_task_STACK             512

#define AEC_ALIGN_SOUND_VOLUME                 10

#define AEC_ALIGN_DELAY_STARTUP_MS             1000
#define AEC_ALIGN_DELAY_BETWEEN_SOUND_PLAYS_MS 5000
#endif /* ENABLE_AMPLIFIER */

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @brief CDC class specific callback function.
 *
 * This function handles the CDC class specific requests.
 *
 * @param handle          The CDC ACM class handle.
 * @param event           The CDC ACM class event type.
 * @param param           The parameter of the class specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t AUDIO_DUMP_USB_DeviceCdcVcomCallback(class_handle_t handle, uint32_t event, void *param);

/*!
 * @brief Forward mics, amp and afe processed buffers via USB.
 *
 * @param micStream            The mics buffer
 * @param ampStream            The amp buffer
 * @param cleanStream          The afe processed buffer
 */
void AUDIO_DUMP_ForwardDataOverUsb(int16_t *micStream, int16_t *ampStream, void *cleanStream);

#if ENABLE_AMPLIFIER
/*!
 * @brief Task responsible for playing the alignment sound
 */
void AUDIO_DUMP_AecAlignSoundTask(void *arg);

/*!
 * @brief Start playing the alignment sound
 */
void AUDIO_DUMP_AecAlignSoundStart(void);

/*!
 * @brief Stop playing the alignment sound
 */
void AUDIO_DUMP_AecAlignSoundStop(void);
#endif /* ENABLE_AMPLIFIER */
#endif /* _AUDIO_DUMP_H_ */
#endif /* ENABLE_USB_AUDIO_DUMP */
