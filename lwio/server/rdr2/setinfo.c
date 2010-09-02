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
 *        setinfo.c
 *
 * Abstract:
 *
 *        Likewise Client Redirector (RDR)
 *
 *        Set information
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */

#include "rdr.h"

static
NTSTATUS
RdrTransceiveSetInfo(
    PRDR_OP_CONTEXT pContext,
    PRDR_CCB pFile,
    SMB_INFO_LEVEL infoLevel,
    PVOID pInfo,
    ULONG ulInfoLength
    );

static
BOOLEAN
RdrSetInfoComplete(
    PRDR_OP_CONTEXT pContext,
    NTSTATUS status,
    PVOID pParam
    );

static
NTSTATUS
RdrMarshalFileInfo(
    PSMB_HEADER pSmbHeader,
    PBYTE* ppCursor,
    PULONG pulRemainingSpace,
    SMB_INFO_LEVEL infoLevel,
    PVOID pInfo,
    ULONG ulInfoLength
    );

static
NTSTATUS
RdrMarshalFileEndOfFileInfo(
    PSMB_HEADER pSmbHeader,
    PBYTE* ppCursor,
    PULONG pulRemainingSpace,
    PVOID pInfo,
    ULONG ulInfoLength
    );

static
NTSTATUS
RdrMarshalFileDispositionInfo(
    PSMB_HEADER pSmbHeader,
    PBYTE* ppCursor,
    PULONG pulRemainingSpace,
    PVOID pInfo,
    ULONG ulInfoLength
    );

static
NTSTATUS
RdrMarshalFileRenameInfo(
    PSMB_HEADER pSmbHeader,
    PBYTE* ppCursor,
    PULONG pulRemainingSpace,
    PVOID pInfo,
    ULONG ulInfoLength
    );

static
VOID
RdrCancelSetInfo(
    PIRP pIrp,
    PVOID pParam
    )
{
}

NTSTATUS
RdrSetInformation(
    IO_DEVICE_HANDLE IoDeviceHandle,
    PIRP pIrp
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PRDR_OP_CONTEXT pContext = NULL;
    SMB_INFO_LEVEL infoLevel = 0;
    PRDR_CCB pFile = NULL;

    switch (pIrp->Args.QuerySetInformation.FileInformationClass)
    {
    case FileEndOfFileInformation:
        infoLevel = SMB_SET_FILE_END_OF_FILE_INFO;
        break;
    case FileRenameInformation:
        /* FIXME: detect non-in-place renames and
           use alternate strategy */
        infoLevel = SMB_SET_FILE_RENAME_INFO;
        break;
    default:
        status = STATUS_NOT_IMPLEMENTED;
        BAIL_ON_NT_STATUS(status);
        break;
    }

    pFile = IoFileGetContext(pIrp->FileHandle);

    IoIrpMarkPending(pIrp, RdrCancelSetInfo, pContext);

    status = RdrCreateContext(pIrp, &pContext);
    BAIL_ON_NT_STATUS(status);

    pContext->Continue = RdrSetInfoComplete;

    status = RdrTransceiveSetInfo(
        pContext,
        pFile,
        infoLevel,
        pIrp->Args.QuerySetInformation.FileInformation,
        pIrp->Args.QuerySetInformation.Length);
    BAIL_ON_NT_STATUS(status);

error:

    if (status != STATUS_PENDING)
    {
        pIrp->IoStatusBlock.Status = status;
        IoIrpComplete(pIrp);
        RdrFreeContext(pContext);
        status = STATUS_PENDING;
    }

    return status;
}

static
NTSTATUS
RdrTransceiveSetInfo(
    PRDR_OP_CONTEXT pContext,
    PRDR_CCB pFile,
    SMB_INFO_LEVEL infoLevel,
    PVOID pInfo,
    ULONG ulInfoLength
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    TRANSACTION_REQUEST_HEADER *pHeader = NULL;
    USHORT usSetup = SMB_SUB_COMMAND_TRANS2_SET_FILE_INFORMATION;
    SMB_SET_FILE_INFO_HEADER setHeader = {0};
    PBYTE pRequestParameters = NULL;
    PBYTE pRequestData = NULL;
    USHORT usRequestDataCount = 0;
    PBYTE pCursor = NULL;
    ULONG ulRemainingSpace = 0;
    PBYTE pByteCount = NULL;

    status = RdrAllocateContextPacket(pContext, 1024*64);
    BAIL_ON_NT_STATUS(status);

    status = SMBPacketMarshallHeader(
        pContext->Packet.pRawBuffer,
        pContext->Packet.bufferLen,
        COM_TRANSACTION2,
        0,
        0,
        pFile->pTree->tid,
        gRdrRuntime.SysPid,
        pFile->pTree->pSession->uid,
        0,
        TRUE,
        &pContext->Packet);
    BAIL_ON_NT_STATUS(status);

    pContext->Packet.pData = pContext->Packet.pParams + sizeof(TRANSACTION_REQUEST_HEADER);

    pCursor = pContext->Packet.pParams;
    ulRemainingSpace = pContext->Packet.bufferLen - (pCursor - pContext->Packet.pRawBuffer);

    status = WireMarshalTrans2RequestSetup(
        pContext->Packet.pSMBHeader,
        &pCursor,
        &ulRemainingSpace,
        &usSetup,
        1,
        &pHeader,
        &pByteCount);
    BAIL_ON_NT_STATUS(status);

    pRequestParameters = pCursor;

    setHeader.usFid = pFile->fid;
    setHeader.infoLevel = infoLevel;

    status = MarshalData(&pCursor, &ulRemainingSpace, (PBYTE) &setHeader, sizeof(setHeader));
    BAIL_ON_NT_STATUS(status);

    pRequestData = pCursor;

    status = RdrMarshalFileInfo(
        pContext->Packet.pSMBHeader,
        &pCursor,
        &ulRemainingSpace,
        infoLevel,
        pInfo,
        ulInfoLength);
    BAIL_ON_NT_STATUS(status);

    usRequestDataCount = pCursor - pRequestData;

    pHeader->totalParameterCount = SMB_HTOL16(sizeof(setHeader));
    pHeader->totalDataCount      = SMB_HTOL16(usRequestDataCount);
    pHeader->maxParameterCount   = SMB_HTOL16(sizeof(setHeader)); /* FIXME: really? */
    pHeader->maxDataCount        = SMB_HTOL16(0);
    pHeader->maxSetupCount       = SMB_HTOL16(0);
    pHeader->flags               = SMB_HTOL16(0);
    pHeader->timeout             = SMB_HTOL16(0);
    pHeader->parameterCount      = SMB_HTOL16(sizeof(setHeader));
    pHeader->parameterOffset     = SMB_HTOL16(pRequestParameters - (PBYTE) pContext->Packet.pSMBHeader);
    pHeader->dataCount           = SMB_HTOL16(usRequestDataCount);
    pHeader->dataOffset          = SMB_HTOL16(pRequestData - (PBYTE) pContext->Packet.pSMBHeader);
    pHeader->setupCount          = SMB_HTOL8(1);

    /* Update byte count */
    status = MarshalUshort(&pByteCount, NULL, (pCursor - pByteCount) - 2);

    /* Update used length */
    pContext->Packet.bufferUsed += (pCursor - pContext->Packet.pParams);

    status = SMBPacketMarshallFooter(&pContext->Packet);
    BAIL_ON_NT_STATUS(status);

    status = RdrSocketTransceive(pFile->pTree->pSession->pSocket, pContext);
    BAIL_ON_NT_STATUS(status);

cleanup:

    return status;

error:

    goto cleanup;
}

static
BOOLEAN
RdrSetInfoComplete(
    PRDR_OP_CONTEXT pContext,
    NTSTATUS status,
    PVOID pParam
    )
{
    PSMB_PACKET pResponsePacket = pParam;

    BAIL_ON_NT_STATUS(status);

    status = pResponsePacket->pSMBHeader->error;
    BAIL_ON_NT_STATUS(status);

cleanup:

    RdrFreePacket(pResponsePacket);

    if (status != STATUS_PENDING)
    {
        pContext->pIrp->IoStatusBlock.Status = status;
        IoIrpComplete(pContext->pIrp);
        RdrFreeContext(pContext);
    }

    return FALSE;

error:

    goto cleanup;
}

static
NTSTATUS
RdrMarshalFileInfo(
    PSMB_HEADER pSmbHeader,
    PBYTE* ppCursor,
    PULONG pulRemainingSpace,
    SMB_INFO_LEVEL infoLevel,
    PVOID pInfo,
    ULONG ulInfoLength
    )
{
    NTSTATUS status = STATUS_SUCCESS;

    switch (infoLevel)
    {
    case SMB_SET_FILE_END_OF_FILE_INFO:
        status = RdrMarshalFileEndOfFileInfo(
            pSmbHeader,
            ppCursor,
            pulRemainingSpace,
            pInfo,
            ulInfoLength);
        BAIL_ON_NT_STATUS(status);
        break;
    case SMB_SET_FILE_DISPOSITION_INFO:
        status = RdrMarshalFileDispositionInfo(
            pSmbHeader,
            ppCursor,
            pulRemainingSpace,
            pInfo,
            ulInfoLength);
        BAIL_ON_NT_STATUS(status);
        break;
    case SMB_SET_FILE_RENAME_INFO:
        status = RdrMarshalFileRenameInfo(
            pSmbHeader,
            ppCursor,
            pulRemainingSpace,
            pInfo,
            ulInfoLength);
        BAIL_ON_NT_STATUS(status);
        break;
    default:
        status = STATUS_NOT_SUPPORTED;
        BAIL_ON_NT_STATUS(status);
        break;
    }

error:

    return status;
}

static
NTSTATUS
RdrMarshalFileEndOfFileInfo(
    PSMB_HEADER pSmbHeader,
    PBYTE* ppCursor,
    PULONG pulRemainingSpace,
    PVOID pInfo,
    ULONG ulInfoLength
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILE_END_OF_FILE_INFORMATION pEndInfo = pInfo;
    PBYTE pCursor = *ppCursor;
    ULONG ulRemainingSpace = *pulRemainingSpace;
    PTRANS2_FILE_END_OF_FILE_INFORMATION pEndInfoPacked = (PTRANS2_FILE_END_OF_FILE_INFORMATION) *ppCursor;

    if (ulInfoLength < sizeof(*pEndInfo))
    {
        status = STATUS_INVALID_PARAMETER;
        BAIL_ON_NT_STATUS(status);
    }

    /* Advance cursor past info structure */
    status = Advance(&pCursor, &ulRemainingSpace, sizeof(*pEndInfoPacked));
    BAIL_ON_NT_STATUS(status);

    pEndInfoPacked->EndOfFile = SMB_HTOL64(pEndInfo->EndOfFile);

    *ppCursor = pCursor;
    *pulRemainingSpace = ulRemainingSpace;

cleanup:

    return status;

error:

    goto cleanup;
}

static
NTSTATUS
RdrMarshalFileDispositionInfo(
    PSMB_HEADER pSmbHeader,
    PBYTE* ppCursor,
    PULONG pulRemainingSpace,
    PVOID pInfo,
    ULONG ulInfoLength
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PBYTE pCursor = *ppCursor;
    ULONG ulRemainingSpace = *pulRemainingSpace;
    PFILE_DISPOSITION_INFORMATION pDispInfo = pInfo;
    PTRANS2_FILE_DISPOSITION_INFORMATION pDispInfoPacked = (PTRANS2_FILE_DISPOSITION_INFORMATION) pCursor;

    if (ulInfoLength < sizeof(*pDispInfo))
    {
        status = STATUS_INVALID_PARAMETER;
        BAIL_ON_NT_STATUS(status);
    }

    /* Advance cursor past info structure */
    status = Advance(&pCursor, &ulRemainingSpace, sizeof(*pDispInfoPacked));
    BAIL_ON_NT_STATUS(status);

    pDispInfoPacked->bFileIsDeleted = pDispInfo->DeleteFile;

    *ppCursor = pCursor;
    *pulRemainingSpace = ulRemainingSpace;

cleanup:

    return status;

error:

    goto cleanup;
}

static
NTSTATUS
RdrBareFilename(
    PWSTR pwszFilename,
    ULONG ulLength,
    PWSTR* ppwszBare,
    PULONG pulBareLength
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG ulIndex = 0;
    ULONG ulStartIndex = 0;

    for (ulIndex = 0; ulIndex < ulLength; ulIndex++)
    {
        if (pwszFilename[ulIndex] == (WCHAR) '/')
        {
            ulStartIndex = ulIndex+1;
        }
    }

    *ppwszBare = pwszFilename + ulStartIndex;
    *pulBareLength = ulLength - ulStartIndex;

    return status;
}

static
NTSTATUS
RdrMarshalFileRenameInfo(
    PSMB_HEADER pSmbHeader,
    PBYTE* ppCursor,
    PULONG pulRemainingSpace,
    PVOID pInfo,
    ULONG ulInfoLength
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILE_RENAME_INFORMATION pRenameInfo = pInfo;
    PBYTE pCursor = *ppCursor;
    ULONG ulRemainingSpace = *pulRemainingSpace;
    PSMB_FILE_RENAME_INFO_HEADER pPacked = (PSMB_FILE_RENAME_INFO_HEADER) *ppCursor;
    PRDR_CCB pRoot = NULL;
    PWSTR pwszFileName = NULL;
    PBYTE pFileName = NULL;
    ULONG ulFileNameLength = 0;

    if (ulInfoLength < sizeof(*pRenameInfo) - sizeof(WCHAR) || ulInfoLength < sizeof(*pRenameInfo) - sizeof(WCHAR) + pRenameInfo->FileNameLength)
    {
        status = STATUS_INVALID_PARAMETER;
        BAIL_ON_NT_STATUS(status);
    }

    if (pRenameInfo->RootDirectory)
    {
        pRoot = IoFileGetContext(pRenameInfo->RootDirectory);
    }

    /* Advance cursor past info structure */
    status = Advance(&pCursor, &ulRemainingSpace, sizeof(*pPacked));
    BAIL_ON_NT_STATUS(status);

    pPacked->ucReplaceIfExists = pRenameInfo->ReplaceIfExists;
    memset(pPacked->ucReserved, 0, sizeof(pPacked->ucReserved));
    pPacked->ulRootDir = pRoot ? pRoot->fid : 0;

    status = RdrBareFilename(
        pRenameInfo->FileName,
        pRenameInfo->FileNameLength,
        &pwszFileName,
        &ulFileNameLength);
    BAIL_ON_NT_STATUS(status);

    pPacked->ulFileNameLength = ulFileNameLength * 2;

    status = Align((PBYTE) pSmbHeader, &pCursor, &ulRemainingSpace, sizeof(WCHAR));
    BAIL_ON_NT_STATUS(status);

    pFileName = pCursor;

    status = Advance(&pCursor, &ulRemainingSpace, LwRtlWC16StringNumChars(pwszFileName) * sizeof(WCHAR));
    BAIL_ON_NT_STATUS(status);

    SMB_HTOLWSTR(
        pFileName,
        pwszFileName,
        LwRtlWC16StringNumChars(pwszFileName));

    *ppCursor = pCursor;
    *pulRemainingSpace = ulRemainingSpace;

cleanup:

    return status;

error:

    goto cleanup;
}