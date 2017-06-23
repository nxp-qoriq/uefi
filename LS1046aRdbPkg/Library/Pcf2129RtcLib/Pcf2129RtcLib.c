/** @PCF2129RtcLib.c
  Implement EFI RealTimeClock with runtime services via RTC Lib for PCF2129 RTC.

  Based on RTC implementation available in
  EmbeddedPkg/Library/TemplateRealTimeClockLib/RealTimeClockLib.c

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/RealTimeClockLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <I2c.h>
#include <Protocol/RealTimeClock.h>

#define Bin(Bcd) ((Bcd) & 0x0f) + ((Bcd) >> 4) * 10
#define Bcd(Bin) (((Bin / 10) << 4) | (Bin % 10))

#define PCF2129_I2C_ADDR            (0x51)

/*
 * RTC register addresses
 */
#define PCF2129_CTRL1_REG_ADDR      (0x00)  /* Control Register 1 */
#define PCF2129_CTRL2_REG_ADDR      (0x01)  /* Control Register 2 */
#define PCF2129_CTRL3_REG_ADDR      (0x02)  /* Control Register 3 */
#define PCF2129_SEC_REG_ADDR        (0x03)
#define PCF2129_MIN_REG_ADDR        (0x04)
#define PCF2129_HR_REG_ADDR         (0x05)
#define PCF2129_DAY_REG_ADDR        (0x06)
#define PCF2129_WEEKDAY_REG_ADDR    (0x07)
#define PCF2129_MON_REG_ADDR        (0x08)
#define PCF2129_YR_REG_ADDR         (0x09)

#define PCF2129_SEC_BIT_OSF         BIT7    /* oscillator fault bit in Register seconds */
#define PCF2129_CTRL3_BIT_BLF       BIT2    /* Battery Low Flag*/

// Define EPOCH (1998-JANUARY-01) in the Julian Date representation
#define EPOCH_JULIAN_DATE           2450815

STATIC CONST CHAR16           mTimeZoneVariableName[] = L"PCF2129RtcTimeZone";
STATIC CONST CHAR16           mDaylightVariableName[] = L"PCF2129RtcDaylight";
STATIC EFI_EVENT              mRtcVirtualAddrChangeEvent;
STATIC UINTN                  mI2c3BaseAddress;

BOOLEAN
IsLeapYear (
  IN EFI_TIME   *Time
  )
{
  if (Time->Year % 4 == 0) {
    if (Time->Year % 100 == 0) {
      if (Time->Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

BOOLEAN
DayValid (
  IN  EFI_TIME  *Time
  )
{
  STATIC CONST INTN DayOfMonth[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  if (Time->Day < 1 ||
      Time->Day > DayOfMonth[Time->Month - 1] ||
      (Time->Month == 2 && (!IsLeapYear (Time) && Time->Day > 28))
     ) {
    return FALSE;
  }

  return TRUE;
}

/*
 * returns Day of the week [0-6] 0=Sunday
 *
 * Don't try to provide a Year that's before 1998, please !
 */
UINTN
EfiTimeToWday (
  IN  EFI_TIME  *Time
  )
{
  UINTN a;
  UINTN y;
  UINTN m;
  UINTN JulianDate;  // Absolute Julian Date representation of the supplied Time
  UINTN EpochDays;   // Number of days elapsed since EPOCH_JULIAN_DAY

  a = (14 - Time->Month) / 12 ;
  y = Time->Year + 4800 - a;
  m = Time->Month + (12*a) - 3;

  JulianDate = Time->Day + ((153*m + 2)/5) + (365*y) + (y/4) - (y/100) + (y/400) - 32045;

  ASSERT (JulianDate >= EPOCH_JULIAN_DATE);
  EpochDays = JulianDate - EPOCH_JULIAN_DATE;

  /*
   * 4=1/1/1998 was a Thursday
   */
  return (EpochDays + 4) % 7;
}

/**
  Returns the current time and date information, and the time-keeping capabilities
  of the hardware platform.

  @param  Time                  A pointer to storage to receive a snapshot of the current time.
  @param  Capabilities          An optional pointer to a buffer to receive the real time clock
                                device's capabilities.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER Time is NULL.
  @retval EFI_DEVICE_ERROR      The time could not be retrieved due to hardware error.

**/

EFI_STATUS
EFIAPI
LibGetTime (
  OUT EFI_TIME                *Time,
  OUT  EFI_TIME_CAPABILITIES  *Capabilities
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  INT16       TimeZone;
  UINT8       Daylight;
  UINT8       Buffer[10] = { 0x00 };
  UINTN       Size;

  if (Time == NULL)
    return EFI_INVALID_PARAMETER;

  I2cDataWrite((VOID*)mI2c3BaseAddress,
                PCF2129_I2C_ADDR,
                PCF2129_CTRL1_REG_ADDR,
                0,
                Buffer,
                1
              );

  I2cDataRead((VOID*)mI2c3BaseAddress,
               PCF2129_I2C_ADDR,
               PCF2129_CTRL1_REG_ADDR,
               0,
               Buffer,
               sizeof(Buffer)
             );

  if (Buffer[PCF2129_CTRL3_REG_ADDR] & PCF2129_CTRL3_BIT_BLF) {
    DEBUG((EFI_D_INFO, "### Warning: RTC battery status low, check/replace RTC battery.\n"));
  }

  if (Buffer[PCF2129_SEC_REG_ADDR] & PCF2129_SEC_BIT_OSF) {
    DEBUG((EFI_D_ERROR, "### Warning: RTC oscillator has stopped, Date/Time is not reliable\n"));
    /*Clear OSF flag by command*/
    Buffer[PCF2129_SEC_REG_ADDR] |= PCF2129_SEC_BIT_OSF;
    I2cDataWrite((VOID*)mI2c3BaseAddress,
                  PCF2129_I2C_ADDR,
                  PCF2129_SEC_REG_ADDR,
                  1,
                  &Buffer[PCF2129_SEC_REG_ADDR],
                  1
                );
    Status = EFI_DEVICE_ERROR;
    goto EXIT;
  }

  Time->Nanosecond = 0;
  Time->Second  = Bin (Buffer[PCF2129_SEC_REG_ADDR] & 0x7F);
  Time->Minute  = Bin (Buffer[PCF2129_MIN_REG_ADDR] & 0x7F);
  Time->Hour = Bin (Buffer[PCF2129_HR_REG_ADDR] & 0x3F);
  Time->Day = Bin (Buffer[PCF2129_DAY_REG_ADDR] & 0x3F);
  Time->Month  = Bin (Buffer[PCF2129_MON_REG_ADDR] & 0x1F);
  Time->Year = Bin (Buffer[PCF2129_YR_REG_ADDR]) + ( Bin (Buffer[PCF2129_YR_REG_ADDR]) >= 98 ? 1900 : 2000);

  // Get the current time zone information from non-volatile storage
  Size = sizeof (TimeZone);
  Status = EfiGetVariable (
                  (CHAR16 *)mTimeZoneVariableName,
                  &gEfiCallerIdGuid,
                  NULL,
                  &Size,
                  (VOID *)&TimeZone
                  );

  if (EFI_ERROR (Status)) {
    ASSERT(Status != EFI_INVALID_PARAMETER);
    ASSERT(Status != EFI_BUFFER_TOO_SMALL);

    if (Status != EFI_NOT_FOUND)
      goto EXIT;

    // The time zone variable does not exist in non-volatile storage, so create it.
    Time->TimeZone = EFI_UNSPECIFIED_TIMEZONE;
    // Store it
    Status = EfiSetVariable (
                    (CHAR16 *)mTimeZoneVariableName,
                    &gEfiCallerIdGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    Size,
                    (VOID *)&(Time->TimeZone)
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        EFI_D_ERROR,
        "LibGetTime: Failed to save %s variable to non-volatile storage, Status = %r\n",
        mTimeZoneVariableName,
        Status
        ));
      goto EXIT;
    }
  } else {
    // Got the time zone
    Time->TimeZone = TimeZone;

    // Check TimeZone bounds:   -1440 to 1440 or 2047
    if (((Time->TimeZone < -1440) || (Time->TimeZone > 1440))
        && (Time->TimeZone != EFI_UNSPECIFIED_TIMEZONE)) {
      Time->TimeZone = EFI_UNSPECIFIED_TIMEZONE;
    }
  }

  // Get the current daylight information from non-volatile storage
  Size = sizeof (Daylight);
  Status = EfiGetVariable (
                  (CHAR16 *)mDaylightVariableName,
                  &gEfiCallerIdGuid,
                  NULL,
                  &Size,
                  (VOID *)&Daylight
                  );

  if (EFI_ERROR (Status)) {
    ASSERT(Status != EFI_INVALID_PARAMETER);
    ASSERT(Status != EFI_BUFFER_TOO_SMALL);

    if (Status != EFI_NOT_FOUND)
      goto EXIT;

    // The daylight variable does not exist in non-volatile storage, so create it.
    Time->Daylight = 0;
    // Store it
    Status = EfiSetVariable (
                    (CHAR16 *)mDaylightVariableName,
                    &gEfiCallerIdGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    Size,
                    (VOID *)&(Time->Daylight)
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        EFI_D_ERROR,
        "LibGetTime: Failed to save %s variable to non-volatile storage, Status = %r\n",
        mDaylightVariableName,
        Status
        ));
      goto EXIT;
    }
  } else {
    // Got the daylight information
    Time->Daylight = Daylight;
  }

  EXIT:
    return Status;
}


/**
  Sets the current local time and date information.

  @param  Time                  A pointer to the current time.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware error.

**/
EFI_STATUS
EFIAPI
LibSetTime (
  IN EFI_TIME                *Time
  )
{
  EFI_STATUS  Status;
  UINT8       Buffer[8] = { 0x00 };
  UINT8       Index = 0;

  // Check the input parameters are within the range specified by UEFI
  if ((Time->Year   < 1900) ||
       (Time->Year   > 9999) ||
       (Time->Month  < 1   ) ||
       (Time->Month  > 12  ) ||
       (!DayValid (Time)    ) ||
       (Time->Hour   > 23  ) ||
       (Time->Minute > 59  ) ||
       (Time->Second > 59  ) ||
       (Time->Nanosecond > 999999999) ||
       (!((Time->TimeZone == EFI_UNSPECIFIED_TIMEZONE) || ((Time->TimeZone >= -1440) && (Time->TimeZone <= 1440)))) ||
       (Time->Daylight & (~(EFI_TIME_ADJUST_DAYLIGHT | EFI_TIME_IN_DAYLIGHT)))
    ) {
    Status = EFI_INVALID_PARAMETER;
    goto EXIT;
  }

  if (Time->Year < 1998 || Time->Year > 2097)
    DEBUG((EFI_D_ERROR, "WARNING: Year should be between 1998 and 2097!\n"));

  /* start register address */
  Buffer[Index++] = PCF2129_SEC_REG_ADDR;

  /* hours, minutes and seconds */
  Buffer[Index++] = Bcd (Time->Second);
  Buffer[Index++] = Bcd (Time->Minute);
  Buffer[Index++] = Bcd (Time->Hour);
  Buffer[Index++] = Bcd (Time->Day);
  Buffer[Index++] = EfiTimeToWday (Time) & 0x07;
  Buffer[Index++] = Bcd (Time->Month);
  Buffer[Index++] = Bcd (Time->Year % 100);

  I2cDataWrite((VOID*)mI2c3BaseAddress,
                PCF2129_I2C_ADDR,
                PCF2129_CTRL1_REG_ADDR,
                0,
                Buffer,
                sizeof(Buffer)
               );

  // The accesses to Variable Services can be very slow, because we may be writing to Flash.
  // Do this after having set the RTC.

  // Save the current time zone information into non-volatile storage
  Status = EfiSetVariable (
                  (CHAR16 *)mTimeZoneVariableName,
                  &gEfiCallerIdGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (Time->TimeZone),
                  (VOID *)&(Time->TimeZone)
                  );
  if (EFI_ERROR (Status)) {
      DEBUG ((
        EFI_D_ERROR,
        "LibSetTime: Failed to save %s variable to non-volatile storage, Status = %r\n",
        mTimeZoneVariableName,
        Status
        ));
    goto EXIT;
  }

  // Save the current daylight information into non-volatile storage
  Status = EfiSetVariable (
                  (CHAR16 *)mDaylightVariableName,
                  &gEfiCallerIdGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(Time->Daylight),
                  (VOID *)&(Time->Daylight)
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      EFI_D_ERROR,
      "LibSetTime: Failed to save %s variable to non-volatile storage, Status = %r\n",
      mDaylightVariableName,
      Status
      ));
    goto EXIT;
  }

  EXIT:
    return Status;
}


/**
  Returns the current wakeup alarm clock setting.

  @param  Enabled               Indicates if the alarm is currently enabled or disabled.
  @param  Pending               Indicates if the alarm signal is pending and requires acknowledgement.
  @param  Time                  The current alarm setting.

  @retval EFI_SUCCESS           The alarm settings were returned.
  @retval EFI_INVALID_PARAMETER Any parameter is NULL.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be retrieved due to a hardware error.

**/
EFI_STATUS
EFIAPI
LibGetWakeupTime (
  OUT BOOLEAN     *Enabled,
  OUT BOOLEAN     *Pending,
  OUT EFI_TIME    *Time
  )
{
  // Not a required feature
  return EFI_UNSUPPORTED;
}


/**
  Sets the system wakeup alarm clock time.

  @param  Enabled               Enable or disable the wakeup alarm.
  @param  Time                  If Enable is TRUE, the time to set the wakeup alarm for.

  @retval EFI_SUCCESS           If Enable is TRUE, then the wakeup alarm was enabled. If
                                Enable is FALSE, then the wakeup alarm was disabled.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be set due to a hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this platform.

**/
EFI_STATUS
EFIAPI
LibSetWakeupTime (
  IN BOOLEAN      Enabled,
  OUT EFI_TIME    *Time
  )
{
  // Not a required feature
  return EFI_UNSUPPORTED;
}

/**
  Fixup internal data so that EFI can be call in virtual mode.
  Call the passed in Child Notify event and convert any pointers in
  lib to virtual mode.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
**/
VOID
EFIAPI
LibRtcVirtualNotifyEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  //
  // Only needed if you are going to support the OS calling RTC functions in virtual mode.
  // You will need to call EfiConvertPointer (). To convert any stored physical addresses
  // to virtual address. After the OS transistions to calling in virtual mode, all future
  // runtime calls will be made in virtual mode.
  //
  EfiConvertPointer (0x0, (VOID**)&mI2c3BaseAddress);
  return;
}

/**
  This is the declaration of an EFI image entry point. This can be the entry point to an application
  written to this specification, an EFI boot service driver, or an EFI runtime driver.

  @param  ImageHandle           Handle that identifies the loaded image.
  @param  SystemTable           System Table for this image.

  @retval EFI_SUCCESS           The operation completed successfully.

**/
EFI_STATUS
EFIAPI
LibRtcInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  //
  // Do some initialization if reqruied to turn on the RTC
  //
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE    Handle;

  Status = I2cBusInit(I2C3, I2C_SPEED);

  if (Status == EFI_SUCCESS) {
    Status = I2cProbeDevices(I2C3, PCF2129_I2C_ADDR);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR,"Failed to Probe Device @ Address 0x%x of I2C Bus %d Error=%d\n",
                          PCF2129_I2C_ADDR, I2C3, Status));
      return Status;
    }
  } else {
    DEBUG((EFI_D_ERROR,"Failed to Initialize I2C Bus %d Error=%d\n", I2C3, Status));
    return Status;
  }

  // Initialize RTC Base Address
  mI2c3BaseAddress = (UINTN)I2C3_BASE_ADDRESS;

  // Declare the controller as EFI_MEMORY_RUNTIME
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  mI2c3BaseAddress, I2C_SIZE,
                  EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gDS->SetMemorySpaceAttributes (mI2c3BaseAddress, I2C_SIZE, EFI_MEMORY_UC | EFI_MEMORY_RUNTIME);
  if (EFI_ERROR (Status))
    return Status;

  // Install the protocol
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiRealTimeClockArchProtocolGuid,  NULL,
                  NULL
                 );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for the virtual address change event
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  LibRtcVirtualNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mRtcVirtualAddrChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
