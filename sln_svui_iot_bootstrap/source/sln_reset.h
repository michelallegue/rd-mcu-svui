/*
 * Copyright 2020-2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SLN_RESET_H_
#define _SLN_RESET_H_

/**
 * @brief Print a reset message (default prefix + passed message), delay 0.5s, reset the board.
 *
 * @param msg[in] Pointer to a message to be displayed.
 */
void sln_reset(const char *msg);

#endif /* _SLN_RESET_H_ */
