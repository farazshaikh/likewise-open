/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        ipc_provider.c
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS)
 *
 *        Inter-process communication (Server) API for Users
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#include "api.h"

LWMsgStatus
LsaSrvIpcProviderIoControl(
    LWMsgAssoc* assoc,
    const LWMsgMessage* pRequest,
    LWMsgMessage* pResponse,
    void* data
    )
{
    DWORD dwError = 0;
    PLSA_IPC_ERROR pError = NULL;
    // Do not free pProviderIoControlReq
    PLSA_IPC_PROVIDER_IO_CONTROL_REQ pProviderIoControlReq =
        (PLSA_IPC_PROVIDER_IO_CONTROL_REQ)pRequest->object;
    PVOID Handle = NULL;
    DWORD dwOutputBufferSize = 0;
    PVOID pOutputBuffer = NULL;
    PLSA_DATA_BLOB pBlob = NULL;

    dwError = MAP_LWMSG_ERROR(lwmsg_assoc_get_session_data(assoc, (PVOID*) (PVOID) &Handle));
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvProviderIoControl(
                  (HANDLE)Handle,
                  pProviderIoControlReq->pszProvider,
                  pProviderIoControlReq->dwIoControlCode,
                  pProviderIoControlReq->dwDataLen,
                  pProviderIoControlReq->pData,
                  &dwOutputBufferSize,
                  &pOutputBuffer);

    if (!dwError)
    {
        if ( dwOutputBufferSize )
        {
            pResponse->tag = LSA_R_PROVIDER_IO_CONTROL_SUCCESS_DATA;
            dwError = LsaDataBlobStore(
                          &pBlob,
                          dwOutputBufferSize,
                          pOutputBuffer);
            BAIL_ON_LSA_ERROR(dwError);
            pResponse->object = pBlob;
        }
        else
        {
            pResponse->tag = LSA_R_PROVIDER_IO_CONTROL_SUCCESS;
            pResponse->object = NULL;
        }
    }
    else
    {
        dwError = LsaSrvIpcCreateError(dwError, NULL, &pError);
        BAIL_ON_LSA_ERROR(dwError);

        pResponse->tag = LSA_R_PROVIDER_IO_CONTROL_FAILURE;;
        pResponse->object = pError;
    }

cleanup:
    if ( pOutputBuffer )
    {
        LsaFreeMemory(pOutputBuffer);
    }

    return MAP_LSA_ERROR_IPC(dwError);

error:

    LsaDataBlobFree( &pBlob );

    goto cleanup;
}

