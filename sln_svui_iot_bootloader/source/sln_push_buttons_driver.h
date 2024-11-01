/*
 * Copyright 2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_PUSH_BUTTONS_DRIVER_H_
#define _SLN_PUSH_BUTTONS_DRIVER_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Push Button Initialization
 *
 * @returns Status of initialization
 */
status_t PUSH_BUTTONS_Init(void);

/*!
 * @brief Checks whether or not the MSD button is pressed
 *
 * @returns true if pressed, false if not
 */
bool PUSH_BUTTONS_MSDPressed(void);

/*!
 * @brief Checks whether or not the Switch Bank button is pressed
 *
 * @returns true if pressed, false if not
 */
bool PUSH_BUTTONS_SwitchBankPressed(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _SLN_PUSH_BUTTONS_DRIVER_H_ */
