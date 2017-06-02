/** @SimpleFileSystemLib.c
#
#  API for implementing SimpleFileSystem protocol over BlockIo protocol
#
#  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
**/


#include <Library/LS2088aFileSystem.h>

#define IS_TAB(a)		   ((a) == L'\t')
#define IS_SPACE(a)                ((a) == L' ')
#define IS_PIPE(a)                 ((a) == L'|')
#define IS_NULL(a)                 ((a) == L'\0')

EFI_STATUS
EFIAPI ParseParams(
  IN CHAR16	*FileName,
  OUT UINT64	*Offset,
  OUT UINTN	*Size,
  OUT UINT32	*FileNameLength
)
{
  UINT8 Length, Index=0;
  EFI_STATUS	Status;
  CHAR16 *TmpFileName, *Ptr;

  Length = StrLen(FileName);
  *FileNameLength = Length;
  TmpFileName = (CHAR16*) AllocatePool(Length);
  StrnCpy(TmpFileName, FileName, Length);
  while((IS_SPACE(TmpFileName[Index]) ||
		IS_TAB(TmpFileName[Index])) && Index < Length)
	Index++;
  if(Index == Length) {
	Status = EFI_INVALID_PARAMETER;
	goto Done;
  }
  Index++;
  Ptr = &TmpFileName[Index];
  while(!IS_PIPE(TmpFileName[Index]) && Index < Length)
	Index++;
  if(Index == Length) {
	Status =  EFI_INVALID_PARAMETER;
	goto Done;
  }
  TmpFileName[Index] = L'\0';
  *Offset = StrHexToUint64(Ptr);
  Ptr = &TmpFileName[Index+1];
  *Size = StrHexToUintn(Ptr);
  if(*Size == 0) {
	Status =  EFI_INVALID_PARAMETER;
	goto Done;
  }
  Status = EFI_SUCCESS;

Done:
  return Status;
}

EFI_STATUS
EFIAPI OpenVolume(
  IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *This,
  OUT EFI_FILE_PROTOCOL                 **Root
)
{
  LS2088A_FILE_SYSTEM	*Instance;
  Instance = CR(This, LS2088A_FILE_SYSTEM, Fs, LS2088A_FILE_SYSTEM_SIGNATURE);
  *Root = &Instance->FileIo;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI Open(
  IN EFI_FILE_PROTOCOL        *This,
  OUT EFI_FILE_PROTOCOL       **NewHandle,
  IN CHAR16                   *FileName,
  IN UINT64                   OpenMode,
  IN UINT64                   Attributes
  )
{
  EFI_STATUS	Status;
  LS2088A_FILE_SYSTEM	*Instance;
  UINT32	FileNameLength;
  UINT64	Offset;
  UINTN		Size;
  PRIVATE_FILE_INFO *PrivateFileInfo;

  Instance = CR(This, LS2088A_FILE_SYSTEM, FileIo,
		  LS2088A_FILE_SYSTEM_SIGNATURE);

  Status = ParseParams(FileName, &Offset, &Size, &FileNameLength);
  if(EFI_ERROR(Status)) {
	return Status;
  }

  PrivateFileInfo = (PRIVATE_FILE_INFO *)
	  AllocatePool(sizeof(PRIVATE_FILE_INFO) + FileNameLength);
  if(PrivateFileInfo == NULL) {
	return EFI_OUT_OF_RESOURCES;
  }
  PrivateFileInfo->DeviceOffset = Offset;
  PrivateFileInfo->FileInfo.Size = sizeof(EFI_FILE_INFO) + FileNameLength;
  PrivateFileInfo->FileInfo.FileSize = Size;
  CopyMem(PrivateFileInfo->FileInfo.FileName, FileName, FileNameLength);
  Instance->PrivateFileInfo = PrivateFileInfo;
  *NewHandle = This;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI Close(
  IN EFI_FILE_PROTOCOL  *This
  )
{
  LS2088A_FILE_SYSTEM	*Instance;

  Instance = CR(This, LS2088A_FILE_SYSTEM, FileIo,
		  LS2088A_FILE_SYSTEM_SIGNATURE);
  if(Instance->PrivateFileInfo != NULL)
	  FreePool(Instance->PrivateFileInfo);
  Instance->PrivateFileInfo = NULL;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI Delete(
  IN EFI_FILE_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI Read(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT UINTN                *BufferSize,
  OUT VOID                    *Buffer
  )
{
  LS2088A_FILE_SYSTEM	*Instance;
  EFI_DISK_IO_PROTOCOL	*DiskIo;
  UINT32	MediaId;
  UINT32	Offset;

  Instance = CR(This, LS2088A_FILE_SYSTEM, FileIo,
		  LS2088A_FILE_SYSTEM_SIGNATURE);
  DiskIo = &Instance->DiskIo;
  MediaId = Instance->BlockIo->Media->MediaId;
  Offset = Instance->PrivateFileInfo->DeviceOffset;
  return DiskIo->ReadDisk(DiskIo, MediaId, Offset, *BufferSize, Buffer);
}

EFI_STATUS
EFIAPI Write(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT UINTN                *BufferSize,
  IN VOID                     *Buffer
  )
{
  LS2088A_FILE_SYSTEM	*Instance;
  EFI_DISK_IO_PROTOCOL	*DiskIo;
  UINT32	MediaId;
  UINT32	Offset;

  Instance = CR(This, LS2088A_FILE_SYSTEM, FileIo,
		  LS2088A_FILE_SYSTEM_SIGNATURE);
  DiskIo = &Instance->DiskIo;
  MediaId = Instance->BlockIo->Media->MediaId;
  Offset = Instance->PrivateFileInfo->DeviceOffset;
  return  DiskIo->WriteDisk(DiskIo, MediaId, Offset, *BufferSize, Buffer);
}

EFI_STATUS
EFIAPI SetPosition(
  IN EFI_FILE_PROTOCOL        *This,
  IN UINT64                   Position
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI GetPosition(
  IN EFI_FILE_PROTOCOL        *This,
  OUT UINT64                  *Position
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI GetInfo(
  IN EFI_FILE_PROTOCOL        *This,
  IN EFI_GUID                 *InformationType,
  IN OUT UINTN                *BufferSize,
  OUT VOID                    *Buffer
  )
{
  LS2088A_FILE_SYSTEM	*Instance;

  Instance = CR(This, LS2088A_FILE_SYSTEM, FileIo,
		  LS2088A_FILE_SYSTEM_SIGNATURE);
  if(Buffer) {
	CopyMem(Buffer, &Instance->PrivateFileInfo->FileInfo,
			Instance->PrivateFileInfo->FileInfo.Size);
  }
  *BufferSize = Instance->PrivateFileInfo->FileInfo.Size;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI SetInfo(
  IN EFI_FILE_PROTOCOL        *This,
  IN EFI_GUID                 *InformationType,
  IN UINTN                    BufferSize,
  IN VOID                     *Buffer
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI Flush(
  IN EFI_FILE_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI OpenEx(
  IN EFI_FILE_PROTOCOL        *This,
  OUT EFI_FILE_PROTOCOL       **NewHandle,
  IN CHAR16                   *FileName,
  IN UINT64                   OpenMode,
  IN UINT64                   Attributes,
  IN OUT EFI_FILE_IO_TOKEN    *Token
  )
{
  return EFI_SUCCESS;
};

EFI_STATUS
EFIAPI ReadEx(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT EFI_FILE_IO_TOKEN    *Token
){
  return EFI_SUCCESS;
};

EFI_STATUS
EFIAPI WriteEx(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT EFI_FILE_IO_TOKEN    *Token
){
  return EFI_SUCCESS;
};

EFI_STATUS
EFIAPI FlushEx(
  IN EFI_FILE_PROTOCOL        *This,
  IN OUT EFI_FILE_IO_TOKEN    *Token
){
  return EFI_SUCCESS;
};
