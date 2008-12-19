/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the license, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.  You should have received a copy
 * of the GNU Lesser General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * LESSER GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */



/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        lsaipc.h
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS) Interprocess Communication
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 *
 */
#ifndef __LSAIPC_H__
#define __LSAIPC_H__

#include <lwmsg/lwmsg.h>

#define LSA_CLIENT_PATH_FORMAT "/var/tmp/.lsaclient_%05ld"
#define LSA_SERVER_FILENAME    ".lsasd"

typedef enum __LSA_IPC_TAG
{
    LSA_Q_OPEN_SERVER,
    LSA_R_OPEN_SERVER_SUCCESS,
    LSA_R_OPEN_SERVER_FAILURE,
    LSA_Q_GROUP_BY_ID,
    LSA_R_GROUP_BY_ID_SUCCESS,
    LSA_R_GROUP_BY_ID_FAILURE,
    LSA_Q_GROUP_BY_NAME,
    LSA_R_GROUP_BY_NAME_SUCCESS,
    LSA_R_GROUP_BY_NAME_FAILURE,
    LSA_Q_BEGIN_ENUM_GROUPS,
    LSA_R_BEGIN_ENUM_GROUPS_SUCCESS,
    LSA_R_BEGIN_ENUM_GROUPS_FAILURE,
    LSA_Q_ENUM_GROUPS,
    LSA_R_ENUM_GROUPS_SUCCESS,
    LSA_R_ENUM_GROUPS_FAILURE,
    LSA_Q_END_ENUM_GROUPS,
    LSA_R_END_ENUM_GROUPS_SUCCESS,
    LSA_R_END_ENUM_GROUPS_FAILURE,
    LSA_Q_ADD_GROUP,
    LSA_R_ADD_GROUP_SUCCESS,
    LSA_R_ADD_GROUP_FAILURE,
    LSA_Q_DELETE_GROUP,
    LSA_R_DELETE_GROUP_SUCCESS,
    LSA_R_DELETE_GROUP_FAILURE,
    LSA_Q_GROUPS_FOR_USER,
    LSA_R_GROUPS_FOR_USER_SUCCESS,
    LSA_R_GROUPS_FOR_USER_FAILURE,
    LSA_Q_USER_BY_NAME,
    LSA_R_USER_BY_NAME_SUCCESS,
    LSA_R_USER_BY_NAME_FAILURE,
    LSA_Q_USER_BY_ID,
    LSA_R_USER_BY_ID_SUCCESS,
    LSA_R_USER_BY_ID_FAILURE,
    LSA_Q_BEGIN_ENUM_USERS,
    LSA_R_BEGIN_ENUM_USERS_SUCCESS,
    LSA_R_BEGIN_ENUM_USERS_FAILURE,
    LSA_Q_ENUM_USERS,
    LSA_R_ENUM_USERS_SUCCESS,
    LSA_R_ENUM_USERS_FAILURE,
    LSA_Q_END_ENUM_USERS,
    LSA_R_END_ENUM_USERS_SUCCESS,
    LSA_R_END_ENUM_USERS_FAILURE,
    LSA_Q_AUTH_USER,
    LSA_R_AUTH_USER_SUCCESS,
    LSA_R_AUTH_USER_FAILURE,
    LSA_Q_VALIDATE_USER,
    LSA_R_VALIDATE_USER_SUCCESS,
    LSA_R_VALIDATE_USER_FAILURE,
    LSA_Q_ADD_USER,
    LSA_R_ADD_USER_SUCCESS,
    LSA_R_ADD_USER_FAILURE,
    LSA_Q_DELETE_USER,
    LSA_R_DELETE_USER_SUCCESS,
    LSA_R_DELETE_USER_FAILURE,
    LSA_Q_CHANGE_PASSWORD,
    LSA_R_CHANGE_PASSWORD_SUCCESS,
    LSA_R_CHANGE_PASSWORD_FAILURE,
    LSA_Q_OPEN_SESSION,
    LSA_R_OPEN_SESSION_SUCCESS,
    LSA_R_OPEN_SESSION_FAILURE,
    LSA_Q_CLOSE_SESSION,
    LSA_R_CLOSE_SESSION_SUCCESS,
    LSA_R_CLOSE_SESSION_FAILURE,
    LSA_Q_MODIFY_USER,
    LSA_R_MODIFY_USER_SUCCESS,
    LSA_R_MODIFY_USER_FAILURE,
    LSA_Q_NAMES_BY_SID_LIST,
    LSA_R_NAMES_BY_SID_LIST_SUCCESS,
    LSA_R_NAMES_BY_SID_LIST_FAILURE,
    LSA_Q_GSS_MAKE_AUTH_MSG,
    LSA_R_GSS_MAKE_AUTH_MSG_SUCCESS,
    LSA_R_GSS_MAKE_AUTH_MSG_FAILURE,
    LSA_Q_GSS_CHECK_AUTH_MSG,
    LSA_R_GSS_CHECK_AUTH_MSG_SUCCESS,
    LSA_R_GSS_CHECK_AUTH_MSG_FAILURE,
    LSA_Q_AUTH_USER_EX,
    LSA_R_AUTH_USER_EX_SUCCESS,
    LSA_R_AUTH_USER_EX_FAILURE,
    LSA_Q_SET_LOGINFO,
    LSA_R_SET_LOGINFO_SUCCESS,
    LSA_R_SET_LOGINFO_FAILURE,
    LSA_Q_GET_LOGINFO,
    LSA_R_GET_LOGINFO_SUCCESS,
    LSA_R_GET_LOGINFO_FAILURE,
    LSA_Q_GET_METRICS,
    LSA_R_GET_METRICS_SUCCESS,
    LSA_R_GET_METRICS_FAILURE,
    LSA_Q_GET_STATUS,
    LSA_R_GET_STATUS_SUCCESS,
    LSA_R_GET_STATUS_FAILURE,
    LSA_Q_REFRESH_CONFIGURATION,
    LSA_R_REFRESH_CONFIGURATION_SUCCESS,
    LSA_R_REFRESH_CONFIGURATION_FAILURE,
    LSA_Q_CHECK_USER_IN_LIST,
    LSA_R_CHECK_USER_IN_LIST_SUCCESS,
    LSA_R_CHECK_USER_IN_LIST_FAILURE,
    LSA_Q_BEGIN_ENUM_NSS_ARTEFACTS,
    LSA_R_BEGIN_ENUM_NSS_ARTEFACTS_SUCCESS,
    LSA_R_BEGIN_ENUM_NSS_ARTEFACTS_FAILURE,
    LSA_Q_ENUM_NSS_ARTEFACTS,
    LSA_R_ENUM_NSS_ARTEFACTS_SUCCESS,
    LSA_R_ENUM_NSS_ARTEFACTS_FAILURE,
    LSA_Q_END_ENUM_NSS_ARTEFACTS,
    LSA_R_END_ENUM_NSS_ARTEFACTS_SUCCESS,
    LSA_R_END_ENUM_NSS_ARTEFACTS_FAILURE,
    LSA_Q_FIND_NSS_ARTEFACT_BY_KEY,
    LSA_R_FIND_NSS_ARTEFACT_BY_KEY_SUCCESS,
    LSA_R_FIND_NSS_ARTEFACT_BY_KEY_FAILURE,
    LSA_Q_SET_TRACE_INFO,
    LSA_R_SET_TRACE_INFO_SUCCESS,
    LSA_R_SET_TRACE_INFO_FAILURE,
    LSA_Q_GET_TRACE_INFO,
    LSA_R_GET_TRACE_INFO_SUCCESS,
    LSA_R_GET_TRACE_INFO_FAILURE,
    LSA_Q_ENUM_TRACE_INFO,
    LSA_R_ENUM_TRACE_INFO_SUCCESS,
    LSA_R_ENUM_TRACE_INFO_FAILURE
} LSA_IPC_TAG;

/* Opaque type -- actual definition in state_p.h - LSA_SRV_ENUM_STATE */

typedef struct __LSA_SRV_ENUM_STATE LsaIpcEnumServerHandle;

typedef struct __LSA_IPC_ERROR
{
    DWORD dwError;
    PCSTR pszErrorMessage;
} LSA_IPC_ERROR, *PLSA_IPC_ERROR;

typedef struct __LSA_IPC_FIND_OBJECT_BY_NAME_REQ
{
    LSA_FIND_FLAGS FindFlags;
    DWORD dwInfoLevel;
    PCSTR pszName;
} LSA_IPC_FIND_OBJECT_BY_NAME_REQ, *PLSA_IPC_FIND_OBJECT_BY_NAME_REQ;

typedef struct __LSA_IPC_FIND_NSSARTEFACT_BY_KEY_REQ
{
    LSA_NIS_MAP_QUERY_FLAGS dwFlags;
    DWORD dwInfoLevel;
    PCSTR pszKeyName;
    PCSTR pszMapName;
} LSA_IPC_FIND_NSSARTEFACT_BY_KEY_REQ, *PLSA_IPC_FIND_NSSARTEFACT_BY_KEY_REQ;

typedef struct __LSA_IPC_FIND_OBJECT_BY_ID_REQ
{
    LSA_FIND_FLAGS FindFlags;
    DWORD dwInfoLevel;
    DWORD id;
} LSA_IPC_FIND_OBJECT_BY_ID_REQ, *PLSA_IPC_FIND_OBJECT_BY_ID_REQ;

typedef struct __LSA_IPC_AUTH_USER_REQ
{
    PCSTR pszLoginName;
    PCSTR pszPassword;
} LSA_IPC_AUTH_USER_REQ, *PLSA_IPC_AUTH_USER_REQ;

typedef struct __LSA_IPC_CHANGE_PASSWORD_REQ
{
    PCSTR pszLoginName;
    PCSTR pszNewPassword;
    PCSTR pszOldPassword;
} LSA_IPC_CHANGE_PASSWORD_REQ, *PLSA_IPC_CHANGE_PASSWORD_REQ;

typedef struct __LSA_IPC_NAMES_BY_SIDS_REQ
{
    size_t sCount;
    PSTR* ppszSidList;
} LSA_IPC_NAMES_BY_SIDS_REQ, *PLSA_IPC_NAMES_BY_SIDS_REQ;

typedef struct __LSA_IPC_MAKE_AUTH_MSG_REQ
{
    ULONG negotiateFlags;
    SEC_BUFFER credentials;
    SEC_BUFFER_S serverChallenge;
    SEC_BUFFER targetInfo;
} LSA_IPC_MAKE_AUTH_MSG_REQ, *PLSA_IPC_MAKE_AUTH_MSG_REQ;

typedef struct __LSA_GSS_R_MAKE_AUTH_MSG
{
    DWORD msgError;
    SEC_BUFFER authenticateMessage;
    SEC_BUFFER_S baseSessionKey;
} LSA_GSS_R_MAKE_AUTH_MSG, *PLSA_GSS_R_MAKE_AUTH_MSG;

typedef struct __LSA_IPC_CHECK_AUTH_MSG_REQ
{
    ULONG negotiateFlags;
    SEC_BUFFER_S serverChallenge;
    SEC_BUFFER targetInfo;
    SEC_BUFFER authenticateMessage;
} LSA_IPC_CHECK_AUTH_MSG_REQ, *PLSA_IPC_CHECK_AUTH_MSG_REQ;

typedef struct __LSA_GSS_R_CHECK_AUTH_MSG
{
    DWORD msgError;
    SEC_BUFFER_S baseSessionKey;
} LSA_GSS_R_CHECK_AUTH_MSG, *PLSA_GSS_R_CHECK_AUTH_MSG;

typedef struct __LSA_IPC_CHECK_USER_IN_LIST_REQ
{
    PCSTR pszLoginName;
    PCSTR pszListName;
} LSA_IPC_CHECK_USER_IN_LIST_REQ, *PLSA_IPC_CHECK_USER_IN_LIST_REQ;

typedef struct __LSA_IPC_SET_TRACE_INFO_REQ
{
    PLSA_TRACE_INFO pTraceFlagArray;
    DWORD dwNumFlags;
} LSA_IPC_SET_TRACE_INFO_REQ, *PLSA_IPC_SET_TRACE_INFO_REQ;

typedef struct __LSA_IPC_BEGIN_ENUM_RECORDS_REQ
{
    LsaIpcEnumServerHandle* Handle;
    DWORD dwInfoLevel;
    DWORD dwNumMaxRecords;
} LSA_IPC_BEGIN_ENUM_RECORDS_REQ, *PLSA_IPC_BEGIN_ENUM_RECORDS_REQ;

typedef struct __LSA_IPC_BEGIN_ENUM_NSSARTEFACT_REQ
{
    LsaIpcEnumServerHandle* Handle;
    DWORD dwInfoLevel;
    DWORD dwMaxNumNSSArtefacts;
    DWORD dwFlags;
    PCSTR pszMapName;
} LSA_IPC_BEGIN_ENUM_NSSARTEFACT_REQ, *PLSA_IPC_BEGIN_ENUM_NSSARTEFACT_REQ;

typedef struct __LSA_IPC_ENUM_RECORDS_REQ {
    LsaIpcEnumServerHandle* Handle;
    PCSTR pszToken;
} LSA_IPC_ENUM_RECORDS_REQ, *PLSA_IPC_ENUM_RECORDS_REQ;

#define MAP_LWMSG_ERROR(_e_) ((_e_) ? -1 : 0)
#define MAP_LSA_ERROR_IPC(_e_) ((_e_) ? LWMSG_STATUS_ERROR : LWMSG_STATUS_SUCCESS)

LWMsgProtocolSpec*
LsaIPCGetProtocolSpec(
    void
    );

DWORD
LsaOpenServer(
    PHANDLE phConnection
    );

DWORD
LsaCloseServer(
    HANDLE hConnection
    );

DWORD
LsaWriteData(
    DWORD dwFd,
    PSTR  pszBuf,
    DWORD dwLen);

DWORD
LsaReadData(
    DWORD  dwFd,
    PSTR   pszBuf,
    DWORD  dwBytesToRead,
    PDWORD pdwBytesRead);

#endif /*__LSAIPC_H__*/
