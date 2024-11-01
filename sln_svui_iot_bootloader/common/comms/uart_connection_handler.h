/*
 * Copyright 2019-2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file uart_connection_handler.h
 * @brief This file handles the uart connection
 */

#ifndef UART_CONNECTION_HANDLER_H_
#define UART_CONNECTION_HANDLER_H_

#include "stdint.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct _sln_uart_connection_context
{
    uint32_t fd;
} sln_uart_connection_context_t;

/* This is a small structure member. This may be added to in the future
 * so it's best to start as a structure
 */
typedef struct _sln_uart_interface_context
{
    uint32_t portbase;
    uint32_t speed_bps;
    uint32_t clksrc_hz;
    uint32_t max_rx_buff_size;
} sln_uart_interface_context_t;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

#endif /* UART_CONNECTION_HANDLER_H_ */
