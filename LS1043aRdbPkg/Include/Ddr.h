/** @Ddr.h
  Header defining the Ddr controller constants (Base addresses, sizes, flags),
  function prototype, structures etc

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DDR_H__
#define __DDR_H__

#include <Library/BaseMemoryLib.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

#include "Bitops.h"
#include "Common.h"
#include "LS1043aRdb.h"

/* DDR4 fixed timing */
#define CONFIG_CS0_BNDS            0x0000007f /* 0x000 */
#define CONFIG_CS0_CONFIG          0x80040322 /* 0x080 */
#define CONFIG_TIMING_CFG_3        0x010c1000 /* 0x100 */
#define CONFIG_TIMING_CFG_0        0x91550018 /* 0x104 */
#define CONFIG_TIMING_CFG_1        0xbbb48c42 /* 0x108 */
#define CONFIG_TIMING_CFG_2        0x0048C114 /* 0x10c */
#define CONFIG_DDR_SDRAM_CFG       0x450C0008 /* 0x110 */
#define CONFIG_DDR_SDRAM_CFG_2     0x00401100 /* 0x114 */
#define CONFIG_DDR_SDRAM_MODE      0x03010210 /* 0x118 */
#define CONFIG_DDR_SDRAM_MODE_3    0x00010210 /* 0x200 */
#define CONFIG_DDR_SDRAM_MODE_5    0x00010210 /* 0x208 */
#define CONFIG_DDR_SDRAM_MODE_7    0x00010210 /* 0x210 */
#define CONFIG_DDR_SDRAM_MODE_9    0x00000500 /* 0x220 */
#define CONFIG_DDR_SDRAM_MODE_10   0x04000000 /* 0x224 */
#define CONFIG_DDR_SDRAM_MODE_11   0x00000400 /* 0x228 */
#define CONFIG_DDR_SDRAM_MODE_12   0x04000000 /* 0x22c */
#define CONFIG_DDR_SDRAM_MODE_13   0x00000400 /* 0x230 */
#define CONFIG_DDR_SDRAM_MODE_14   0x04000000 /* 0x234 */
#define CONFIG_DDR_SDRAM_MODE_15   0x00000400 /* 0x238 */
#define CONFIG_DDR_SDRAM_MODE_16   0x04000000 /* 0x23c */
#define CONFIG_DDR_SDRAM_INTERVAL  0x18600618 /* 0x124 */
#define CONFIG_DDR_INIT_DATA       0xDEADBEEF /* 0x128 */
#define CONFIG_DDR_SDRAM_CLK_CNTL  0x03000000 /* 0x130 */
#define CONFIG_TIMING_CFG_4        0x00000002 /* 0x160 */
#define CONFIG_TIMING_CFG_5        0x03401400 /* 0x164 */
#define CONFIG_TIMING_CFG_7        0x13300000 /* 0x16c */
#define CONFIG_DDR_ZQ_CNTL         0x8A090705 /* 0x170 */
#define CONFIG_DDR_WRLVL_CNTL      0x8675F607 /* 0x174 */
#define CONFIG_DDR_WRLVL_CNTL_2    0x07090800 /* 0x190 */
#define CONFIG_TIMING_CFG_8        0x02115600 /* 0x250 */
#define CONFIG_DDRCDR_1            0x80040000 /* 0xb28 */
#define CONFIG_DDRCDR_2            0x0000A181 /* 0xb2c */
#define CONFIG_DDR_EOR             0x07100000 /* 0xc00 */ 

#define CONFIG_DDR_SDRAM_CFG_MEM_EN    0x80000000

#define SDRAM_INTERVAL_BSTOPRE         0x3FFF

/* DDR_EOR register */
#define DDR_EOR_READ_REOD_DIS          0x07000000
#define DDR_EOR_WRITE_REOD_DIS         0x00100000

/* DDR_SDRAM_CFG - DDR SDRAM Control Configuration */
#define SDRAM_DBW_MASK                 0x00180000
#define SDRAM_DBW_SHIFT                19
#define SDRAM_32_BE                    0x00080000
#define SDRAM_16_BE                    0x00100000
#define SDRAM_8_BE                     0x00040000
#define SDRAM_ECC_EN                   0x20000000
#define SDRAM_TYPE_MASK                0x07000000
#define SDRAM_TYPE_SHIFT               24

#define DDR3                           7
#define DDR4                           5

/**
  DDR memory controller registers
**/
struct CcsrDdr {
  UINT32	Cs0Bnds;		/** Chip Select 0 Memory Bounds */
  CHAR8	Res04[4];
  UINT32	Cs1Bnds;		/** Chip Select 1 Memory Bounds */
  CHAR8	Res0c[4];
  UINT32	Cs2Bnds;		/** Chip Select 2 Memory Bounds */
  CHAR8	Res14[4];
  UINT32	Cs3Bnds;		/** Chip Select 3 Memory Bounds */
  CHAR8	Res1c[100];
  UINT32	Cs0Config;		/** Chip Select Configuration */
  UINT32	Cs1Config;		/** Chip Select Configuration */
  UINT32	Cs2Config;		/** Chip Select Configuration */
  UINT32	Cs3Config;		/** Chip Select Configuration */
  CHAR8	Res90[48];
  UINT32	Cs0Config2;		/** Chip Select Configuration 2 */
  UINT32	Cs1Config2;		/** Chip Select Configuration 2 */
  UINT32	Cs2Config2;		/** Chip Select Configuration 2 */
  UINT32	Cs3Config2;		/** Chip Select Configuration 2 */
  CHAR8	Resd0[48];
  UINT32	TimingCfg3;		/** SDRAM Timing Configuration 3 */
  UINT32	TimingCfg0;		/** SDRAM Timing Configuration 0 */
  UINT32	TimingCfg1;		/** SDRAM Timing Configuration 1 */
  UINT32	TimingCfg2;		/** SDRAM Timing Configuration 2 */
  UINT32	SdramCfg;		/** SDRAM Control Configuration */
  UINT32	SdramCfg2;		/** SDRAM Control Configuration 2 */
  UINT32	SdramMode;		/** SDRAM Mode Configuration */
  UINT32	SdramMode2;		/** SDRAM Mode Configuration 2 */
  UINT32	SdramMdCntl;		/** SDRAM Mode Control */
  UINT32	SdramInterval;	/** SDRAM Interval Configuration */
  UINT32	SdramDataInit;	/** SDRAM Data initialization */
  CHAR8	Res12c[4];
  UINT32	SdramClkCntl;		/** SDRAM Clock Control */
  CHAR8	Res134[20];
  UINT32	InitAddr;		/** training init addr */
  UINT32	InitExtAddr;		/** training init extended addr */
  CHAR8	Res150[16];
  UINT32	TimingCfg4;		/** SDRAM Timing Configuration 4 */
  UINT32	TimingCfg5;		/** SDRAM Timing Configuration 5 */
  UINT32	TimingCfg6;		/** SDRAM Timing Configuration 6 */
  UINT32	TimingCfg7;		/** SDRAM Timing Configuration 7 */
  UINT32	DdrZqCntl;		/** ZQ calibration control*/
  UINT32	DdrWrlvlCntl;		/** write leveling control*/
  CHAR8	Reg178[4];
  UINT32	DdrSrCntr;		/** self refresh counter */
  UINT32	DdrSdramRcw1;		/** Control Words 1 */
  UINT32	DdrSdramRcw2;		/** Control Words 2 */
  CHAR8	Reg188[8];
  UINT32	DdrWrlvlCntl2;	/** write leveling control 2 */
  UINT32	DdrWrlvlCntl3;	/** write leveling control 3 */
  CHAR8	Res198[8];
  UINT32      DdrSdramRcw3;
  UINT32      DdrSdramRcw4;
  UINT32      DdrSdramRcw5;
  UINT32      DdrSdramRcw6;
  CHAR8       Res1b0[80];
  UINT32	SdramMode3;		/** SDRAM Mode Configuration 3 */
  UINT32	SdramMode4;		/** SDRAM Mode Configuration 4 */
  UINT32	SdramMode5;		/** SDRAM Mode Configuration 5 */
  UINT32	SdramMode6;		/** SDRAM Mode Configuration 6 */
  UINT32	SdramMode7;		/** SDRAM Mode Configuration 7 */
  UINT32	SdramMode8;		/** SDRAM Mode Configuration 8 */
  CHAR8       Res218[8];
  UINT32	SdramMode9;		/** SDRAM Mode Configuration 9 */
  UINT32	SdramMode10;		/** SDRAM Mode Configuration 10 */
  UINT32	SdramMode11;		/** SDRAM Mode Configuration 11 */
  UINT32	SdramMode12;		/** SDRAM Mode Configuration 12 */
  UINT32	SdramMode13;		/** SDRAM Mode Configuration 13 */
  UINT32	SdramMode14;		/** SDRAM Mode Configuration 14 */
  UINT32	SdramMode15;		/** SDRAM Mode Configuration 15 */
  UINT32	SdramMode16;		/** SDRAM Mode Configuration 16 */
  CHAR8       Res240[16];
  UINT32      TimingCfg8;        /* SDRAM Timing Configuration 8 */
  CHAR8       Res254[12];
  UINT32      SdramCfg3;
  CHAR8       Res264[412];
  UINT32      DqMap0;
  UINT32      DqMap1;
  UINT32      DqMap2;
  UINT32      DqMap3;
  CHAR8       Res410[1808];
  UINT32	DdrDsr1;		/** Debug Status 1 */
  UINT32	DdrDsr2;		/** Debug Status 2 */
  UINT32	DdrCdr1;		/** Control Driver 1 */
  UINT32	DdrCdr2;		/** Control Driver 2 */
  CHAR8	ResB30[200];
  UINT32	IpRev1;		/** IP Block Revision 1 */
  UINT32	IpRev2;		/** IP Block Revision 2 */
  UINT32	Eor;			/** Enhanced Optimization Register */
  CHAR8	ResC04[252];
  UINT32	Mtcr;			/** Memory Test Control Register */
  CHAR8	ResD04[28];
  UINT32	Mtp1;			/** Memory Test Pattern 1 */
  UINT32	Mtp2;			/** Memory Test Pattern 2 */
  UINT32	Mtp3;			/** Memory Test Pattern 3 */
  UINT32	Mtp4;			/** Memory Test Pattern 4 */
  UINT32	Mtp5;			/** Memory Test Pattern 5 */
  UINT32	Mtp6;			/** Memory Test Pattern 6 */
  UINT32	Mtp7;			/** Memory Test Pattern 7 */
  UINT32	Mtp8;			/** Memory Test Pattern 8 */
  UINT32	Mtp9;			/** Memory Test Pattern 9 */
  UINT32	Mtp10;			/** Memory Test Pattern 10 */
  CHAR8	ResD48[184];
  UINT32	DataErrInjectHi;	/** Data Path Err Injection Mask High */
  UINT32	DataErrInjectLo;	/** Data Path Err Injection Mask Low */
  UINT32	EccErrInject;		/** Data Path Err Injection Mask ECC */
  CHAR8	ResE0c[20];
  UINT32	CaptureDataHi;	/** Data Path Read Capture High */
  UINT32	CaptureDataLo;	/** Data Path Read Capture Low */
  UINT32	CaptureEcc;		/** Data Path Read Capture ECC */
  CHAR8	ResE2c[20];
  UINT32	ErrDetect;		/** Error Detect */
  UINT32	ErrDisable;		/** Error Disable */
  UINT32	ErrIntEn;
  UINT32	CaptureAttributes;	/** Error Attrs Capture */
  UINT32	CaptureAddress;	/** Error Addr Capture */
  UINT32	CaptureExtAddress;	/** Error Extended Addr Capture */
  UINT32	ErrSbe;		/** Single-Bit ECC Error Management */
  CHAR8	ResE5c[164];
  UINT32	Debug[32];		/** Debug_1 to Debug_32 */
  CHAR8	ResF80[128];
};

/**
  Main function to initialize DDR
 **/
VOID
DramInit(
  );

/**
  Function to dump DDRC registers

**/
VOID
DdrRegDump (
  VOID
  );
#endif
