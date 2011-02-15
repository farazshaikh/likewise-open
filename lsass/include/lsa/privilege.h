/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * Editor Settings: expandtabs and use 4 spaces for indentation */

/*
 * Copyright Likewise Software
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
 *        privilege.h
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS) Client API
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 *
 */
#ifndef __LSACLIENT_PRIVILEGE_H__
#define __LSACLIENT_PRIVILEGE_H__

#include <lsa/lsa.h>
#include <sys/types.h>


DWORD
LsaPrivsAddAccountRights(
    IN HANDLE hLsaConnection,
    IN PSID pAccountSid,
    IN PWSTR *ppwszAccountRights,
    IN DWORD NumAccountRights
    );


DWORD
LsaPrivsRemoveAccountRights(
    IN HANDLE hLsaConnection,
    IN PSID pAccountSid,
    IN BOOLEAN RemoveAll,
    IN PWSTR *ppwszAccountRights,
    IN DWORD NumAccountRights
    );


DWORD
LsaPrivsLookupPrivilegeValue(
    IN HANDLE hLsaConnection,
    IN PCWSTR pwszPrivilegeName,
    OUT PLUID pPrivilegeValue
    );


DWORD
LsaPrivsLookupPrivilegeName(
    IN HANDLE hLsaConnection,
    IN PLUID pPrivilegeValue,
    OUT PWSTR *ppwszPrivilegeName
    );


DWORD
LsaPrivsEnumAccountRights(
    IN HANDLE hLsaConnection,
    IN PSID pAccountSid,
    OUT PWSTR **pppwszAccountRights,
    OUT PDWORD pNumAccountRights
    );


#endif /* __LSACLIENT_PRIVILEGE_H__ */