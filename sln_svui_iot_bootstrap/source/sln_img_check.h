/*
 * Copyright 2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_IMG_CHECK_H_
#define _SLN_IMG_CHECK_H_

#include <stdint.h>
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief  Search through (Bootloader, Active Application, Backup Application) images list and detects
 *         first valid image address.
 *
 * @param imageAddr Pointer where to store first valid image address.
 *
 * @return kStatus_Success if a valid image was found.
 */
status_t SLN_IMG_CHECK_GetValidImgAddress(uint32_t *imageAddr);

#endif /* _SLN_IMG_CHECK_H_ */
