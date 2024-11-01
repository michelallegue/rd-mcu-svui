/*
 * Copyright 2023 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DEMO_ACTIONS_H_

#define DEMO_ACTIONS_H_

#if ENABLE_DSMT_ASR
#include "demo_actions_dsmt.h"
#elif ENABLE_VIT_ASR
#include "demo_actions_vit.h"
#endif /* ENABLE_DSMT_ASR */

#endif /* DEMO_ACTIONS_H_ */
