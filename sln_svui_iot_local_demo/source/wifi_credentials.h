/*
 * Copyright 2019-2020, 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WIFI_CREDENTIALS_H_
#define WIFI_CREDENTIALS_H_

#include "sln_flash_ops.h"
#include "fsl_common.h"
#include "sln_flash.h"
#include "stdint.h"
#include "sln_flash_fs_ops.h"

#define WIFI_CRED_FILE_NAME "wifi.dat"

#define SSID_NAME_SIZE 32
#define PASS_SIZE      63

/**
 * @brief Structure for storing a Service Set Identifier (i.e. Name of Access Point)
 */
typedef struct
{
    uint8_t length;                  /**< SSID length */
    uint8_t value[SSID_NAME_SIZE];   /**< SSID name (AP name)  */
} wifi_ssid_t;

/**
 * @brief Structure for storing password (i.e. Name of Access Point)
 */
typedef struct
{
    uint8_t length;                  /**< pass length */
    uint8_t value[PASS_SIZE];        /**< pass value */
} wifi_pass_t;

/**
 * @brief The wifi credentials
 */
typedef struct _wifi_cred
{
    wifi_ssid_t ssid;    /**< The network name */
    wifi_pass_t password; /**< The network password, can be \0 */
} wifi_cred_t;

/**
 * @brief Reads wifi credentials from flash memory
 *
 * @param cred Pointer to a pre-allocated wifi_cred_t structure where the credentials will be stored
 * @return 0 on success, flash status otherwise
 */
sln_flash_fs_status_t wifi_credentials_flash_get(wifi_cred_t *cred);

/**
 * @brief Writes wifi credentials in flash memory
 *
 * @param cred Pointer to a wifi_cred_t structure containing the data that will be written
 * @return 0 on success, flash status otherwise
 */
sln_flash_fs_status_t wifi_credentials_flash_set(wifi_cred_t *cred);

/**
 * @brief Resets wifi credentials stored in flash memory
 *
 * @return 0 on success, flash status otherwise
 */
sln_flash_fs_status_t wifi_credentials_flash_reset(void);

#endif /* WIFI_CREDENTIALS_H_ */
