/*
 * Copyright 2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_CFG_FILE_
#define _SLN_CFG_FILE_

#define DEVICE_CONFIG_FILE_NAME "dev_cfg.dat"

#define DEFAULT_USB_LOGGING_VAL (0) // By default and factory new, the realtime logging isn't enabled.
#define DEFAULT_AUDIO_AMP_CAL_STATE_VAL \
    (0) // By default and factory new, the amplifier isn't calibrated. This forces calibration
#define DEFAULT_MIC_MUTE_STATE_VAL (0)  // By default, the mic isn't muted
#define DEFAULT_STREAM_VOLUME_VAL  (-1) // This is an invalid default value to force using cloud volume on factory new
#define DEFAULT_ALERT_VOLUME_VAL \
    (6) // This is to ensure that going offline before first interaction with service doesn't effect volume

#define DEFAULT_CFG_VALUES                                                                    \
    {                                                                                         \
        DEFAULT_USB_LOGGING_VAL, DEFAULT_AUDIO_AMP_CAL_STATE_VAL, DEFAULT_MIC_MUTE_STATE_VAL, \
            DEFAULT_STREAM_VOLUME_VAL, DEFAULT_ALERT_VOLUME_VAL                               \
    }

typedef struct _sln_dev_cfg
{
    uint8_t usb_logging_flag : 1;
    uint8_t audio_amp_calibration_state : 1;
    uint8_t mic_mute_mode : 1;
    uint32_t streamVolume : 32;
    uint32_t alertVolume : 32;
} sln_dev_cfg_t;

#endif //_SLN_CFG_FILE_
