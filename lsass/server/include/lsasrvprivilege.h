/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * Editor Settings: expandtabs and use 4 spaces for indentation */

/*
 * Copyright Likewise Software    2004-2011
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
 *        lsasrvprivilege.h
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS)
 *
 *        Local Privilege Server API
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#ifndef __LSASRV_PRIVILEGE_H__
#define __LSASRV_PRIVILEGE_H__


struct _LSA_ACCOUNT_CONTEXT;
typedef struct _LSA_ACCOUNT_CONTEXT
LSA_ACCOUNT_CONTEXT, *PLSA_ACCOUNT_CONTEXT;


DWORD
LsaSrvInitPrivileges(
    VOID
    );


VOID
LsaSrvFreePrivileges(
    VOID
    );


DWORD
LsaSrvPrivsOpenAccount(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PSID Sid,
    IN ACCESS_MASK AccessRights,
    OUT PLSA_ACCOUNT_CONTEXT *pAccountContext
    );


DWORD
LsaSrvPrivsCreateAccount(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PSID Sid,
    IN ACCESS_MASK AccessRights,
    OUT PLSA_ACCOUNT_CONTEXT *pAccountContext
    );


VOID
LsaSrvPrivsCloseAccount(
    IN HANDLE hProvider,
    IN OUT PLSA_ACCOUNT_CONTEXT *pAccountContext
    );


DWORD
LsaSrvPrivsAddAccountRights(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PSID AccountSid,
    IN PWSTR *ppwszAccountRights,
    IN DWORD NumAccountRights
    );


DWORD
LsaSrvPrivsRemoveAccountRights(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PSID AccountSid,
    IN BOOLEAN RemoveAll,
    IN PWSTR *ppwszAccountRights,
    IN DWORD NumAccountRights
    );


DWORD
LsaSrvPrivsEnumAccountRights(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PSID AccountSid,
    OUT PWSTR **ppwszAccountRights,
    OUT PDWORD pNumAccountRights
    );


DWORD
LsaSrvPrivsLookupPrivilegeValue(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PCWSTR pwszPrivilegeName,
    OUT PLUID pPrivilegeValue
    );


DWORD
LsaSrvPrivsLookupPrivilegeName(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PLUID pPrivilegeValue,
    OUT PWSTR *ppwszPrivilegeName
    );


DWORD
LsaSrvPrivsAddPrivilegesToAccount(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PLSA_ACCOUNT_CONTEXT pAccountContext,
    IN PPRIVILEGE_SET pPrivilegeSet
    );


DWORD
LsaSrvPrivsRemovePrivilegesFromAccount(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PLSA_ACCOUNT_CONTEXT pAccountContext,
    IN BOOLEAN RemoveAll,
    IN PPRIVILEGE_SET pPrivilegeSet
    );


DWORD
LsaSrvPrivsGetSystemAccessRights(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PLSA_ACCOUNT_CONTEXT pAccountContext,
    OUT PDWORD pSystemAccessRights
    );


DWORD
LsaSrvPrivsSetSystemAccessRights(
    IN HANDLE hProvider,
    IN OPTIONAL PACCESS_TOKEN AccessToken,
    IN PLSA_ACCOUNT_CONTEXT pAccountContext,
    IN DWORD SystemAccessRights
    );


#endif /* __LSASRV_PRIVILEGE_H__ */