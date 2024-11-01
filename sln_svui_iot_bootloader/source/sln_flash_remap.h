/*
 * Copyright 2022 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_FLASH_REMAP_H_
#define _SLN_FLASH_REMAP_H_

#include "fsl_common.h"

/* Remap address uses only addr[31:12] */
#define FLASH_REMAP_ALIGN_MASK 0xFFF

/*!
 * @brief Remap flash in order to support dual bank applications.
 * Once activated any access made in the range of bank B -> bank B + bank size will be treated like an access made
 * in the range bank A -> bank A + bank size
 *
 * Usage:
 *
 * Build only one application for both Bank A/B.
 * Bank A Starting address = BankAStartAddr, Size BankSize
 * Bank B Starting address = BankBStartAddr, Size BankSize
 * BankAStartAddr < BankACode < BankAStartAddr + BankSize
 * BankBStartAddr < BankBCode < BankBStartAddr + BankSize
 * BankACode after remap will point to (BankACode + offset)
 * which, if the offset was set right, it will reference BankBCode
 *
 * @code
 * offset = BankBStartAddr - BankAStartAddr;
 * SLN_EnableRemap(BankAStartAddr, BankAStartAddr + BankSize, offset)
 * @endcode
 * uint32_t addrStart Starting address that will be remapped
 * uint32_t addrStop  Last address that will be remapped
 * uint32_t offset    The value of the jump
 * @returns Status of remapping
 */
status_t SLN_EnableRemap(uint32_t addrStart, uint32_t addrStop, uint32_t offset);

/*!
 * @brief Remap disable
 */
void SLN_DisableRemap(void);

#endif /* _SLN_FLASH_REMAP_H_ */
