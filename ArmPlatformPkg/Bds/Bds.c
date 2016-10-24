/** @file
*
*  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include "BdsInternal.h"

#include <Library/PcdLib.h>
#include <Library/PerformanceLib.h>

#include <Protocol/Bds.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/I2cMaster.h>

#include <Guid/EventGroup.h>
#include <Include/I2c.h>
#include <Library/LS2080aFileSystem.h>
#include <Include/Library/FslDdr.h>

#define EFI_SET_TIMER_TO_SECOND   10000000

UINT32
GetMediaId(
  IN EFI_FILE_PROTOCOL        *This
  )
{
  LS2080A_FILE_SYSTEM       *Instance;

  Instance = CR(This, LS2080A_FILE_SYSTEM, FileIo,
                LS2080A_FILE_SYSTEM_SIGNATURE);
  return(Instance->BlockIo->Media->MediaId);
}

EFI_STATUS
LS2080aTestBlockIoDevice (
  VOID
)
{
  EFI_STATUS Status;
  UINTN Size, BufferSize = 0x20000, Index, Temp;
  EFI_HANDLE *Handle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FsProtocol;
  EFI_FILE_PROTOCOL *Fs, *File;
  UINT8 SourceBuffer[BufferSize];
  UINT8 DestinationBuffer[BufferSize];
  UINT32 MediaId = 0;

  Status = gBS->LocateHandleBuffer (ByProtocol,
		&gEfiBlockIoProtocolGuid,
		NULL, &Size, &Handle);

  if (Size == 0)
    return EFI_SUCCESS;

  if (Status != EFI_SUCCESS) {
	return Status;
  }

  for(Index = 0; Index < Size; Index++) {
    Status = gBS->ConnectController (Handle[Index], NULL, NULL, FALSE);
    if (Status != EFI_SUCCESS) {
      return Status;
    }

    Status = gBS->HandleProtocol (Handle[Index],
		&gEfiSimpleFileSystemProtocolGuid,
		(VOID **)&FsProtocol);

    if (Status != EFI_SUCCESS) {
      gBS->FreePool(Handle);
      return Status;
    }

    // Try to Open the volume and get root directory
    Status = FsProtocol->OpenVolume (FsProtocol, &Fs);
    if (Status != EFI_SUCCESS) {
      gBS->FreePool(Handle);
      return Status;
    }

    File = NULL;
    Status = Fs->Open (Fs, &File, L"0x400000|0x1000", EFI_FILE_MODE_READ, 0);
    if (Status != EFI_SUCCESS) {
      gBS->FreePool(Handle);
      return Status;
    }

    MediaId = GetMediaId(File);
    if (MediaId == 0)
      BufferSize = 0x20000;
    else if (MediaId == SIGNATURE_32('m','m','c','o'))
      goto Disconnect;
    else
      BufferSize = 4096;

    for(Temp = 0; Temp < BufferSize; Temp++) {
         if (Temp % 2 == 0)
		 SourceBuffer[Temp] = 0x61;
	 else
		 SourceBuffer[Temp] = 0x52;
    }

    for(Temp = 0; Temp < BufferSize; Temp++) {
         DestinationBuffer[Temp] = 0x00;
    }

    Status = File->Write (File, &BufferSize, (VOID*)SourceBuffer);
    if (Status != EFI_SUCCESS) {
        gBS->FreePool(Handle);
         return Status;
    }

    Status = File->Read (File, &BufferSize, (VOID*)DestinationBuffer);
    if (Status != EFI_SUCCESS) {
        gBS->FreePool(Handle);
         return Status;
    }

    for (Temp = 0; Temp < BufferSize; Temp++) {
      if (SourceBuffer[Temp] != DestinationBuffer[Temp]) {
    	 gBS->DisconnectController (Handle[Index], NULL, NULL);
	 Status = EFI_COMPROMISED_DATA;
        if (MediaId == SIGNATURE_32('d','s','p','i')) {
	   DEBUG((EFI_D_ERROR, "Dspi Flash Test Result: FAIL, Error:'%r'\n",
				Status));
	   break;
	 } else if (MediaId == SIGNATURE_32('n', 'a', 'n', 'd')) {
	   DEBUG((EFI_D_ERROR, "Nand Flash Test Result: FAIL, Error:'%r'\n",
				Status));
	   break;
	 } else if (MediaId == 0) {
	   DEBUG((EFI_D_ERROR, "Nor Flash Test Result: FAIL, Error:'%r'\n",
				Status));
	   break;
	 }
      }
    }

    if (Temp == BufferSize) {
      if (MediaId == SIGNATURE_32('d','s','p','i'))
	 Print(L"Dspi Test Result: PASS\n");
      else if (MediaId == SIGNATURE_32('n', 'a', 'n', 'd'))
	 Print(L"Nand Test Result: PASS\n");
      else if (MediaId == 0)
	 Print(L"Nor Test Result: PASS\n");

Disconnect:
      Status = gBS->DisconnectController (Handle[Index], NULL, NULL);
    }
  }

  gBS->FreePool(Handle);

  return Status;
}

EFI_STATUS
LS2080aTestI2c (
  VOID
)
{
  EFI_STATUS 			Status;
  EFI_I2C_MASTER_PROTOCOL 	*I2c;
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  UINT8 Rbuf[6];
//  UINT8 Wbuf[6] = {1, 2, 3, 4, 5};
  UINTN BusFreq = I2C_SPEED;

  Status = gBS->LocateProtocol (&gEfiI2cMasterProtocolGuid,
		NULL,
		(VOID **)&I2c);

  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR,"Failed to locate i2c protocol (Error '%r')\n", Status));
    return Status;
  }

  /* Test I2c Set Bus Frequency Function */
  Status = I2c->SetBusFrequency(I2c, &BusFreq);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR,"Failed to set i2c bus frequency (Error '%r')\n", Status));
    return Status;
  }
#if 0
  /* Test I2c reset Function */
  Status = I2c->Reset(I2c);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR,"Failed to reset i2c bus(Error '%r')\n", Status));
    return Status;
  }
#endif

  /* Test I2c Read Function */
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)AllocatePool
					(sizeof(EFI_I2C_REQUEST_PACKET));
  RequestPacket->OperationCount = 1;
  RequestPacket->Operation[0].Flags = I2C_READ_FLAG;
  RequestPacket->Operation[0].LengthInBytes = 10;
  RequestPacket->Operation[0].Buffer = Rbuf;

  /* EEPROM0_ADDRESS */
  Status = I2c->StartRequest (I2c, EEPROM0_ADDRESS, RequestPacket, NULL, NULL);

  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR,"Failed to read eeprom on i2c bus Error '%r')\n",
		Status));
    FreePool(RequestPacket);
    return Status;
  }

  DEBUG((EFI_D_RELEASE,"EEPROM0: 0x%x 0x%x 0x%x  0x%x  0x%x ",
	Rbuf[0], Rbuf[1], Rbuf[2], Rbuf[3], Rbuf[4]));
  DEBUG((EFI_D_RELEASE,"0x%x 0x%x 0x%x  0x%x  0x%x \n",
	Rbuf[5], Rbuf[6], Rbuf[7], Rbuf[8], Rbuf[9]));

#if 0
  if (Rbuf[0] != 0x92 || Rbuf[1] != 0x10 || Rbuf[2] != 0xb ||
      Rbuf[3] != 0x2 || Rbuf[4] != 0x2) {
    DEBUG((EFI_D_ERROR,"Read data is invalid\n"));
    FreePool(RequestPacket);
    return EFI_COMPROMISED_DATA;
  }
  FreePool(RequestPacket);

  /* Test I2c Write Function */
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)AllocatePool
				(sizeof(EFI_I2C_OPERATION) +
				 sizeof(EFI_I2C_REQUEST_PACKET));
  RequestPacket->OperationCount = 2;

  RequestPacket->Operation[0].Flags = 0x2;
  RequestPacket->Operation[0].LengthInBytes = 5;
  RequestPacket->Operation[0].Buffer = Wbuf;

  RequestPacket->Operation[1].Flags = 0x1;
  RequestPacket->Operation[1].LengthInBytes = 5;
  RequestPacket->Operation[1].Buffer = Rbuf;

  Status = I2c->StartRequest (I2c, 0x51, RequestPacket, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Failed to W/R eeprom on i2c bus \
    (Error '%r')\n", Status));
    FreePool(RequestPacket);
    return Status;
  }

  if (Rbuf[0] != Wbuf[0] || Rbuf[1] != Wbuf[1] || Rbuf[2] != Wbuf[2] ||
      Rbuf[3] != Wbuf[3] || Rbuf[4] != Wbuf[4]) {
    DEBUG((EFI_D_ERROR, "Read back data is not similar to written data\n"));
    FreePool(RequestPacket);
    return EFI_COMPROMISED_DATA;
  }
#endif
  FreePool(RequestPacket);
  return EFI_SUCCESS;
}

VOID LS2080aTestCode
(
  VOID
)
{
#ifdef I2CTEST
  EFI_STATUS Status;
  Status = LS2080aTestI2c();
  if(Status == EFI_SUCCESS)
	Print(L"I2c Test Result: PASS\n");
#endif

#ifdef BLOCKIO
  EFI_STATUS Status;
  Status = LS2080aTestBlockIoDevice();
  if (EFI_ERROR(Status))
    DEBUG((EFI_D_ERROR, "BlockIoDevice Test FAILED, Error:'%r'\n", Status));
#endif
}

STATIC
EFI_STATUS
GetConsoleDevicePathFromVariable (
  IN  CHAR16*             ConsoleVarName,
  IN  CHAR16*             DefaultConsolePaths,
  OUT EFI_DEVICE_PATH**   DevicePaths
  )
{
  EFI_STATUS                Status;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL* DevicePathInstances;
  EFI_DEVICE_PATH_PROTOCOL* DevicePathInstance;
  CHAR16*                   DevicePathStr;
  CHAR16*                   NextDevicePathStr;
  EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL  *EfiDevicePathFromTextProtocol;

  Status = GetGlobalEnvironmentVariable (ConsoleVarName, NULL, NULL, (VOID**)&DevicePathInstances);
  if (EFI_ERROR(Status)) {
    // In case no default console device path has been defined we assume a driver handles the console (eg: SimpleTextInOutSerial)
    if ((DefaultConsolePaths == NULL) || (DefaultConsolePaths[0] == L'\0')) {
      *DevicePaths = NULL;
      return EFI_SUCCESS;
    }

    Status = gBS->LocateProtocol (&gEfiDevicePathFromTextProtocolGuid, NULL, (VOID **)&EfiDevicePathFromTextProtocol);
    ASSERT_EFI_ERROR(Status);

    DevicePathInstances = NULL;

    // Extract the Device Path instances from the multi-device path string
    while ((DefaultConsolePaths != NULL) && (DefaultConsolePaths[0] != L'\0')) {
      NextDevicePathStr = StrStr (DefaultConsolePaths, L";");
      if (NextDevicePathStr == NULL) {
        DevicePathStr = DefaultConsolePaths;
        DefaultConsolePaths = NULL;
      } else {
        DevicePathStr = (CHAR16*)AllocateCopyPool ((NextDevicePathStr - DefaultConsolePaths + 1) * sizeof(CHAR16), DefaultConsolePaths);
        *(DevicePathStr + (NextDevicePathStr - DefaultConsolePaths)) = L'\0';
        DefaultConsolePaths = NextDevicePathStr;
        if (DefaultConsolePaths[0] == L';') {
          DefaultConsolePaths++;
        }
      }

      DevicePathInstance = EfiDevicePathFromTextProtocol->ConvertTextToDevicePath (DevicePathStr);
      ASSERT(DevicePathInstance != NULL);
      DevicePathInstances = AppendDevicePathInstance (DevicePathInstances, DevicePathInstance);

      if (NextDevicePathStr != NULL) {
        FreePool (DevicePathStr);
      }
      FreePool (DevicePathInstance);
    }

    // Set the environment variable with this device path multi-instances
    Size = GetDevicePathSize (DevicePathInstances);
    if (Size > 0) {
      gRT->SetVariable (
          ConsoleVarName,
          &gEfiGlobalVariableGuid,
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
          Size,
          DevicePathInstances
          );
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  }

  if (!EFI_ERROR(Status)) {
    *DevicePaths = DevicePathInstances;
  }
  return Status;
}

STATIC
EFI_STATUS
InitializeConsolePipe (
  IN EFI_DEVICE_PATH    *ConsoleDevicePaths,
  IN EFI_GUID           *Protocol,
  OUT EFI_HANDLE        *Handle,
  OUT VOID*             *Interface
  )
{
  EFI_STATUS                Status;
  UINTN                     Size;
  UINTN                     NoHandles;
  EFI_HANDLE                *Buffer;
  EFI_DEVICE_PATH_PROTOCOL* DevicePath;

  // Connect all the Device Path Consoles
  while (ConsoleDevicePaths != NULL) {
    DevicePath = GetNextDevicePathInstance (&ConsoleDevicePaths, &Size);

    Status = BdsConnectDevicePath (DevicePath, Handle, NULL);
    DEBUG_CODE_BEGIN();
      if (EFI_ERROR(Status)) {
        // We convert back to the text representation of the device Path
        EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* DevicePathToTextProtocol;
        CHAR16* DevicePathTxt;
        EFI_STATUS Status;

        Status = gBS->LocateProtocol(&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **)&DevicePathToTextProtocol);
        if (!EFI_ERROR(Status)) {
          DevicePathTxt = DevicePathToTextProtocol->ConvertDevicePathToText (DevicePath, TRUE, TRUE);

          DEBUG((EFI_D_ERROR,"Fail to start the console with the Device Path '%s'. (Error '%r')\n", DevicePathTxt, Status));

          FreePool (DevicePathTxt);
        }
      }
    DEBUG_CODE_END();

    // If the console splitter driver is not supported by the platform then use the first Device Path
    // instance for the console interface.
    if (!EFI_ERROR(Status) && (*Interface == NULL)) {
      Status = gBS->HandleProtocol (*Handle, Protocol, Interface);
    }
  }

  // No Device Path has been defined for this console interface. We take the first protocol implementation
  if (*Interface == NULL) {
    Status = gBS->LocateHandleBuffer (ByProtocol, Protocol, NULL, &NoHandles, &Buffer);
    if (EFI_ERROR (Status)) {
      BdsConnectAllDrivers();
      Status = gBS->LocateHandleBuffer (ByProtocol, Protocol, NULL, &NoHandles, &Buffer);
    }

    if (!EFI_ERROR(Status)) {
      *Handle = Buffer[0];
      Status = gBS->HandleProtocol (*Handle, Protocol, Interface);
      ASSERT_EFI_ERROR(Status);
      FreePool (Buffer);
    }
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}

EFI_STATUS
InitializeConsole (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH*          ConOutDevicePaths;
  EFI_DEVICE_PATH*          ConInDevicePaths;
  EFI_DEVICE_PATH*          ConErrDevicePaths;

  // By getting the Console Device Paths from the environment variables before initializing the console pipe, we
  // create the 3 environment variables (ConIn, ConOut, ConErr) that allows to initialize all the console interface
  // of newly installed console drivers
  Status = GetConsoleDevicePathFromVariable (L"ConOut", (CHAR16*)PcdGetPtr(PcdDefaultConOutPaths), &ConOutDevicePaths);
  ASSERT_EFI_ERROR (Status);
  Status = GetConsoleDevicePathFromVariable (L"ConIn", (CHAR16*)PcdGetPtr(PcdDefaultConInPaths), &ConInDevicePaths);
  ASSERT_EFI_ERROR (Status);
  Status = GetConsoleDevicePathFromVariable (L"ErrOut", (CHAR16*)PcdGetPtr(PcdDefaultConOutPaths), &ConErrDevicePaths);
  ASSERT_EFI_ERROR (Status);

  // Initialize the Consoles
  Status = InitializeConsolePipe (ConOutDevicePaths, &gEfiSimpleTextOutProtocolGuid, &gST->ConsoleOutHandle, (VOID **)&gST->ConOut);
  ASSERT_EFI_ERROR (Status);
  Status = InitializeConsolePipe (ConInDevicePaths, &gEfiSimpleTextInProtocolGuid, &gST->ConsoleInHandle, (VOID **)&gST->ConIn);
  ASSERT_EFI_ERROR (Status);
  Status = InitializeConsolePipe (ConErrDevicePaths, &gEfiSimpleTextOutProtocolGuid, &gST->StandardErrorHandle, (VOID **)&gST->StdErr);
  if (EFI_ERROR(Status)) {
    // In case of error, we reuse the console output for the error output
    gST->StandardErrorHandle = gST->ConsoleOutHandle;
    gST->StdErr = gST->ConOut;
  }

  // Free Memory allocated for reading the UEFI Variables
  if (ConOutDevicePaths) {
    FreePool (ConOutDevicePaths);
  }
  if (ConInDevicePaths) {
    FreePool (ConInDevicePaths);
  }
  if (ConErrDevicePaths) {
    FreePool (ConErrDevicePaths);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DefineDefaultBootEntries (
  VOID
  )
{
  BDS_LOAD_OPTION*                    BdsLoadOption;
  UINTN                               Size;
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL* EfiDevicePathFromTextProtocol;
  EFI_DEVICE_PATH*                    BootDevicePath;
  UINTN                               CmdLineSize;
  UINTN                               CmdLineAsciiSize;
  CHAR16*                             DefaultBootArgument;
  CHAR8*                              AsciiDefaultBootArgument;

  //
  // If Boot Order does not exist then create a default entry
  //
  Size = 0;
  Status = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &Size, NULL);
  if (Status == EFI_NOT_FOUND) {
    if ((PcdGetPtr(PcdDefaultBootDevicePath) == NULL) || (StrLen ((CHAR16*)PcdGetPtr(PcdDefaultBootDevicePath)) == 0)) {
      return EFI_UNSUPPORTED;
    }

    Status = gBS->LocateProtocol (&gEfiDevicePathFromTextProtocolGuid, NULL, (VOID **)&EfiDevicePathFromTextProtocol);
    if (EFI_ERROR(Status)) {
      // You must provide an implementation of DevicePathFromTextProtocol in your firmware (eg: DevicePathDxe)
      DEBUG((EFI_D_ERROR,"Error: Bds requires DevicePathFromTextProtocol\n"));
      return Status;
    }
    BootDevicePath = EfiDevicePathFromTextProtocol->ConvertTextToDevicePath ((CHAR16*)PcdGetPtr(PcdDefaultBootDevicePath));

    DEBUG_CODE_BEGIN();
      // We convert back to the text representation of the device Path to see if the initial text is correct
      EFI_DEVICE_PATH_TO_TEXT_PROTOCOL* DevicePathToTextProtocol;
      CHAR16* DevicePathTxt;

      Status = gBS->LocateProtocol(&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **)&DevicePathToTextProtocol);
      ASSERT_EFI_ERROR(Status);
      DevicePathTxt = DevicePathToTextProtocol->ConvertDevicePathToText (BootDevicePath, TRUE, TRUE);

      if (StrCmp ((CHAR16*)PcdGetPtr (PcdDefaultBootDevicePath), DevicePathTxt) != 0) {
        DEBUG ((EFI_D_ERROR, "Device Path given: '%s' Device Path expected: '%s'\n",
            (CHAR16*)PcdGetPtr (PcdDefaultBootDevicePath), DevicePathTxt));
        ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
      }

      FreePool (DevicePathTxt);
    DEBUG_CODE_END();

    // Create the entry is the Default values are correct
    if (BootDevicePath != NULL) {
      // We do not support NULL pointer
      ASSERT (PcdGetPtr (PcdDefaultBootArgument) != NULL);

      //
      // Logic to handle ASCII or Unicode default parameters
      //
      if (*(CHAR8*)PcdGetPtr (PcdDefaultBootArgument) == '\0') {
        CmdLineSize = 0;
        CmdLineAsciiSize = 0;
        DefaultBootArgument = NULL;
        AsciiDefaultBootArgument = NULL;
      } else if (IsUnicodeString ((CHAR16*)PcdGetPtr (PcdDefaultBootArgument))) {
        // The command line is a Unicode string
        DefaultBootArgument = (CHAR16*)PcdGetPtr (PcdDefaultBootArgument);
        CmdLineSize = StrSize (DefaultBootArgument);

        // Initialize ASCII variables
        CmdLineAsciiSize = CmdLineSize / 2;
        AsciiDefaultBootArgument = AllocatePool (CmdLineAsciiSize);
        if (AsciiDefaultBootArgument == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        UnicodeStrToAsciiStr ((CHAR16*)PcdGetPtr (PcdDefaultBootArgument), AsciiDefaultBootArgument);
      } else {
        // The command line is a ASCII string
        AsciiDefaultBootArgument = (CHAR8*)PcdGetPtr (PcdDefaultBootArgument);
        CmdLineAsciiSize = AsciiStrSize (AsciiDefaultBootArgument);

        // Initialize ASCII variables
        CmdLineSize = CmdLineAsciiSize * 2;
        DefaultBootArgument = AllocatePool (CmdLineSize);
        if (DefaultBootArgument == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        AsciiStrToUnicodeStr (AsciiDefaultBootArgument, DefaultBootArgument);
      }

      BootOptionCreate (LOAD_OPTION_ACTIVE | LOAD_OPTION_CATEGORY_BOOT,
        (CHAR16*)PcdGetPtr (PcdDefaultBootDescription),
        BootDevicePath,
        (UINT8 *)DefaultBootArgument, // OptionalData
        CmdLineSize,                  // OptionalDataSize
        &BdsLoadOption
        );
      FreePool (BdsLoadOption);

      if (DefaultBootArgument == (CHAR16*)PcdGetPtr (PcdDefaultBootArgument)) {
        FreePool (AsciiDefaultBootArgument);
      } else if (DefaultBootArgument != NULL) {
        FreePool (DefaultBootArgument);
      }
    } else {
      Status = EFI_UNSUPPORTED;
    }
  }

  return Status;
}

EFI_STATUS
StartDefaultBootOnTimeout (
  VOID
  )
{
  UINTN               Size;
  UINT16              Timeout;
  UINT16              *TimeoutPtr;
  EFI_EVENT           WaitList[2];
  UINTN               WaitIndex;
  UINT16              *BootOrder;
  UINTN               BootOrderSize;
  UINTN               Index;
  CHAR16              BootVariableName[9];
  EFI_STATUS          Status;
  EFI_INPUT_KEY       Key;

  Size = sizeof(UINT16);
  Timeout = (UINT16)PcdGet16 (PcdPlatformBootTimeOut);
  Status = GetGlobalEnvironmentVariable (L"Timeout", &Timeout, &Size, (VOID**)&TimeoutPtr);
  if (!EFI_ERROR (Status)) {
    Timeout = *TimeoutPtr;
    FreePool (TimeoutPtr);
  }

  if (Timeout != 0xFFFF) {
    if (Timeout > 0) {
      // Create the waiting events (keystroke and 1sec timer)
      gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &WaitList[0]);
      gBS->SetTimer (WaitList[0], TimerPeriodic, EFI_SET_TIMER_TO_SECOND);
      WaitList[1] = gST->ConIn->WaitForKey;

      // Start the timer
      WaitIndex = 0;
      Print(L"The default boot selection will start in ");
      while ((Timeout > 0) && (WaitIndex == 0)) {
        Print(L"%3d seconds",Timeout);
        gBS->WaitForEvent (2, WaitList, &WaitIndex);
        if (WaitIndex == 0) {
          Print(L"\b\b\b\b\b\b\b\b\b\b\b");
          Timeout--;
        }
      }
      // Discard key in the buffer
      do {
        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
      } while(!EFI_ERROR(Status));
      gBS->CloseEvent (WaitList[0]);
      Print(L"\n\r");
    }

    // In case of Timeout we start the default boot selection
    if (Timeout == 0) {
      // Get the Boot Option Order from the environment variable (a default value should have been created)
      GetGlobalEnvironmentVariable (L"BootOrder", NULL, &BootOrderSize, (VOID**)&BootOrder);

      for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
        UnicodeSPrint (BootVariableName, 9 * sizeof(CHAR16), L"Boot%04X", BootOrder[Index]);
        Status = BdsStartBootOption (BootVariableName);
        if(!EFI_ERROR(Status)){
          // Boot option returned successfully, hence don't need to start next boot option
          break;
        }
        // In case of success, we should not return from this call.
      }
      FreePool (BootOrder);
    }
  }
  return EFI_SUCCESS;
}

/**
  An empty function to pass error checking of CreateEventEx ().

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context,
                                which is implementation-dependent.

**/
VOID
EFIAPI
EmptyCallbackFunction (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  return;
}

/**
  This function uses policy data from the platform to determine what operating
  system or system utility should be loaded and invoked.  This function call
  also optionally make the use of user input to determine the operating system
  or system utility to be loaded and invoked.  When the DXE Core has dispatched
  all the drivers on the dispatch queue, this function is called.  This
  function will attempt to connect the boot devices required to load and invoke
  the selected operating system or system utility.  During this process,
  additional firmware volumes may be discovered that may contain addition DXE
  drivers that can be dispatched by the DXE Core.   If a boot device cannot be
  fully connected, this function calls the DXE Service Dispatch() to allow the
  DXE drivers from any newly discovered firmware volumes to be dispatched.
  Then the boot device connection can be attempted again.  If the same boot
  device connection operation fails twice in a row, then that boot device has
  failed, and should be skipped.  This function should never return.

  @param  This             The EFI_BDS_ARCH_PROTOCOL instance.

  @return None.

**/
VOID
EFIAPI
BdsEntry (
  IN EFI_BDS_ARCH_PROTOCOL  *This
  )
{
  UINTN               Size;
  EFI_STATUS          Status;
  UINT16             *BootNext;
  UINTN               BootNextSize;
  CHAR16              BootVariableName[9];
  EFI_EVENT           EndOfDxeEvent;

  //
  // Signal EndOfDxe PI Event
  //
  Status = gBS->CreateEventEx (
      EVT_NOTIFY_SIGNAL,
      TPL_NOTIFY,
      EmptyCallbackFunction,
      NULL,
      &gEfiEndOfDxeEventGroupGuid,
      &EndOfDxeEvent
      );
  if (!EFI_ERROR (Status)) {
    gBS->SignalEvent (EndOfDxeEvent);
  }

  PERF_END   (NULL, "DXE", NULL, 0);

  //
  // Declare the Firmware Vendor
  //
  if (FixedPcdGetPtr(PcdFirmwareVendor) != NULL) {
    Size = 0x100;
    gST->FirmwareVendor = AllocateRuntimePool (Size);
    ASSERT (gST->FirmwareVendor != NULL);
    UnicodeSPrint (gST->FirmwareVendor, Size, L"%a EFI %a %a", PcdGetPtr(PcdFirmwareVendor), __DATE__, __TIME__);
  }

  //
  // Fixup Table CRC after we updated Firmware Vendor
  //
  gST->Hdr.CRC32 = 0;
  Status = gBS->CalculateCrc32 ((VOID*)gST, gST->Hdr.HeaderSize, &gST->Hdr.CRC32);
  ASSERT_EFI_ERROR (Status);

  // If BootNext environment variable is defined then we just load it !
  BootNextSize = sizeof(UINT16);
  Status = GetGlobalEnvironmentVariable (L"BootNext", NULL, &BootNextSize, (VOID**)&BootNext);
  if (!EFI_ERROR(Status)) {
    ASSERT(BootNextSize == sizeof(UINT16));

    // Generate the requested Boot Entry variable name
    UnicodeSPrint (BootVariableName, 9 * sizeof(CHAR16), L"Boot%04X", *BootNext);

    // Set BootCurrent variable
    gRT->SetVariable (L"BootCurrent", &gEfiGlobalVariableGuid,
              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
              BootNextSize, BootNext);

    FreePool (BootNext);

    // Start the requested Boot Entry
    Status = BdsStartBootOption (BootVariableName);
    if (Status != EFI_NOT_FOUND) {
      // BootNext has not been succeeded launched
      if (EFI_ERROR(Status)) {
        Print(L"Fail to start BootNext.\n");
      }

      // Delete the BootNext environment variable
      gRT->SetVariable (L"BootNext", &gEfiGlobalVariableGuid,
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
          0, NULL);
    }

    // Clear BootCurrent variable
    gRT->SetVariable (L"BootCurrent", &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        0, NULL);
  }

  // If Boot Order does not exist then create a default entry
  DefineDefaultBootEntries ();

  // Now we need to setup the EFI System Table with information about the console devices.
  InitializeConsole ();

  //
  // Update the CRC32 in the EFI System Table header
  //
  gST->Hdr.CRC32 = 0;
  Status = gBS->CalculateCrc32 ((VOID*)gST, gST->Hdr.HeaderSize, &gST->Hdr.CRC32);
  ASSERT_EFI_ERROR (Status);

DEBUG_CODE_BEGIN();
	if(PcdGet32(PcdBootMode) == 0) //For now only for NOR boot
		LS2080aTestCode();
DEBUG_CODE_END();

  // Timer before initiating the default boot selection
  StartDefaultBootOnTimeout ();

  // Start the Boot Menu
  Status = BootMenuMain ();
  ASSERT_EFI_ERROR (Status);

}

EFI_BDS_ARCH_PROTOCOL  gBdsProtocol = {
  BdsEntry,
};

EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiBdsArchProtocolGuid, &gBdsProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
