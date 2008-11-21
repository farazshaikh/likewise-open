/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

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

#include "includes.h"


NTSTATUS LsaLookupSids(handle_t b, PolicyHandle *handle, SidArray *sids,
                       RefDomainList **domains, TranslatedName **names,
                       uint16 level, uint32 *count)
{
    NTSTATUS status = STATUS_SUCCESS;
    NTSTATUS ret_status = STATUS_SUCCESS;
    TranslatedNameArray name_array = {0};
    RefDomainList *ref_domains = NULL;
    TranslatedName *out_names = NULL;
    RefDomainList *out_domains = NULL;

    goto_if_invalid_param_ntstatus(b, cleanup);
    goto_if_invalid_param_ntstatus(handle, cleanup);
    goto_if_invalid_param_ntstatus(sids, cleanup);
    goto_if_invalid_param_ntstatus(domains, cleanup);
    goto_if_invalid_param_ntstatus(names, cleanup);
    goto_if_invalid_param_ntstatus(count, cleanup);

    /* windows allows level to be in range 1-6 */

    *count = 0;

    DCERPC_CALL(_LsaLookupSids(b, handle, sids, &ref_domains, &name_array,
                               level, count));
    ret_status = status;

    /* Status other than success doesn't have to mean failure here */
    if (ret_status != STATUS_SUCCESS &&
        ret_status != STATUS_SOME_UNMAPPED) goto error;

    status = LsaAllocateTranslatedNames(&out_names, &name_array);
    goto_if_ntstatus_not_success(status, error);

    status = LsaAllocateRefDomainList(&out_domains, ref_domains);
    goto_if_ntstatus_not_success(status, error);

    *names   = out_names;
    *domains = out_domains;

cleanup:

    /* Free pointers returned from stub */
    if (ref_domains) {
        LsaFreeStubRefDomainList(ref_domains);
    }

    LsaCleanStubTranslatedNameArray(&name_array);

    if (status == STATUS_SUCCESS &&
        (ret_status == STATUS_SUCCESS ||
         ret_status == STATUS_SOME_UNMAPPED)) {
        status = ret_status;
    }

    return status;

error:
    if (out_names) {
        LsaRpcFreeMemory((void*)out_names);
    }

    if (out_domains) {
        LsaRpcFreeMemory((void*)out_domains);
    }

    *names   = NULL;
    *domains = NULL;

    goto cleanup;
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
