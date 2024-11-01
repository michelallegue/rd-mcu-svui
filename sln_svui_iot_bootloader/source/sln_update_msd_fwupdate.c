/*
 * Copyright 2021-2022 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sln_update.h"

#if BOOTLOADER_MSD_ENABLED
#include "disk.h"
#endif /* BOOTLOADER_MSD_ENABLED */

#if BOOTLOADER_FWUPDATE_ENABLED
#include "sln_fwupdate.h"
#endif /* BOOTLOADER_FWUPDATE_ENABLED */

bool SLN_CheckForUpdate(void)
{
    bool update = false;

#if BOOTLOADER_MSD_ENABLED
    update = CheckForMSDMode();
#endif /* BOOTLOADER_MSD_ENABLED */

#if BOOTLOADER_FWUPDATE_ENABLED
    if (update == false)
    {
        update = FWUpdate_CheckForUpdate();
    }
#endif /* BOOTLOADER_FWUPDATE_ENABLED */

    return update;
}

bool SLN_PerformFallback(void)
{
    bool state = false;

#if BOOTLOADER_FWUPDATE_ENABLED
    /* TODO: Add FW update code here! */
    state = true;
#elif BOOTLOADER_MSD_ENABLED
    SLN_MSD_FallbackSet(true);
    PerformMSD();
    state = true;
#endif /* BOOTLOADER_AWS_IOT_OTA_ENABLED */

    return state;
}
