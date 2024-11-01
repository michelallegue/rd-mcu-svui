/*
 * Copyright 2022-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_USB_AUDIO_DUMP

#include "usb.h"
#include "usb_device.h"
#include "usb_device_config.h"
#include "usb_device_descriptor.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "virtual_com.h"

#include "audio_dump.h"
#include "sln_mic_config.h"
#include "rectangular_sound_wav.h"
#include "sln_amplifier.h"

#if ENABLE_AMPLIFIER
#include "fsl_gpio.h"
#endif /* ENABLE_AMPLIFIER */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USB_BUFFER_OUTPUT_VCOM_SIZE \
    ((PCM_SAMPLE_COUNT * PCM_SAMPLE_SIZE_BYTES) + (PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES * 2))

#define DUMP_QUEUE_SLOTS 3
/*******************************************************************************
 * Variables
 ******************************************************************************/
SDK_ALIGN(static int16_t __attribute__((section(".bss.$SRAM_OC_NON_CACHEABLE")))
          s_dumpStream[PCM_SINGLE_CH_SMPL_COUNT * (SLN_MIC_COUNT + 2)],
          8);
QueueHandle_t g_xDumpQueue        = NULL;
TaskHandle_t  audioDumpTaskHandle = NULL;

/* Abstract state of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_abstractState[COMM_FEATURE_DATA_SIZE] = {(STATUS_ABSTRACT_STATE >> 0U) & 0x00FFU,
                                                          (STATUS_ABSTRACT_STATE >> 8U) & 0x00FFU};

/* Country code of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_countryCode[COMM_FEATURE_DATA_SIZE] = {(COUNTRY_SETTING >> 0U) & 0x00FFU,
                                                        (COUNTRY_SETTING >> 8U) & 0x00FFU};
static bool s_startUsbForward                        = false;

#if ENABLE_AMPLIFIER
static bool s_playSound                              = false;
#endif /* ENABLE_AMPLIFIER */

extern usb_device_endpoint_struct_t g_cdcVcomDicEndpoints[];
extern usb_device_class_struct_t g_UsbDeviceCdcVcomConfig;
extern volatile long unsigned int g_processedFrames;

/* Data structure of virtual com device */
extern usb_cdc_vcom_struct_t *audio_dump_vcom;
/* Line coding of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_lineCoding[LINE_CODING_SIZE] = {
    /* E.g. 0x00,0xC2,0x01,0x00 : 0x0001C200 is 115200 bits per second */
    (LINE_CODING_DTERATE >> 0U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 8U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 16U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 24U) & 0x000000FFU,
    LINE_CODING_CHARFORMAT,
    LINE_CODING_PARITYTYPE,
    LINE_CODING_DATABITS};
/* CDC ACM information */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static usb_cdc_acm_info_t s_usbCdcAcmInfo;
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Handle command received on serial port
 *
 * @param command         The character representing the command
 */
static void AUDIO_DUMP_handle_command(char command)
{
    if ('s' == command)
    {
        s_startUsbForward = true;
    }
    else if ('c' == command)
    {
        s_startUsbForward = false;
    }
}

usb_status_t AUDIO_DUMP_USB_DeviceCdcVcomCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    uint32_t len;
    uint8_t *uartBitmap;
    usb_cdc_acm_info_t *acmInfo = &s_usbCdcAcmInfo;
    usb_device_cdc_acm_request_param_struct_t *acmReqParam;
    usb_device_endpoint_callback_message_struct_t *epCbParam;
    acmReqParam = (usb_device_cdc_acm_request_param_struct_t *)param;
    epCbParam   = (usb_device_endpoint_callback_message_struct_t *)param;
    switch (event)
    {
        case kUSB_DeviceCdcEventSendResponse:
        {
            if ((epCbParam->length != 0) && (!(epCbParam->length % g_cdcVcomDicEndpoints[0].maxPacketSize)))
            {
                /* If the last packet is the size of endpoint, then send also zero-ended packet,
                ** meaning that we want to inform the host that we do not have any additional
                ** data, so it can flush the output.*/
                error = USB_DeviceCdcAcmSend(handle, USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT, NULL, 0);
            }
            else if ((1 == audio_dump_vcom->attach) && (1 == audio_dump_vcom->startTransactions))
            {
                if ((epCbParam->buffer != NULL) || ((epCbParam->buffer == NULL) && (epCbParam->length == 0)))
                {
                    /* User: add your own code for send complete event
                       Schedule buffer for next receive event */
#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
                    s_waitForDataReceive = 1;
                    USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
#endif
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventRecvResponse:
        {
            if ((1 == audio_dump_vcom->attach) && (1 == audio_dump_vcom->startTransactions))
            {
                if (0 != epCbParam->length)
                {
                    AUDIO_DUMP_handle_command(epCbParam->buffer[0]);
                }

#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
                s_waitForDataReceive = 0;
                USB0->INTEN |= USB_INTEN_SOFTOKEN_MASK;
#endif
                /* Schedule buffer for next receive event */
                error = USB_DeviceCdcAcmRecv(handle, USB_CDC_VCOM_DIC_BULK_OUT_ENDPOINT, epCbParam->buffer,
                                             g_cdcVcomDicEndpoints[0].maxPacketSize);
#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
                s_waitForDataReceive = 1;
                USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
#endif
            }
        }
        break;
        case kUSB_DeviceCdcEventSerialStateNotif:
            ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 0;
            error                                                 = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSendEncapsulatedCommand:
            break;
        case kUSB_DeviceCdcEventGetEncapsulatedResponse:
            break;
        case kUSB_DeviceCdcEventSetCommFeature:
            if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue)
            {
                if (1 == acmReqParam->isSetup)
                {
                    *(acmReqParam->buffer) = s_abstractState;
                }
                else
                {
                    *(acmReqParam->length) = 0;
                }
            }
            else if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue)
            {
                if (1 == acmReqParam->isSetup)
                {
                    *(acmReqParam->buffer) = s_countryCode;
                }
                else
                {
                    *(acmReqParam->length) = 0;
                }
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventGetCommFeature:
            if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue)
            {
                *(acmReqParam->buffer) = s_abstractState;
                *(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
            }
            else if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue)
            {
                *(acmReqParam->buffer) = s_countryCode;
                *(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventClearCommFeature:
            break;
        case kUSB_DeviceCdcEventGetLineCoding:
            *(acmReqParam->buffer) = s_lineCoding;
            *(acmReqParam->length) = LINE_CODING_SIZE;
            error                  = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSetLineCoding:
        {
            if (1 == acmReqParam->isSetup)
            {
                *(acmReqParam->buffer) = s_lineCoding;
            }
            else
            {
                *(acmReqParam->length) = 0;
            }
        }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSetControlLineState:
        {
            s_usbCdcAcmInfo.dteStatus = acmReqParam->setupValue;
            // activate/deactivate Tx carrier
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }

            // activate carrier and DTE
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }

            // Indicates to DCE if DTE is present or not
            acmInfo->dtePresent = (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE) ? true : false;

            // Initialize the serial state buffer
            acmInfo->serialStateBuf[0] = NOTIF_REQUEST_TYPE;                // bmRequestType
            acmInfo->serialStateBuf[1] = USB_DEVICE_CDC_NOTIF_SERIAL_STATE; // bNotification
            acmInfo->serialStateBuf[2] = 0x00;                              // wValue
            acmInfo->serialStateBuf[3] = 0x00;
            acmInfo->serialStateBuf[4] = 0x00; // wIndex
            acmInfo->serialStateBuf[5] = 0x00;
            acmInfo->serialStateBuf[6] = UART_BITMAP_SIZE; // wLength
            acmInfo->serialStateBuf[7] = 0x00;
            // Notifiy to host the line state
            acmInfo->serialStateBuf[4] = acmReqParam->interfaceIndex;
            // Lower byte of UART BITMAP
            uartBitmap    = (uint8_t *)&acmInfo->serialStateBuf[NOTIF_PACKET_SIZE + UART_BITMAP_SIZE - 2];
            uartBitmap[0] = acmInfo->uartState & 0xFFu;
            uartBitmap[1] = (acmInfo->uartState >> 8) & 0xFFu;
            len           = (uint32_t)(NOTIF_PACKET_SIZE + UART_BITMAP_SIZE);
            if (0 == ((usb_device_cdc_acm_struct_t *)handle)->hasSentState)
            {
                error =
                    USB_DeviceCdcAcmSend(handle, USB_CDC_VCOM_CIC_INTERRUPT_IN_ENDPOINT, acmInfo->serialStateBuf, len);
                error = kStatus_USB_Success;
                if (kStatus_USB_Success != error)
                {
                    usb_echo("kUSB_DeviceCdcEventSetControlLineState error!");
                }
                ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 1;
            }

            // Update status
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION)
            {
                // To do: CARRIER_ACTIVATED
            }
            else
            {
                // To do: CARRIER_DEACTIVATED
            }
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE)
            {
                // DTE_ACTIVATED
                if (1 == audio_dump_vcom->attach)
                {
                    audio_dump_vcom->startTransactions = 1;
#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
    defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
    defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
                    s_waitForDataReceive = 1;
                    USB0->INTEN &= ~USB_INTEN_SOFTOKEN_MASK;
                    s_comOpen = 1;
                    usb_echo("USB_APP_CDC_DTE_ACTIVATED\r\n");
#endif
                }
            }
            else
            {
                // DTE_DEACTIVATED
                if (1 == audio_dump_vcom->attach)
                {
                    audio_dump_vcom->startTransactions = 0;
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventSendBreak:
            break;
        default:
            break;
    }

    return error;
}

void AUDIO_DUMP_Task(void *arg)
{
    static uint8_t *s_usbBuffer = NULL;
    usb_status_t status = kStatus_USB_Success;

    while (!g_xDumpQueue)
        vTaskDelay(10);

    if (s_usbBuffer == NULL)
    {
        configPRINTF(("Audio Dump Started\r\n"));
        s_usbBuffer = (uint8_t *)pvPortMalloc(USB_BUFFER_OUTPUT_VCOM_SIZE);
    }

    if (s_usbBuffer == NULL)
    {
        configPRINTF(("ERROR: Failed to allocate memory for usbBuffer of %d!\r\n", USB_BUFFER_OUTPUT_VCOM_SIZE));
    }
    else
    {
        memset(s_usbBuffer, 0, USB_BUFFER_OUTPUT_VCOM_SIZE);

        while (s_startUsbForward)
        {
            if (xQueueReceive(g_xDumpQueue, s_usbBuffer, 1000) != pdPASS)
            {
                configPRINTF(("Could not receive from the queue\r\n"));
            }
            else
            {
                /* retry up to 5 times to send a package if getting busy status */
                for (int i = 0; i < 5; i++)
                {
                    status = USB_DeviceCdcAcmSend(audio_dump_vcom->cdcAcmHandle, USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT, s_usbBuffer,
                                                  USB_BUFFER_OUTPUT_VCOM_SIZE);

                    /* terminate the loop if successfully sent the package or got an error different than busy */
                    if ((kStatus_USB_Success == status) || (kStatus_USB_Busy != status))
                    {
                        break;
                    }
                    else
                    if (kStatus_USB_Busy == status)
                    {
                        vTaskDelay(1);
                    }
                }

                if (status != kStatus_USB_Success)
                {
                    // Suppress error log
                    configPRINTF(("ERROR: USB_DeviceCdcAcmSend failed %d\r\n", status));
                }
            }
        }

        vTaskDelay(100);

        vPortFree(s_usbBuffer);
        s_usbBuffer = NULL;
        configPRINTF(("Audio Dump Canceled\r\n"));
    }

    if (g_xDumpQueue != NULL)
    {
        vQueueDelete(g_xDumpQueue);
    }

    //set handlers to NULL so they'll be re-created when a new dump starts
    g_xDumpQueue        = NULL;
    audioDumpTaskHandle = NULL;

    vTaskDelete(NULL);
}

void AUDIO_DUMP_ForwardDataOverUsb(int16_t *micStream, int16_t *ampStream, void *cleanStream)
{
    uint32_t u32Element = 0;

    if (s_startUsbForward)
    {
        if (g_xDumpQueue == NULL)
        {
            g_xDumpQueue = xQueueCreate(DUMP_QUEUE_SLOTS, PCM_SINGLE_CH_SMPL_COUNT * (SLN_MIC_COUNT + 2) * sizeof(short));
            if (g_xDumpQueue == NULL)
            {
                configPRINTF(("Failed to create DumpQueue!\r\n"));
            }

            // reset g_processedFrames to match the exported audio
            g_processedFrames = 0;
        }

        if (audioDumpTaskHandle == NULL)
        {
            if (xTaskCreate(AUDIO_DUMP_Task, "audio_dump_task", 256, NULL, configMAX_PRIORITIES - 3,
                                &audioDumpTaskHandle) != pdPASS)
            {
                configPRINTF(("xTaskCreate AUDIO_DUMP_Task failed!\r\n"));
            }
        }

        /* Prepare and send clean data to Audio Dump task */
        memcpy(&s_dumpStream[u32Element], (uint8_t *)micStream, PCM_SAMPLE_COUNT * PCM_SAMPLE_SIZE_BYTES);
        u32Element += PCM_SAMPLE_COUNT;
        if (ampStream != NULL)
        {
            memcpy(&s_dumpStream[u32Element], (uint8_t *)ampStream, PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES);
        }
        else
        {
            memset(&s_dumpStream[u32Element], 0, PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES);
        }
        u32Element += PCM_SINGLE_CH_SMPL_COUNT;
        memcpy(&s_dumpStream[u32Element], (uint8_t *)cleanStream, PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES);
        u32Element += PCM_SINGLE_CH_SMPL_COUNT;

        if (xQueueSendToBack(g_xDumpQueue, s_dumpStream, 0) != pdPASS)
        {
            configPRINTF(("Failed to push frames to audio dump queue\r\n"));
        }
    }
}

#if ENABLE_AMPLIFIER
void AUDIO_DUMP_AecAlignSoundTask(void *arg)
{
    status_t status    = kStatus_Success;
    uint8_t *sound     = (uint8_t *)rectangular_sound_wav;
    uint32_t soundSize = sizeof(rectangular_sound_wav);

    vTaskDelay(AEC_ALIGN_DELAY_STARTUP_MS);

    while (1)
    {
        if (s_playSound == true)
        {
            /* power on the amp */
            GPIO_PinWrite(GPIO2, 2, 1);
            vTaskDelay(150);

            SLN_AMP_SetVolume(AEC_ALIGN_SOUND_VOLUME);
            status = SLN_AMP_WriteAudioBlocking(sound, soundSize, DEFAULT_AMP_SLOT_SIZE, DEFAULT_AMP_SLOT_CNT, true);
            if (status != kStatus_Success)
            {
                configPRINTF(("[Error] SLN_AMP_WriteAudioBlocking failed %d\r\n", status));
            }

            /* power off the amp */
            GPIO_PinWrite(GPIO2, 2, 0);

            vTaskDelay(AEC_ALIGN_DELAY_BETWEEN_SOUND_PLAYS_MS);
        }
        vTaskDelay(100);
    }

    vTaskDelete(NULL);
}

void AUDIO_DUMP_AecAlignSoundStart(void)
{
    s_playSound = true;
}

void AUDIO_DUMP_AecAlignSoundStop(void)
{
    s_playSound = false;
}
#endif /* ENABLE_AMPLIFIER */
#endif /* ENABLE_USB_AUDIO_DUMP */
