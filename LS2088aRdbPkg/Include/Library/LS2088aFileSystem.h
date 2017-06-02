/** @LS2088aFileSystem.h

	Based on SimpleFileSystem protocol implementation available in
	MdePkg/Include/Protocol/SimpleFileSystem.h

  Copyright (c) 2006 - 2014, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _LS2088A_FS_LIB_
#define _LS2088A_FS_LIB_

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>

/**
  Open the root directory on a volume.

  @param	This A pointer to the volume to open the root directory.
  @param	Root A pointer to the location to return the opened file handle
		for the root directory.

  @retval EFI_SUCCESS		The device was opened.
  @retval EFI_UNSUPPORTED	This volume does not support the requested file
				system type.
  @retval EFI_NO_MEDIA		The device has no medium.
  @retval EFI_DEVICE_ERROR	The device reported an error.
  @retval EFI_VOLUME_CORRUPTED	The file system structures are corrupted.
  @retval EFI_ACCESS_DENIED	The service denied access to the file.
  @retval EFI_OUT_OF_RESOURCES	The volume was not opened due to lack of
				resources.
  @retval EFI_MEDIA_CHANGED	The device has a different medium in it or the
				medium is no longer supported. Any existing file
				handles for this volume are no longer valid. To
				access the files on the new medium, the volume
				must be reopened with OpenVolume().

**/
EFI_STATUS
EFIAPI OpenVolume(
  IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *This,
  OUT EFI_FILE_PROTOCOL                 **Root
);

/**
  Opens a new file relative to the source file's location.

  @param  This		A pointer to the EFI_FILE_PROTOCOL instance that is the
			file handle to the source location. This would typically
			be an open handle to a directory.
  @param  NewHandle  	A pointer to the location to return the opened handle
			for the new file.
  @param  FileName	The Null-terminated string of the name of the file to be
			opened. The file name may contain the following path
			modifiers: "\", ".", and "..".
  @param  OpenMode	The mode to open the file. The only valid combinations
			that the file may be opened with are: Read, Read/Write,
			or Create/Read/Write.
  @param  Attributes	Only valid for EFI_FILE_MODE_CREATE, in which case these
			are the attribute bits for the newly created file.

  @retval EFI_SUCCESS		The file was opened.
  @retval EFI_NOT_FOUND		The specified file could not be found on the
				device.
  @retval EFI_NO_MEDIA		The device has no medium.
  @retval EFI_MEDIA_CHANGED	The device has a different medium in it or the
				medium is no longer supported.
  @retval EFI_DEVICE_ERROR	The device reported an error.
  @retval EFI_VOLUME_CORRUPTED	The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED	An attempt was made to create a file, or open a
				file for write when the media is write-protected
  @retval EFI_ACCESS_DENIED	The service denied access to the file.
  @retval EFI_OUT_OF_RESOURCES	Not enough resources were available to open the
				file.
  @retval EFI_VOLUME_FULL	The volume is full.

**/
EFI_STATUS
EFIAPI Open(
  IN EFI_FILE_PROTOCOL        *This,
  OUT EFI_FILE_PROTOCOL       **NewHandle,
  IN CHAR16                   *FileName,
  IN UINT64                   OpenMode,
  IN UINT64                   Attributes
  );

/**
  Closes a specified file handle.

  @param  This		A pointer to the EFI_FILE_PROTOCOL instance that is the
			file handle to close.

  @retval EFI_SUCCESS	The file was closed.

**/
EFI_STATUS
EFIAPI Close(
  IN EFI_FILE_PROTOCOL  *This
  );

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI Delete(
  IN EFI_FILE_PROTOCOL  *This
  );

/**
  Reads data from a file.

  @param  This		A pointer to the EFI_FILE_PROTOCOL instance that is the
			file handle to read data from.
  @param  BufferSize	On input, the size of the Buffer. On output, the amount
			of data returned in Buffer. In both cases, the size is
			measured in bytes.
  @param  Buffer	The buffer into which the data is read.

  @retval EFI_SUCCESS		Data was read.
  @retval EFI_NO_MEDIA		The device has no medium.
  @retval EFI_DEVICE_ERROR	The device reported an error.
  @retval EFI_DEVICE_ERROR	An attempt was made to read from a deleted file.
  @retval EFI_DEVICE_ERROR	On entry, the current file position is beyond
				the end of the file.
  @retval EFI_VOLUME_CORRUPTED	The file system structures are corrupted.
  @retval EFI_BUFFER_TOO_SMALL	The BufferSize is too small to read the current
				directory entry. BufferSize has been updated
				with the size needed to complete the request.

**/
EFI_STATUS
EFIAPI Read(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT UINTN                *BufferSize,
  OUT VOID                    *Buffer
  );

/**
  Writes data to a file.

  @param  This		A pointer to the EFI_FILE_PROTOCOL instance that is the
			file handle to write data to.
  @param  BufferSize	On input, the size of the Buffer. On output, the amount
			of data actually written. In both cases, the size is
			measured in bytes.
  @param  Buffer	The buffer of data to write.

  @retval EFI_SUCCESS		Data was written.
  @retval EFI_UNSUPPORTED	Writes to open directory files are not supported
  @retval EFI_NO_MEDIA		The device has no medium.
  @retval EFI_DEVICE_ERROR	The device reported an error.
  @retval EFI_DEVICE_ERROR	An attempt was made to write to a deleted file.
  @retval EFI_VOLUME_CORRUPTED	The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED	The file or medium is write-protected.
  @retval EFI_ACCESS_DENIED	The file was opened read only.
  @retval EFI_VOLUME_FULL	The volume is full.

**/
EFI_STATUS
EFIAPI Write(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT UINTN                *BufferSize,
  IN VOID                     *Buffer
  );

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI SetPosition(
  IN EFI_FILE_PROTOCOL        *This,
  IN UINT64                   Position
  );

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI GetPosition(
  IN EFI_FILE_PROTOCOL        *This,
  OUT UINT64                  *Position
  );

/**
  Returns information about a file.

  @param  This			A pointer to the EFI_FILE_PROTOCOL instance that
				is the file handle the requested information is
				for.
  @param  InformationType	The type identifier for the information being
				requested.
  @param  BufferSize		On input, the size of Buffer. On output, the
				amount of data returned in Buffer. In both
				cases, the size is measured in bytes.
  @param  Buffer		A pointer to the data buffer to return. The
				buffer's type is indicated by InformationType.

  @retval EFI_SUCCESS		The information was returned.
  @retval EFI_UNSUPPORTED	The InformationType is not known.
  @retval EFI_NO_MEDIA		The device has no medium.
  @retval EFI_DEVICE_ERROR	The device reported an error.
  @retval EFI_VOLUME_CORRUPTED	The file system structures are corrupted.
  @retval EFI_BUFFER_TOO_SMALL	The BufferSize is too small to read the current
				directory entry. BufferSize has been updated
				with the size needed to complete the request.
**/
EFI_STATUS
EFIAPI GetInfo(
  IN EFI_FILE_PROTOCOL        *This,
  IN EFI_GUID                 *InformationType,
  IN OUT UINTN                *BufferSize,
  OUT VOID                    *Buffer
  );

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI SetInfo(
  IN EFI_FILE_PROTOCOL        *This,
  IN EFI_GUID                 *InformationType,
  IN UINTN                    BufferSize,
  IN VOID                     *Buffer
  );

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI Flush(
  IN EFI_FILE_PROTOCOL  *This
  );

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI OpenEx(
  IN EFI_FILE_PROTOCOL        *This,
  OUT EFI_FILE_PROTOCOL       **NewHandle,
  IN CHAR16                   *FileName,
  IN UINT64                   OpenMode,
  IN UINT64                   Attributes,
  IN OUT EFI_FILE_IO_TOKEN    *Token
  );

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI ReadEx(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT EFI_FILE_IO_TOKEN    *Token
);

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI WriteEx(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT EFI_FILE_IO_TOKEN    *Token
);

/**
  Not implemented. Returns EFI_SUCESS.
**/
EFI_STATUS
EFIAPI FlushEx(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT EFI_FILE_IO_TOKEN    *Token
);

/**
  Read BufferSize bytes from Offset into Buffer.
  Reads may support reads that are not aligned on
  sector boundaries. There are three cases:
    UnderRun - The first byte is not on a sector boundary or the read request is
               less than a sector in length.
    Aligned  - A read of N contiguous sectors.
    OverRun  - The last byte is not on a sector boundary.

  @param  This                  Protocol instance pointer.
  @param  MediaId               Id of the media, changes every time the media is
				replaced.
  @param  Offset                The starting byte offset to read from
  @param  BufferSize            Size of Buffer
  @param  Buffer                Buffer containing read data

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
				the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_INVALID_PARAMETER The read request contains device addresses that
				are not valid for the device.

**/

EFI_STATUS
EFIAPI
DiskIoReadDisk (
  IN EFI_DISK_IO_PROTOCOL  *This,
  IN UINT32                MediaId,
  IN UINT64                Offset,
  IN UINTN                 BufferSize,
  OUT VOID                 *Buffer
  );

/**
  Writes BufferSize bytes from Buffer into Offset.
  Writes may require a read modify write to support writes that are not
  aligned on sector boundaries. There are three cases:
    UnderRun - The first byte is not on a sector boundary or the write request
               is less than a sector in length. Read modify write is required.
    Aligned  - A write of N contiguous sectors.
    OverRun  - The last byte is not on a sector boundary. Read modified write
               required.

  @param  This       Protocol instance pointer.
  @param  MediaId    Id of the media, changes every time the media is replaced.
  @param  Offset     The starting byte offset to read from
  @param  BufferSize Size of Buffer
  @param  Buffer     Buffer containing read data

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
				the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_INVALID_PARAMETER The write request contains device addresses that
				are not valid for the device.

**/
EFI_STATUS
EFIAPI
DiskIoWriteDisk (
  IN EFI_DISK_IO_PROTOCOL  *This,
  IN UINT32                MediaId,
  IN UINT64                Offset,
  IN UINTN                 BufferSize,
  IN VOID                  *Buffer
  );

typedef struct {
  UINT64	DeviceOffset;
  EFI_FILE_INFO	FileInfo;
} PRIVATE_FILE_INFO;

#define LS2088A_FILE_SYSTEM_REVISION	0x10001000
#define LS2088A_FILE_SYSTEM_SIGNATURE SIGNATURE_32 ('l', 's', 'f', 's')

typedef struct {
	UINT32				Signature;
	EFI_DISK_IO_PROTOCOL		DiskIo;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL Fs;
	EFI_FILE_PROTOCOL		FileIo;
	PRIVATE_FILE_INFO		*PrivateFileInfo;
	EFI_BLOCK_IO_PROTOCOL           *BlockIo;
} LS2088A_FILE_SYSTEM;

#endif
