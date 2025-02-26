/*
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright 2024 NXP.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file ota_over_mqtt_demo.c
 * @brief Over The Air Update demo using coreMQTT Agent.
 *
 * The file demonstrates how to perform Over The Air update using OTA agent and coreMQTT
 * library. It creates an OTA agent task which manages the OTA firmware update
 * for the device. The example also provides implementations to subscribe, publish,
 * and receive data from an MQTT broker. The implementation uses coreMQTT agent which manages
 * thread safety of the MQTT operations and allows OTA agent to share the same MQTT
 * broker connection with other tasks. OTA agent invokes the callback implementations to
 * publish job related control information, as well as receive chunks
 * of presigned firmware image from the MQTT broker.
 *
 * See https://freertos.org/mqtt/mqtt-agent-demo.html
 * See https://freertos.org/ota/ota-mqtt-agent-demo.html
 */

/* Standard includes. */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "ota_config.h"

/* MQTT library includes. */
#include "core_mqtt_agent.h"

/* OTA Library include. */
#include "ota.h"

/* OTA Library Interface include. */
#include "ota_os_freertos.h"
#include "ota_mqtt_interface.h"

/* Include firmware version struct definition. */
#include "ota_appversion32.h"

/* Include platform abstraction header. */
#include "ota_pal.h"

/* Key Value store for fetching configuration info. */
//#include "kvstore.h"

/* Device utils to get device name */
#include "device_utils.h"

/* Include file system */
#include "sln_flash_files.h"
#include "sln_flash_fs_ops.h"

#include "mqtt_agent_task.h"

/*------------- Demo configurations -------------------------*/

/**
 * @brief The maximum size of the file paths used in the demo.
 */
#define otaexampleMAX_FILE_PATH_SIZE (260)

/**
 * @brief The maximum size of the stream name required for downloading update file
 * from streaming service.
 */
#define otaexampleMAX_STREAM_NAME_SIZE (128)

/**
 * @brief The delay used in the OTA demo task to periodically output the OTA
 * statistics like number of packets received, dropped, processed and queued per connection.
 */
#define otaexampleTASK_DELAY_MS (5 * 1000U)

/**
 * @brief The maximum time for which OTA demo waits for an MQTT operation to be complete.
 * This involves receiving an acknowledgment for broker for SUBSCRIBE, UNSUBSCRIBE and non
 * QOS0 publishes.
 */
#define otaexampleMQTT_TIMEOUT_MS (10 * 1000U)

/**
 * @brief The common prefix for all OTA topics.
 *
 * Thing name is substituted with a wildcard symbol `+`. OTA agent
 * registers with MQTT broker with the thing name in the topic. This topic
 * filter is used to match incoming packet received and route them to OTA.
 * Thing name is not needed for this matching.
 */
#define OTA_TOPIC_PREFIX "$aws/things"

/**
 * @brief Length of OTA topics prefix.
 */
#define OTA_PREFIX_LENGTH (sizeof(OTA_TOPIC_PREFIX) - 1UL)

/**
 * @brief Wildcard topic filter for job notification.
 * The filter is used to match the constructed job notify topic filter from OTA agent and register
 * appropriate callback for it.
 */
#define OTA_JOB_NOTIFY_TOPIC_FILTER OTA_TOPIC_PREFIX "/+/jobs/notify-next"

/**
 * @brief Length of job notification topic filter.
 */
#define OTA_JOB_NOTIFY_TOPIC_FILTER_LENGTH ((uint16_t)(sizeof(OTA_JOB_NOTIFY_TOPIC_FILTER) - 1UL))

/**
 * @brief Wildcard topic filter for matching job response messages.
 * This topic filter is used to match the responses from OTA service for OTA agent job requests. THe
 * topic filter is a reserved topic which is not subscribed with MQTT broker.
 *
 */
#define OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER OTA_TOPIC_PREFIX "/+/jobs/$next/get/accepted"

/**
 * @brief Length of job accepted response topic filter.
 */
#define OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH ((uint16_t)(sizeof(OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER) - 1))

/**
 * @brief Wildcard topic filter for matching OTA data packets.
 *  The filter is used to match the constructed data stream topic filter from OTA agent and register
 * appropriate callback for it.
 */
#define OTA_DATA_STREAM_TOPIC_FILTER OTA_TOPIC_PREFIX "/+/streams/#"

/**
 * @brief Length of data stream topic filter.
 */
#define OTA_DATA_STREAM_TOPIC_FILTER_LENGTH ((uint16_t)(sizeof(OTA_DATA_STREAM_TOPIC_FILTER) - 1))

/**
 * @brief Starting index of client identifier within OTA topic.
 */
#define OTA_TOPIC_CLIENT_IDENTIFIER_START_IDX (OTA_PREFIX_LENGTH + 1)

/**
 * @brief Used to clear bits in a task's notification value.
 */
#define otaexampleMAX_UINT32 (0xffffffff)

/**
 * @brief Task priority of OTA agent.
 */
#define otaexampleAGENT_TASK_PRIORITY (tskIDLE_PRIORITY + 2)

/**
 * @brief Maximum stack size of OTA agent task.
 */
#define otaexampleAGENT_TASK_STACK_SIZE (1024)

/**
 * @brief Defines the structure to use as the command callback context in this
 * demo.
 */
struct MQTTAgentCommandContext
{
    TaskHandle_t xTaskToNotify;
    void *pArgs;
};

/**
 * @brief Function used by OTA agent to publish control messages to the MQTT broker.
 *
 * The implementation uses MQTT agent to queue a publish request. It then waits
 * for the request complete notification from the agent. The notification along with result of the
 * operation is sent back to the caller task using xTaskNotify API. For publishes involving QOS 1 and
 * QOS2 the operation is complete once an acknowledgment (PUBACK) is received. OTA agent uses this function
 * to fetch new job, provide status update and send other control related messages to the MQTT broker.
 *
 * @param[in] pacTopic Topic to publish the control packet to.
 * @param[in] topicLen Length of the topic string.
 * @param[in] pMsg Message to publish.
 * @param[in] msgSize Size of the message to publish.
 * @param[in] qos Qos for the publish.
 * @return OtaMqttSuccess if successful. Appropriate error code otherwise.
 */
static OtaMqttStatus_t prvMQTTPublish(
    const char *const pacTopic, uint16_t topicLen, const char *pMsg, uint32_t msgSize, uint8_t qos);

/**
 * @brief The callback invoked by the MQTT agent on a successful subscription of a topic filter
 * with broker.
 * The implementation adds a local subscription for the topic filter with the MQTT agent.
 *
 * @param pCommandContext Pointer to the command context passed from the caller.
 * @param pxReturnInfo Pointer to the return status of the subscribe command.
 */
static void prvSubscribeCommandCallback(MQTTAgentCommandContext_t *pxCommandContext,
                                        MQTTAgentReturnInfo_t *pxReturnInfo);

/**
 * @brief The callback invoked by the MQTT agent on unsubscribing a topic filter
 * with broker.
 * The implementation removes the local subscription for the topic filter with the MQTT agent.
 *
 * @param pCommandContext Pointer to the command context passed from the caller.
 * @param pxReturnInfo Pointer to the return status of the unsubscribe command.
 */
static void prvUnSubscribeCommandCallback(MQTTAgentCommandContext_t *pxCommandContext,
                                          MQTTAgentReturnInfo_t *pxReturnInfo);

/**
 * @brief Function used by OTA agent to subscribe for a control or data packet from the MQTT broker.
 *
 * The implementation queues a SUBSCRIBE request for the topic filter with the MQTT agent. It then waits for
 * a notification of the request completion. Notification will be sent back to caller task,
 * using xTaskNotify APIs. MQTT agent also stores a callback provided by this function with
 * the associated topic filter. The callback will be used to
 * route any data received on the matching topic to the OTA agent. OTA agent uses this function
 * to subscribe to all topic filters necessary for receiving job related control messages as
 * well as firmware image chunks from MQTT broker.
 *
 * @param[in] pTopicFilter The topic filter used to subscribe for packets.
 * @param[in] topicFilterLength Length of the topic filter string.
 * @param[in] ucQoS Intended qos value for the messages received on this topic.
 * @return OtaMqttSuccess if successful. Appropriate error code otherwise.
 */
static OtaMqttStatus_t prvMQTTSubscribe(const char *pTopicFilter, uint16_t topicFilterLength, uint8_t ucQoS);

/**
 * @brief Function is used by OTA agent to unsubscribe a topicfilter from MQTT broker.
 *
 * The implementation queues an UNSUBSCRIBE request for the topic filter with the MQTT agent. It then waits
 * for a successful completion of the request from the agent. Notification along with results of
 * operation is sent using xTaskNotify API to the caller task. MQTT agent also removes the topic filter
 * subscription from its memory so any future
 * packets on this topic will not be routed to the OTA agent.
 *
 * @param[in] pTopicFilter Topic filter to be unsubscribed.
 * @param[in] topicFilterLength Length of the topic filter.
 * @param[in] ucQos Qos value for the topic.
 * @return OtaMqttSuccess if successful. Appropriate error code otherwise.
 *
 */
static OtaMqttStatus_t prvMQTTUnsubscribe(const char *pTopicFilter, uint16_t topicFilterLength, uint8_t ucQoS);

/**
 * @brief Fetch an unused OTA event buffer from the pool.
 *
 * Demo uses a simple statically allocated array of fixed size event buffers. The
 * number of event buffers is configured by the param otaconfigMAX_NUM_OTA_DATA_BUFFERS
 * within ota_config.h. This function is used to fetch a free buffer from the pool for processing
 * by the OTA agent task. It uses a mutex for thread safe access to the pool.
 *
 * @return A pointer to an unused buffer. NULL if there are no buffers available.
 */
static OtaEventData_t *prvOTAEventBufferGet(void);

/**
 * @brief Free an event buffer back to pool
 *
 * OTA demo uses a statically allocated array of fixed size event buffers . The
 * number of event buffers is configured by the param otaconfigMAX_NUM_OTA_DATA_BUFFERS
 * within ota_config.h. The function is used by the OTA application callback to free a buffer,
 * after OTA agent has completed processing with the event. The access to the pool is made thread safe
 * using a mutex.
 *
 * @param[in] pxBuffer Pointer to the buffer to be freed.
 */
static void prvOTAEventBufferFree(OtaEventData_t *const pxBuffer);

/**
 * @brief The function which runs the OTA agent task.
 *
 * The function runs the OTA Agent Event processing loop, which waits for
 * any events for OTA agent and process them. The loop never returns until the OTA agent
 * is shutdown. The tasks exits gracefully by freeing up all resources in the event of an
 *  OTA agent shutdown.
 *
 * @param[in] pvParam Any parameters to be passed to OTA agent task.
 */
static void prvOTAAgentTask(void *pvParam);

/**
 * @brief The function which runs the OTA update task.
 *
 * The demo task initializes the OTA agent an loops until OTA agent is shutdown.
 * It reports OTA update statistics (which includes number of blocks received, processed and dropped),
 * at regular intervals.
 *
 * @param[in] pvParam Any parameters to be passed to OTA demo task.
 */
static void vOTAUpdateTask(void);

/**
 * @brief Callback to receive either data or control messages on OTA topics.
 *
 * Function can get invoked for a job notification message or a data message from MQTT broker.
 * Function matches the topic of the message with list of topic filters and routes the message to
 * either control message or data message processing functions.
 *
 * @param[in] pxSubscriptionContext Context which is passed unmodified from the MQTT agent.
 * @param[in] pPublishInfo Pointer to the structure containing the details of the MQTT packet.
 */
static void prvProcessIncomingMessage(void *pxSubscriptionContext, MQTTPublishInfo_t *pxPublishInfo);

/**
 * @brief Callback invoked for data messages received from MQTT broker.
 *
 * Function gets invoked for the firmware image blocks received on OTA data stream topic.
 * The function is registered with MQTT agent's subscription manger along with the
 * topic filter for data stream. For each packet received, the
 * function fetches a free event buffer from the pool and queues the firmware image chunk for
 * OTA agent task processing.
 *
 * @param[in] pxSubscriptionContext Context which is passed unmodified from the MQTT agent.
 * @param[in] pPublishInfo Pointer to the structure containing the details of the MQTT packet.
 */
static void prvProcessIncomingData(void *pxSubscriptionContext, MQTTPublishInfo_t *pPublishInfo);

/**
 * @brief Callback invoked for job control messages from MQTT broker.
 *
 * Callback gets invoked for any OTA job related control messages from the MQTT broker.
 * The function is registered with MQTT agent's subscription manger along with the topic filter for
 * job stream. The function fetches a free event buffer from the pool and queues the appropriate event type
 * based on the control message received.
 *
 * @param[in] pxSubscriptionContext Context which is passed unmodified from the MQTT agent.
 * @param[in] pPublishInfo Pointer to the structure containing the details of MQTT packet.
 */
static void prvProcessIncomingJobMessage(void *pxSubscriptionContext, MQTTPublishInfo_t *pPublishInfo);

/**
 * @brief Matches a client identifier within an OTA topic.
 * This function is used to validate that topic is valid and intended for this device thing name.
 *
 * @param[in] pTopic Pointer to the topic
 * @param[in] topicNameLength length of the topic
 * @param[in] pClientIdentifier Client identifier, should be null terminated.
 * @param[in] clientIdentifierLength Length of the client identifier.
 * @return pdTRUE if client identifier is found within the topic at the right index.
 */
static BaseType_t prvMatchClientIdentifierInTopic(const char *pTopic,
                                                  size_t topicNameLength,
                                                  const char *pClientIdentifier,
                                                  size_t clientIdentifierLength);

/**
 * @brief Buffer used to store the firmware image file path.
 * Buffer is passed to the OTA agent during initialization.
 */
static uint8_t updateFilePath[otaexampleMAX_FILE_PATH_SIZE];

/**
 * @brief Buffer used to store the code signing certificate file path.
 * Buffer is passed to the OTA agent during initialization.
 */
static uint8_t certFilePath[otaexampleMAX_FILE_PATH_SIZE];

/**
 * @brief Buffer used to store the name of the data stream.
 * Buffer is passed to the OTA agent during initialization.
 */
static uint8_t streamName[otaexampleMAX_STREAM_NAME_SIZE];

/**
 * @brief Buffer used decode the CBOR message from the MQTT payload.
 * Buffer is passed to the OTA agent during initialization.
 */
static uint8_t __attribute__((section(".bss.$SRAM_OC_CACHEABLE")))decodeMem[(1U << otaconfigLOG2_FILE_BLOCK_SIZE)];

/**
 * @brief Application buffer used to store the bitmap for requesting firmware image
 * chunks from MQTT broker. Buffer is passed to the OTA agent during initialization.
 */
static uint8_t bitmap[OTA_MAX_BLOCK_BITMAP_SIZE];

/**
 * @brief A statically allocated array of event buffers used by the OTA agent.
 * Maximum number of buffers are determined by how many chunks are requested
 * by OTA agent at a time along with an extra buffer to handle control message.
 * The size of each buffer is determined by the maximum size of firmware image
 * chunk, and other metadata send along with the chunk.
 */
static OtaEventData_t __attribute__((section(".bss.$SRAM_OC_NON_CACHEABLE")))eventBuffer[otaconfigMAX_NUM_OTA_DATA_BUFFERS] = {0};

/*
 * @brief Mutex used to manage thread safe access of OTA event buffers.
 */
static SemaphoreHandle_t xBufferSemaphore;

/**
 * @brief Static handle used for MQTT agent context.
 */
extern MQTTAgentContext_t xGlobalMqttAgentContext;

/**
 * @brief Structure containing all application allocated buffers used by the OTA agent.
 * Structure is passed to the OTA agent during initialization.
 */
static const OtaAppBuffer_t otaBuffer = {.pUpdateFilePath    = updateFilePath,
                                   .updateFilePathsize = otaexampleMAX_FILE_PATH_SIZE,
                                   .pCertFilePath      = certFilePath,
                                   .certFilePathSize   = otaexampleMAX_FILE_PATH_SIZE,
                                   .pStreamName        = streamName,
                                   .streamNameSize     = otaexampleMAX_STREAM_NAME_SIZE,
                                   .pDecodeMemory      = decodeMem,
                                   .decodeMemorySize   = (1U << otaconfigLOG2_FILE_BLOCK_SIZE),
                                   .pFileBitmap        = bitmap,
                                   .fileBitmapSize     = OTA_MAX_BLOCK_BITMAP_SIZE};

/**
 * @brief Structure used for encoding firmware version.
 */
const AppVersion32_t appFirmwareVersion = {
    .u.x.major = APP_VERSION_MAJOR,
    .u.x.minor = APP_VERSION_MINOR,
    .u.x.build = APP_VERSION_BUILD,
};


/**
 * @brief Holds the unique name of this device
 */
static char *s_thingName;
TaskHandle_t otaDoneTaskHandle = NULL;

/*---------------------------------------------------------*/

static void prvOTAEventBufferFree(OtaEventData_t *const pxBuffer)
{
    if (xSemaphoreTake(xBufferSemaphore, portMAX_DELAY) == pdTRUE)
    {
        pxBuffer->bufferUsed = false;
        (void)xSemaphoreGive(xBufferSemaphore);
    }
    else
    {
        LogError(("Failed to get buffer semaphore."));
    }
}

/*-----------------------------------------------------------*/

static OtaEventData_t *prvOTAEventBufferGet(void)
{
    uint32_t ulIndex            = 0;
    OtaEventData_t *pFreeBuffer = NULL;

    if (xSemaphoreTake(xBufferSemaphore, portMAX_DELAY) == pdTRUE)
    {
        for (ulIndex = 0; ulIndex < otaconfigMAX_NUM_OTA_DATA_BUFFERS; ulIndex++)
        {
            if (eventBuffer[ulIndex].bufferUsed == false)
            {
                eventBuffer[ulIndex].bufferUsed = true;
                pFreeBuffer                     = &eventBuffer[ulIndex];
                break;
            }
        }

        (void)xSemaphoreGive(xBufferSemaphore);
    }
    else
    {
        LogError(("Failed to get buffer semaphore."));
    }

    return pFreeBuffer;
}

/*-----------------------------------------------------------*/
static void prvOTAAgentTask(void *pvParam)
{
    OTA_EventProcessingTask(pvParam);
    vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

/**
 * @brief The OTA agent has completed the update job or it is in
 * self test mode. If it was accepted, we want to activate the new image.
 * This typically means we should reset the device to run the new firmware.
 * If now is not a good time to reset the device, it may be activated later
 * by your user code. If the update was rejected, just return without doing
 * anything and we will wait for another job. If it reported that we should
 * start test mode, normally we would perform some kind of system checks to
 * make sure our new firmware does the basic things we think it should do
 * but we will just go ahead and set the image as accepted for demo purposes.
 * The accept function varies depending on your platform. Refer to the OTA
 * PAL implementation for your platform in aws_ota_pal.c to see what it
 * does for you.
 *
 * @param[in] event Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pData Data associated with the event.
 * @return None.
 */
static void otaAppCallback(OtaJobEvent_t event, void *pData)
{
    OtaErr_t err = OtaErrUninitialized;

    switch (event)
    {
        case OtaJobEventActivate:
            LogInfo(("Received OtaJobEventActivate callback from OTA Agent."));

            /**
             * Activate the new firmware image immediately. Applications can choose to postpone
             * the activation to a later stage if needed.
             */
            err = OTA_ActivateNewImage();

            /**
             * Activation of the new image failed. This indicates an error that requires a follow
             * up through manual activation by resetting the device. The demo reports the error
             * and shuts down the OTA agent.
             */
            LogError(("New image activation failed."));

            /* Shutdown OTA Agent, if it is required that the unsubscribe operations are not
             * performed while shutting down please set the second parameter to 0 instead of 1. */
            OTA_Shutdown(0, 1);

            break;

        case OtaJobEventFail:

            /**
             * No user action is needed here. OTA agent handles the job failure event.
             */
            LogInfo(("Received an OtaJobEventFail notification from OTA Agent."));

            break;

        case OtaJobEventStartTest:

            /* This demo just accepts the image since it was a good OTA update and networking
             * and services are all working (or we would not have made it this far). If this
             * were some custom device that wants to test other things before validating new
             * image, this would be the place to kick off those tests before calling
             * OTA_SetImageState() with the final result of either accepted or rejected. */

            LogInfo(("Received OtaJobEventStartTest callback from OTA Agent."));

            err = OTA_SetImageState(OtaImageStateAccepted);

            if (err == OtaErrNone)
            {
                LogInfo(("New image validation succeeded in self test mode."));
            }
            else
            {
                LogError(("Failed to set image state as accepted with error %d.", err));
            }

            break;

        case OtaJobEventProcessed:

            LogDebug(("OTA Event processing completed. Freeing the event buffer to pool."));
            configASSERT(pData != NULL);
            prvOTAEventBufferFree((OtaEventData_t *)pData);

            break;

        case OtaJobEventSelfTestFailed:
            LogDebug(("Received OtaJobEventSelfTestFailed callback from OTA Agent."));

            /* Requires manual activation of previous image as self-test for
             * new image downloaded failed.*/
            LogError(("OTA Self-test failed for new image. shutting down OTA Agent."));

            /* Shutdown OTA Agent, if it is required that the unsubscribe operations are not
             * performed while shutting down please set the second parameter to 0 instead of 1. */
            OTA_Shutdown(0, 1);

            break;

        default:
            LogWarn(("Received an unhandled callback event from OTA Agent, event = %d", event));

            break;
    }
}

/*-----------------------------------------------------------*/

static void prvProcessIncomingMessage(void *pxSubscriptionContext, MQTTPublishInfo_t *pxPublishInfo)
{
    bool isMatched = false;

    {
        (void)MQTT_MatchTopic(pxPublishInfo->pTopicName, pxPublishInfo->topicNameLength, OTA_JOB_NOTIFY_TOPIC_FILTER,
                              OTA_JOB_NOTIFY_TOPIC_FILTER_LENGTH, &isMatched);

        if (isMatched == true)
        {
            prvProcessIncomingJobMessage(pxSubscriptionContext, pxPublishInfo);
        }
    }

    if (isMatched == false)
    {
        (void)MQTT_MatchTopic(pxPublishInfo->pTopicName, pxPublishInfo->topicNameLength, OTA_DATA_STREAM_TOPIC_FILTER,
                              OTA_DATA_STREAM_TOPIC_FILTER_LENGTH, &isMatched);

        if (isMatched == true)
        {
            prvProcessIncomingData(pxSubscriptionContext, pxPublishInfo);
        }
    }
}

/*-----------------------------------------------------------*/

static void prvProcessIncomingData(void *pxSubscriptionContext, MQTTPublishInfo_t *pPublishInfo)
{
    BaseType_t isMatch = pdFALSE;
    OtaEventData_t *pData;
    OtaEventMsg_t eventMsg = {0};

    configASSERT(pPublishInfo != NULL);
    (void)pxSubscriptionContext;

    isMatch = prvMatchClientIdentifierInTopic(pPublishInfo->pTopicName, pPublishInfo->topicNameLength, s_thingName,
                                              strlen(s_thingName));

    if (isMatch == pdTRUE)
    {
        if (pPublishInfo->payloadLength <= OTA_DATA_BLOCK_SIZE)
        {
            pData = prvOTAEventBufferGet();

            if (pData != NULL)
            {
                memcpy(pData->data, pPublishInfo->pPayload, pPublishInfo->payloadLength);
                pData->dataLength   = pPublishInfo->payloadLength;
                eventMsg.eventId    = OtaAgentEventReceivedFileBlock;
                eventMsg.pEventData = pData;

                /* Send job document received event. */
                OTA_SignalEvent(&eventMsg);
            }
            else
            {
                LogError(("Error: No OTA data buffers available.\r\n"));
            }
        }
        else
        {
            LogError(("Received OTA data block of size (%d) larger than maximum size(%d) defined. ",
                      pPublishInfo->payloadLength, OTA_DATA_BLOCK_SIZE));
        }
    }
    else
    {
        LogWarn(("Received data block on an unsolicited topic, thing name does not match. topic: %.*s ",
                 pPublishInfo->topicNameLength, pPublishInfo->pTopicName));
    }
}

/*-----------------------------------------------------------*/

static void prvProcessIncomingJobMessage(void *pxSubscriptionContext, MQTTPublishInfo_t *pPublishInfo)
{
    OtaEventData_t *pData;
    OtaEventMsg_t eventMsg = {0};
    BaseType_t isMatch     = pdFALSE;

    (void)pxSubscriptionContext;
    configASSERT(pPublishInfo != NULL);

    isMatch = prvMatchClientIdentifierInTopic(pPublishInfo->pTopicName, pPublishInfo->topicNameLength, s_thingName,
                                              strlen(s_thingName));

    if (isMatch == pdTRUE)
    {
        if (pPublishInfo->payloadLength <= OTA_DATA_BLOCK_SIZE)
        {
            LogInfo(("Received OTA job message, size: %d.\n\n", pPublishInfo->payloadLength));
            pData = prvOTAEventBufferGet();

            if (pData != NULL)
            {
                memcpy(pData->data, pPublishInfo->pPayload, pPublishInfo->payloadLength);
                pData->dataLength   = pPublishInfo->payloadLength;
                eventMsg.eventId    = OtaAgentEventReceivedJobDocument;
                eventMsg.pEventData = pData;

                /* Send job document received event. */
                OTA_SignalEvent(&eventMsg);
            }
            else
            {
                LogError(("Error: No OTA data buffers available.\r\n"));
            }
        }
        else
        {
            LogError(("Received OTA job message size (%d) is larger than the OTA maximum size (%d) defined.\n\n",
                      pPublishInfo->payloadLength, OTA_DATA_BLOCK_SIZE));
        }
    }
    else
    {
        LogWarn(("Received a job message on an unsolicited topic, thing name does not match. topic: %.*s ",
                 pPublishInfo->topicNameLength, pPublishInfo->pTopicName));
    }
}

/*-----------------------------------------------------------*/

static BaseType_t prvMatchClientIdentifierInTopic(const char *pTopic,
                                                  size_t topicNameLength,
                                                  const char *pClientIdentifier,
                                                  size_t clientIdentifierLength)
{
    BaseType_t isMatch = pdFALSE;
    size_t idx, matchIdx = 0;

    for (idx = OTA_TOPIC_CLIENT_IDENTIFIER_START_IDX; idx < topicNameLength; idx++)
    {
        if (matchIdx == clientIdentifierLength)
        {
            if (pTopic[idx] == '/')
            {
                isMatch = pdTRUE;
            }

            break;
        }
        else
        {
            if (pClientIdentifier[matchIdx] != pTopic[idx])
            {
                break;
            }
        }

        matchIdx++;
    }

    return isMatch;
}

/*-----------------------------------------------------------*/

static void prvCommandCallback(MQTTAgentCommandContext_t *pCommandContext, MQTTAgentReturnInfo_t *pxReturnInfo)
{
    if (pCommandContext->xTaskToNotify != NULL)
    {
        xTaskNotify(pCommandContext->xTaskToNotify, (uint32_t)(pxReturnInfo->returnCode), eSetValueWithOverwrite);
    }
}

/*-----------------------------------------------------------*/

static void prvSubscribeCommandCallback(MQTTAgentCommandContext_t *pxCommandContext,
                                        MQTTAgentReturnInfo_t *pxReturnInfo)
{
    MQTTAgentSubscribeArgs_t *pxSubscribeArgs = (MQTTAgentSubscribeArgs_t *)pxCommandContext->pArgs;
    BaseType_t xResult;

    if (pxReturnInfo->returnCode == MQTTSuccess)
    {
        xResult = xAddMQTTTopicFilterCallback(pxSubscribeArgs->pSubscribeInfo[0].pTopicFilter,
                                              pxSubscribeArgs->pSubscribeInfo[0].topicFilterLength,
                                              prvProcessIncomingMessage, NULL, pdFALSE);
        configASSERT(xResult == pdTRUE);
    }

    if (pxCommandContext->xTaskToNotify != NULL)
    {
        xTaskNotify(pxCommandContext->xTaskToNotify, (uint32_t)(pxReturnInfo->returnCode), eSetValueWithOverwrite);
    }
}

/*-----------------------------------------------------------*/

static void prvUnSubscribeCommandCallback(MQTTAgentCommandContext_t *pxCommandContext,
                                          MQTTAgentReturnInfo_t *pxReturnInfo)
{
    MQTTAgentSubscribeArgs_t *pxSubscribeArgs = (MQTTAgentSubscribeArgs_t *)pxCommandContext->pArgs;

    if (pxReturnInfo->returnCode == MQTTSuccess)
    {
        vRemoveMQTTTopicFilterCallback(pxSubscribeArgs->pSubscribeInfo[0].pTopicFilter,
                                       pxSubscribeArgs->pSubscribeInfo[0].topicFilterLength);
    }

    if (pxCommandContext->xTaskToNotify != NULL)
    {
        xTaskNotify(pxCommandContext->xTaskToNotify, (uint32_t)(pxReturnInfo->returnCode), eSetValueWithOverwrite);
    }
}

/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMQTTSubscribe(const char *pTopicFilter, uint16_t topicFilterLength, uint8_t ucQoS)
{
    MQTTStatus_t mqttStatus;
    uint32_t ulNotifiedValue;
    MQTTAgentSubscribeArgs_t xSubscribeArgs = {0};
    MQTTSubscribeInfo_t xSubscribeInfo      = {MQTTQoS0};
    BaseType_t result;
    MQTTAgentCommandInfo_t xCommandParams     = {0};
    MQTTAgentCommandContext_t xCommandContext = {0};
    OtaMqttStatus_t otaRet                    = OtaMqttSuccess;

    configASSERT(pTopicFilter != NULL);
    configASSERT(topicFilterLength > 0);

    xSubscribeInfo.pTopicFilter      = pTopicFilter;
    xSubscribeInfo.topicFilterLength = topicFilterLength;
    xSubscribeInfo.qos               = (MQTTQoS_t)ucQoS;
    xSubscribeArgs.pSubscribeInfo    = &xSubscribeInfo;
    xSubscribeArgs.numSubscriptions  = 1;

    xCommandContext.pArgs         = (void *)(&xSubscribeArgs);
    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();

    xCommandParams.blockTimeMs                 = otaexampleMQTT_TIMEOUT_MS;
    xCommandParams.cmdCompleteCallback         = prvSubscribeCommandCallback;
    xCommandParams.pCmdCompleteCallbackContext = (void *)&xCommandContext;

    /*
     * Wait for Agent to be connected before sending a subscribe message.
     */
    if (xGetMQTTAgentState() != MQTT_AGENT_STATE_CONNECTED)
    {
        (void)xWaitForMQTTAgentState(MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY);
    }

    xTaskNotifyStateClear(NULL);

    mqttStatus = MQTTAgent_Subscribe(&xGlobalMqttAgentContext, &xSubscribeArgs, &xCommandParams);

    /* Block for command to complete so MQTTSubscribeInfo_t remains in scope for the
     * duration of the command. */
    if (mqttStatus == MQTTSuccess)
    {
        result = xTaskNotifyWait(0, otaexampleMAX_UINT32, &ulNotifiedValue, portMAX_DELAY);

        if (result == pdTRUE)
        {
            mqttStatus = (MQTTStatus_t)(ulNotifiedValue);
        }
        else
        {
            mqttStatus = MQTTRecvFailed;
        }
    }

    if (mqttStatus != MQTTSuccess)
    {
        LogError(("Failed to SUBSCRIBE to topic with error = %u.", mqttStatus));

        otaRet = OtaMqttSubscribeFailed;
    }
    else
    {
        LogInfo(("Subscribed to topic %.*s.\n\n", topicFilterLength, pTopicFilter));

        otaRet = OtaMqttSuccess;
    }

    return otaRet;
}

static OtaMqttStatus_t prvMQTTPublish(
    const char *const pacTopic, uint16_t topicLen, const char *pMsg, uint32_t msgSize, uint8_t qos)
{
    OtaMqttStatus_t otaRet = OtaMqttSuccess;
    BaseType_t result;
    MQTTStatus_t mqttStatus                   = MQTTBadParameter;
    MQTTPublishInfo_t publishInfo             = {MQTTQoS0};
    MQTTAgentCommandInfo_t xCommandParams     = {0};
    MQTTAgentCommandContext_t xCommandContext = {0};
    uint32_t ulNotifiedValue;

    configASSERT(qos <= MQTTQoS2);

    publishInfo.pTopicName      = pacTopic;
    publishInfo.topicNameLength = topicLen;
    publishInfo.qos             = (MQTTQoS_t)qos;
    publishInfo.pPayload        = pMsg;
    publishInfo.payloadLength   = msgSize;

    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();

    xCommandParams.blockTimeMs                 = otaexampleMQTT_TIMEOUT_MS;
    xCommandParams.cmdCompleteCallback         = prvCommandCallback;
    xCommandParams.pCmdCompleteCallbackContext = (void *)&xCommandContext;

    /*
     * Wait for Agent to be connected before sending a publish message.
     */
    if (xGetMQTTAgentState() != MQTT_AGENT_STATE_CONNECTED)
    {
        (void)xWaitForMQTTAgentState(MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY);
    }

    xTaskNotifyStateClear(NULL);

    mqttStatus = MQTTAgent_Publish(&xGlobalMqttAgentContext, &publishInfo, &xCommandParams);

    /* Block for command to complete so MQTTPublishInfo_t remains in scope for the
     * duration of the command. */
    if (mqttStatus == MQTTSuccess)
    {
        result = xTaskNotifyWait(0, otaexampleMAX_UINT32, &ulNotifiedValue, portMAX_DELAY);

        if (result != pdTRUE)
        {
            mqttStatus = MQTTSendFailed;
        }
        else
        {
            mqttStatus = (MQTTStatus_t)(ulNotifiedValue);
        }
    }

    if (mqttStatus != MQTTSuccess)
    {
        LogError(("Failed to send PUBLISH packet to broker with error = %u.", mqttStatus));
        otaRet = OtaMqttPublishFailed;
    }
    else
    {
        LogInfo(("Sent PUBLISH packet to broker %.*s to broker.\n\n", topicLen, pacTopic));

        otaRet = OtaMqttSuccess;
    }

    return otaRet;
}

static OtaMqttStatus_t prvMQTTUnsubscribe(const char *pTopicFilter, uint16_t topicFilterLength, uint8_t ucQoS)
{
    MQTTStatus_t mqttStatus;
    uint32_t ulNotifiedValue;
    MQTTAgentSubscribeArgs_t xSubscribeArgs = {0};
    MQTTSubscribeInfo_t xSubscribeInfo      = {MQTTQoS0};
    BaseType_t result;
    MQTTAgentCommandInfo_t xCommandParams     = {0};
    MQTTAgentCommandContext_t xCommandContext = {0};
    OtaMqttStatus_t otaRet                    = OtaMqttSuccess;

    configASSERT(pTopicFilter != NULL);
    configASSERT(topicFilterLength > 0);
    configASSERT(ucQoS <= MQTTQoS2);

    xSubscribeInfo.pTopicFilter      = pTopicFilter;
    xSubscribeInfo.topicFilterLength = topicFilterLength;
    xSubscribeInfo.qos               = (MQTTQoS_t)ucQoS;
    xSubscribeArgs.pSubscribeInfo    = &xSubscribeInfo;
    xSubscribeArgs.numSubscriptions  = 1;

    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xCommandContext.pArgs         = (void *)(&xSubscribeArgs);

    xCommandParams.blockTimeMs                 = otaexampleMQTT_TIMEOUT_MS;
    xCommandParams.cmdCompleteCallback         = prvUnSubscribeCommandCallback;
    xCommandParams.pCmdCompleteCallbackContext = (void *)&xCommandContext;

    LogInfo((" Unsubscribing to topic filter: %s", pTopicFilter));
    xTaskNotifyStateClear(NULL);

    mqttStatus = MQTTAgent_Unsubscribe(&xGlobalMqttAgentContext, &xSubscribeArgs, &xCommandParams);

    /* Block for command to complete so MQTTSubscribeInfo_t remains in scope for the
     * duration of the command. */
    if (mqttStatus == MQTTSuccess)
    {
        result = xTaskNotifyWait(0, otaexampleMAX_UINT32, &ulNotifiedValue, portMAX_DELAY);

        if (result == pdTRUE)
        {
            mqttStatus = (MQTTStatus_t)(ulNotifiedValue);
        }
        else
        {
            mqttStatus = MQTTRecvFailed;
        }
    }

    if (mqttStatus != MQTTSuccess)
    {
        LogError(
            ("Failed to UNSUBSCRIBE from topic %.*s with error = %u.", topicFilterLength, pTopicFilter, mqttStatus));

        otaRet = OtaMqttUnsubscribeFailed;
    }
    else
    {
        LogInfo(("UNSUBSCRIBED from topic %.*s.\n\n", topicFilterLength, pTopicFilter));

        otaRet = OtaMqttSuccess;
    }

    return otaRet;
}

/*-----------------------------------------------------------*/

static void setOtaInterfaces(OtaInterfaces_t *pOtaInterfaces)
{
    configASSERT(pOtaInterfaces != NULL);

    /* Initialize OTA library OS Interface. */
    pOtaInterfaces->os.event.init   = OtaInitEvent_FreeRTOS;
    pOtaInterfaces->os.event.send   = OtaSendEvent_FreeRTOS;
    pOtaInterfaces->os.event.recv   = OtaReceiveEvent_FreeRTOS;
    pOtaInterfaces->os.event.deinit = OtaDeinitEvent_FreeRTOS;
    pOtaInterfaces->os.timer.start  = OtaStartTimer_FreeRTOS;
    pOtaInterfaces->os.timer.stop   = OtaStopTimer_FreeRTOS;
    pOtaInterfaces->os.timer.delete = OtaDeleteTimer_FreeRTOS;
    pOtaInterfaces->os.mem.malloc   = Malloc_FreeRTOS;
    pOtaInterfaces->os.mem.free     = Free_FreeRTOS;

    /* Initialize the OTA library MQTT Interface.*/
    pOtaInterfaces->mqtt.subscribe   = prvMQTTSubscribe;
    pOtaInterfaces->mqtt.publish     = prvMQTTPublish;
    pOtaInterfaces->mqtt.unsubscribe = prvMQTTUnsubscribe;

    /* Initialize the OTA library PAL Interface.*/
    pOtaInterfaces->pal.getPlatformImageState = xOtaPalGetPlatformImageState;
    pOtaInterfaces->pal.setPlatformImageState = xOtaPalSetPlatformImageState;
    pOtaInterfaces->pal.writeBlock            = xOtaPalWriteBlock;
    pOtaInterfaces->pal.activate              = xOtaPalActivateNewImage;
    pOtaInterfaces->pal.closeFile             = xOtaPalCloseFile;
    pOtaInterfaces->pal.reset                 = xOtaPalResetDevice;
    pOtaInterfaces->pal.abort                 = xOtaPalAbort;
    pOtaInterfaces->pal.createFile            = xOtaPalCreateFileForRx;
}

static void prvSuspendOTAUpdate(void)
{
    if ((OTA_GetState() != OtaAgentStateSuspended) && (OTA_GetState() != OtaAgentStateStopped))
    {
        OTA_Suspend();

        while ((OTA_GetState() != OtaAgentStateSuspended) && (OTA_GetState() != OtaAgentStateStopped))
        {
            vTaskDelay(pdMS_TO_TICKS(otaexampleTASK_DELAY_MS));
        }
    }
}

static void prvResumeOTAUpdate(void)
{
    if (OTA_GetState() == OtaAgentStateSuspended)
    {
        OTA_Resume();

        while (OTA_GetState() == OtaAgentStateSuspended)
        {
            vTaskDelay(pdMS_TO_TICKS(otaexampleTASK_DELAY_MS));
        }
    }
}

void vOTAUpdateTask()
{
    /* FreeRTOS APIs return status. */
    BaseType_t xResult = pdPASS;

    /* OTA library return status. */
    OtaErr_t otaRet = OtaErrNone;

    /* OTA event message used for sending event to OTA Agent.*/
    OtaEventMsg_t eventMsg = {0};

    /* OTA interface context required for library interface functions.*/
    OtaInterfaces_t otaInterfaces;

    /* OTA library packet statistics per job.*/
    OtaAgentStatistics_t otaStatistics = {0};

    /* Set OTA Library interfaces.*/
    setOtaInterfaces(&otaInterfaces);

    LogInfo(("OTA over MQTT demo, Application version %u.%u.%u", appFirmwareVersion.u.x.major,
             appFirmwareVersion.u.x.minor, appFirmwareVersion.u.x.build));
    /****************************** Init OTA Library. ******************************/

    xBufferSemaphore = xSemaphoreCreateMutex();

    if (xBufferSemaphore == NULL)
    {
        xResult = pdFAIL;
    }

    if (xResult == pdPASS)
    {
        /*
         * Topic is an unsolicited topic, ie a subscription is not required with the MQTT broker.
         * Messages on this topic is directly sent to client. Hence this topic filter is added as
         * a static entry in the MQTT agent topic filter store. Last parameter indicates MQTT agent
         * should not subscribe to topic on a reconnection.
         */
        xResult = xAddMQTTTopicFilterCallback(OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER,
                                              OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH,
                                              prvProcessIncomingJobMessage, NULL, pdFALSE);
    }

    if (xResult == pdPASS)
    {
        memset(eventBuffer, 0x00, sizeof(eventBuffer));

        if ((otaRet = OTA_Init(&otaBuffer, &otaInterfaces, (const uint8_t *)(s_thingName), otaAppCallback)) !=
            OtaErrNone)
        {
            LogError(("Failed to initialize OTA Agent, exiting = %u.", otaRet));
            xResult = pdFAIL;
        }
    }

    if (xResult == pdPASS)
    {
        if ((xResult = xTaskCreate(prvOTAAgentTask, "OTAAgent", otaexampleAGENT_TASK_STACK_SIZE, NULL,
                                   otaexampleAGENT_TASK_PRIORITY, NULL)) != pdPASS)
        {
            LogError(
                ("Failed to start OTA Agent task: "
                 ",errno=%d",
                 xResult));
        }
    }

    /***************************Start OTA demo loop. ******************************/

    if (xResult == pdPASS)
    {
        /* Start the OTA Agent.*/
        eventMsg.eventId = OtaAgentEventStart;
        OTA_SignalEvent(&eventMsg);

        while (OTA_GetState() != OtaAgentStateStopped)
        {
            /* Get OTA statistics for currently executing job. */
            OTA_GetStatistics(&otaStatistics);
            LogDebug((" Received: %u   Queued: %u   Processed: %u   Dropped: %u", otaStatistics.otaPacketsReceived,
                     otaStatistics.otaPacketsQueued, otaStatistics.otaPacketsProcessed,
                     otaStatistics.otaPacketsDropped));

            if (xWaitForMQTTAgentState(MQTT_AGENT_STATE_DISCONNECTED, pdMS_TO_TICKS(otaexampleTASK_DELAY_MS)) == pdTRUE)
            {
                /* Suspend ongoing OTA job if any until MQTT agent is reconnected. */
                prvSuspendOTAUpdate();

                (void)xWaitForMQTTAgentState(MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY);

                /* Resume OTA Update so that agent checks for any new jobs during a lost connection. */
                prvResumeOTAUpdate();
            }
        }
    }

    LogInfo(("OTA agent task stopped. Exiting OTA demo."));

    /* Unconditionally remove the static subscription. */
    vRemoveMQTTTopicFilterCallback(OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER,
                                   OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH);

    if (s_thingName != NULL)
    {
        vPortFree(s_thingName);
        s_thingName      = NULL;
    }

    vTaskDelete(NULL);
}

void otaDoneTask(void *arg)
{
    /* suspend the task here, wait for the OTA PAL to wake it */
    ulTaskNotifyTake(true, portMAX_DELAY);

    /* extra delay, precaution measure, give time to OTA agent
     * to send status message to cloud */
    vTaskDelay(2000);

    /* wait for max 10 seconds to have the agent in ready state */
    int retry = 100;
    while (retry--)
    {
        if (OTA_GetState() != OtaAgentStateReady)
        {
            vTaskDelay(100);
        }
        else
        {
            break;
        }
    }

    NVIC_SystemReset();

    /* prvPAL_ResetDevice should reset the board, but, hey, this is still a FreeRTOS task */
    vTaskDelete(NULL);
}

void ota_task(void *pvParameters)
{
    sln_flash_fs_status_t fs_status = SLN_FLASH_FS_OK;
    uint32_t certLen                = 0;

    APP_GetHexUniqueID(&s_thingName);

    configPRINTF(("Thing ID: %s\r\n", s_thingName));
    fs_status = sln_flash_fs_ops_read(clientcredentialCLIENT_CERTIFICATE_PEM, NULL, 0, &certLen);
    fs_status |= sln_flash_fs_ops_read(clientcredentialCLIENT_PRIVATE_KEY_PEM, NULL, 0, &certLen);

    if (fs_status == SLN_FLASH_FS_OK)
    {
        if (otaDoneTaskHandle == NULL)
        {
            /* Create a task for getting signaled when OTA finishes and a reset is needed */
            if (xTaskCreate(otaDoneTask, "ota_done_task", 256, NULL, tskIDLE_PRIORITY + 1, &otaDoneTaskHandle) !=
                pdPASS)
            {
                configPRINTF(("xTaskCreate ota_done_task failed\r\n"));
                goto exit;
            }
        }

        /* Inform the AWS OTA PAL about the task handle */
        otaPalOtaDoneTaskSet(otaDoneTaskHandle);
        /* Start the OTA Agent */
        vOTAUpdateTask();
    }
    else
    {
        LogError(("TLS credentials not found. OTA Agent will not start"));
    }

exit:
    vPortFree(s_thingName);
    s_thingName = NULL;
    vTaskDelete(NULL);
}
