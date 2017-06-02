/** LS1046aWatchDog.c
*
*  Based on Watchdog driver implemenation available in
*  ArmPlatformPkg/Drivers/SP805WatchdogDxe/SP805Watchdog.c
*
*  Copyright (c) 2011-2013, ARM Limited. All rights reserved.
*  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
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


#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/WatchdogTimer.h>
#include <Include/LS1046aRdb.h>

#define LS1046A_WT_MAX_TIME	128 
#define LS1046A_WD_COUNT(sec)	(((sec) * 2 - 1) << 8)
#define LS1046A_WD_SEC(cnt)	(((cnt) + 1) / 2)

EFI_EVENT                           EfiExitBootServicesEvent = (EFI_EVENT)NULL;

inline
VOID
LS1046aWdogPing (
  VOID
  )
{
  MmioWriteBe16(WDOG1_BASE_ADDR + WDOG_WSR_OFFSET, WDOG_SERVICE_SEQ1);
  MmioWriteBe16(WDOG1_BASE_ADDR + WDOG_WSR_OFFSET, WDOG_SERVICE_SEQ2);
}

/**
  Stop the LS1046aWdog watchdog timer from counting down by disabling interrupts.
**/
inline
VOID
LS1046aWdogStop (
  VOID
  )
{
  // LS1046a Watchdog cannot be disabled by software once started.
  // At best, we can keep pinging the watchdog
  LS1046aWdogPing();
}

/**
  Starts the LS1046aWdog counting down by enabling interrupts.
  The count down will start from the value stored in the Load register,
  not from the value where it was previously stopped.
**/
inline
VOID
LS1046aWdogStart (
  VOID
  )
{
  /* Watchdog is enabled already in LS1046aWdogInitialize - time to reload the timeout value */
  LS1046aWdogPing();
}

/**
    On exiting boot services we must make sure the LS1046aWdog Watchdog Timer
    is stopped.
**/
VOID
EFIAPI
ExitBootServicesEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  LS1046aWdogStop();
}

/**
  This function registers the handler NotifyFunction so it is called every time
  the watchdog timer expires.  It also passes the amount of time since the last
  handler call to the NotifyFunction.
  If NotifyFunction is not NULL and a handler is not already registered,
  then the new handler is registered and EFI_SUCCESS is returned.
  If NotifyFunction is NULL, and a handler is already registered,
  then that handler is unregistered.
  If an attempt is made to register a handler when a handler is already registered,
  then EFI_ALREADY_STARTED is returned.
  If an attempt is made to unregister a handler when a handler is not registered,
  then EFI_INVALID_PARAMETER is returned.

  @param  This             The EFI_TIMER_ARCH_PROTOCOL instance.
  @param  NotifyFunction   The function to call when a timer interrupt fires. This
                           function executes at TPL_HIGH_LEVEL. The DXE Core will
                           register a handler for the timer interrupt, so it can know
                           how much time has passed. This information is used to
                           signal timer based events. NULL will unregister the handler.

  @retval EFI_SUCCESS           The watchdog timer handler was registered.
  @retval EFI_ALREADY_STARTED   NotifyFunction is not NULL, and a handler is already
                                registered.
  @retval EFI_INVALID_PARAMETER NotifyFunction is NULL, and a handler was not
                                previously registered.

**/
EFI_STATUS
EFIAPI
LS1046aWdogRegisterHandler (
  IN CONST EFI_WATCHDOG_TIMER_ARCH_PROTOCOL   *This,
  IN EFI_WATCHDOG_TIMER_NOTIFY                NotifyFunction
  )
{
  // ERROR: This function is not supported.
  // The hardware watchdog will reset the board
  return EFI_INVALID_PARAMETER;
}

/**

  This function adjusts the period of timer interrupts to the value specified
  by TimerPeriod.  If the timer period is updated, then the selected timer
  period is stored in EFI_TIMER.TimerPeriod, and EFI_SUCCESS is returned.  If
  the timer hardware is not programmable, then EFI_UNSUPPORTED is returned.
  If an error occurs while attempting to update the timer period, then the
  timer hardware will be put back in its state prior to this call, and
  EFI_DEVICE_ERROR is returned.  If TimerPeriod is 0, then the timer interrupt
  is disabled.  This is not the same as disabling the CPU's interrupts.
  Instead, it must either turn off the timer hardware, or it must adjust the
  interrupt controller so that a CPU interrupt is not generated when the timer
  interrupt fires.

  @param  This             The EFI_TIMER_ARCH_PROTOCOL instance.
  @param  TimerPeriod      The rate to program the timer interrupt in 100 nS units. If
                           the timer hardware is not programmable, then EFI_UNSUPPORTED is
                           returned. If the timer is programmable, then the timer period
                           will be rounded up to the nearest timer period that is supported
                           by the timer hardware. If TimerPeriod is set to 0, then the
                           timer interrupts will be disabled.


  @retval EFI_SUCCESS           The timer period was changed.
  @retval EFI_UNSUPPORTED       The platform cannot change the period of the timer interrupt.
  @retval EFI_DEVICE_ERROR      The timer period could not be changed due to a device error.

**/
EFI_STATUS
EFIAPI
LS1046aWdogSetTimerPeriod (
  IN CONST EFI_WATCHDOG_TIMER_ARCH_PROTOCOL   *This,
  IN UINT64                                   TimerPeriod   // In 100ns units
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT64      TimerPeriodInSec;
  UINT16      Val;

  if( TimerPeriod == 0 ) {
    // This is a watchdog stop request
    LS1046aWdogStop();
    goto EXIT;
  } else {
    // Convert the TimerPeriod (in 100 ns unit) to an equivalent second value

    TimerPeriodInSec = DivU64x32(TimerPeriod, 10000000);

    // The registers in the LS1046aWdog are only 32 bits
    if(TimerPeriodInSec > LS1046A_WT_MAX_TIME) {
      // We could load the watchdog with the maximum supported value but
      // if a smaller value was requested, this could have the watchdog
      // triggering before it was intended.
      // Better generate an error to let the caller know.
      Status = EFI_DEVICE_ERROR;
      goto EXIT;
    }

    // set the new timeout value in the WCR
    Val = MmioReadBe16(WDOG1_BASE_ADDR + WDOG_WCR_OFFSET);
    Val &= ~WDOG_WCR_WT;
    // Convert the timeout value from Seconds to timer count
    Val |= ((LS1046A_WD_COUNT(TimerPeriodInSec) & 0xff00) << 8);
    MmioWriteBe16(WDOG1_BASE_ADDR + WDOG_WCR_OFFSET, Val);

    // Start the watchdog
    LS1046aWdogStart();
  }

  EXIT:
  return Status;
}

/**
  This function retrieves the period of timer interrupts in 100 ns units,
  returns that value in TimerPeriod, and returns EFI_SUCCESS.  If TimerPeriod
  is NULL, then EFI_INVALID_PARAMETER is returned.  If a TimerPeriod of 0 is
  returned, then the timer is currently disabled.

  @param  This             The EFI_TIMER_ARCH_PROTOCOL instance.
  @param  TimerPeriod      A pointer to the timer period to retrieve in 100 ns units. If
                           0 is returned, then the timer is currently disabled.


  @retval EFI_SUCCESS           The timer period was returned in TimerPeriod.
  @retval EFI_INVALID_PARAMETER TimerPeriod is NULL.

**/
EFI_STATUS
EFIAPI
LS1046aWdogGetTimerPeriod (
  IN CONST EFI_WATCHDOG_TIMER_ARCH_PROTOCOL   *This,
  OUT UINT64                                  *TimerPeriod
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT64      ReturnValue;
  UINT16      Val;

  if (TimerPeriod == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Check if the watchdog is stopped
  if ( (MmioReadBe16(WDOG1_BASE_ADDR + WDOG_WCR_OFFSET) & WDOG_WCR_WDE) == 0 ) {
    // It is stopped, so return zero.
    ReturnValue = 0;
  } else {
    // Convert the Watchdog ticks into equivalent TimerPeriod second
    // value. 
    Val = (MmioReadBe16(WDOG1_BASE_ADDR + WDOG_WCR_OFFSET) & WDOG_WCR_WT ) >> 8;
    ReturnValue = LS1046A_WD_SEC(Val);
  }

  *TimerPeriod = ReturnValue;
  return Status;
}

/**
  Interface structure for the Watchdog Architectural Protocol.

  @par Protocol Description:
  This protocol provides a service to set the amount of time to wait
  before firing the watchdog timer, and it also provides a service to
  register a handler that is invoked when the watchdog timer fires.

  @par When the watchdog timer fires, control will be passed to a handler
  if one has been registered.  If no handler has been registered,
  or the registered handler returns, then the system will be
  reset by calling the Runtime Service ResetSystem().

  @param RegisterHandler
  Registers a handler that will be called each time the
  watchdogtimer interrupt fires.  TimerPeriod defines the minimum
  time between timer interrupts, so TimerPeriod will also
  be the minimum time between calls to the registered
  handler.
  NOTE: If the watchdog resets the system in hardware, then
        this function will not have any chance of executing.

  @param SetTimerPeriod
  Sets the period of the timer interrupt in 100 nS units.
  This function is optional, and may return EFI_UNSUPPORTED.
  If this function is supported, then the timer period will
  be rounded up to the nearest supported timer period.

  @param GetTimerPeriod
  Retrieves the period of the timer interrupt in 100 nS units.

**/
EFI_WATCHDOG_TIMER_ARCH_PROTOCOL    gWatchdogTimer = {
  (EFI_WATCHDOG_TIMER_REGISTER_HANDLER) LS1046aWdogRegisterHandler,
  (EFI_WATCHDOG_TIMER_SET_TIMER_PERIOD) LS1046aWdogSetTimerPeriod,
  (EFI_WATCHDOG_TIMER_GET_TIMER_PERIOD) LS1046aWdogGetTimerPeriod
};

/**
  Initialize the state information for the Watchdog Timer Architectural Protocol.

  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      Hardware problems

**/
EFI_STATUS
EFIAPI
LS1046aWdogInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  UINT16      Val;


  Val = MmioReadBe16(WDOG1_BASE_ADDR + WDOG_WCR_OFFSET);

	Val &= ~WDOG_WCR_WT;

	Val &= ~WDOG_WCR_WDE;
	
  Val |= LS1046A_WD_COUNT(LS1046A_WT_MAX_TIME) & 0xff00;

  MmioWriteBe16(WDOG1_BASE_ADDR + WDOG_WCR_OFFSET, Val);

  /* FIXME:not enabling the watchdog for now, since Gic is under debug*/
  Val |= WDOG_WCR_WDE;

  //MmioWriteBe16(WDOG1_BASE_ADDR + WDOG_WCR_OFFSET, Val);

  //
  // Make sure the Watchdog Timer Architectural Protocol has not been installed in the system yet.
  // This will avoid conflicts with the universal watchdog
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiWatchdogTimerArchProtocolGuid);

  // Register for an ExitBootServicesEvent
  Status = gBS->CreateEvent (EVT_SIGNAL_EXIT_BOOT_SERVICES, TPL_NOTIFY, ExitBootServicesEvent, NULL, &EfiExitBootServicesEvent);
  if (EFI_ERROR(Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  // Install the Timer Architectural Protocol onto a new handle
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces(
                  &Handle,
                  &gEfiWatchdogTimerArchProtocolGuid, &gWatchdogTimer,
                  NULL
                  );
  if (EFI_ERROR(Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

EXIT:
  if(EFI_ERROR(Status)) {
    // The watchdog failed to initialize
    ASSERT(FALSE);
  }
	LS1046aWdogPing();
  return Status;
}
