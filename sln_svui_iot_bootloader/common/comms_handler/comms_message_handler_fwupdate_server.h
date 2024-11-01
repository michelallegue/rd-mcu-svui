/*
 * Copyright 2019-2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file comms_message_handler_fwupdate_server.h
 * @brief This file contains the FwUpdate server handler
 */

#ifndef COMMS_MESSAGE_HANDLER_FWUPDATE_SERVER_H_
#define COMMS_MESSAGE_HANDLER_FWUPDATE_SERVER_H_

#include "comms_message_handler.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef enum _sln_comms_message_fwupdate_server_type
{
    kCommsFwUpdateBlock = 0,
    kCommsFwUpdateStop,
    kCommsFwUpdateStartSelfCheck,
    kCommsFwUpdateActivateImage
} sln_comms_message_fwupdate_server_type_t;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/**
 * @brief Process the FwUpdate server JSON message
 *
 * @param clientInstance: The client instance data (received data and connection context)
 * @param json: Incoming payload to be processed
 *
 * @return          sln_comms_message_status
 */
sln_comms_message_status_t processFwUpdateServerReq(void *clientInstance, cJSON *json);

/**
 * @brief Start the FwUpdate process
 *
 * @param img_type: The type of the firmware image
 *
 * @return          sln_comms_message_status
 */
sln_comms_message_status_t handleServerFwUpdateStartReq(uint32_t img_type);

#endif /* COMMS_MESSAGE_HANDLER_FWUPDATE_SERVER_H_ */
