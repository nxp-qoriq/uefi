/** @FslDdr.h

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

// Generic DDR driver support for DDR memory controler on Freescale QorIQ SoC.

#ifndef __DDR_H__
#define __DDR_H__

#include "LS2088aRdb.h"
#include "Bitops.h"
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>


// Big-Endian on LS102x/LS1043 and POWER SoCs, Little-Endian on LS1088, LS2088, etc
#if 0
#ifdef DDR_BIG_ENDIAN
#define DdrRead32(a)       MmioReadBe32((UINTN)a)
#define DdrWrite32(a, v)   MmioWriteBe32((UINTN)a, v)
#else
#define DdrRead32(a)       MmioRead32((UINTN)a)
#define DdrWrite32(a, v)   MmioWrite32((UINTN)a, v)
#endif
#endif


typedef UINTN PhysSizeT;

#define CONFIG_MEM_INIT_VALUE    0x0

// DDR_SDRAM_CFG - DDR SDRAM Control Configuration
#define SDRAM_CFG_DBW_MASK   0x00180000
#define SDRAM_CFG_DBW_SHIFT  19

#define SDRAM_CFG2_D_INIT    0x00000010
#define SDRAM_CFG2_ODT_ONLY_READ  2
#define SDRAM_CFG2_AP_EN           0x00000020


#if defined(MEMORY_TYPE_DDR3)
typedef struct Ddr3SpdEepromS GenericSpdEepromT;
#ifndef CONFIG_FSL_SDRAM_TYPE
#define CONFIG_FSL_SDRAM_TYPE   SDRAM_TYPE_DDR3
#endif
#elif defined(MEMORY_TYPE_DDR4)
#define DDR_MIN_TCKE_PULSE_WIDTH_DDR  3
typedef struct Ddr4SpdEepromS GenericSpdEepromT;
#ifndef CONFIG_FSL_SDRAM_TYPE
#define CONFIG_FSL_SDRAM_TYPE   SDRAM_TYPE_DDR4
#endif
#endif

/* DDR ERR_DISABLE */
#define DDR_APARITY_ERR_DISABLE      (1 << 8)  /* Address parity error disable */

#define EDC_ECC               2
#define HWCONFIG_BUFFER_SIZE  128
#define DDR3_RTT_OFF          0
#define DDR3_RTT_120_OHM      2 // RTT_Nom = RZQ/2
#define DDR3_RTT_40_OHM       3 // RTT_Nom = RZQ/6
#define DDR3_RTT_20_OHM       4 // RTT_Nom = RZQ/12
#define DDR3_RTT_30_OHM       5 // RTT_Nom = RZQ/8
#define DDR_ODT_NEVER         0x0
#define DDR_ODT_CS            0x1
#define DDR_ODT_OTHER_DIMM    0x3
#define DDR_ODT_ALL           0x4
#define DDR_ODT_SAME_DIMM     0x5
#define DDR_ODT_CS_AND_OTHER_DIMM  0x6
#define DDR_BL4    4        // burst length 4
#define DDR_BC4    DDR_BL4  // burst chop for ddr3
#define DDR_OTF    6        // on-the-fly BC4 and BL8
#define DDR_BL8    8        // burst length 8

// memory controller interleaving mode
#define DDR_CACHE_LINE_INTERLEAVING  0x0
#define DDR_PAGE_INTERLEAVING        0x1
#define DDR_BANK_INTERLEAVING        0x2
#define DDR_SUPERBANK_INTERLEAVING   0x3
#define DDR_256B_INTERLEAVING        0x8
#define DDR_3WAY_1KB_INTERLEAVING    0xA
#define DDR_3WAY_4KB_INTERLEAVING    0xC
#define DDR_3WAY_8KB_INTERLEAVING    0xD

// placeholder for 4-way interleaving
#define DDR_4WAY_1KB_INTERLEAVING  0x1A
#define DDR_4WAY_4KB_INTERLEAVING  0x1C
#define DDR_4WAY_8KB_INTERLEAVING  0x1D
#define SDRAM_CS_CONFIG_EN     0x80000000
#define CONFIG_EMU_DRAM_SIZ    (1 << 30)

// bank(chip select) interleaving mode
#define DDR_CS0_CS1      0x40
#define DDR_CS2_CS3      0x20
#define DDR_CS0_CS1_AND_CS2_CS3  (DDR_CS0_CS1 | DDR_CS2_CS3)
#define DDR_CS0_CS1_CS2_CS3    (DDR_CS0_CS1_AND_CS2_CS3 | 0x04)

#define DDR_VER_4_4    44
#define DDR_VER_4_6    46
#define DDR_VER_4_7    47
#define DDR_VER_5_0    50

#ifndef DDR_CTRL_VERSION
#define DDR_CTRL_VERSION     DDR_VER_5_0
#endif

#if defined(DDR_CTRL_VERSION) && (DDR_CTRL_VERSION > DDR_VER_4_4)
#define RD_TO_PRE_MASK        0xf
#define RD_TO_PRE_SHIFT       13
#define WR_DATA_DELAY_MASK    0xf
#define WR_DATA_DELAY_SHIFT   9
#else
#define RD_TO_PRE_MASK        0x7
#define RD_TO_PRE_SHIFT       13
#define WR_DATA_DELAY_MASK    0x7
#define WR_DATA_DELAY_SHIFT   10
#endif

/* DDR_SDRAM_CFG - DDR SDRAM Control Configuration */
#define SDRAM_CFG_ECC_EN           0x20000000
#define SDRAM_CFG_RD_EN            0x10000000
#define SDRAM_CFG_MEM_EN           0x80000000
#define SDRAM_CFG_BI               0x00000001
#define SDRAM_CFG_SDRAM_TYPE_MASK  0x07000000
#define SDRAM_CFG_SDRAM_TYPE_SHIFT 24
#define SDRAM_CFG_32_BE            0x00080000
#define SDRAM_CFG_16_BE            0x00100000

#define SDRAM_TYPE_DDR1  2
#define SDRAM_TYPE_DDR2  3
#define SDRAM_TYPE_DDR3  7
#define SDRAM_TYPE_DDR4  5

#define SDRAM_INTERVAL_BSTOPRE       0x3FFF

#define DDR_MODULETYPE_EXT          0x00
#define DDR_MODULETYPE_RDIMM        0x01
#define DDR_MODULETYPE_UDIMM        0x02
#define DDR_MODULETYPE_SO_DIMM      0x03
#define DDR_MODULETYPE_MICRO_DIMM   0x04
#define DDR_MODULETYPE_MINI_RDIMM   0x05
#define DDR_MODULETYPE_MINI_UDIMM   0x06
#define DDR_MODULETYPE_MINI_CDIMM   0x07
#define DDR_MODULETYPE_72B_SO_UDIMM 0x08
#define DDR_MODULETYPE_72B_SO_RDIMM 0x09
#define DDR_MODULETYPE_72B_SO_CDIMM 0x0A
#define DDR_MODULETYPE_LRDIMM       0x0B
#define DDR_MODULETYPE_16B_SO_DIMM  0x0C
#define DDR_MODULETYPE_32B_SO_DIMM  0x0D
#define DDR_MODULETYPE_MASK         0x0F

#define SPD_MEMTYPE_DDR3    0x0B
#define SPD_MEMTYPE_DDR4    0x0C

#define DDRCDR1_DHC_EN     0x80000000
#define DDRCDR1_ODT_SHIFT  17
#define DDRCDR1_ODT_MASK   0x6
#define DDRCDR2_ODT_MASK   0x1
#define DDRCDR1_ODT(x)   ((x & DDRCDR1_ODT_MASK) << DDRCDR1_ODT_SHIFT)
#define DDRCDR2_ODT(x)   (x & DDRCDR2_ODT_MASK)
#define DDRCDR2_VREF_OVRD(x)  (0x00008080 | ((((x) - 37) & 0x3F) << 8))
#define DDRCDR2_VREF_TRAIN_EN  0x00000080
#define DDRCDR2_VREF_RANGE_2   0x00000040

#if (defined(DDR_CTRL_VERSION) && (DDR_CTRL_VERSION >= DDR_VER_4_7))
#ifdef MEMORY_TYPE_DDR3L
#define DDRCDR_ODT_OFF     0x0
#define DDRCDR_ODT_120ohm  0x1
#define DDRCDR_ODT_200ohm  0x2
#define DDRCDR_ODT_75ohm   0x3
#define DDRCDR_ODT_60ohm   0x5
#define DDRCDR_ODT_46ohm   0x7
#elif defined(MEMORY_TYPE_DDR4)
#define DDRCDR_ODT_OFF     0x0
#define DDRCDR_ODT_100ohm  0x1
#define DDRCDR_ODT_120OHM  0x2
#define DDRCDR_ODT_80ohm   0x3
#define DDRCDR_ODT_60ohm   0x4
#define DDRCDR_ODT_40ohm   0x5
#define DDRCDR_ODT_50ohm   0x6
#define DDRCDR_ODT_30ohm   0x7
#else
#define DDRCDR_ODT_OFF     0x0
#define DDRCDR_ODT_120ohm  0x1
#define DDRCDR_ODT_180ohm  0x2
#define DDRCDR_ODT_75ohm   0x3
#define DDRCDR_ODT_110ohm  0x4
#define DDRCDR_ODT_60hm    0x5
#define DDRCDR_ODT_70ohm   0x6
#define DDRCDR_ODT_47ohm   0x7
#endif /* DDR3L */
#else
#define DDRCDR_ODT_75ohm   0x0
#define DDRCDR_ODT_55ohm   0x1
#define DDRCDR_ODT_60ohm   0x2
#define DDRCDR_ODT_50ohm   0x3
#define DDRCDR_ODT_150ohm  0x4
#define DDRCDR_ODT_43ohm   0x5
#define DDRCDR_ODT_120ohm  0x6
#endif

// DDR SDRAM mode control register
#define DDR_MODE_SEL_MR      (0 << 24)
#define DDR_MODE_SEL_EMR     (1 << 24)
#define DDR_MODE_SEL_EMR2    (2 << 24)
#define DDR_MODE_SEL_EMR3    (3 << 24)
#define DDR_MODE_SET_REF     (1 << 23)
#define DDR_MODE_SET_PRE     (1 << 22)
#define DDR_MODE_CKE_HIGH    (1 << 21)
#define DDR_MODE_CKE_LOW     (1 << 20)
#define DDR_MODE_WRCW        (1 << 19)
#define DDR_MODE_SEL_CS0     (0 << 28)
#define DDR_MODE_SEL_CS1     (1 << 28)
#define DDR_MODE_SEL_CS2     (2 << 28)
#define DDR_MODE_SEL_CS3     (3 << 28)
#define DDR_MODE_SEL_CS0_CS1 (4 << 28)
#define DDR_MODE_SEL_CS2_CS3 (5 << 28)
#define DDR_MODE_EN          (1 << 31)
#define DDR_MODE_MD_SEL(v)   (((v) & 0xF) << 24)
#define DDR_MODE_CS_SEL(v)   (((v) & 0x7) << 28)
#define DDR_MODE_MD_MASK(v)  (v & 0xFFFF)

#define ULL_2E12       2000000000000ULL
#define UL_5POW12      244140625UL
#define UL_2POW13      (1UL << 13)
#define ULL_8FS        0xFFFFFFFFULL

/* DEBUG 29 register */
#define DDR_TX_DSKW_DISABLE		(1 << 10) /* Transmit Bit Deskew Disable */
 
/* DEBUG_26 register */
#define CAS_TO_PRE_SUB_MASK  	0x0000f000 /* CAS to preamble subtract value */
#define CAS_TO_PRE_SUB_SHIFT 	12

// ODT (On Die Termination) parameters
struct DynamicOdt {
  UINT32 OdtRdCfg;
  UINT32 OdtWrCfg;
  UINT32 OdtRttNorm;
  UINT32 OdtRttWr;
};

// board-specific parameters
struct BoardSpecificParameters {
  UINT32 NRanks;
  UINT32 DatarateMhzHigh;
  UINT32 RankGb;
  UINT32 ClkAdjust;
  UINT32 WrlvlStart;
  UINT32 WrlvlCtl2;
  UINT32 WrlvlCtl3;
};

typedef struct FslDdrCfgRegsS {
  struct {
    UINT32 Bnds;
    UINT32 Config;
    UINT32 Config2;
  } Cs[DDR_CHIP_SELECTS_PER_CTRL];
  UINT32 TimingCfg3;
  UINT32 TimingCfg0;
  UINT32 TimingCfg1;
  UINT32 TimingCfg2;
  UINT32 DdrSdramCfg;
  UINT32 DdrSdramCfg2;
  UINT32 DdrSdramCfg3;
  UINT32 DdrSdramMode;
  UINT32 DdrSdramMode2;
  UINT32 DdrSdramMode3;
  UINT32 DdrSdramMode4;
  UINT32 DdrSdramMode5;
  UINT32 DdrSdramMode6;
  UINT32 DdrSdramMode7;
  UINT32 DdrSdramMode8;
  UINT32 DdrSdramMode9;
  UINT32 DdrSdramMode10;
  UINT32 DdrSdramMode11;
  UINT32 DdrSdramMode12;
  UINT32 DdrSdramMode13;
  UINT32 DdrSdramMode14;
  UINT32 DdrSdramMode15;
  UINT32 DdrSdramMode16;
  UINT32 DdrSdramMdCntl;
  UINT32 DdrSdramInterval;
  UINT32 DdrDataInit;
  UINT32 DdrSdramClkCntl;
  UINT32 DdrInitAddr;
  UINT32 DdrInitExtAddr;
  UINT32 TimingCfg4;
  UINT32 TimingCfg5;
  UINT32 TimingCfg6;
  UINT32 TimingCfg7;
  UINT32 TimingCfg8;
  UINT32 TimingCfg9;
  UINT32 DdrZqCntl;
  UINT32 DdrWrlvlCntl;
  UINT32 DdrWrlvlCntl2;
  UINT32 DdrWrlvlCntl3;
  UINT32 DdrSrCntr;
  UINT32 DdrSdramRcw1;
  UINT32 DdrSdramRcw2;
  UINT32 DdrSdramRcw3;
  UINT32 DdrSdramRcw4;
  UINT32 DdrSdramRcw5;
  UINT32 DdrSdramRcw6;
  UINT32 DqMap0;
  UINT32 DqMap1;
  UINT32 DqMap2;
  UINT32 DqMap3;
  UINT32 DdrEor;
  UINT32 DdrCdr1;
  UINT32 DdrCdr2;
  UINT32 ErrDisable;
  UINT32 ErrIntEn;
  UINT32 Debug[64];
} DdrCfgRegsT;


typedef struct {
  UINT32 TckminXPs;
  UINT32 TckmaxPs;
  UINT32 TrcdPs;
  UINT32 TrpPs;
  UINT32 TrasPs;
  UINT32 TaaminPs;
#ifdef MEMORY_TYPE_DDR4
  UINT32 Trfc1Ps;
  UINT32 Trfc2Ps;
  UINT32 Trfc4Ps;
  UINT32 TrrdsPs;
  UINT32 TrrdlPs;
  UINT32 TccdlPs;
#endif
  UINT32 TwtrPs;  // maximum = 63750 ps
  UINT32 TrfcPs;  // maximum = 255 ns + 256 ns + 0.75 ns = 511750 ps
  UINT32 TrrdPs;  // maximum = 63750 ps
  UINT32 TrtpPs;  // byte 38, spd->trtp
  UINT32 TwrPs;   // maximum = 63750 ps
  UINT32 TrcPs;   // maximum = 254 ns + 0.75 ns = 254750 ps
  UINT32 RefreshRatePs;
  UINT32 ExtendedOpSrt;
  UINT32 NdimmsPresent;
  UINT32 LowestCommonSpdCaslat;
  UINT32 HighestCommonDeratedCaslat;
  UINT32 AdditiveLatency;
  UINT32 AllDimmsBurstLengthsBitmask;
  UINT32 AllDimmsRegistered;
  UINT32 AllDimmsUnbuffered;
  UINT32 AllDimmsEccCapable;
  UINT64 TotalMem;
  UINT64 BaseAddress;
  UINT8 Rcw[16];     // Register Control Word
} CommonTimingParamsT;


typedef struct MemctlOptionsS {
  // Memory organization parameters
  UINT32 RegisteredDimmEn;    // use Registered DIMM
  // Options local to a chip select
  struct CsLocalOptsS {
    UINT32 AutoPrecharge;
    UINT32 OdtRdCfg;
    UINT32 OdtWrCfg;
    UINT32 OdtRttNorm;
    UINT32 OdtRttWr;
  } CsLocalOpts[DDR_CHIP_SELECTS_PER_CTRL];
  // Special configurations for chip select
  UINT32 MemctlInterleaving;
  UINT32 MemctlInterleavingMode;
  UINT32 BaIntlvCtl;
  UINT32 AddrHash;
  UINT32 EccMode;            // 1:use ECC, 0: not use ECC
  UINT32 EccInitUsingMemctl;
  UINT32 DqsConfig;
  UINT32 SelfRefreshInSleep; // SREN - self-refresh during sleep
  UINT32 DynamicPower;       // DYN_PWR
  UINT32 DataBusWidth;
  UINT32 BurstLength;
  UINT32 OtfBurstChopEn;    // On-the-fly burst chop enable
  UINT32 MirroredDimm;      // Mirrior DIMMs for DDR3
  UINT32 QuadRankPresent;
  UINT32 ApEn;             // Address parity enable for RDIMM
  UINT32 X4En;             // Enable x4 devices
  // Global timing parameters
  UINT32 CasLatencyOverride;
  UINT32 CasLatencyOverrideValue;
  UINT32 UseDeratedCaslat;
  UINT32 AdditiveLatencyOverride;
  UINT32 AdditiveLatencyOverrideValue;
  UINT32 ClkAdjust;
  UINT32 CpoOverride;
  UINT32 WriteDataDelay;     // DQS adjust
  UINT32 CswlOverride;
  UINT32 WrlvlOverride;
  UINT32 WrlvlSample;        // write leveling
  UINT32 WrlvlStart;
  UINT32 WrlvlCtl2;
  UINT32 WrlvlCtl3;
  UINT32 HalfStrengthDriverEnable;
  UINT32 TwotEn;
  UINT32 ThreetEn;
  UINT32 Bstopre;
  UINT32 TfawWindowFourActivatesPs;
  UINT32 RttOverride;              // RttOverride enable
  UINT32 RttOverrideValue;    // Rtt_Nom for DDR3
  UINT32 RttWrOverrideValue;  // Rtt_WR for DDR3
  UINT32 AutoSelfRefreshEn;   // Automatic self refresh
  UINT32 SrIt;
  UINT32 ZqEn;                // ZQ calibration
  UINT32 WrlvlEn;             // Write leveling
  UINT32 RcwOverride;         // RCW override for RDIMM
  UINT32 Rcw1;
  UINT32 Rcw2;
  UINT32 DdrCdr1;            // Control register 1
  UINT32 DdrCdr2;
  UINT32 TrwtOverride;
  UINT32 Trwt;               // read-to-write turnaround
} MemctlOptionsT;


typedef struct DimmParamsS {
  // DIMM organization parameters
  UINT8 Mpart[19];
  UINT32 NRanks;
  UINT64 RankDensity;
  UINT64 Capacity;
  UINT32 DataWidth;
  UINT32 PrimarySdramWidth;
  UINT32 EcSdramWidth;
  UINT32 RegisteredDimm;
  UINT32 DeviceWidth;
  UINT32 NRowAddr;
  UINT32 NColAddr;
  UINT32 EdcConfig;          // 0 = none, 1 = parity, 2 = ECC
#ifdef MEMORY_TYPE_DDR4
  UINT32 BankAddrBits;
  UINT32 BankGroupBits;
#endif
  UINT32 NBanksPerSdramDevice;
  UINT32 BurstLengthsBitmask;  // BL=4 bit 2, BL=8 bit 3
  UINT32 RowDensity;
  UINT64 BaseAddress;
  UINT32 MirroredDimm;  // only for DDR3
  // DIMM timing parameters
  INT32 MtbPs;         // medium timebase ps
  INT32 Ftb10thPs;     // fine timebase, in 1/10 ps
  INT32 TaaPs;         // minimum CAS latency time
  INT32 TfawPs;        // four active window delay
  // SDRAM clock periods
  INT32 TckminXPs;
  INT32 TckminXMinus1Ps;
  INT32 TckminXMinus2Ps;
  INT32 TckmaxPs;
  // SPD-defined CAS latencies
  UINT32 CaslatX;
  UINT32 CaslatXMinus1;
  UINT32 CaslatXMinus2;
  UINT32 CaslatLowestDerated;  // Derated CAS latency
  // Basic timing parameters
  INT32 TrcdPs;
  INT32 TrpPs;
  INT32 TrasPs;
#ifdef MEMORY_TYPE_DDR4
  INT32 Trfc1Ps;
  INT32 Trfc2Ps;
  INT32 Trfc4Ps;
  INT32 TrrdsPs;
  INT32 TrrdlPs;
  INT32 TccdlPs;
#endif
  INT32 TwrPs;           // maximum = 63750 ps
  INT32 TrfcPs;          // max = 255 ns + 256 ns + .75 ns = 511750 ps
  INT32 TrrdPs;          // maximum = 63750 ps
  INT32 TwtrPs;          // maximum = 63750 ps
  INT32 TrtpPs;          // byte 38, spd->trtp
  INT32 TrcPs;           // maximum = 254 ns + .75 ns = 254750 ps */
  INT32 RefreshRatePs;
  INT32 ExtendedOpSrt;
  UINT8 Rcw[16];         // Register Control Word
#ifdef MEMORY_TYPE_DDR4
  UINT32 DqMapping[18];
  UINT32 DqMappingOrs;
#endif
} DimmParamsT;

typedef struct Ddr3SpdEepromS {
  // general section: bytes 0-59
  UINT8 InfoSizeCrc;    //  0 # bytes written into serial memory, CRC coverage
  UINT8 SpdRev;         //  1 Total # bytes of SPD mem device
  UINT8 MemType;        //  2 Key Byte / Fundamental mem type
  UINT8 ModuleType;     //  3 Key Byte / Module Type
  UINT8 DensityBanks;   //  4 SDRAM Density and Banks
  UINT8 Addressing;     //  5 SDRAM Addressing
  UINT8 ModuleVdd;      //  6 Module nominal voltage, VDD
  UINT8 Organization;   //  7 Module Organization
  UINT8 BusWidth;       //  8 Module Memory Bus Width
  UINT8 FtbDiv;         //  9 Fine Timebase (FTB) Dividend / Divisor
  UINT8 MtbDividend;    // 10 Medium Timebase (MTB) Dividend
  UINT8 MtbDivisor;     // 11 Medium Timebase (MTB) Divisor
  UINT8 TckMin;         // 12 SDRAM Minimum Cycle Time
  UINT8 Res13;          // 13 Reserved
  UINT8 CaslatLsb;      // 14 CAS Latencies Supported, Least Significant Byte
  UINT8 CaslatMsb;      // 15 CAS Latencies Supported, Most Significant Byte
  UINT8 TaaMin;         // 16 Min CAS Latency Time
  UINT8 TwrMin;         // 17 Min Write REcovery Time
  UINT8 TrcdMin;        // 18 Min RAS# to CAS# Delay Time
  UINT8 TrrdMin;        // 19 Min Row Active to Row Active Delay Time
  UINT8 TrpMin;         // 20 Min Row Precharge Delay Time
  UINT8 TrasTrcExt;     // 21 Upper Nibbles for tRAS and tRC
  UINT8 TrasMinLsb;     // 22 Min Active to Precharge Delay Time
  UINT8 TrcMinLsb;      // 23 Min Active to Active/Refresh Delay Time, LSB
  UINT8 TrfcMinLsb;     // 24 Min Refresh Recovery Delay Time
  UINT8 TrfcMinMsb;     // 25 Min Refresh Recovery Delay Time
  UINT8 TwtrMin;        // 26 Min Internal Write to Read Command Delay Time
  UINT8 TrtpMin;        // 27 Min Internal Read to Precharge Command Delay Time
  UINT8 TfawMsb;        // 28 Upper Nibble for tFAW
  UINT8 TfawMin;        // 29 Min Four Activate Window Delay Time
  UINT8 OptFeatures;    // 30 SDRAM Optional Features
  UINT8 ThermRefOpt;    // 31 SDRAM Thermal and Refresh Opts
  UINT8 ThermSensor;    // 32 Module Thermal Sensor
  UINT8 DeviceType;     // 33 SDRAM device type
  INT8 FineTckMin;      // 34 Fine offset for tCKmin
  INT8 FineTaaMin;      // 35 Fine offset for tAAmin
  INT8 FineTrcdMin;     // 36 Fine offset for tRCDmin
  INT8 FineTrpMin;      // 37 Fine offset for tRPmin
  INT8 FinetrcMin;      // 38 Fine offset for tRCmin
  UINT8 Res3959[21];    // 39-59 Reserved, General Section
  // Module-Specific Section: Bytes 60-116
  union {
    struct {
    UINT8 ModHeight;    // 60 (Unbuffered) Module Nominal Height
    UINT8 ModThickness; // 61 (Unbuffered) Module Maximum Thickness
    UINT8 RefRawCard;   // 62 (Unbuffered) Reference Raw Card Used
    UINT8 AddrMapping;  // 63 (Unbuffered) Address Mapping from Edge Connector to DRAM
    UINT8 Res64116[53]; // 64-116 (Unbuffered) Reserved
  } Unbuffered;
  struct {
    UINT8 ModHeight;     // 60 (Registered) Module Nominal Height
    UINT8 ModThickness;  // 61 (Registered) Module Maximum Thickness
    UINT8 RefRawCard;    // 62 (Registered) Reference Raw Card Used
    UINT8 ModuAttr;      // 63 DIMM Module Attributes
    UINT8 Thermal;       // 64 RDIMM Thermal Heat Spreader Solution
    UINT8 RegIdLo;       // 65 Register Manufacturer ID Code, LSB byte
    UINT8 RegIdHi;       // 66 Register Manufacturer ID Code, MSB Byte
    UINT8 RegRev;        // 67 Register Revision Number
    UINT8 RegType;       // 68 Register Type
    UINT8 Rcw[8];        // 69-76 RC1,3,5...15 (MS Nibble) / RC0,2,4...14 (LS Nibble)
  } Registered;
  UINT8 Uc[57];          // 60-116 Module-Specific Section
  } ModSection;
  // Unique Module ID: Bytes 117-125
  UINT8 MmidLsb;        // 117 Module MfgID Code LSB - JEP-106
  UINT8 MmidMsb;        // 118 Module MfgID Code MSB - JEP-106
  UINT8 Mloc;           // 119 Mfg Location
  UINT8 Mdate[2];       // 120-121 Mfg Date
  UINT8 Sernum[4];      // 122-125 Module Serial Number
  UINT8 Crc[2];         // 126-127 SPD CRC
  // Other Manufacturer Fields and User Space: Bytes 128-255
  UINT8 Mpart[18];      // 128-145 Mfg's Module Part Number
  UINT8 Mrev[2];        // 146-147 Module Revision Code
  UINT8 DmidLsb;        // 148 DRAM MfgID Code LSB - JEP-106
  UINT8 DmidMsb;        // 149 DRAM MfgID Code MSB - JEP-106
  UINT8 Msd[26];        // 150-175 Mfg's Specific Data
  UINT8 Cust[80];       // 176-255 Open for Customer Use
} Ddr3SpdEepromT;


// From JEEC Standard No. 21-C release 23A
typedef struct Ddr4SpdEepromS {
  // General Section: Bytes 0-127
  UINT8 InfoSizeCrc;       //  0 # Bytes
  UINT8 SpdRev;            //  1 Total # Bytes Of SPD
  UINT8 MemType;           //  2 Mem Type
  UINT8 ModuleType;        //  3 Module Type
  UINT8 DensityBanks;      //  4 Density And Banks
  UINT8 Addressing;        //  5 Addressing
  UINT8 PackageType;       //  6 Package Type
  UINT8 OptFeature;        //  7 Optional Features
  UINT8 ThermalRef;        //  8 Thermal And Refresh
  UINT8 OthOptFeatures;    //  9 Other Optional Features
  UINT8 Res10;             // 10 Reserved
  UINT8 ModuleVdd;         // 11 Module Nominal Voltage
  UINT8 Organization;      // 12 Module Organization
  UINT8 BusWidth;          // 13 Module Memory Bus Width
  UINT8 ThermSensor;       // 14 Module Thermal Sensor
  UINT8 ExtType;           // 15 Extended Module Type
  UINT8 Res16;             // 16 Reserved
  UINT8 Timebases;         // 17 MTb And FTB
  UINT8 TckMin;            // 18 TCKAVGmin
  UINT8 TckMax;            // 19 TCKAVGmax
  UINT8 CaslatB1;          // 20 CAS Latencies, 1st Byte
  UINT8 CaslatB2;          // 21 CAS Latencies, 2nd Byte
  UINT8 CaslatB3;          // 22 CAS Latencies, 3rd Byte
  UINT8 CaslatB4;          // 23 CAS Latencies, 4th Byte
  UINT8 TaaMin;            // 24 Min CAS Latency Time
  UINT8 TrcdMin;           // 25 Min RAS# To CAS# Delay Time
  UINT8 TrpMin;            // 26 Min Row Precharge Delay Time
  UINT8 TrasTrcExt;        // 27 Upper Nibbles for TRAS And TRC
  UINT8 TrasMinLsb;        // 28 TRASmin, Lsb
  UINT8 TrcMinLsb;         // 29 TRCmin, Lsb
  UINT8 Trfc1MinLsb;       // 30 Min Refresh Recovery Delay Time
  UINT8 Trfc1MinMsb;       // 31 Min Refresh Recovery Delay Time
  UINT8 Trfc2MinLsb;       // 32 Min Refresh Recovery Delay Time
  UINT8 Trfc2MinMsb;       // 33 Min Refresh Recovery Delay Time
  UINT8 Trfc4MinLsb;       // 34 Min Refresh Recovery Delay Time
  UINT8 Trfc4MinMsb;       // 35 Min Refresh Recovery Delay Time
  UINT8 TfawMsb;           // 36 Upper Nibble for TFAW
  UINT8 TfawMin;           // 37 TFAW, Lsb
  UINT8 TrrdsMin;          // 38 TRRD_Smin, MTB
  UINT8 TrrdlMin;          // 39 TRRD_Lmin, MTB
  UINT8 TccdlMin;          // 40 TCCS_Lmin, MTB
  UINT8 Res41[60-41];      // 41 Rserved
  UINT8 Mapping[78-60];    // 60~77 Connector To SDRAM Bit Map
  UINT8 Res78[117-78];     // 78~116, Reserved
  INT8 FineTccdlMin;      // 117 Fine Offset for TCCD_Lmin
  INT8 FineTrrdlMin;      // 118 Fine Offset for TRRD_Lmin
  INT8 FineTrrdsMin;      // 119 Fine Offset for TRRD_Smin
  INT8 FineTrcMin;        // 120 Fine Offset for TRCmin
  INT8 FineTrpMin;        // 121 Fine Offset for TRPmin
  INT8 FineTrcdMin;       // 122 Fine Offset for TRCDmin
  INT8 FineTaaMin;        // 123 Fine Offset for TAAmin
  INT8 FineTckMax;        // 124 Fine Offset for TCKAVGmax
  INT8 FineTckMin;        // 125 Fine Offset for TCKAVGmin
  UINT8 Crc[2];           // 126-127 SPD CRC

  // Module-Specific Section: Bytes 128-255
  union {
    struct {
      UINT8 ModHeight;       // 128 (Unbuffered) Module Nominal Height
      UINT8 ModThickness;    // 129 (Unbuffered) Module Maximum Thickness
      UINT8 RefRawCard;      // 130 (Unbuffered) Reference Raw Card Used
      UINT8 AddrMapping;     // 131 (Unbuffered) Address Mapping From Edge Connector To DRAM
      UINT8 Res132[254-132]; // 132~253 (Unbuffered) Reserved
      UINT8 Crc[2];          // 254~255 CRC
    } Unbuffered;
    struct {
      UINT8 ModHeight;       // 128 (Registered) Module Nominal Height
      UINT8 ModThickness;    // 129 (Registered) Module Maximum Thickness
      UINT8 RefRawCard;      // 130 (Registered) Reference Raw Card Used
      UINT8 ModuAttr;        // 131 DIMM Module Attributes
      UINT8 Thermal;         // 132 RDIMM Thermal Heat Spreader Solution
      UINT8 RegIdLo;         // 133 Register Manufacturer ID Code, LSB
      UINT8 RegIdHi;         // 134 Register Manufacturer ID Code, MSB
      UINT8 RegRev;          // 135 Register Revision Number
      UINT8 RegMap;          // 136 Address Mapping From Register To DRAM
      UINT8 Res137[254-137]; // 137~253 Reserved
      UINT8 Crc[2];          // 254~255 CRC
    } Registered;
    struct {
      UINT8 ModHeight;       // 128 (Loadreduced) Module Nominal Height
      UINT8 ModThickness;    // 129 (Loadreduced) Module Maximum Thickness
      UINT8 RefRawCard;      // 130 (Loadreduced) Reference raw card used
      UINT8 ModuAttr;        // 131 DIMM Module Attributes
      UINT8 Thermal;         // 132 RDIMM Thermal Heat Spreader Solution
      UINT8 RegIdLo;         // 133 Register Manufacturer ID Code, LSB
      UINT8 RegIdHi;         // 134 Register Manufacturer ID Code, MSB
      UINT8 RegRev;          // 135 Register Revision Number
      UINT8 RegMap;          // 136 Address Mapping From Register to DRAM
      UINT8 RegDrv;          // 137 Register Output Drive Strength for CMD/Add
      UINT8 RegDrvCk;        // 138 Register Output Drive Strength for CK
      UINT8 DataBufRev;      // 139 Data Buffer Revision Number
      UINT8 VrefqeR0;        // 140 DRAM VrefDQ for Package Rank 0
      UINT8 VrefqeR1;        // 141 DRAM VrefDQ for Package Rank 1
      UINT8 VrefqeR2;        // 142 DRAM VrefDQ for Package Rank 2
      UINT8 VrefqeR3;        // 143 DRAM VrefDQ for Package Rank 3
      UINT8 DataIntf;        // 144 Data Buffer VrefDQ for DRAM Interface
      UINT8 DataDrv1866;     // 145 Data Buffer MDQ Drive strength and RTT, DataRate <= 1866
      UINT8 DataDrv2400;     // 146 for 1866 < DataRate <= 2400
      UINT8 DataDrv3200;     // 147 for 2400 < DataRate <= 3200
      UINT8 DramDrv;         // 148 DRAM Drive Strength
      UINT8 DramOdt1866;     // 149 DRAM ODT (RTT_WR, RTT_NOM) for DataRate <= 1866
      UINT8 DramOdt2400;     // 150 DRAM ODT (RTT_WR, RTT_NOM) for 1866 < DataRate <= 2400
      UINT8 DramOdt3200;     // 151 DRAM ODT (RTT_WR, RTT_NOM) for 2400 < DataRate <= 3200
      UINT8 DramOdtPark1866; // 152 DRAM ODT (RTT_PARK) for DataRate <= 1866
      UINT8 DramOdtPark2400; // 153 DRAM ODT (RTT_PARK) for 1866 < DataRate <= 2400
      UINT8 DramOdtPark3200; // 154 DRAM ODT (RTT_PARK) for 2400 < Data Rate <= 3200
      UINT8 Res155[254-155]; // 155~253 Reserved
      UINT8 Crc[2];          // 254~255 CRC
    } Loadreduced;
    UINT8 Uc[128];           // 128-255 Module-Specific Section
  } ModSection;

  UINT8 Res256[320-256];     // 256~319 Reserved

  // Module Supplier'S Data: Byte 320~383
  UINT8 MmidLsb;            // 320 Module MfgID code LSB
  UINT8 MmidMsb;            // 321 Module MfgID code MSB
  UINT8 Mloc;               // 322 Mfg Location
  UINT8 Mdate[2];           // 323~324 Mfg Date
  UINT8 Sernum[4];          // 325~328 Module Serial Number
  UINT8 Mpart[20];          // 329~348 Mfg'S Module part number
  UINT8 Mrev;               // 349 Module Revision Code
  UINT8 DmidLsb;            // 350 DRAM MfgID Code LSB
  UINT8 DmidMsb;            // 351 DRAM MfgID Code MSB
  UINT8 Stepping;           // 352 DRAM Stepping
  UINT8 Msd[29];            // 353~381 Mfg'S specific data
  UINT8 Res382[2];          // 382~383 Reserved
  UINT8 User[512-384];      // 384~511 End user programmable
}Ddr4SpdEepromT;


// Record of all information about DDR
typedef struct {
  GenericSpdEepromT   SpdInstalledDimms[DDR_CTLRS_NUM][DDR_DIMM_SLOTS_PER_CTLR];
  DimmParamsT         DimmParams[DDR_CTLRS_NUM][DDR_DIMM_SLOTS_PER_CTLR];
  MemctlOptionsT      MemctlOpts[DDR_CTLRS_NUM];
  CommonTimingParamsT CommonTimingParams[DDR_CTLRS_NUM];
  DdrCfgRegsT         DdrConfigReg[DDR_CTLRS_NUM];
  UINT32              FirstCtrl;
  UINT32    NumCtrls;
  UINT64    MemBase;
  UINT32    DimmSlotsPerCtrl;
  INT32    (*BoardNeedMemReset)(VOID);
  VOID     (*BoardMemReset)(VOID);
  VOID     (*BoardMemDeReset)(VOID);
} DdrInfoT;


// DDR memory controller registers
struct CcsrDdr {
  UINT32  Cs0Bnds;      // Chip Select 0 Memory Bounds
  CHAR8   Res04[4];
  UINT32  Cs1Bnds;      // Chip Select 1 Memory Bounds
  CHAR8   Res0c[4];
  UINT32  Cs2Bnds;      // Chip Select 2 Memory Bounds
  CHAR8   Res14[4];
  UINT32  Cs3Bnds;      // Chip Select 3 Memory Bounds
  CHAR8   Res1c[100];
  UINT32  Cs0Config;    // Chip Select Configuration
  UINT32  Cs1Config;    // Chip Select Configuration
  UINT32  Cs2Config;    // Chip Select Configuration
  UINT32  Cs3Config;    // Chip Select Configuration
  CHAR8   Res90[48];
  UINT32  Cs0Config2;    // Chip Select Configuration 2
  UINT32  Cs1Config2;    // Chip Select Configuration 2
  UINT32  Cs2Config2;    // Chip Select Configuration 2
  UINT32  Cs3Config2;    // Chip Select Configuration 2
  CHAR8   Resd0[48];
  UINT32  TimingCfg3;    // SDRAM Timing Configuration 3
  UINT32  TimingCfg0;    // SDRAM Timing Configuration 0
  UINT32  TimingCfg1;    // SDRAM Timing Configuration 1
  UINT32  TimingCfg2;    // SDRAM Timing Configuration 2
  UINT32  SdramCfg;      // SDRAM Control Configuration
  UINT32  SdramCfg2;     // SDRAM Control Configuration 2
  UINT32  SdramMode;     // SDRAM Mode Configuration
  UINT32  SdramMode2;    // SDRAM Mode Configuration 2
  UINT32  SdramMdCntl;   // SDRAM Mode Control
  UINT32  SdramInterval; // SDRAM Interval Configuration
  UINT32  SdramDataInit; // SDRAM Data initialization
  CHAR8   Res12c[4];
  UINT32  SdramClkCntl;  // SDRAM Clock Control
  CHAR8   Res134[20];
  UINT32  InitAddr;      // training init addr
  UINT32  InitExtAddr;   // training init extended addr
  CHAR8   Res150[16];
  UINT32  TimingCfg4;    // SDRAM Timing Configuration 4
  UINT32  TimingCfg5;    // SDRAM Timing Configuration 5
  UINT32  TimingCfg6;    // SDRAM Timing Configuration 6
  UINT32  TimingCfg7;    // SDRAM Timing Configuration 7
  UINT32  DdrZqCntl;     // ZQ calibration control
  UINT32  DdrWrlvlCntl;  // write leveling control
  CHAR8   Reg178[4];
  UINT32  DdrSrCntr;     // self refresh counter
  UINT32  DdrSdramRcw1;  // Control Words 1
  UINT32  DdrSdramRcw2;  // Control Words 2
  CHAR8   Reg188[8];
  UINT32  DdrWrlvlCntl2; // write leveling control 2
  UINT32  DdrWrlvlCntl3; // write leveling control 3
  CHAR8   Res198[8];
  UINT32  DdrSdramRcw3;
  UINT32  DdrSdramRcw4;
  UINT32  DdrSdramRcw5;
  UINT32  DdrSdramRcw6;
  CHAR8   Res1b0[80];
  UINT32  SdramMode3;    // SDRAM Mode Configuration 3
  UINT32  SdramMode4;    // SDRAM Mode Configuration 4
  UINT32  SdramMode5;    // SDRAM Mode Configuration 5
  UINT32  SdramMode6;    // SDRAM Mode Configuration 6
  UINT32  SdramMode7;    // SDRAM Mode Configuration 7
  UINT32  SdramMode8;    // SDRAM Mode Configuration 8
  CHAR8   Res218[8];
  UINT32  SdramMode9;    // SDRAM Mode Configuration 9
  UINT32  SdramMode10;   // SDRAM Mode Configuration 10
  UINT32  SdramMode11;   // SDRAM Mode Configuration 11
  UINT32  SdramMode12;   // SDRAM Mode Configuration 12
  UINT32  SdramMode13;   // SDRAM Mode Configuration 13
  UINT32  SdramMode14;   // SDRAM Mode Configuration 14
  UINT32  SdramMode15;   // SDRAM Mode Configuration 15
  UINT32  SdramMode16;   // SDRAM Mode Configuration 16
  CHAR8   Res240[16];
  UINT32  TimingCfg8;    // SDRAM Timing Configuration 8
  UINT32  TimingCfg9;    // SDRAM Timing Configuration 9
  CHAR8   Res254[8];
  UINT32  SdramCfg3;
  CHAR8   Res264[412];
  UINT32  DqMap0;
  UINT32  DqMap1;
  UINT32  DqMap2;
  UINT32  DqMap3;
  CHAR8   Res410[1808];
  UINT32  DdrDsr1;      // Debug Status 1
  UINT32  DdrDsr2;      // Debug Status 2
  UINT32  DdrCdr1;      // Control Driver 1
  UINT32  DdrCdr2;      // Control Driver 2
  CHAR8   ResB30[200];
  UINT32  IpRev1;       // IP Block Revision 1
  UINT32  IpRev2;       // IP Block Revision 2
  UINT32  Eor;          // Enhanced Optimization Register
  CHAR8   ResC04[252];
  UINT32  Mtcr;         // Memory Test Control Register
  CHAR8   ResD04[28];
  UINT32  Mtp1;         // Memory Test Pattern 1
  UINT32  Mtp2;         // Memory Test Pattern 2
  UINT32  Mtp3;         // Memory Test Pattern 3
  UINT32  Mtp4;         // Memory Test Pattern 4
  UINT32  Mtp5;         // Memory Test Pattern 5
  UINT32  Mtp6;         // Memory Test Pattern 6
  UINT32  Mtp7;         // Memory Test Pattern 7
  UINT32  Mtp8;         // Memory Test Pattern 8
  UINT32  Mtp9;         // Memory Test Pattern 9
  UINT32  Mtp10;        // Memory Test Pattern 10
  CHAR8   ResD48[184];
  UINT32  DataErrInjectHi;  // Data Path Err Injection Mask High
  UINT32  DataErrInjectLo;  // Data Path Err Injection Mask Low
  UINT32  EccErrInject;     // Data Path Err Injection Mask ECC
  CHAR8   ResE0c[20];
  UINT32  CaptureDataHi;    // Data Path Read Capture High
  UINT32  CaptureDataLo;    // Data Path Read Capture Low
  UINT32  CaptureEcc;       // Data Path Read Capture ECC
  CHAR8   ResE2c[20];
  UINT32  ErrDetect;        // Error Detect
  UINT32  ErrDisable;       // Error Disable
  UINT32  ErrIntEn;
  UINT32  CaptureAttributes; // Error Attrs Capture
  UINT32  CaptureAddress;    // Error Addr Capture
  UINT32  CaptureExtAddress; // Error Extended Addr Capture
  UINT32  ErrSbe;            // Single-Bit ECC Error Management
  CHAR8   ResE5c[164];
  UINT32  Debug[64];         // Debug_1 to Debug_32 registers
};

STATIC INT32
__BoardNeedMemReset (
    VOID
  )
{
  return 0;
}

INT32
BoardNeedMemReset(
  VOID
) __attribute__ ((weak, alias("__BoardNeedMemReset")));


STATIC VOID
__BoardAssertMemReset (
  IN  VOID
  )
{
  return;
}

VOID
BoardAssertMemReset (
  VOID
) __attribute__ ((weak, alias("__BoardAssertMemReset")));


STATIC VOID
__BoardDeassertMemReset (
   VOID
  )
{
  return;
}

VOID
BoardDeassertMemReset (
  VOID
) __attribute__ ((weak, alias("__BoardDeassertMemReset")));


// Functions declarations
UINT32 ComputeFslMemctlConfigRegs (
  IN   CONST MemctlOptionsT      *Popts,
  OUT  DdrCfgRegsT               *Ddr,
  IN   CONST CommonTimingParamsT *CommonDimm,
  IN   CONST DimmParamsT         *DimmParams,
  IN   UINT32                    DbwCapAdj
);

UINT32 DdrCfgMemCtrlOptions (
  IN   CommonTimingParamsT  *CommonDimm,
  OUT  MemctlOptionsT       *Popts,
  IN   DimmParamsT          *Pdimm,
  IN   UINT32               CtrlNum
);

UINT32 DdrComputeDimmParameters (
  IN CONST GenericSpdEepromT  *Spd,
  OUT DimmParamsT             *Pdimm,
  IN  UINT32                  DimmNumber
);

VOID DdrReadSpd (
  OUT GenericSpdEepromT  *DimmSpd,
  IN  UINT32             CtrlNum
);

VOID DdrBoardOptions (
  OUT   MemctlOptionsT  *Popts,
  IN    DimmParamsT     *Pdimm
);

INT32 DdrGetRawDimmParams (
  OUT  DimmParamsT  *Pdimm,
  IN   UINT32       CtrlNum,
  IN   UINT32       DimmNum
);

VOID DdrRegDump (IN UINT32 CtrlNum);
VOID PrintDdrInfo(IN UINT32 StartCtrl);
UINT64 GetDdrFreq (VOID);
UINT32 PicosToMclk (IN UINT32 Picos);
UINT32 DdrGetCtrlVersion (VOID);
UINT32 GetMemoryClkPeriodPs (VOID);
UINT64 DramInit (VOID);
UINT32 DdrRead32(VOID *a);
void Ddr4SpdDump(const struct Ddr4SpdEepromS *spd);
#endif
