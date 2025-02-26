/*
 * Copyright 2019-2022 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_FLASH_OPS_
#define _SLN_FLASH_OPS_

#include "flexspi_nor_flash_ops.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define sln_flash_ops_init(a)               flexspi_nor_flash_init(a)
#define sln_flash_ops_get_flash_id(a, b)    flexspi_nor_get_vendor_id(a, b)
#define sln_flash_ops_page_program(a, b, c) flexspi_nor_flash_page_program(a, b, c)
#define sln_flash_ops_erase_sector(a, b)    flexspi_nor_flash_erase_sector(a, b)
#define sln_flash_ops_read(a, b, c, d)      flexspi_nor_flash_read(a, b, c, d)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#endif /* _SLN_FLASH_OPS_ */
