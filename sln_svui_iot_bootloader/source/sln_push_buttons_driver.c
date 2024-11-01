/*
 * Copyright 2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sln_push_buttons_driver.h"

__attribute__((weak)) status_t PUSH_BUTTONS_Init(void)
{
    /* Initialize device's push buttons */
    return kStatus_Success;
}

__attribute__((weak)) bool PUSH_BUTTONS_MSDPressed(void)
{
    /* Check if the button that triggers the MSD is pressed */
    return false;
}

__attribute__((weak)) bool PUSH_BUTTONS_SwitchBankPressed(void)
{
    /* Check if the button that triggers the FWUPDATE OTW is pressed */
    return false;
}
