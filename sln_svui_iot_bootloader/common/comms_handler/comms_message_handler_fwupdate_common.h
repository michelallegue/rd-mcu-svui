/*
 * Copyright 2019-2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file comms_message_handler_fwupdate_common.h
 * @brief This file contains the FwUpdate common handlers
 */

#ifndef COMMS_MESSAGE_HANDLER_FWUPDATE_COMMON_H_
#define COMMS_MESSAGE_HANDLER_FWUPDATE_COMMON_H_

#include "comms_message_handler.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define COMMS_FWUPDATE_MAX_SIGNATURE_SIZE      256
#define COMMS_PACKET_CRC_SIZE                  4
#define COMMS_PACKET_CRC_OFFSET(packet_length) (packet_length - 4)

/* Use this index to determine the base programming address from the passed image reset vector */
#define COMMS_PACKET_IMG_ADDR_IDX 6

typedef struct _sln_comms_fwupdate_job_desc
{
    uint32_t status;
    uint32_t imageSize;
    uint32_t appBankType;
    uint32_t dataWritten;
    char signature[COMMS_FWUPDATE_MAX_SIGNATURE_SIZE];
    char *jobId;
} sln_comms_fwupdate_job_desc_t;

typedef enum _sln_comms_message_fwupdate_type
{
    kCommsFwUpdateCommonMsg = 0,
    kCommsFwUpdateServerMsg,
} sln_comms_message_fwupdate_type_t;

typedef enum _sln_comms_message_fwupdate_common_type
{
    kCommsFwUpdateStart = 0,
    kCommsFwUpdateState,
    kCommsFwUpdateClean,
} sln_comms_message_fwupdate_common_type_t;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/**
 * @brief Process the incoming FwUpdate JSON message
 *
 * @param clientInstance: The client instance data (received data and connection context)
 * @param json: Incoming payload to be processed
 *
 * @return          sln_comms_message_status_t
 */
sln_comms_message_status_t processFwUpdateReq(void *clientInstance, cJSON *json);

/**
 * @brief Get the current FwUpdate job handle
 *
 * @return          sln_comms_fwupdate_job_desc_t: Handle of the FwUpdate descriptor
 */
sln_comms_fwupdate_job_desc_t *getCurrentFwUpdateJob(void);

#endif /* COMMS_MESSAGE_HANDLER_FWUPDATE_COMMON_H_ */
