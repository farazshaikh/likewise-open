/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
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
 *        ipc_auth.c
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS)
 *
 *        Inter-process communication (Server) API for Authentication
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#include "ipc.h"

DWORD
LsaSrvIpcAuthenticateUser(
    HANDLE hConnection,
    PLSAMESSAGE pMessage
    )
{
    DWORD dwError = 0;
    PSTR  pszLoginName = NULL;
    PSTR  pszPassword = NULL;
    PLSAMESSAGE pResponse = NULL;
    PLSASERVERCONNECTIONCONTEXT pContext =
        (PLSASERVERCONNECTIONCONTEXT)hConnection;
    HANDLE hServer = (HANDLE)NULL;

    dwError = LsaUnmarshalCredentials(
                    pMessage->pData,
                    pMessage->header.messageLength,
                    &pszLoginName,
                    &pszPassword,
                    NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvIpcOpenServer(hConnection, &hServer);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvAuthenticateUser(
                    hServer,
                    pszLoginName,
                    pszPassword);
    if (!dwError) {

       dwError = LsaBuildMessage(
                    LSA_R_AUTH_USER,
                    0, /* Empty message */
                    1,
                    1,
                    &pResponse
                    );
       BAIL_ON_LSA_ERROR(dwError);

    } else {

       DWORD dwOrigErrCode = 0;
       DWORD dwMsgLen = 0;

       dwOrigErrCode = dwError;

       dwError = LsaMarshalError(dwOrigErrCode, NULL, NULL, &dwMsgLen);
       BAIL_ON_LSA_ERROR(dwError);

       dwError = LsaBuildMessage(
                    LSA_ERROR,
                    dwMsgLen,
                    1,
                    1,
                    &pResponse
                    );
       BAIL_ON_LSA_ERROR(dwError);

       dwError = LsaMarshalError(dwOrigErrCode, NULL, pResponse->pData, &dwMsgLen);
       BAIL_ON_LSA_ERROR(dwError);
    }

    dwError = LsaWriteMessage(pContext->fd, pResponse);
    BAIL_ON_LSA_ERROR(dwError);

cleanup:

    LSA_SAFE_FREE_STRING(pszLoginName);
    LSA_SAFE_CLEAR_FREE_STRING(pszPassword);

    LSA_SAFE_FREE_MESSAGE(pResponse);

    return dwError;

error:

    goto cleanup;
}

DWORD
LsaSrvIpcValidateUser(
    HANDLE hConnection,
    PLSAMESSAGE pMessage
    )
{
    DWORD dwError = 0;
    PSTR  pszLoginName = NULL;
    PSTR  pszPassword = NULL;
    PLSAMESSAGE pResponse = NULL;
    PLSASERVERCONNECTIONCONTEXT pContext =
       (PLSASERVERCONNECTIONCONTEXT)hConnection;
    HANDLE hServer = (HANDLE)NULL;

    dwError = LsaUnmarshalCredentials(
                    pMessage->pData,
                    pMessage->header.messageLength,
                    &pszLoginName,
                    &pszPassword,
                    NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvIpcOpenServer(hConnection, &hServer);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvValidateUser(hServer, pszLoginName, pszPassword);
    if (!dwError) {

       dwError = LsaBuildMessage(
                    LSA_R_VALIDATE_USER,
                    0, /* Empty message */
                    1,
                    1,
                    &pResponse
                    );
       BAIL_ON_LSA_ERROR(dwError);

    } else {

       DWORD dwOrigErrCode = 0;
       DWORD dwMsgLen = 0;

       dwOrigErrCode = dwError;

       dwError = LsaMarshalError(dwOrigErrCode, NULL, NULL, &dwMsgLen);
       BAIL_ON_LSA_ERROR(dwError);

       dwError = LsaBuildMessage(
                    LSA_ERROR,
                    dwMsgLen,
                    1,
                    1,
                    &pResponse
                    );
       BAIL_ON_LSA_ERROR(dwError);

       dwError = LsaMarshalError(dwOrigErrCode, NULL, pResponse->pData, &dwMsgLen);
       BAIL_ON_LSA_ERROR(dwError);
    }

    dwError = LsaWriteMessage(pContext->fd, pResponse);
    BAIL_ON_LSA_ERROR(dwError);

cleanup:

    LSA_SAFE_FREE_STRING(pszLoginName);
    LSA_SAFE_CLEAR_FREE_STRING(pszPassword);

    LSA_SAFE_FREE_MESSAGE(pResponse);

    return dwError;

error:

    goto cleanup;
}

DWORD
LsaSrvIpcCheckUserInList(
    HANDLE hConnection,
    PLSAMESSAGE pMessage
    )
{
    DWORD dwError = 0;
    PSTR  pszLoginName = NULL;
    PSTR  pszListName = NULL;
    PLSAMESSAGE pResponse = NULL;
    PLSASERVERCONNECTIONCONTEXT pContext =
       (PLSASERVERCONNECTIONCONTEXT)hConnection;
    HANDLE hServer = (HANDLE)NULL;

    dwError = LsaUnmarshalCredentials(
                    pMessage->pData,
                    pMessage->header.messageLength,
                    &pszLoginName,
                    &pszListName,
                    NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvIpcOpenServer(hConnection, &hServer);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvCheckUserInList(
                    hServer,
                    pszLoginName,
                    pszListName);
    if (!dwError) {

       dwError = LsaBuildMessage(
                    LSA_R_CHECK_USER_IN_LIST,
                    0, /* Empty message */
                    1,
                    1,
                    &pResponse
                    );
       BAIL_ON_LSA_ERROR(dwError);

    } else {

       DWORD dwOrigErrCode = 0;
       DWORD dwMsgLen = 0;

       dwOrigErrCode = dwError;

       dwError = LsaMarshalError(dwOrigErrCode, NULL, NULL, &dwMsgLen);
       BAIL_ON_LSA_ERROR(dwError);

       dwError = LsaBuildMessage(
                    LSA_ERROR,
                    dwMsgLen,
                    1,
                    1,
                    &pResponse
                    );
       BAIL_ON_LSA_ERROR(dwError);

       dwError = LsaMarshalError(dwOrigErrCode, NULL, pResponse->pData, &dwMsgLen);
       BAIL_ON_LSA_ERROR(dwError);
    }

    dwError = LsaWriteMessage(pContext->fd, pResponse);
    BAIL_ON_LSA_ERROR(dwError);

cleanup:

    LSA_SAFE_FREE_STRING(pszLoginName);
    LSA_SAFE_CLEAR_FREE_STRING(pszListName);

    LSA_SAFE_FREE_MESSAGE(pResponse);

    return dwError;

error:

    goto cleanup;
}

DWORD
LsaSrvIpcChangePassword(
    HANDLE hConnection,
    PLSAMESSAGE pMessage
    )
{
    DWORD dwError = 0;
    PSTR  pszLoginName = NULL;
    PSTR  pszPassword = NULL;
    PSTR  pszOldPassword = NULL;
    PLSAMESSAGE pResponse = NULL;
    PLSASERVERCONNECTIONCONTEXT pContext =
       (PLSASERVERCONNECTIONCONTEXT)hConnection;
    HANDLE hServer = (HANDLE)NULL;

    dwError = LsaUnmarshalCredentials(
                    pMessage->pData,
                    pMessage->header.messageLength,
                    &pszLoginName,
                    &pszPassword,
                    &pszOldPassword);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvIpcOpenServer(hConnection, &hServer);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LsaSrvChangePassword(
                    hServer,
                    pszLoginName,
                    pszPassword,
                    pszOldPassword);
    if (!dwError) {

       dwError = LsaBuildMessage(
                    LSA_R_CHANGE_PASSWORD,
                    0, /* Empty message */
                    1,
                    1,
                    &pResponse
                    );
       BAIL_ON_LSA_ERROR(dwError);

    } else {

       DWORD dwOrigErrCode = 0;
       DWORD dwMsgLen = 0;

       dwOrigErrCode = dwError;

       dwError = LsaMarshalError(dwOrigErrCode, NULL, NULL, &dwMsgLen);
       BAIL_ON_LSA_ERROR(dwError);

       dwError = LsaBuildMessage(
                    LSA_ERROR,
                    dwMsgLen,
                    1,
                    1,
                    &pResponse
                    );
       BAIL_ON_LSA_ERROR(dwError);

       dwError = LsaMarshalError(dwOrigErrCode, NULL, pResponse->pData, &dwMsgLen);
       BAIL_ON_LSA_ERROR(dwError);
    }

    dwError = LsaWriteMessage(pContext->fd, pResponse);
    BAIL_ON_LSA_ERROR(dwError);

cleanup:

    LSA_SAFE_FREE_STRING(pszLoginName);
    LSA_SAFE_CLEAR_FREE_STRING(pszPassword);
    LSA_SAFE_CLEAR_FREE_STRING(pszOldPassword);

    LSA_SAFE_FREE_MESSAGE(pResponse);

    return dwError;

error:

    goto cleanup;
}


/*********************************************************
 * The Extended Authenticate function. 
 */

DWORD
LsaSrvIpcAuthenticateUserEx(
    HANDLE hConnection,
    PLSAMESSAGE pMessage
    )
{
	DWORD dwError = LSA_ERROR_NOT_IMPLEMENTED;
	PLSAMESSAGE pResponse = NULL;       
	LSA_AUTH_USER_PARAMS Params;
	LSA_AUTH_USER_INFO UserInfo;	

	BAIL_ON_LSA_ERROR(dwError);

	memset(&Params, 0x0, sizeof(Params));
	memset(&UserInfo, 0x0, sizeof(UserInfo));

	/* Unmarshall the request */	
	
	dwError = LsaUnmarshallAuthenticateUserExQuery(
		pMessage->pData,
		&pMessage->header.messageLength,
		&Params);
	BAIL_ON_LSA_ERROR(dwError);	

	/* Do the work */

	/* Marshall the response and send it */

	dwError = LsaMarshallAuthenticateUserExReply(
		&pResponse,
		&UserInfo);
	BAIL_ON_LSA_ERROR(dwError);

cleanup:
	LSA_SAFE_FREE_MESSAGE(pResponse);	

	return dwError;
	
error:
	goto cleanup;	

}
