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
 * Module Name:
 *
 *        structs.h
 *
 * Abstract:
 *
 *        Pvfs Driver internal structures
 *
 * Authors: Gerald Carter <gcarter@likewise.com>
 *
 */

#ifndef _PVFS_STRUCTS_H
#define _PVFS_STRUCTS_H

#define PVFS_FS_NAME     "NTFS"


/* HP-UX does not use blksize_t type for st_blksize
   (see stat(5))
 */
#if !defined(HAVE_BLKSIZE_T)
typedef long blksize_t;
#endif

typedef struct _PVFS_STAT_STRUCT
{
    mode_t       s_mode;
    ino_t        s_ino;
    dev_t        s_dev;
    dev_t        s_rdev;
    nlink_t      s_nlink;
    uid_t        s_uid;
    gid_t        s_gid;
    off_t        s_size;
    off_t        s_alloc;
    time_t       s_atime;
    time_t       s_ctime;
    time_t       s_mtime;
    time_t       s_crtime;     /* creation time */
    blksize_t    s_blksize;
    blkcnt_t     s_blocks;

} PVFS_STAT, *PPVFS_STAT;

typedef struct _PVFS_STATFS_STRUCT
{
    LONG   BlockSize;
    LONG   TotalBlocks;
    LONG   TotalFreeBlocks;
    LONG   MaximumNameLength;

} PVFS_STATFS, *PPVFS_STATFS;

typedef enum _PVFS_INFO_TYPE {
    PVFS_QUERY = 1,
    PVFS_SET
} PVFS_INFO_TYPE, *PPVFS_INFO_TYPE;

typedef struct _PVFS_DIRECTORY_ENTRY
{
    PSTR pszFilename;
    BOOLEAN bValidStat;
    PVFS_STAT Stat;

} PVFS_DIRECTORY_ENTRY, *PPVFS_DIRECTORY_ENTRY;

typedef struct _PVFS_DIRECTORY_CONTEXT
{
    DIR *pDir;
    BOOLEAN bScanned;
    DWORD dwIndex;
    DWORD dwNumEntries;
    ULONG ulAllocated;
    PPVFS_DIRECTORY_ENTRY pDirEntries;

} PVFS_DIRECTORY_CONTEXT, *PPVFS_DIRECTORY_CONTEXT;

typedef struct _PVFS_INTERLOCKED_ULONG
{
    ULONG           ulCounter;
    pthread_mutex_t CounterMutex;

} PVFS_INTERLOCKED_ULONG, *PPVFS_INTERLOCKED_ULONG;

struct _PVFS_CCB;

typedef struct _PVFS_CCB_LIST_NODE
{
    struct _PVFS_CCB_LIST_NODE  *pNext;
    struct _PVFS_CCB_LIST_NODE  *pPrevious;
    struct _PVFS_CCB            *pCcb;

} PVFS_CCB_LIST_NODE, *PPVFS_CCB_LIST_NODE;


typedef struct _PVFS_FCB
{
    LONG RefCount;
    pthread_mutex_t ControlBlock;   /* For ensuring atomic operations
                                       on an individual FCB */
    pthread_rwlock_t rwLock;        /* For managing the CCB list */
    pthread_rwlock_t rwBrlLock;     /* For managing the LockTable in the CCB list */

    PSTR pszFilename;
    LONG64 LastWriteTime;          /* Saved mode time from SET_FILE_INFO */

    PPVFS_CCB_LIST_NODE pCcbList;

} PVFS_FCB, *PPVFS_FCB;


typedef DWORD PVFS_LOCK_FLAGS;

#define PVFS_LOCK_EXCLUSIVE            0x00000001
#define PVFS_LOCK_BLOCK                0x00000002

typedef struct _PVFS_LOCK_ENTRY
{
    BOOLEAN bExclusive;
    ULONG Key;
    LONG64 Offset;
    LONG64 Length;

} PVFS_LOCK_ENTRY, *PPVFS_LOCK_ENTRY;

typedef struct _PVFS_LOCK_LIST
{
    ULONG NumberOfLocks;
    ULONG ListSize;
    PPVFS_LOCK_ENTRY pLocks;

} PVFS_LOCK_LIST, *PPVFS_LOCK_LIST;


typedef struct _PVFS_LOCK_TABLE
{
    pthread_rwlock_t rwLock;

    PVFS_LOCK_LIST ExclusiveLocks;
    PVFS_LOCK_LIST SharedLocks;

} PVFS_LOCK_TABLE, *PPVFS_LOCK_TABLE;

typedef struct _PVFS_CCB
{
    pthread_mutex_t FileMutex;      /* Use for fd buffer operations */
    pthread_mutex_t ControlMutex;   /* Use for CCB SetFileInfo operations */

    LONG RefCount;

    /* Open fd to the File or Directory */
    int fd;
    dev_t device;
    ino_t inode;

    /* Pointer to the shared PVFS FileHandle */
    PPVFS_FCB pFcb;

    /* Save parameters from the CreateFile() */
    PSTR pszFilename;
    FILE_CREATE_OPTIONS CreateOptions;
    FILE_SHARE_FLAGS ShareFlags;
    ACCESS_MASK AccessGranted;

    PACCESS_TOKEN pUserToken;

    /* Handle for Directory enumeraqtion */
    PPVFS_DIRECTORY_CONTEXT pDirContext;

    PVFS_LOCK_TABLE LockTable;

} PVFS_CCB, *PPVFS_CCB;

typedef struct _PVFS_IRP_CONTEXT
{
    pthread_mutex_t Mutex;
    pthread_cond_t  Event;    /* synchronize point for worker threads */

    BOOL bFinished;
    NTSTATUS ntError;

    PIRP pIrp;

} PVFS_IRP_CONTEXT, *PPVFS_IRP_CONTEXT;


/* Used for Query/Set level handlers */

struct _InfoLevelDispatchEntry {
    FILE_INFORMATION_CLASS Level;
    NTSTATUS (*fn)(PVFS_INFO_TYPE RequestType,
                   PPVFS_IRP_CONTEXT pIrpContext);
};

#endif    /* _PVFS_STRUCTS_H */


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
