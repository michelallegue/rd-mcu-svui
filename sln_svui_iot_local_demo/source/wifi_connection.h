/*
 * Copyright 2020-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _WIFI_CONNECTION_H_
#define _WIFI_CONNECTION_H_

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define WIFI_LOG                 "WIFI->"

/**
 * @brief Connect the board to a local WiFi network, using credentials stored in flash
 *
 * @return 0 on success or status of the operation otherwise.
 */
uint32_t wifi_connect(void);

/**
 * @brief Print WiFi connection status
 *
 * @return void
 */
void get_wifi_status(void);

#endif
