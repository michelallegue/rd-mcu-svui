/*
 * Copyright 2018, 2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file tcp_conection_handler.h
 * @brief This file handles the tcp connection
 */

#ifndef TCP_CONNECTION_HANDLER_H_
#define TCP_CONNECTION_HANDLER_H_

#include "stdint.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct _sln_tcp_connection_context
{
    uint32_t fd;
} sln_tcp_connection_context_t;

/* This is a small structure member. This may be added to in the future
 * so it's best to start as a structure
 */
typedef struct _sln_tcp_interface_context
{
    uint32_t port;
    uint32_t max_rx_buff_size;
} sln_tcp_interface_context_t;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

#endif /* TCP_CONNECTION_HANDLER_H_ */
