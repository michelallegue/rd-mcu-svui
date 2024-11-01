/*
 * FreeRTOS OTA PAL V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright 2024 NXP
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* OTA PAL implementation for NXP MCUXpresso SDK. */

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "ota_pal.h"
#include "fsl_common.h"

/* Include file system */
#include "sln_flash_files.h"
#include "sln_flash_fs_ops.h"

/* Include update modules */
#include "flash_ica_driver.h"
#include "sln_update.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char OTA_JsonFileSignatureKey[OTA_FILE_SIG_KEY_STR_MAX_LENGTH] = "sig-sha256-rsa";

/* PAL file context structure */
/* PAL file context structure */
typedef struct
{
    const OtaFileContext_t *FileXRef;
    uint32_t file_size;
    uint8_t percentage;
} PAL_FileContext_t;

static PAL_FileContext_t prvPAL_CurrentFileContext;
#define DEFINE_OTA_METHOD_NAME(name) static const char OTA_METHOD_NAME[] = name;

#define ImgType_BOOT_PATH   "Boot"
#define ImgType_APP_A_PATH  "AppA"
#define ImgType_APP_B_PATH  "AppB"

static int32_t s_imgType;
static OtaImageState_t s_platformState  = OtaImageStateUnknown;
static TaskHandle_t s_otaDoneTaskHandle = NULL;

static uint8_t *prvPAL_GetCertificate(const char *pCertName)
{
    sln_flash_fs_status_t status = SLN_FLASH_FS_OK;
    uint32_t certLen             = 0;
    uint8_t *certPem             = NULL;

    status = sln_flash_fs_ops_read(pCertName, NULL, 0, &certLen);

    if (SLN_FLASH_FS_OK != status)
    {
        LogError(("Unable to obtain certificate %s size: %d\r\n", pCertName, status));
    }

    if (SLN_FLASH_FS_OK == status)
    {
        // Found a file
        certPem = (uint8_t *)pvPortMalloc(certLen);

        if (NULL == certPem)
        {
            status = SLN_FLASH_ERROR;
        }
        else
        {
            status = sln_flash_fs_ops_read(pCertName, certPem, 0, &certLen);
        }
    }

    if (SLN_FLASH_FS_OK == status)
    {
        return certPem;
    }
    else
    {
        LogError(("Unable to obtain certificate: %d\r\n", status));
        return NULL;
    }
}

static OtaPalMainStatus_t prvPAL_CheckFileSignature(OtaFileContext_t *const C)
{
    OtaPalMainStatus_t status = OtaPalSuccess;

    if (SLN_Update_VerifyImgBank(s_imgType, C->pSignature->data) != kStatus_Success)
    {
        status = OtaPalSignatureCheckFailed;
    }

    return status;
}

static PAL_FileContext_t *prvPAL_GetPALFileContext(OtaFileContext_t *const C)
{
    PAL_FileContext_t *PalFileContext;

    if ((C == NULL) || (C->pFile == NULL))
    {
        return NULL;
    }

    PalFileContext = (PAL_FileContext_t *)C->pFile;

    if ((PalFileContext == NULL) || (PalFileContext->FileXRef != C))
    {
        return NULL;
    }

    return PalFileContext;
}

OtaPalStatus_t xOtaPalAbort(OtaFileContext_t *const C)
{
    OtaPalStatus_t result = OtaPalSuccess;

    LogInfo(("[OTA-NXP] Abort"));

    C->pFile = NULL;
    return OTA_PAL_COMBINE_ERR(result, 0);
}

OtaPalStatus_t xOtaPalCreateFileForRx(OtaFileContext_t *const C)
{
    DEFINE_OTA_METHOD_NAME("otaPal_CreateFileForRx");
    OtaPalStatus_t ret = OtaPalSuccess;
    s_imgType          = FICA_IMG_TYPE_NONE;

    PAL_FileContext_t *PalFileContext = &prvPAL_CurrentFileContext;

    /* Decide what image type we have based on the file path */
    if (!strcmp((const char *)C->pFilePath, ImgType_APP_A_PATH))
    {
        s_imgType = FICA_IMG_TYPE_APP_A;
    }
    else if (!strcmp((const char *)C->pFilePath, ImgType_APP_B_PATH))
    {
        s_imgType = FICA_IMG_TYPE_APP_B;
    }
    else
    {
        LogError(("[%s] Invalid file path received: '%s'.\r\n", OTA_METHOD_NAME, C->pFilePath));
        ret = OtaPalRxFileCreateFailed;
    }

    /* Init FICA to be ready for the new application */
    if (OtaPalSuccess == ret)
    {
        status_t status = SLN_Update_PrepareImgBank(s_imgType, C->fileSize);
        if (kStatus_Success != status)
        {
            LogError(("[%s] SLN_Update_PrepareImgBank failed, error %d.\r\n", OTA_METHOD_NAME, status));
            ret = OtaPalRxFileCreateFailed;
        }
        else
        {
            /* Set LED Blue to indicate update is in progress */
            // RGB_LED_SetBrightnessColor(LED_BRIGHT_MEDIUM, LED_COLOR_BLUE);
        }
    }

    if (OtaPalSuccess == ret)
    {
        /* We don't need yet anything here, but this field
         * must not remain NULL after this function returns */
        C->pFile                   = (void *)PalFileContext;
        PalFileContext->FileXRef   = C;
        PalFileContext->percentage = 0;
        PalFileContext->file_size  = 0;
        LogInfo(("[%s] OK.\r\n", OTA_METHOD_NAME));
    }
    else
    {
        LogError(("[%s] Failed.\r\n", OTA_METHOD_NAME));
    }

    return OTA_PAL_COMBINE_ERR(ret, 0);
}

OtaPalStatus_t xOtaPalCloseFile(OtaFileContext_t *const C)
{
    OtaPalStatus_t result = OtaPalSuccess;
    PAL_FileContext_t *PalFileContext;

    LogInfo(("[OTA-NXP] CloseFile"));

    PalFileContext = prvPAL_GetPALFileContext(C);
    if (PalFileContext == NULL)
    {
        return OTA_PAL_COMBINE_ERR(OtaPalFileClose, 0);
    }

    if (PalFileContext->file_size != C->fileSize)
    {
        LogError(("[OTA-NXP] Actual file size %d is not as expected %d.", PalFileContext->file_size, C->fileSize));
        return OTA_PAL_COMBINE_ERR(OtaPalFileClose, 0);
    }

    result = prvPAL_CheckFileSignature(C);
    if (result != OtaPalSuccess)
    {
        LogError(("[OTA-NXP] CheckFileSignature failed"));
        return OTA_PAL_COMBINE_ERR(OtaPalFileClose, 0);
    }

    C->pFile = NULL;
    return OTA_PAL_COMBINE_ERR(result, 0);
}

int16_t xOtaPalWriteBlock(OtaFileContext_t *const C, uint32_t ulOffset, uint8_t *const pcData, uint32_t ulBlockSize)
{
    DEFINE_OTA_METHOD_NAME("otaPal_WriteBlock");
    status_t status = kStatus_Success;
    int16_t retVal  = ulBlockSize;

    uint8_t *data           = pcData;
    uint32_t data_offset    = ulOffset;
    uint32_t data_remaining = ulBlockSize;

    PAL_FileContext_t *PalFileContext;

    LogInfo(("[OTA-NXP] WriteBlock 0x%x : 0x%x", ulOffset, ulBlockSize));

    PalFileContext = prvPAL_GetPALFileContext(C);
    if (PalFileContext == NULL)
    {
        return -1;
    }

    status = SLN_Update_WriteImg(s_imgType, data_offset, data, data_remaining);

    if (kStatus_Success == status)
    {
        uint32_t newPercentage = PalFileContext->file_size / (float)C->fileSize * 100;

        if (newPercentage != PalFileContext->percentage)
        {
            PalFileContext->percentage = newPercentage;
        }

        LogInfo(("[%s] SLN_Update_WriteImg ended with success, file size is now %d.\r\n", OTA_METHOD_NAME,
                  PalFileContext->file_size));

        /* Update size of file received so far */
        PalFileContext->file_size += data_remaining;
    }
    else
    {
        LogError(("[%s] SLN_Update_WriteImg failed, error %d.\r\n", OTA_METHOD_NAME, status));
        retVal = -1;
    }

    return retVal;
}

OtaPalStatus_t xOtaPalActivateNewImage(OtaFileContext_t *const C)
{
    OtaPalStatus_t result = OtaPalSuccess;

    LogInfo(("[OTA-NXP] ActivateNewImage"));

    if (SLN_Update_FinalizeUpdate(s_imgType, C->pSignature->data) != kStatus_Success)
    {
        result = OtaPalFileClose;
    }
    else
    {
        result = xOtaPalResetDevice(C);
    }

    return OTA_PAL_COMBINE_ERR(result, 0);
}

OtaPalStatus_t xOtaPalResetDevice(OtaFileContext_t *const C)
{
    LogInfo(("[OTA-NXP] SystemReset"));

    /* give some time for last logs printing */
    vTaskDelay(2000);

    NVIC_SystemReset();

    /* this should never return */
    return OTA_PAL_COMBINE_ERR(OtaPalSuccess, 0);
}

OtaPalStatus_t xOtaPalSetPlatformImageState(OtaFileContext_t *const C, OtaImageState_t eState)
{
    DEFINE_OTA_METHOD_NAME("otaPal_SetPlatformImageState");
    OtaPalStatus_t result = OtaPalSuccess;

    LogInfo(("[OTA-NXP] SetPlatformImageState %d", eState));

    if (xOtaPalGetPlatformImageState(C) == OtaPalImageStatePendingCommit)
    {
        /* Device in test mode */
        switch (eState)
        {
            case OtaImageStateAccepted:

                LogInfo(("[%s] Image successfully committed, using updated app after reset\r\n", OTA_METHOD_NAME));

#if ENABLE_UNSIGNED_USB_MSD

                bool isUSBMode   = false;
                int32_t fica_ret = FICA_get_usb_mode(&isUSBMode);

                /* Clear USB mode here. Need to do this when switching from an MSD written firmware
                 * to a successfully OTA-ed firmware */
                if (fica_ret || isUSBMode)
                {
                    fica_ret = FICA_clr_usb_mode();

                    if (SLN_FLASH_NO_ERROR != fica_ret)
                    {
                        LogError(("[%s] Failed to clear usb mode flag, error %d\r\n", OTA_METHOD_NAME, fica_ret));
                    }
                }
#endif /* ENABLE_UNSIGNED_USB_MSD */

                /* signal ota done task, it'll reset in order to not get stuck in the self test  */
                xTaskNotifyGive(s_otaDoneTaskHandle);
                break;

            case OtaImageStateRejected:
                /* Invalidate the image */
                SLN_Update_Rollback();
                LogInfo(("[%s] Rejected image.\r\n", OTA_METHOD_NAME));
                break;

            case OtaImageStateAborted:
                /* Invalidate the image */
                LogInfo(("[%s] Aborted image.\r\n", OTA_METHOD_NAME));
                break;

            case OtaImageStateTesting:
                LogInfo(("[%s] Image state testing.\r\n", OTA_METHOD_NAME));
                break;

            default:
                result = OtaPalBadImageState;
                break;
        }
    }
    else
    {
        /* Normal mode */
        switch (eState)
        {
            case OtaImageStateAccepted:
                /* No pending commit */
                result = OtaPalCommitFailed;
                break;

            case OtaImageStateRejected:
                result = OtaPalRejectFailed;
                break;

            case OtaImageStateAborted:
                result = OtaPalAbortFailed;
                break;

            case OtaImageStateTesting:
                s_platformState = OtaPalImageStatePendingCommit;
                break;

            default:
                result = OtaPalBadImageState;
                break;
        }
    }

    return OTA_PAL_COMBINE_ERR(result, 0);
}

OtaPalImageState_t xOtaPalGetPlatformImageState(OtaFileContext_t *const C)
{
    return s_platformState;
}

void otaPalOtaDoneTaskSet(TaskHandle_t handle)
{
    s_otaDoneTaskHandle = handle;
}

