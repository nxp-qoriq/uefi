/** @file
  Implement EFI RealTimeClock with runtime services via RTC Lib for
  DS1307 and PCF85263 RTC.

  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <CpldLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/Ds1307RtcLib.h>
#include <Library/IoLib.h>
#include <Library/Pcf85263RtcLib.h>
#include <Library/RealTimeClockLib.h>
#include <LS1043aRdb.h>
#include <LS1043aSocLib.h>

STATIC BOOLEAN  mX2Board;

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
  EFI_STATUS  Status;

  if (Time == NULL)
    return EFI_INVALID_PARAMETER;

  if (mX2Board) {
    Status = Pcf85263LibGetTime (Time);
  } else {
    Status = Ds1307LibGetTime (Time);
  }

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

  if (Time == NULL)
    return EFI_INVALID_PARAMETER;

  if (mX2Board) {
    Status = Pcf85263LibSetTime (Time);
  } else {
    Status = Ds1307LibSetTime (Time);
  }

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
  // Do some initialization if required to turn on the RTC
  //
  EFI_STATUS    Status;
  struct CcsrGur *GurBase;
  UINTN Svr;

  GurBase = (VOID *)(GUTS_ADDR);
  Svr = MmioReadBe32 ((UINTN)&GurBase->svr);

  if (SVR_MINOR (Svr) == 1) {
    if (CPLD_READ (PcbaVersion) >= 4) {
       mX2Board = TRUE;
    } else {
       mX2Board = FALSE;
    }
  }

  if (mX2Board) {
    Status = Pcf85263LibRtcInitialize ();
  } else {
    Status = Ds1307LibRtcInitialize ();
  }

  return Status;
}
