/** @Ds1307Rtc.h

  Copyright 2017 NXP

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DS1307_H__
#define __DS1307_H__

#define Bin(Bcd) ((Bcd) & 0x0f) + ((Bcd) >> 4) * 10
#define Bcd(Bin) (((Bin / 10) << 4) | (Bin % 10))

#define DS1307_I2C_ADDR      0x68

/*
 * RTC register addresses
 */
#define DS1307_SEC_REG_ADDR  0x00
#define DS1307_MIN_REG_ADDR  0x01
#define DS1307_HR_REG_ADDR   0x02
#define DS1307_DAY_REG_ADDR  0x03
#define DS1307_DATE_REG_ADDR 0x04
#define DS1307_MON_REG_ADDR  0x05
#define DS1307_YR_REG_ADDR   0x06
#define DS1307_CTL_REG_ADDR  0x07

#define DS1307_SEC_BIT_CH    0x80  /* Clock Halt (in Register 0)   */

#define DS1307_CTL_BIT_RS0   0x01  /* Rate select 0                */
#define DS1307_CTL_BIT_RS1   0x02  /* Rate select 1                */
#define DS1307_CTL_BIT_SQWE  0x10  /* Square Wave Enable           */
#define DS1307_CTL_BIT_OUT   0x80  /* Output Control               */

#endif //__DS1307_H__
