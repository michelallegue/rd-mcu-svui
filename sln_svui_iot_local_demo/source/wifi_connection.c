/*
 * Copyright 2020-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "wifi_connection.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "sln_flash_fs_ops.h"
#include "wpl.h"
#include "wifi_credentials.h"
#if ENABLE_WIFI_AUDIO_DUMP
#include "socket_task.h"
#endif /* ENABLE_WIFI_AUDIO_DUMP */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Max number of attempts to connect to the given network */
#define MAX_CONNECTION_ATTEMPTS 3U

/*******************************************************************************
 * Variables
 ******************************************************************************/

static bool s_WiFiConnected = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

#if ENABLE_WIFI_AUDIO_DUMP
static void call_socket_task_init(int is_tcp, const char *ip_str, const char *port_str)
{
    uint32_t ret = 0;

    ret = socket_task_init(is_tcp, ip_str, port_str);
    if (ret < 0)
    {
        configPRINTF(("SOCKET INIT FAILED. Status: %d", ret));
    }
    else
    {
        configPRINTF(("SOCKET INIT success\r\n", ret));
    }
}
#endif /* ENABLE_WIFI_AUDIO_DUMP */

static void LinkStatusChangeCallback(bool linkState)
{
    if (linkState == false)
    {
        /* -------- LINK LOST -------- */
        s_WiFiConnected = false;
        configPRINTF(("-------- LINK LOST --------\r\n"));
    }
    else
    {
        /* -------- LINK REESTABLISHED -------- */
        s_WiFiConnected = true;
        configPRINTF(("-------- LINK REESTABLISHED --------\r\n"));
    }
}

uint32_t wifi_connect(void)
{
    wpl_ret_t wplStatus                  = WPLRET_SUCCESS;
    sln_flash_fs_status_t flashStatus    = SLN_FLASH_FS_OK;
    wifi_cred_t wifi_cred = {0};
    static bool s_wifiAlreadyInitialized = false;
    char ip[16];
    int32_t s_connAttempts;

    flashStatus = wifi_credentials_flash_get(&wifi_cred);

    if (flashStatus != SLN_FLASH_FS_OK)
    {
        configPRINTF(("%s Could not read WiFi credentials from flash...\r\n", WIFI_LOG));
        configPRINTF(("%s Skip WiFi connection...\r\n", WIFI_LOG));
        return flashStatus;
    }

    if (s_wifiAlreadyInitialized == false)
    {
        configPRINTF(("%s Initializing WiFi...\r\n", WIFI_LOG));
        /* Initialize GPIO functionality on PMIC_ON_REQ (pin K7) */
        GPIO_PinWrite(GPIO5, 1U, 1);

        wplStatus = WPL_Init();
        if (wplStatus != WPLRET_SUCCESS)
        {
            configPRINTF(("%s WPL Init failed. Status: %d\r\n", WIFI_LOG, wplStatus));
            return wplStatus;
        }

        s_wifiAlreadyInitialized = true;
    }
    configPRINTF(("%s WiFi driver and WPL layer initialized successfully.\r\n", WIFI_LOG));

    wplStatus = WPL_Start(LinkStatusChangeCallback);
    if (wplStatus != WPLRET_SUCCESS)
    {
        configPRINTF(("%s WPL Start failed. Status: %d\r\n", WIFI_LOG, wplStatus));
        return wplStatus;
    }
    configPRINTF(("%s WiFi driver started successfully.\r\n", WIFI_LOG));

    /* Join network test */
    wplStatus = WPL_AddNetwork((char*)wifi_cred.ssid.value, (char*)wifi_cred.password.value, (char*)wifi_cred.ssid.value);
    if (wplStatus != WPLRET_SUCCESS)
    {
        configPRINTF(("%s Adding new network failed. Status: %d\r\n", WIFI_LOG, wplStatus));
        return wplStatus;
    }

    configPRINTF(("%s Joining %s...\r\n", WIFI_LOG, (char*)wifi_cred.ssid.value));
    for (s_connAttempts = 0; s_connAttempts < MAX_CONNECTION_ATTEMPTS; s_connAttempts++)
    {
        wplStatus = WPL_Join((char*)wifi_cred.ssid.value);
        if (wplStatus == WPLRET_NOT_FOUND)
        {
            configPRINTF(("%s Attempt %d/3 to join %s failed. Status: %d\r\n",
                    WIFI_LOG, (s_connAttempts + 1), (char*)wifi_cred.ssid.value, wplStatus));
        }
        else if (wplStatus == WPLRET_SUCCESS)
        {
            s_WiFiConnected = true;
            break;
        }
        else
        {
            configPRINTF(("%s Joining %s failed. Status: %d\r\n",
                    WIFI_LOG, (char*)wifi_cred.ssid.value, wplStatus));
            return wplStatus;
        }
    }

    wplStatus = WPL_GetIP(ip, 1);
    if (wplStatus != WPLRET_SUCCESS)
    {
        configPRINTF(("%s Getting IP failed. Status: %d\r\n", WIFI_LOG, wplStatus));
        return wplStatus;
    }
    else
    {
        configPRINTF(("%s Successfully joined %s. IP address: %s\r\n", WIFI_LOG, (char*)wifi_cred.ssid.value, ip));
    }

#if DUMP_OVER_UDP
    call_socket_task_init(0, NULL, "10001");
#endif /* DUMP_OVER_UDP */
#if DUMP_OVER_TCP
    call_socket_task_init(1, NULL, "10001");
#endif /* DUMP_OVER_TCP */

    return wplStatus;
}

void get_wifi_status(void)
{
    char ip[16];
    wpl_ret_t wplStatus = WPLRET_SUCCESS;

    if (s_WiFiConnected)
    {
        wplStatus = WPL_GetIP(ip, 1);
        if (wplStatus == WPLRET_SUCCESS)
        {
            configPRINTF(("Connected. IP: %s\r\n", ip));
        }
        else
        {
            configPRINTF(("Getting IP failed. Status: %d\r\n", wplStatus));
        }
    }
    else
    {
        configPRINTF(("Board is not connected to WiFi\r\n"));
    }
}
