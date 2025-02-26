/*
 * Copyright 2019-2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common_connection_handler.h"
#include "comms_connection_handler_cfg.h"
#if COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTA_TCP
#include "tcp_connection_handler_private.h"
#endif
#if COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTW_UART
#include "uart_connection_handler_private.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

sln_common_connection_message_status_t SLN_CommonConnectionHandlerInit(sln_common_connection_desc_t *desc)
{
    sln_common_connection_message_status_t status = kCommon_Success;

    if (NULL == desc->recv_cb)
    {
        status = kCommon_Failed;
    }

    if (kCommon_Success == status)
    {
        switch (desc->connType)
        {
            case kCommonInterfaceTcp:
#if COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTA_TCP
                desc->context.sTcpContext.max_rx_buff_size = OTA_MAX_BUFFER_SIZE;
                desc->context.sTcpContext.port             = OTA_TCP_COMMS_PORT;
                status                                     = SLN_TCP_COMMS_Init(desc);
#else
                status = kCommon_Failed;
#endif
                break;
            case kCommonInterfaceUart:
#if COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTW_UART
                /* Configure the UART connection */
                desc->context.sUartContext.portbase         = FWUPDATE_OTW_UART_BASEADDR;
                desc->context.sUartContext.speed_bps        = FWUPDATE_OTW_UART_BAUDRATE;
                desc->context.sUartContext.clksrc_hz        = FWUPDATE_OTW_UART_SRCFREQ;
                desc->context.sUartContext.max_rx_buff_size = OTW_MAX_BUFFER_SIZE;

                status = SLN_UART_COMMS_Init(desc);
#else
                status = kCommon_Failed;
#endif
                break;
            default:
                status = kCommon_Failed;
        }
    }

    return status;
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
