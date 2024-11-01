/*
 * Copyright 2019-2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WIFI_CREDENTIALS_H_
#define WIFI_CREDENTIALS_H_

#if 0 // TODO add wifi support
#include "sln_flash_ops.h"
#include "fsl_common.h"
#include "sln_flash.h"
#include "stdint.h"

#define WIFI_CRED_FILE_NAME "wifi.dat"
#define WIFI_CRED_FILE_ADDR (0x1C80000)

/* Max password length */
#define WSEC_MAX_PSK_LEN 64

/**
 * @brief The wifi Pre-Shared Key structure
 */
typedef struct _wifi_field
{
    uint8_t length;                  /**< psk length */
    uint8_t value[WSEC_MAX_PSK_LEN]; /**< actual psk value */
} wifi_field_t;

/**
 * @brief The wifi credentials
 */
typedef struct _wifi_cred
{
    wifi_field_t ssid;    /**< The network name */
    wifi_field_t password; /**< The network password, can be \0 */
} wifi_cred_t;

/**
 * @brief Check if wifi credentials are present in memory
 *
 * @return                0 on success, 1 otherwise
 */
status_t wifi_credentials_present();

/**
 * @brief Reads wifi credentials from flash memory
 *
 * @param cred Pointer to a pre-allocated wifi_cred_t structure where the credentials will be stored
 * @return 0 on success, 1 otherwise
 */
status_t wifi_credentials_flash_get(wifi_cred_t *cred);

/**
 * @brief Writes wifi credentials in flash memory
 *
 * @param cred Pointer to a wifi_cred_t structure containing the data that will be written
 * @return 0 on success, 1 otherwise
 */
status_t wifi_credentials_flash_set(wifi_cred_t *cred);

/**
 * @brief Resets wifi credentials stored in flash memory
 *
 * @return                0 on success, 1 otherwise
 */
status_t wifi_credentials_flash_reset(void);

status_t check_valid_credentials(wifi_cred_t *wifi_cred);

#ifdef FFS_ENABLED
/**
 * @brief Copy the WiFi credentials from KVS to flash memory
 *
 * @return                0 on success, 1 otherwise
 */
status_t wifi_credentials_sync(void);
#endif

#endif
#endif /* WIFI_CREDENTIALS_H_ */
