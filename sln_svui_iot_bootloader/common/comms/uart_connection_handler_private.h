/*
 * Copyright 2019-2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file uart_connection_handler_private.h
 * @brief This file declares the private functions
 */

#ifndef UART_CONNECTION_HANDLER_PRIVATE_H_
#define UART_CONNECTION_HANDLER_PRIVATE_H_

#include "common_connection_handler.h"

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
sln_common_connection_message_status_t SLN_UART_COMMS_Init(sln_common_connection_desc_t *descriptor);

#endif /* UART_CONNECTION_HANDLER_PRIVATE_H_ */
