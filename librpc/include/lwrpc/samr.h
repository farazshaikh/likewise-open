/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

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
 * Abstract: Samr interface (rpc client library)
 *
 * Authors: Rafal Szczesniak (rafal@likewisesoftware.com)
 */


#ifndef _SAMR_H_
#define _SAMR_H_

#include <secdesc/sectypes.h>

#include <lwrpc/samrbinding.h>
#include <lwrpc/unicodestring.h>
#include <lwrpc/samrflags.h>
#include <lwrpc/samrconn.h>
#include <lwrpc/userinfo.h>
#include <lwrpc/aliasinfo.h>
#include <lwrpc/domaininfo.h>
#include <lwrpc/displayinfo.h>


NTSTATUS
SamrConnect2(
    IN  handle_t      hSamrBinding,
    IN  PCWSTR        pwszSysName,
    IN  UINT32        AccessMask,
    OUT PolicyHandle *phConn
    );

NTSTATUS
SamrConnect3(
    IN  handle_t      hSamrBinding,
    IN  PCWSTR        pwszSysName,
    IN  UINT32        AccessMask,
    OUT PolicyHandle *phConn
    );

NTSTATUS
SamrConnect4(
    IN  handle_t      hSamrBinding,
    IN  PCWSTR        pwszSysName,
    IN  UINT32        ClientVersion,
    IN  UINT32        AccessMask,
    OUT PolicyHandle *phConn
    );

NTSTATUS
SamrConnect5(
    IN  handle_t         hSamrBinding,
    IN  PCWSTR           pwszSysName,
    IN  UINT32           AccessMask,
    IN  UINT32           LevelIn,
    IN  SamrConnectInfo *pInfoIn,
    IN  PUINT32          pLevelOut,
    OUT SamrConnectInfo *pInfoOut,
    OUT PolicyHandle    *phConn
    );

NTSTATUS
SamrClose(
    IN  handle_t         hSamrBinding,
    IN OUT PolicyHandle *phContext
    );

NTSTATUS
SamrQuerySecurity(
    IN  handle_t                       hSamrBinding,
    IN  PolicyHandle                  *pHandle,
    IN  ULONG                          ulSecurityInfo,
    OUT PSECURITY_DESCRIPTOR_RELATIVE *ppSecDesc,
    OUT PUINT32                        pulSecDescLen
    );

NTSTATUS
SamrLookupDomain(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phConn,
    IN  PCWSTR        pwszDomainName,
    OUT PSID         *ppSid
    );

NTSTATUS
SamrOpenDomain(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phConn,
    IN  UINT32        AccessMask,
    IN  PSID          pSid,
    OUT PolicyHandle *phDomain
    );

NTSTATUS
SamrQueryDomainInfo(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  UINT16        Level,
    OUT DomainInfo  **ppInfo
    );

NTSTATUS
SamrLookupNames(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  UINT32        NumNames,
    IN  PWSTR        *ppwszNames,
    OUT UINT32      **ppRids,
    OUT UINT32      **ppTypes,
    OUT UINT32       *pRidsCount
    );

NTSTATUS
SamrOpenUser(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  UINT32        AccessMask,
    IN  UINT32        Rid,
    OUT PolicyHandle *phUser
    );

NTSTATUS
SamrQueryUserInfo(
    IN  handle_t       hSamrBinding,
    IN  PolicyHandle  *phUser,
    IN  UINT16         Level,
    OUT UserInfo     **ppInfo
    );

NTSTATUS
SamrEnumDomainUsers(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN OUT UINT32    *pResume,
    IN  UINT32        AccountFlags,
    IN  UINT32        MaxSize,
    OUT PWSTR       **pppwszNames,
    OUT UINT32      **ppRids,
    OUT UINT32       *pCount
    );

NTSTATUS
SamrChangePasswordUser2(
    IN  handle_t hSamrBinding,
    IN  PCWSTR   pwszHostname,
    IN  PCWSTR   pwszAccount,
    IN  BYTE     ntpass[516],
    IN  BYTE     ntverify[16],
    IN  BYTE     bLmChange,
    IN  BYTE     lmpass[516],
    IN  BYTE     lmverify[16]
    );

NTSTATUS
SamrEnumDomains(
    IN  handle_t hSamrBinding,
    IN  PolicyHandle *phConn,
    IN OUT UINT32 *pResume,
    IN  UINT32 Size,
    OUT PWSTR **pppwszNames,
    OUT UINT32 *pCount
    );

NTSTATUS
SamrCreateUser(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  PWSTR         pwszAccountName,
    IN  UINT32        AccessMask,
    OUT PolicyHandle *phUser,
    OUT PUINT32       pRid
    );

NTSTATUS
SamrDeleteUser(
    IN  handle_t hSamrBinding,
    IN  PolicyHandle *phUser
    );

NTSTATUS
SamrSetUserInfo(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phUser,
    IN  UINT16        Level,
    IN  UserInfo     *pInfo
    );

NTSTATUS
SamrSetUserInfo2(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phUser,
    IN  UINT16        Level,
    IN  UserInfo     *pInfo
    );

NTSTATUS
SamrCreateDomAlias(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  PWSTR         pwszAliasName,
    IN  UINT32        AccessMask,
    OUT PolicyHandle *phAlias,
    OUT PUINT32       pRid
    );

NTSTATUS
SamrDeleteDomAlias(
    IN  handle_t      hSamrBinding,
    OUT PolicyHandle *phAlias
    );

NTSTATUS
SamrOpenAlias(
    IN  handle_t hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  UINT32 AccessMask,
    IN  UINT32 Rid,
    OUT PolicyHandle *phAlias
    );

NTSTATUS
SamrQueryAliasInfo(
    IN  handle_t       hSamrBinding,
    IN  PolicyHandle  *phAlias,
    IN  UINT16         Level,
    OUT AliasInfo    **ppInfo
    );

NTSTATUS
SamrSetAliasInfo(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phAlias,
    IN  UINT16        Level,
    IN  AliasInfo    *pInfo
    );

NTSTATUS
SamrEnumDomainAliases(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  PUINT32       pResume,
    IN  UINT32        AccountFlags,
    OUT PWSTR       **pppwszNames,
    OUT PUINT32      *ppRids,
    OUT PUINT32       pCount
    );

NTSTATUS
SamrGetAliasMembership(
    IN  handle_t       hSamrBinding,
    IN  PolicyHandle  *phDomain,
    IN  PSID          *ppSids,
    IN  UINT32         NumSids,
    OUT UINT32       **ppRids,
    OUT UINT32        *pCount
    );

NTSTATUS
SamrGetMembersInAlias(
    IN  handle_t       hSamrBinding,
    IN  PolicyHandle  *phAlias,
    OUT PSID         **pppSids,
    OUT UINT32        *pCount
    );

NTSTATUS
SamrAddAliasMember(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phAlias,
    IN  PSID          pSid
    );

NTSTATUS
SamrDeleteAliasMember(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phAlias,
    IN  PSID          pSid
    );

NTSTATUS
SamrCreateUser2(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  PWSTR         pwszAccountName,
    IN  UINT32        AccountFlags,
    IN  UINT32        AccessMask,
    OUT PolicyHandle *phUser,
    OUT PUINT32       pAccessGranted,
    OUT PUINT32       pRid
    );

NTSTATUS
SamrGetUserPwInfo(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phUser,
    OUT PwInfo       *pInfo
    );

NTSTATUS
SamrLookupRids(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  UINT32        NumRids,
    IN  UINT32       *pRids,
    OUT PWSTR       **pppwszNames,
    OUT UINT32      **ppTypes
    );

NTSTATUS
SamrGetUserGroups(
    IN  handle_t       hSamrBinding,
    IN  PolicyHandle  *phUser,
    OUT UINT32       **ppRids,
    OUT UINT32       **ppAttributes,
    OUT UINT32        *pCount
    );

NTSTATUS
SamrQueryDisplayInfo(
    IN  handle_t          hSamrBinding,
    IN  PolicyHandle     *phDomain,
    IN  UINT16            Level,
    IN  UINT32            StartIdx,
    IN  UINT32            MaxEntries,
    IN  UINT32            BufferSize,
    OUT UINT32           *pTotalSize,
    OUT UINT32           *pReturnedSize,
    OUT SamrDisplayInfo **ppInfo
    );

NTSTATUS
SamrCreateDomGroup(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  PWSTR         pwszGroupName,
    IN  UINT32        AccessMask,
    OUT PolicyHandle *phGroup,
    OUT PUINT32       pRid
    );

NTSTATUS
SamrDeleteDomGroup(
    IN  handle_t      hSamrBinding,
    IN  PolicyHandle *phGroup
    );

NTSTATUS
SamrOpenGroup(
    IN  handle_t hSamrBinding,
    IN  PolicyHandle *phDomain,
    IN  UINT32 AccessMask,
    IN  UINT32 Rid,
    OUT PolicyHandle *phGroup
    );

NTSTATUS
SamrInitMemory(
    void
    );

NTSTATUS
SamrDestroyMemory(
    void
    );

NTSTATUS SamrFreeMemory(
    void *ptr
    );



#endif /* _SAMR_H_ */


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
