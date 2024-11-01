/*
 * Copyright 2021-2022 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_UPDATE_H_
#define _SLN_UPDATE_H_

#include "stdbool.h"

/**
 * @brief Check if there is an update request pending. This is project/platform related.
 *
 * @return Return true if there is a request waiting, false otherwise.
 */
bool SLN_CheckForUpdate(void);

/**
 * @brief Start the fallback mechanism if both images (from BankA and BankB) are corrupted.
 *
 * @return Return true if fallback was started, false otherwise.
 */
bool SLN_PerformFallback(void);

#endif /* _SLN_UPDATE_H_ */
