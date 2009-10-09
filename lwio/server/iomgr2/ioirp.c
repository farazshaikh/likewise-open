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

#include "iop.h"

typedef ULONG IRP_FLAGS;

#define IRP_FLAG_PENDING                    0x00000001
#define IRP_FLAG_COMPLETE                   0x00000002
#define IRP_FLAG_CANCEL_PENDING             0x00000004
#define IRP_FLAG_CANCELLED                  0x00000008
#define IRP_FLAG_DISPATCHED                 0x00000010

typedef struct _IRP_INTERNAL {
    IRP Irp;
    LONG ReferenceCount;
    IRP_FLAGS Flags;
    LW_LIST_LINKS FileObjectLinks;
    LW_LIST_LINKS CancelLinks;
    struct {
        PIO_IRP_CALLBACK Callback;
        PVOID CallbackContext;
    } Cancel;
    struct {
        BOOLEAN IsAsyncCall;
        union {
            struct {
                PIO_ASYNC_COMPLETE_CALLBACK Callback;
                PVOID CallbackContext;
                PIO_STATUS_BLOCK pIoStatusBlock;
                PIO_FILE_HANDLE pCreateFileHandle;
            } Async;
            struct {
                PLW_RTL_EVENT Event;
            } Sync;
        };
    } Completion;
} IRP_INTERNAL, *PIRP_INTERNAL;

// TODO -- make inline -- just want type-safe macro.
PIRP_INTERNAL
IopIrpGetInternal(
    IN PIRP pIrp
    )
{
    return (PIRP_INTERNAL) pIrp;
}

PIO_ASYNC_CANCEL_CONTEXT
IopIrpGetAsyncCancelContextFromIrp(
    IN PIRP Irp
    )
{
    return (PIO_ASYNC_CANCEL_CONTEXT) Irp;
}

PIRP
IopIrpGetIrpFromAsyncCancelContext(
    IN PIO_ASYNC_CANCEL_CONTEXT Context
    )
{
    return (PIRP) Context;
}

VOID
IopIrpAcquireCancelLock(
    IN PIRP pIrp
    )
{
    LwRtlLockMutex(&pIrp->FileHandle->pDevice->CancelMutex);
}

VOID
IopIrpReleaseCancelLock(
    IN PIRP pIrp
    )
{
    LwRtlUnlockMutex(&pIrp->FileHandle->pDevice->CancelMutex);
}

NTSTATUS
IopIrpCreate(
    OUT PIRP* ppIrp,
    IN IRP_TYPE Type,
    IN PIO_FILE_OBJECT pFileObject
    )
{
    NTSTATUS status = 0;
    int EE = 0;
    PIRP pIrp = NULL;
    PIRP_INTERNAL irpInternal = NULL;

    // Note that we allocate enough space for the internal fields.
    status = IO_ALLOCATE(&pIrp, IRP, sizeof(IRP_INTERNAL));
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    irpInternal = IopIrpGetInternal(pIrp);
    irpInternal->ReferenceCount = 1;

    pIrp->Type = Type;
    pIrp->FileHandle = pFileObject;
    IopFileObjectReference(pFileObject);
    pIrp->DeviceHandle = pFileObject->pDevice;
    pIrp->DriverHandle = pFileObject->pDevice->Driver;

    IopFileObjectLock(pFileObject);
    // TODO-Add FILE_OBJECT_FLAG_CLOSED
    if ((Type != IRP_TYPE_CLOSE) && IsSetFlag(pFileObject->Flags, FILE_OBJECT_FLAG_RUNDOWN))
    {
        status = STATUS_CANCELLED;
    }
    else
    {
        LwListInsertTail(&pFileObject->IrpList,
                         &irpInternal->FileObjectLinks);
    }
    IopFileObjectUnlock(pFileObject);

    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

cleanup:
    if (status)
    {
        IopIrpDereference(&pIrp);
    }

    *ppIrp = pIrp;

    IO_LOG_LEAVE_ON_STATUS_EE(status, EE);
    return status;
}

static
VOID
IopIrpFree(
    IN OUT PIRP* ppIrp
    )
{
    PIRP pIrp = *ppIrp;

    if (pIrp)
    {
        PIRP_INTERNAL irpInternal = IopIrpGetInternal(pIrp);

        LWIO_ASSERT(0 == irpInternal->ReferenceCount);
        LWIO_ASSERT(STATUS_PENDING != pIrp->IoStatusBlock.Status);

        switch (pIrp->Type)
        {
            case IRP_TYPE_CREATE:
            case IRP_TYPE_CREATE_NAMED_PIPE:
                IoSecurityDereferenceSecurityContext(&pIrp->Args.Create.SecurityContext);
                RtlWC16StringFree(&pIrp->Args.Create.FileName.FileName);
                break;
            case IRP_TYPE_QUERY_DIRECTORY:
                if (pIrp->Args.QueryDirectory.FileSpec)
                {
                    LwRtlUnicodeStringFree(&pIrp->Args.QueryDirectory.FileSpec->Pattern);
                    IO_FREE(&pIrp->Args.QueryDirectory.FileSpec);
                }
                break;
            default:
                break;
        }

        // Note that the parent (FO) lock is already held
        // by IopIrpDereference().

        // Might not be in the list if IRP creation failed.
        if (irpInternal->FileObjectLinks.Next)
        {
            LwListRemove(&irpInternal->FileObjectLinks);
        }
        IopFileObjectDereference(&pIrp->FileHandle);

        IoMemoryFree(pIrp);
        *ppIrp = NULL;
    }
}

VOID
IoIrpMarkPending(
    IN PIRP pIrp,
    IN PIO_IRP_CALLBACK CancelCallback,
    IN OPTIONAL PVOID CancelCallbackContext
    )
{
    PIRP_INTERNAL irpInternal = IopIrpGetInternal(pIrp);

    LWIO_ASSERT(CancelCallback);

    IopIrpAcquireCancelLock(pIrp);

    LWIO_ASSERT(!irpInternal->Cancel.Callback);
    LWIO_ASSERT(!IsSetFlag(irpInternal->Flags, IRP_FLAG_PENDING));
    LWIO_ASSERT(!IsSetFlag(irpInternal->Flags, IRP_FLAG_COMPLETE));
    LWIO_ASSERT(!IsSetFlag(irpInternal->Flags, IRP_FLAG_CANCELLED));

    SetFlag(irpInternal->Flags, IRP_FLAG_PENDING);
    irpInternal->Cancel.Callback = CancelCallback;
    irpInternal->Cancel.CallbackContext = CancelCallbackContext;

    IopIrpReleaseCancelLock(pIrp);

    //
    // Take a reference that will be released by IoIrpComplete.
    //

    IopIrpReference(pIrp);
}

static
VOID
IopIrpCompleteInternal(
    IN OUT PIRP pIrp,
    IN BOOLEAN IsAsyncCompletion
    )
{
    PIRP_INTERNAL irpInternal = IopIrpGetInternal(pIrp);

    IopIrpAcquireCancelLock(pIrp);

    LWIO_ASSERT_MSG(IS_BOTH_OR_NEITHER(IsAsyncCompletion, IsSetFlag(irpInternal->Flags, IRP_FLAG_PENDING)), "IRP pending state is inconsistent.");
    LWIO_ASSERT_MSG(IsSetFlag(irpInternal->Flags, IRP_FLAG_DISPATCHED), "IRP cannot be completed unless it was properly dispatched.");
    LWIO_ASSERT_MSG(!IsSetFlag(irpInternal->Flags, IRP_FLAG_COMPLETE), "IRP cannot be completed more than once.");

    //
    // Note that the IRP may be CANCEL_PENDING or CANCELLED, but that
    // is ok.  In fact, completion may have been called in response
    // to cancellation.
    //

    SetFlag(irpInternal->Flags, IRP_FLAG_COMPLETE);

    IopIrpReleaseCancelLock(pIrp);

    IopFileObjectRemoveDispatched(pIrp->FileHandle, pIrp->Type);

    LWIO_ASSERT(NT_SUCCESS_OR_NOT(pIrp->IoStatusBlock.Status));

    if (STATUS_SUCCESS == pIrp->IoStatusBlock.Status)
    {
        // Handle special success processing having to do with file handle.
        switch (pIrp->Type)
        {
            case IRP_TYPE_CREATE:
            case IRP_TYPE_CREATE_NAMED_PIPE:
                // ISSUE-May not need lock since it should be only reference
                IopFileObjectLock(pIrp->FileHandle);
                SetFlag(pIrp->FileHandle->Flags, FILE_OBJECT_FLAG_CREATE_DONE);
                IopFileObjectUnlock(pIrp->FileHandle);

                IopFileObjectReference(pIrp->FileHandle);
                if (IsAsyncCompletion && irpInternal->Completion.IsAsyncCall)
                {
                    *irpInternal->Completion.Async.pCreateFileHandle = pIrp->FileHandle;
                }
                break;

            case IRP_TYPE_CLOSE:
            {
                PIO_FILE_OBJECT pFileObject = NULL;

                SetFlag(pIrp->FileHandle->Flags, FILE_OBJECT_FLAG_CLOSE_DONE);

                // Note that we must delete the reference from the create
                // w/o removing the file object value from the IRP (which
                // will be removed when the IRP is freed).

                pFileObject = pIrp->FileHandle;
                IopFileObjectDereference(&pFileObject);

                break;
            }
        }
    }
    else if (IRP_TYPE_CLOSE == pIrp->Type)
    {
        LWIO_LOG_ERROR("Unable to close file object, status = 0x%08x",
                       pIrp->IoStatusBlock.Status);
    }

    if (IsAsyncCompletion)
    {
        if (irpInternal->Completion.IsAsyncCall)
        {
            *irpInternal->Completion.Async.pIoStatusBlock = pIrp->IoStatusBlock;
            irpInternal->Completion.Async.Callback(
                    irpInternal->Completion.Async.CallbackContext);
        }
        else
        {
            LwRtlSetEvent(irpInternal->Completion.Sync.Event);
        }

        //
        // Release reference from IoIrpMarkPending().
        //

        IopIrpDereference(&pIrp);
    }
}

// must have set IO status block in IRP.
VOID
IoIrpComplete(
    IN OUT PIRP pIrp
    )
{
    IopIrpCompleteInternal(pIrp, TRUE);
}

VOID
IopIrpReference(
    IN PIRP pIrp
    )
{
    PIRP_INTERNAL irpInternal = IopIrpGetInternal(pIrp);
    InterlockedIncrement(&irpInternal->ReferenceCount);
}

VOID
IopIrpDereference(
    IN OUT PIRP* ppIrp
    )
{
    PIRP pIrp = *ppIrp;
    if (pIrp)
    {
        PIRP_INTERNAL irpInternal = IopIrpGetInternal(pIrp);
        LONG count = 0;
        PIO_FILE_OBJECT pFileObject = pIrp->FileHandle;

        if (pFileObject)
        {
            // Take a reference in case we free the IRP.
            IopFileObjectReference(pFileObject);
            // Lock since we may free and manipulate the FO IRP list.
            IopFileObjectLock(pFileObject);
        }

        count = InterlockedDecrement(&irpInternal->ReferenceCount);
        LWIO_ASSERT(count >= 0);
        if (0 == count)
        {
            IopIrpFree(ppIrp);
        }

        // Remove our reference.
        if (pFileObject)
        {
            IopFileObjectUnlock(pFileObject);
            IopFileObjectDereference(&pFileObject);
        }
        *ppIrp = NULL;
    }
}

BOOLEAN
IopIrpCancel(
    IN PIRP pIrp
    )
{
    PIRP_INTERNAL irpInternal = IopIrpGetInternal(pIrp);
    BOOLEAN isCancellable = FALSE;
    BOOLEAN isAcquired = FALSE;

    if (!pIrp)
    {
        GOTO_CLEANUP();
    }

    IopIrpReference(pIrp);

    IopIrpAcquireCancelLock(pIrp);
    isAcquired = TRUE;

    if (!IsSetFlag(irpInternal->Flags, IRP_FLAG_CANCELLED | IRP_FLAG_COMPLETE))
    {
        if (irpInternal->Cancel.Callback)
        {
            ClearFlag(irpInternal->Flags, IRP_FLAG_CANCEL_PENDING);
            SetFlag(irpInternal->Flags, IRP_FLAG_CANCELLED);
            isCancellable = TRUE;
            irpInternal->Cancel.Callback(
                    pIrp,
                    irpInternal->Cancel.CallbackContext);
        }
        else
        {
            SetFlag(irpInternal->Flags, IRP_FLAG_CANCEL_PENDING);
        }
    }
    else
    {
        // If already cancelled or complete, we consider it as cancellable.
        isCancellable = TRUE;
    }

cleanup:

    if (isAcquired)
    {
        IopIrpReleaseCancelLock(pIrp);
    }

    if (pIrp)
    {
        IopIrpDereference(&pIrp);
    }

    return isCancellable;
}

NTSTATUS
IopIrpDispatch(
    IN PIRP pIrp,
    IN OUT OPTIONAL PIO_ASYNC_CONTROL_BLOCK AsyncControlBlock,
    IN PIO_STATUS_BLOCK pIoStatusBlock,
    IN OPTIONAL PIO_FILE_HANDLE pCreateFileHandle
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    int EE = 0;
    BOOLEAN isAsyncCall = FALSE;
    LW_RTL_EVENT event = LW_RTL_EVENT_ZERO_INITIALIZER;
    PIRP pExtraIrpReference = NULL;
    PIRP_INTERNAL irpInternal = IopIrpGetInternal(pIrp);
    BOOLEAN needCancel = FALSE;

    LWIO_ASSERT(pIoStatusBlock);
    LWIO_ASSERT(IS_BOTH_OR_NEITHER(pCreateFileHandle,
                                   (IRP_TYPE_CREATE == pIrp->Type) ||
                                   (IRP_TYPE_CREATE_NAMED_PIPE == pIrp->Type)));

    isAsyncCall = AsyncControlBlock ? TRUE : FALSE;

    irpInternal->Completion.IsAsyncCall = isAsyncCall;

    if (isAsyncCall)
    {
        LWIO_ASSERT(!AsyncControlBlock->AsyncCancelContext);
        LWIO_ASSERT(AsyncControlBlock->Callback);

        irpInternal->Completion.Async.Callback = AsyncControlBlock->Callback;
        irpInternal->Completion.Async.CallbackContext = AsyncControlBlock->CallbackContext;
        irpInternal->Completion.Async.pIoStatusBlock = pIoStatusBlock;
        irpInternal->Completion.Async.pCreateFileHandle = pCreateFileHandle;

        // Reference IRP since we may need to return an an async cancel context.
        IopIrpReference(pIrp);
        pExtraIrpReference = pIrp;
    }
    else
    {
        status = LwRtlInitializeEvent(&event);
        GOTO_CLEANUP_ON_STATUS_EE(status, EE);

        irpInternal->Completion.Sync.Event = &event;
    }

    // We have to dispatch once we add the IRP as "dipatched"
    // and we have to call IopIrpCompleteInternal() so that
    // it gets subtracted.

    status = IopFileObjectAddDispatched(pIrp->FileHandle, pIrp->Type);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    SetFlag(irpInternal->Flags, IRP_FLAG_DISPATCHED);

    status = IopDeviceCallDriver(pIrp->DeviceHandle, pIrp);
    // Handle synchronous completion
    if (STATUS_PENDING != status)
    {
        IopIrpCompleteInternal(pIrp, FALSE);
    }
    // Handle asynchronous dispatch
    else
    {
        IopIrpAcquireCancelLock(pIrp);

        LWIO_ASSERT(IsSetFlag(irpInternal->Flags, IRP_FLAG_PENDING));
        LWIO_ASSERT(irpInternal->Cancel.Callback);

        needCancel = IsSetFlag(irpInternal->Flags, IRP_FLAG_CANCEL_PENDING);

        IopIrpReleaseCancelLock(pIrp);

        if (needCancel)
        {
            IopIrpCancel(pIrp);
        }

        // Handle waiting for asynchronous completion for synchronous caller
        if (!isAsyncCall)
        {
            LwRtlWaitEvent(&event, NULL);

            LWIO_ASSERT(pIrp->IoStatusBlock.Status != STATUS_PENDING);
            status = pIrp->IoStatusBlock.Status;
        }
    }

    //
    // At this point, we are either complete or this is
    // an async call that returned STATUS_PENDING.
    //

    LWIO_ASSERT((STATUS_PENDING == status) || (pIrp->IoStatusBlock.Status == status));

    if (STATUS_PENDING == status)
    {
        LWIO_ASSERT(isAsyncCall);

        AsyncControlBlock->AsyncCancelContext = IopIrpGetAsyncCancelContextFromIrp(pIrp);
    }
    else
    {
        if (isAsyncCall)
        {
            //
            // Remove async cancel context reference added earlier since we
            // are returning synchronously w/o an async cancel context.
            //

            IopIrpDereference(&pExtraIrpReference);
        }
    }

cleanup:
    if (STATUS_PENDING != status)
    {
        pIrp->IoStatusBlock.Status = status;
        *pIoStatusBlock = pIrp->IoStatusBlock;
    }

    LwRtlCleanupEvent(&event);

    LWIO_ASSERT(IS_BOTH_OR_NEITHER(pExtraIrpReference, (STATUS_PENDING == status)));
    LWIO_ASSERT((STATUS_PENDING != status) || isAsyncCall);
    LWIO_ASSERT(NT_PENDING_OR_SUCCESS_OR_NOT(status));
    return status;
}

VOID
IopIrpCancelFileObject(
    IN PIO_FILE_OBJECT pFileObject
    )
{
    PLW_LIST_LINKS pLinks = NULL;
    PIRP_INTERNAL irpInternal = NULL;
    LW_LIST_LINKS cancelList = { 0 };
    PIRP pIrp = NULL;

    LwListInit(&cancelList);

    // Gather IRPs we want to cancel while holding FO lock.
    IopFileObjectLock(pFileObject);
    // gather list of IRPs
    for (pLinks = pFileObject->IrpList.Next;
         pLinks != &pFileObject->IrpList;
         pLinks = pLinks->Next)
    {
        irpInternal = LW_STRUCT_FROM_FIELD(pLinks, IRP_INTERNAL, FileObjectLinks);

        LWIO_ASSERT(irpInternal->Irp.FileHandle == pFileObject);

        // Verify that this IRP is not already being cancelled.
        if (!irpInternal->CancelLinks.Next)
        {
            IopIrpReference(&irpInternal->Irp);
            LwListInsertTail(&cancelList, &irpInternal->CancelLinks);
        }
    }
    IopFileObjectUnlock(pFileObject);

    // Iterate over list, calling IopIrpCancel as appropriate.
    while (!LwListIsEmpty(&cancelList))
    {
        pLinks = LwListRemoveHead(&cancelList);
        irpInternal = LW_STRUCT_FROM_FIELD(pLinks, IRP_INTERNAL, CancelLinks);
        pIrp = &irpInternal->Irp;

        IopIrpCancel(pIrp);
        IopIrpDereference(&pIrp);
    }
}
