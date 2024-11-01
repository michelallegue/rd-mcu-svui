/*
 * Copyright 2018, 2021-2022 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file bootloader.h
 * @brief Bootloader Main
 */

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

/* Includes required by the FreeRTOS timers structure. */
#include "FreeRTOS.h"
#include "timers.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief ReRun Bootloader function.
 *
 * Restarts the Bootloader, soft reset.
 */
void ReRunBootloader();

/**
 * @brief Bootloader Main function.
 *
 * Execute the primary Bootloader main functions. Cycles LEDs to alert user it is running,
 * detects if the user is pressing a button for MSD, to start Factory or Main Application OTA, or
 * do a factory reset.
 */
void BootloaderMain(void *args);

#endif /* _BOOTLOADER_H_ */
