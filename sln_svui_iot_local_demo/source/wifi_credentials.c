/*
 * Copyright 2019-2021, 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "wifi_credentials.h"
#include "FreeRTOSConfig.h"

sln_flash_fs_status_t wifi_credentials_flash_get(wifi_cred_t *wifi_cred)
{
    sln_flash_fs_status_t status       = SLN_FLASH_FS_OK;
    uint32_t len                       = 0;

    status = sln_flash_fs_ops_read(WIFI_CRED_FILE_NAME, NULL, 0, &len);
    if (status == SLN_FLASH_FS_OK)
    {
        status = sln_flash_fs_ops_read(WIFI_CRED_FILE_NAME, (uint8_t *)wifi_cred, 0, &len);
    }

    if (status != SLN_FLASH_FS_OK)
    {
        configPRINTF(("Failed reading WiFi credentials.\r\n"));
    }

    return status;
}

sln_flash_fs_status_t wifi_credentials_flash_set(wifi_cred_t *wifi_cred)
{
    sln_flash_fs_status_t status = SLN_FLASH_FS_OK;

    status = sln_flash_fs_ops_save(WIFI_CRED_FILE_NAME, (uint8_t *)wifi_cred,
                                              sizeof(wifi_cred_t));
    if (status != SLN_FLASH_FS_OK)
    {
        configPRINTF(("Failed to save WiFi credentials in flash memory.\r\n"));
    }
    else
    {
        configPRINTF(("Updated WiFi credentials in flash memory.\r\n"));
    }

    return status;
}

sln_flash_fs_status_t wifi_credentials_flash_reset(void)
{
    sln_flash_fs_status_t status = SLN_FLASH_FS_OK;

    status = sln_flash_fs_ops_erase(WIFI_CRED_FILE_NAME);

    if (status != SLN_FLASH_FS_OK)
    {
        configPRINTF(("Failed to delete WiFi credentials from flash memory.\r\n\r\n"));
    }

    return status;
}
