/*
 * BootMaster/driver_support.c
 * Functions related to drivers. Original copyright notices below.
 */
/*
 * Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
 * This program and the accompanying materials are licensed and made available under
 * the terms and conditions of the BSD License that accompanies this distribution.
 * The full text of the license may be found at
 * http://opensource.org/licenses/bsd-license.php.
 *
 * THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 *
 */
/*
 * Copyright (c) 2006-2010 Christoph Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Modifications copyright (c) 2012-2017 Roderick W. Smith
 *
 * Modifications distributed under the terms of the GNU General Public
 * License (GPL) version 3 (GPLv3), or (at your option) any later version.
 *
 */
/*
 * Modified for RefindPlus
 * Copyright (c) 2020-2021 Dayo Akanji (sf.net/u/dakanji/profile)
 *
 * Modifications distributed under the preceding terms.
 */

#include "driver_support.h"
#include "lib.h"
#include "mystrings.h"
#include "screenmgt.h"
#include "launch_efi.h"
#include "../include/refit_call_wrapper.h"

#if defined (EFIX64)
    #define DRIVER_DIRS             L"drivers,drivers_x64"
#elif defined (EFI32)
    #define DRIVER_DIRS             L"drivers,drivers_ia32"
#elif defined (EFIAARCH64)
    #define DRIVER_DIRS             L"drivers,drivers_aa64"
#else
    #define DRIVER_DIRS             L"drivers"
#endif

// Following "global" constants are from EDK2's AutoGen.c.
EFI_GUID gMyEfiLoadedImageProtocolGuid = {
    0x5B1B31A1,
    0x9562,
    0x11D2,
    { 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B }
};
EFI_GUID gMyEfiDriverBindingProtocolGuid = {
    0x18A031AB,
    0xB443,
    0x4D1A,
    { 0xA5, 0xC0, 0x0C, 0x09, 0x26, 0x1E, 0x9F, 0x71 }
};
EFI_GUID gMyEfiDriverConfigurationProtocolGuid = {
    0x107A772B,
    0xD5E1,
    0x11D4,
    { 0x9A, 0x46, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }
};
EFI_GUID gMyEfiDriverDiagnosticsProtocolGuid = {
    0x0784924F,
    0xE296,
    0x11D4,
    { 0x9A, 0x49, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }
};
EFI_GUID gMyEfiComponentNameProtocolGuid = {
    0x107A772C,
    0xD5E1,
    0x11D4,
    { 0x9A, 0x46, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }
};
EFI_GUID gMyEfiDevicePathProtocolGuid = {
    0x09576E91,
    0x6D3F,
    0x11D2,
    { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B }
};
EFI_GUID gMyEfiDiskIoProtocolGuid = {
    0xCE345171,
    0xBA0B,
    0x11D2,
    { 0x8E, 0x4F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B }
};
EFI_GUID gMyEfiBlockIoProtocolGuid = {
    0x964E5B21,
    0x6459,
    0x11D2,
    { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B }
};
EFI_GUID gMyEfiSimpleFileSystemProtocolGuid = {
    0x964E5B22,
    0x6459,
    0x11D2,
    { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B }
};

#ifdef __MAKEWITH_GNUEFI
struct MY_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
struct MY_EFI_FILE_PROTOCOL;

typedef
EFI_STATUS (EFIAPI *MY_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME) (
    IN struct MY_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *This,
    OUT struct MY_EFI_FILE_PROTOCOL                **Root
);

typedef struct _MY_MY_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64                                         Revision;
    MY_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
} MY_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef struct _MY_EFI_FILE_PROTOCOL {
    UINT64                Revision;
    EFI_FILE_OPEN         Open;
    EFI_FILE_CLOSE        Close;
    EFI_FILE_DELETE       Delete;
    EFI_FILE_READ         Read;
    EFI_FILE_WRITE        Write;
    EFI_FILE_GET_POSITION GetPosition;
    EFI_FILE_SET_POSITION SetPosition;
    EFI_FILE_GET_INFO     GetInfo;
    EFI_FILE_SET_INFO     SetInfo;
    EFI_FILE_FLUSH        Flush;
} MY_EFI_FILE_PROTOCOL;

typedef struct _MY_EFI_BLOCK_IO_PROTOCOL {
    UINT64              Revision;
    EFI_BLOCK_IO_MEDIA *Media;
    EFI_BLOCK_RESET     Reset;
    EFI_BLOCK_READ      ReadBlocks;
    EFI_BLOCK_WRITE     WriteBlocks;
    EFI_BLOCK_FLUSH     FlushBlocks;
} MY_EFI_BLOCK_IO_PROTOCOL;
#else /* Make with Tianocore */
#define MY_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
#define MY_EFI_FILE_PROTOCOL               EFI_FILE_PROTOCOL
#define MY_EFI_BLOCK_IO_PROTOCOL           EFI_BLOCK_IO_PROTOCOL
#endif

/* LibScanHandleDatabase() is used by RefindPlus' driver-loading code (inherited
 * from rEFIt), but has not been implemented in GNU-EFI and seems to have been
 * dropped from current versions of the Tianocore library. This function was taken from
 * http://git.etherboot.org/?p=mirror/efi/shell/.git;a=commitdiff;h=b1b0c63423cac54dc964c2930e04aebb46a946ec,
 * The original files are copyright 2006-2011 Intel and BSD-licensed. Minor
 * modifications by Roderick Smith are GPLv3.
 */
EFI_STATUS LibScanHandleDatabase (
    EFI_HANDLE             DriverBindingHandle,
    OPTIONAL UINT32       *DriverBindingHandleIndex,
    OPTIONAL EFI_HANDLE    ControllerHandle,
    OPTIONAL UINT32       *ControllerHandleIndex,
    OPTIONAL UINTN        *HandleCount,
    EFI_HANDLE           **HandleBuffer,
    UINT32               **HandleType
) {
    EFI_STATUS                             Status;
    UINTN                                  HandleIndex;
    EFI_GUID                             **ProtocolGuidArray;
    UINTN                                  ArrayCount;
    UINTN                                  ProtocolIndex;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY   *OpenInfo;
    UINTN                                  OpenInfoCount;
    UINTN                                  OpenInfoIndex;
    UINTN                                  ChildIndex;
    BOOLEAN                                DriverBindingHandleIndexValid;

    DriverBindingHandleIndexValid = FALSE;
    if (DriverBindingHandleIndex != NULL) {
        *DriverBindingHandleIndex = 0xffffffff;
    }

    if (ControllerHandleIndex != NULL) {
        *ControllerHandleIndex = 0xffffffff;
    }

    *HandleCount  = 0;
    *HandleBuffer = NULL;
    *HandleType   = NULL;

    //
    // Retrieve the list of all handles from the handle database
    //

    Status = REFIT_CALL_5_WRAPPER(
        gBS->LocateHandleBuffer,
        AllHandles,
        NULL, NULL,
        HandleCount,
        HandleBuffer
    );
    if (EFI_ERROR(Status)) {
        goto Error;
    }

    *HandleType = AllocatePool (*HandleCount * sizeof (UINT32));
    if (*HandleType == NULL) {
        goto Error;
    }

    for (HandleIndex = 0; HandleIndex < *HandleCount; HandleIndex++) {
        //
        // Assume that the handle type is unknown
        //
        (*HandleType)[HandleIndex] = EFI_HANDLE_TYPE_UNKNOWN;

        if (DriverBindingHandle != NULL &&
            DriverBindingHandleIndex != NULL &&
            (*HandleBuffer)[HandleIndex] == DriverBindingHandle
        ) {
            *DriverBindingHandleIndex     = (UINT32) HandleIndex;
            DriverBindingHandleIndexValid = TRUE;
        }

        if (ControllerHandle != NULL &&
            ControllerHandleIndex != NULL &&
            (*HandleBuffer)[HandleIndex] == ControllerHandle
        ) {
            *ControllerHandleIndex = (UINT32) HandleIndex;
        }
    }

    for (HandleIndex = 0; HandleIndex < *HandleCount; HandleIndex++) {
        //
        // Retrieve the list of all the protocols on each handle
        //
        Status = REFIT_CALL_3_WRAPPER(
            gBS->ProtocolsPerHandle,
            (*HandleBuffer)[HandleIndex],
            &ProtocolGuidArray,
            &ArrayCount
        );

        if (!EFI_ERROR(Status)) {
            for (ProtocolIndex = 0; ProtocolIndex < ArrayCount; ProtocolIndex++) {
                if (CompareGuid (
                    ProtocolGuidArray[ProtocolIndex], &gMyEfiLoadedImageProtocolGuid
                ) == 0) {
                    (*HandleType)[HandleIndex] |= EFI_HANDLE_TYPE_IMAGE_HANDLE;
                }
                if (CompareGuid (
                    ProtocolGuidArray[ProtocolIndex], &gMyEfiDriverBindingProtocolGuid
                ) == 0) {
                    (*HandleType)[HandleIndex] |= EFI_HANDLE_TYPE_DRIVER_BINDING_HANDLE;
                }
                if (CompareGuid (
                    ProtocolGuidArray[ProtocolIndex], &gMyEfiDriverConfigurationProtocolGuid
                ) == 0) {
                    (*HandleType)[HandleIndex] |= EFI_HANDLE_TYPE_DRIVER_CONFIGURATION_HANDLE;
                }
                if (CompareGuid (
                    ProtocolGuidArray[ProtocolIndex], &gMyEfiDriverDiagnosticsProtocolGuid
                ) == 0) {
                    (*HandleType)[HandleIndex] |= EFI_HANDLE_TYPE_DRIVER_DIAGNOSTICS_HANDLE;
                }
                if (CompareGuid (
                    ProtocolGuidArray[ProtocolIndex], &gMyEfiComponentNameProtocolGuid
                ) == 0) {
                    (*HandleType)[HandleIndex] |= EFI_HANDLE_TYPE_COMPONENT_NAME_HANDLE;
                }
                if (CompareGuid (
                    ProtocolGuidArray[ProtocolIndex], &gMyEfiDevicePathProtocolGuid
                ) == 0) {
                    (*HandleType)[HandleIndex] |= EFI_HANDLE_TYPE_DEVICE_HANDLE;
                }

                //
                // Retrieve the list of agents that have opened each protocol
                //
                Status = REFIT_CALL_4_WRAPPER(
                    gBS->OpenProtocolInformation,
                    (*HandleBuffer)[HandleIndex],
                    ProtocolGuidArray[ProtocolIndex],
                    &OpenInfo, &OpenInfoCount
                );

                if (!EFI_ERROR(Status)) {
                    for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++) {
                        if (DriverBindingHandle != NULL &&
                            OpenInfo[OpenInfoIndex].AgentHandle == DriverBindingHandle
                        ) {
                            if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_DRIVER)
                                == EFI_OPEN_PROTOCOL_BY_DRIVER
                            ) {
                                //
                                // Mark the device handle as being managed by the driver
                                // specified by DriverBindingHandle
                                //
                                (*HandleType)[HandleIndex] |=
                                (EFI_HANDLE_TYPE_DEVICE_HANDLE | EFI_HANDLE_TYPE_CONTROLLER_HANDLE);
                                //
                                // Mark the DriverBindingHandle as being a driver that is
                                // managing at least one controller
                                //
                                if (DriverBindingHandleIndexValid) {
                                    (*HandleType)[*DriverBindingHandleIndex] |= EFI_HANDLE_TYPE_DEVICE_DRIVER;
                                }
                            }

                            if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)
                                == EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                            ) {
                                //
                                // Mark the DriverBindingHandle as being a driver that is
                                // managing at least one child controller
                                //
                                if (DriverBindingHandleIndexValid) {
                                    (*HandleType)[*DriverBindingHandleIndex] |= EFI_HANDLE_TYPE_BUS_DRIVER;
                                }
                            }

                            if (ControllerHandle != NULL
                                && (*HandleBuffer)[HandleIndex] == ControllerHandle
                            ) {
                                if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)
                                    == EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                                ) {
                                    for (ChildIndex = 0; ChildIndex < *HandleCount; ChildIndex++) {
                                        if (
                                            (*HandleBuffer)[ChildIndex] ==
                                            OpenInfo[OpenInfoIndex].ControllerHandle
                                        ) {
                                            (*HandleType)[ChildIndex] |=
                                            (EFI_HANDLE_TYPE_DEVICE_HANDLE | EFI_HANDLE_TYPE_CHILD_HANDLE);
                                        }
                                    }
                                }
                            }
                        }

                        if (DriverBindingHandle == NULL &&
                            OpenInfo[OpenInfoIndex].ControllerHandle == ControllerHandle
                        ) {
                            if ((OpenInfo[OpenInfoIndex].Attributes &
                                EFI_OPEN_PROTOCOL_BY_DRIVER) == EFI_OPEN_PROTOCOL_BY_DRIVER
                            ) {
                                for (ChildIndex = 0; ChildIndex < *HandleCount; ChildIndex++) {
                                    if ((*HandleBuffer)[ChildIndex] == OpenInfo[OpenInfoIndex].AgentHandle) {
                                        (*HandleType)[ChildIndex] |= EFI_HANDLE_TYPE_DEVICE_DRIVER;
                                    }
                                }
                            }

                            if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)
                                == EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                            ) {
                                (*HandleType)[HandleIndex] |= EFI_HANDLE_TYPE_PARENT_HANDLE;
                                for (ChildIndex = 0; ChildIndex < *HandleCount; ChildIndex++) {
                                    if ((*HandleBuffer)[ChildIndex] == OpenInfo[OpenInfoIndex].AgentHandle) {
                                        (*HandleType)[ChildIndex] |= EFI_HANDLE_TYPE_BUS_DRIVER;
                                    }
                                }
                            }
                        }
                    }

                    MY_FREE_POOL(OpenInfo);
                }
            }
            MY_FREE_POOL(ProtocolGuidArray);
        }
    }

    return EFI_SUCCESS;

Error:
    MY_FREE_POOL(*HandleType);
    MY_FREE_POOL(*HandleBuffer);

    *HandleCount  = 0;
    *HandleBuffer = NULL;
    *HandleType   = NULL;

    return Status;
} // EFI_STATUS LibScanHandleDatabase()

/* Modified from EDK2 function of a similar name; original copyright Intel &
 * BSD-licensed; modifications by Roderick Smith are GPLv3.
 */
EFI_STATUS ConnectAllDriversToAllControllers (
    IN BOOLEAN ResetGOP
) {
#ifndef __MAKEWITH_GNUEFI
    BdsLibConnectAllDriversToAllControllers (ResetGOP);
    return 0;
#else
    EFI_STATUS   Status;
    UINTN        AllHandleCount;
    EFI_HANDLE  *AllHandleBuffer;
    UINTN        Index;
    UINTN        HandleCount;
    EFI_HANDLE  *HandleBuffer;
    UINT32      *HandleType;
    UINTN        HandleIndex;
    BOOLEAN      Parent;
    BOOLEAN      Device;

    Status = LibLocateHandle (
        AllHandles,
        NULL, NULL,
        &AllHandleCount,
        &AllHandleBuffer
    );

    if (EFI_ERROR(Status)) {
        return Status;
    }

    for (Index = 0; Index < AllHandleCount; Index++) {
        // Scan the handle database
        Status = LibScanHandleDatabase (
            NULL, NULL,
            AllHandleBuffer[Index], NULL,
            &HandleCount,
            &HandleBuffer,
            &HandleType
        );

        if (EFI_ERROR(Status)) {
            break;
        }

        Device = TRUE;
        if (HandleType[Index] & EFI_HANDLE_TYPE_DRIVER_BINDING_HANDLE
            || HandleType[Index] & EFI_HANDLE_TYPE_IMAGE_HANDLE
        ) {
            Device = FALSE;
        }

        // Dummy ... just to use passed parameter which is only needed for TianoCore
        Parent = ResetGOP;
        if (Device) {
            Parent = FALSE;
            for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
                if (HandleType[HandleIndex] & EFI_HANDLE_TYPE_PARENT_HANDLE) {
                    Parent = TRUE;
                }
            } // for

            if (!Parent) {
                if (HandleType[Index] & EFI_HANDLE_TYPE_DEVICE_HANDLE) {
                   Status = REFIT_CALL_4_WRAPPER(
                       gBS->ConnectController,
                       AllHandleBuffer[Index],
                       NULL, NULL, TRUE
                   );
               }
            }
        }

        MY_FREE_POOL(HandleBuffer);
        MY_FREE_POOL(HandleType);
    }
    MY_FREE_POOL(AllHandleBuffer);

    return Status;
#endif
} // EFI_STATUS ConnectAllDriversToAllControllers()

/*
 * ConnectFilesystemDriver() is modified from DisconnectInvalidDiskIoChildDrivers()
 * in Clover (https://sourceforge.net/projects/cloverefiboot/), which is derived
 * from rEFIt. The refit/main.c file from which this function was taken continues
 * to bear rEFIt's original copyright/licence notice (BSD); modifications by
 * Roderick Smith (2016) are GPLv3.
 */
/**
 * Some UEFI's (like HPQ EFI from HP notebooks) have DiskIo protocols
 * opened BY_DRIVER (by Partition driver in HP case) even when no file system
 * is produced from this DiskIo. This then blocks our FS drivers from connecting
 * and producing file systems.
 * To fix it: we will disconnect drivers that connected to DiskIo BY_DRIVER
 * if this is partition volume and if those drivers did not produce file system,
 * then try to connect every unconnected device to the driver whose handle is
 * passed to us. This should have no effect on systems unaffected by this EFI
 * bug/quirk.
 */
VOID ConnectFilesystemDriver (
    EFI_HANDLE DriverHandle
) {
    EFI_STATUS                             Status;
    UINTN                                  HandleCount = 0;
    UINTN                                  Index;
    UINTN                                  OpenInfoIndex;
    EFI_HANDLE                            *Handles = NULL;
    MY_EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *Fs;
    MY_EFI_BLOCK_IO_PROTOCOL              *BlockIo;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY   *OpenInfo;
    UINTN                                  OpenInfoCount;
    EFI_HANDLE                             DriverHandleList[2];

    //
    // Get all DiskIo handles
    //
    Status = REFIT_CALL_5_WRAPPER(
        gBS->LocateHandleBuffer, ByProtocol,
        &gMyEfiDiskIoProtocolGuid, NULL,
        &HandleCount, &Handles
    );

    if (EFI_ERROR(Status) || HandleCount == 0) {
        return;
    }

    //
    // Check every DiskIo handle
    //
    for (Index = 0; Index < HandleCount; Index++) {
        //
        // If this is not partition - skip it.
        // This is then whole disk and DiskIo
        // should be opened here BY_DRIVER by Partition driver
        // to produce partition volumes.
        //
        Status = REFIT_CALL_3_WRAPPER(
            gBS->HandleProtocol,
            Handles[Index],
            &gMyEfiBlockIoProtocolGuid,
            (VOID **) &BlockIo
        );

        if (EFI_ERROR(Status)) {
            continue;
        }

        if (BlockIo->Media == NULL || !BlockIo->Media->LogicalPartition) {
            continue;
        }

        //
        // If SimpleFileSystem is already produced - skip it, this is ok
        //
        Status = REFIT_CALL_3_WRAPPER(
            gBS->HandleProtocol,
            Handles[Index],
            &gMyEfiSimpleFileSystemProtocolGuid,
            (VOID **) &Fs
        );

        if (Status == EFI_SUCCESS) {
            continue;
        }

        //
        // If no SimpleFileSystem on this handle but DiskIo is opened BY_DRIVER
        // then disconnect this connection and try to connect our driver to it
        //
        Status = REFIT_CALL_4_WRAPPER(
            gBS->OpenProtocolInformation,
            Handles[Index],
            &gMyEfiDiskIoProtocolGuid,
            &OpenInfo,
            &OpenInfoCount
        );

        if (EFI_ERROR(Status)) {
            continue;
        }

        DriverHandleList[1] = NULL;
        for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++) {
            if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_DRIVER)
                == EFI_OPEN_PROTOCOL_BY_DRIVER
            ) {
                Status = REFIT_CALL_3_WRAPPER(
                    gBS->DisconnectController, Handles[Index],
                    OpenInfo[OpenInfoIndex].AgentHandle, NULL
                );

                if (!EFI_ERROR(Status)) {
                    DriverHandleList[0] = DriverHandle;
                    REFIT_CALL_4_WRAPPER(
                        gBS->ConnectController, Handles[Index],
                        DriverHandleList, NULL, FALSE
                    );
                }
            }
        } // for

        MY_FREE_POOL(OpenInfo);
    }

    MY_FREE_POOL(Handles);
} // VOID ConnectFilesystemDriver()

// Scan a directory for drivers.
// Originally from rEFIt's main.c (BSD), but modified since then (GPLv3).
static
UINTN ScanDriverDir (
    IN CHAR16 *Path
) {
    EFI_STATUS       Status;
    REFIT_DIR_ITER   DirIter;
    EFI_FILE_INFO   *DirEntry;
    CHAR16          *FileName;
    CHAR16          *ErrMsg;
    UINTN            NumFound  = 0;

    CleanUpPathNameSlashes (Path);

    #if REFIT_DEBUG > 0
    MsgLog ("\n");
    MsgLog ("Scan '%s' Folder:", Path);
    MsgLog ("\n");
    #endif

    // look through contents of the directory
    DirIterOpen (SelfRootDir, Path, &DirIter);

    #if REFIT_DEBUG > 0
    BOOLEAN RunOnce = FALSE;
    #endif

    while (DirIterNext (&DirIter, 2, LOADER_MATCH_PATTERNS, &DirEntry)) {
        if (DirEntry->FileName[0] == '.') {
            // skip this
            MY_FREE_POOL(DirEntry);
            continue;
        }

        NumFound++;
        FileName = PoolPrint (L"%s\\%s", Path, DirEntry->FileName);

        Status = StartEFIImage (
            SelfVolume, FileName, L"",
            DirEntry->FileName, 0,
            FALSE, TRUE
        );

        MY_FREE_POOL(DirEntry);

        #if REFIT_DEBUG > 0
        if (RunOnce) {
            MsgLog ("\n");
        }

        RunOnce = TRUE;

        MsgLog ("  - %r ... UEFI Driver:- '%s'", Status, FileName);
        #endif

        MY_FREE_POOL(FileName);
    } // while

    Status = DirIterClose (&DirIter);
    if (Status != EFI_NOT_FOUND) {
        ErrMsg = PoolPrint (L"While Scanning the '%s' Directory", Path);
        CheckError (Status, ErrMsg);
        MY_FREE_POOL(ErrMsg);
    }

    return (NumFound);
} // static UINTN ScanDriverDir()


// Load all UEFI drivers from RefindPlus' "drivers" subdirectory and from the
// directories specified by the user in the "scan_driver_dirs" configuration
// file line.
// Originally from rEFIt's main.c (BSD), but modified since then (GPLv3).
// Returns TRUE if any drivers are loaded, FALSE otherwise.
BOOLEAN LoadDrivers (VOID) {
    CHAR16  *Directory;
    CHAR16  *SelfDirectory;
    UINTN    i        = 0;
    UINTN    NumFound = 0;
    UINTN    CurFound = 0;

    #if REFIT_DEBUG > 0
    CHAR16  *MsgNotFound = L"Not Found or Empty";

    LOG(1, LOG_LINE_SEPARATOR, L"Load UEFI Drivers");
    #endif

    // load drivers from the subdirectories of RefindPlus' home directory
    // specified in the DRIVER_DIRS constant.
    #if REFIT_DEBUG > 0
    MsgLog ("Load UEFI Drivers from Program Default Folder...");
    #endif


    while ((Directory = FindCommaDelimited (DRIVER_DIRS, i++)) != NULL) {
        CleanUpPathNameSlashes (Directory);
        SelfDirectory = SelfDirPath ? StrDuplicate (SelfDirPath) : NULL;
        CleanUpPathNameSlashes (SelfDirectory);
        MergeStrings (&SelfDirectory, Directory, L'\\');

        CurFound = ScanDriverDir (SelfDirectory);
        if (CurFound > 0) {
            NumFound = NumFound + CurFound;

            // We only process one default folder
            // Exit loop if drivers were found
            break;
        }
        else {
            #if REFIT_DEBUG > 0
            LOG(3, LOG_LINE_NORMAL,
                L"'%s' ... Program Default Driver Folder:- '%s'",
                MsgNotFound, SelfDirectory
            );
            MsgLog ("  - %s", MsgNotFound);
            #endif
        }

        MY_FREE_POOL(Directory);
        MY_FREE_POOL(SelfDirectory);
    } // while

    // Scan additional user-specified driver directories.
    if (GlobalConfig.DriverDirs != NULL) {
        #if REFIT_DEBUG > 0
        MsgLog ("\n\n");
        MsgLog ("Load UEFI Drivers from User Defined Folders...");
        #endif

        i = 0;
        while ((Directory = FindCommaDelimited (GlobalConfig.DriverDirs, i++)) != NULL) {
            CleanUpPathNameSlashes (Directory);
            if (StrLen (Directory) > 0) {
                SelfDirectory = SelfDirPath ? StrDuplicate (SelfDirPath) : NULL;
                CleanUpPathNameSlashes (SelfDirectory);

                if (MyStrBegins (SelfDirectory, Directory)) {
                    MY_FREE_POOL(SelfDirectory);
                    SelfDirectory = StrDuplicate (Directory);
                }
                else {
                    MergeStrings (&SelfDirectory, Directory, L'\\');
                }

                CurFound = ScanDriverDir (SelfDirectory);
                if (CurFound > 0) {
                    NumFound = NumFound + CurFound;
                }
                else {
                    #if REFIT_DEBUG > 0
                    LOG(3, LOG_LINE_NORMAL,
                        L"'%s' ... User Defined Driver Folder:- '%s'",
                        MsgNotFound, SelfDirectory
                    );
                    MsgLog ("  - %s", MsgNotFound);
                    #endif
                }
            }

            MY_FREE_POOL(Directory);
            MY_FREE_POOL(SelfDirectory);
        } // while
    }


    #if REFIT_DEBUG > 0
    MsgLog ("\n\n");
    #endif

    #if REFIT_DEBUG > 0
    CHAR16 *MsgStr = PoolPrint (
        L"Processed %d UEFI Driver%s",
        NumFound, (NumFound == 1) ? L"" : L"s"
    );
    LOG(2, LOG_THREE_STAR_SEP, L"%s", MsgStr);
    MY_FREE_POOL(MsgStr);
    #endif

    // connect all devices
    // DA-TAG: Always run this
    ConnectAllDriversToAllControllers (TRUE);

    return (NumFound > 0);
} // BOOLEAN LoadDrivers()
