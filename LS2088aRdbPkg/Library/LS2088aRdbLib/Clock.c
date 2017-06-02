/** Clock.c

  Copyright (c) 2013, Freescale Ltd. All rights reserved.
  Author: Bhupesh Sharma <bhupesh.sharma@freescale.com>
  
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/IoLib.h>
#include <Library/DebugLib.h>

VOID
ClockInitSerdes (
  VOID
  )
{
	/*
	 * FIXME: It seems we need to set-up clocks only for SerDes and
	 * no other clocks. Revisit this and add any clock
	 * configurations that may be required for LS2088a.
	 */ 
}
