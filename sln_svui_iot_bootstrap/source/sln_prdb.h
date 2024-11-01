/*
 * Copyright 2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_PRDB_H_
#define _SLN_PRDB_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief  Validate encryption contexts.
 *         If any context missing, restore them and reboot the device.
 *
 * @return kStatus_Success if all contexts are valid.
 */
status_t SLN_PRDB_EncryptionCheck(void);

#endif /* _SLN_PRDB_H_ */
