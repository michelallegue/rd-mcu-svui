/*
 * Copyright 2022, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_WIFI_AUDIO_DUMP
#ifndef SOCKET_TASK_H
#define SOCKET_TASK_H

#include "fsl_shell.h"
#include "lwip/sockets.h"

int socket_task_init(int is_tcp, const char *ip_str, const char *port_str);
void socket_task_terminate(void);
void socket_task_print_ips(void);

/*!
 * @brief Forward afe processed buffers over TCP or UDP.
 *
 * @param cleanStream          The afe processed buffer
 */
void AUDIO_DUMP_ForwardDataOverWiFi(int16_t *micStream, int16_t *ampStream, void *cleanStream);

#endif /* SOCKET_TASK_H */
#endif /* ENABLE_WIFI_AUDIO_DUMP */
