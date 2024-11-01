/*
 * Copyright 2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#define RESET_PREFIX_MSG "[System reset]"

#define RESET_DELAY_MS    500
#define RESET_DELAY_TICKS (RESET_DELAY_MS * portTICK_PERIOD_MS)

void sln_reset(const char *msg)
{
    char prefix_msg[] = RESET_PREFIX_MSG;

    if (msg == NULL)
    {
        configPRINTF(("%s - no reason specified\r\n", prefix_msg));
    }
    else
    {
        configPRINTF(("%s - %s\r\n", prefix_msg, msg));
    }

    vTaskDelay(RESET_DELAY_TICKS);

    NVIC_SystemReset();
}
