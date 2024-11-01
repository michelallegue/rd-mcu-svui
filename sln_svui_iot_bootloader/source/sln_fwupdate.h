/*
 * Copyright 2019-2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SLN_FWUPDATE_H_
#define SLN_FWUPDATE_H_

/* Compare the value read from FICA_COMM_FWUPDATE_METHOD_BIT */
#define METHOD_SLN_OTA 0
#define METHOD_SLN_OTW 1

/*!
 * @brief TBD
 */
#define selfTestCleanup() \
    {                     \
    }
/*!
 * @brief Check for any update flags in the FICA and starts the update mechanism.
 *
 * @return True if there is an update request pending, false othewise.
 */

bool FWUpdate_CheckForUpdate(void);

/*!
 * @brief Starts IoT Solution firmware update over UART
 */

void otwAppStart(void);

/*!
 * @brief Starts IoT Solution firmware update over TCP
 */

void otaAppStart(void);

#endif /* SLN_FWUPDATE_H_ */
