/*
 * Copyright 2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOOTSTRAP_MAIN_H_
#define _BOOTSTRAP_MAIN_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief BOOTSTRAP Task settings */
#define BOOTSTRAP_TASK_NAME       "Bootstrap_Task"
#define BOOTSTRAP_TASK_STACK_SIZE 1024
#define BOOTSTRAP_TASK_PRIORITY   (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief If ENABLE_ENCRYPTION is 1 and device has encrypted XIP enabled, check PRDB contexts.
 *        If something wrong, reset.
 *        If DISABLE_IMAGE_VERIFICATION is 0, validate available images (Bootloader, Current Application, Backup
 *        Application) using theirs certificates. Select as "next application" first valid image. If none validated,
 *        reset. Jump to "next application".
 *
 * @param arg Not used
 */
void BOOTSTRAP_Task(void *arg);

#endif /* _BOOTSTRAP_MAIN_H_ */
