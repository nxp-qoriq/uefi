/** @Sdxc.h
  Header Defining The Sdxc Memory Controller Constants (Base Addresses, Sizes,
  Flags), Function Prototype, Structures Etc

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This Program And The Accompanying Materials
  Are Licensed And Made Available Under The Terms And Conditions Of The BSD
  License Which Accompanies This Distribution. The Full Text Of The License
  May Be Found At
  Http://Opensource.Org/Licenses/Bsd-License.Php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SDXC_H__
#define __SDXC_H__

#include "LS1043aRdb.h"
#include "LS1043aSocLib.h"
#include "Common.h"
#include "Bitops.h"
#include "CpldLib.h"
#include <Library/MemoryAllocationLib.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Protocol/BlockIo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TimerLib.h>
#include <Library/PrintLib.h>
#include <Library/DmaLib.h>

/**
  Set Block Count Limit Because Of 16 Bit Register Limit On Some Hardware
**/
#ifndef CONFIG_SYS_MMC_MAX_BLK_COUNT
#define CONFIG_SYS_MMC_MAX_BLK_COUNT 65535
#endif

/**
  Pre Div factor for DDR mode
**/
#define DIV_2				2
#define DIV_1				1

/**
  SDXC-Specific Constants
**/
#define SYSCTL              	0x0002e02c
#define SYSCTL_INITA        	0x08000000
#define SYSCTL_TIMEOUT_MASK 	0x000f0000
#define SYSCTL_CLOCK_MASK   	0x0000fff0
#define SYSCTL_CKEN         	0x00000008
#define SYSCTL_PEREN        	0x00000004
#define SYSCTL_HCKEN        	0x00000002
#define SYSCTL_IPGEN        	0x00000001
#define SYSCTL_RSTA         	0x01000000
#define SYSCTL_RSTC         	0x02000000
#define SYSCTL_RSTD         	0x04000000

/**
  Host Capabilities
**/
#define SDXC_HOSTCAPBLT_VS18      0x04000000
#define SDXC_HOSTCAPBLT_VS30      0x02000000
#define SDXC_HOSTCAPBLT_VS33      0x01000000
#define SDXC_HOSTCAPBLT_SRS       0x00800000
#define SDXC_HOSTCAPBLT_DMAS      0x00400000
#define SDXC_HOSTCAPBLT_HSS       0x00200000

/**
  VDD Voltage Range
**/
#define MMC_VDD_165_195            0x00000080    // VDD Voltage 1.65 - 1.95
#define MMC_VDD_20_21              0x00000100    // VDD Voltage 2.0 ~ 2.1
#define MMC_VDD_21_22              0x00000200    // VDD Voltage 2.1 ~ 2.2
#define MMC_VDD_22_23              0x00000400    // VDD Voltage 2.2 ~ 2.3
#define MMC_VDD_23_24              0x00000800    // VDD Voltage 2.3 ~ 2.4
#define MMC_VDD_24_25              0x00001000    // VDD Voltage 2.4 ~ 2.5
#define MMC_VDD_25_26              0x00002000    // VDD Voltage 2.5 ~ 2.6
#define MMC_VDD_26_27              0x00004000    // VDD Voltage 2.6 ~ 2.7
#define MMC_VDD_27_28              0x00008000    // VDD Voltage 2.7 ~ 2.8
#define MMC_VDD_28_29              0x00010000    // VDD Voltage 2.8 ~ 2.9
#define MMC_VDD_29_30              0x00020000    // VDD Voltage 2.9 ~ 3.0
#define MMC_VDD_30_31              0x00040000    // VDD Voltage 3.0 ~ 3.1
#define MMC_VDD_31_32              0x00080000    // VDD Voltage 3.1 ~ 3.2
#define MMC_VDD_32_33              0x00100000    // VDD Voltage 3.2 ~ 3.3
#define MMC_VDD_33_34              0x00200000    // VDD Voltage 3.3 ~ 3.4
#define MMC_VDD_34_35              0x00400000    // VDD Voltage 3.4 ~ 3.5
#define MMC_VDD_35_36              0x00800000    // VDD Voltage 3.5 ~ 3.6

/**
  MMC Operating Modes
**/
#define MMC_MODE_HS         	(1 << 0)
#define MMC_MODE_HS_52MHz   	(1 << 1)
#define MMC_MODE_4_BIT       	(1 << 2)
#define MMC_MODE_8_BIT       	(1 << 3)
#define MMC_MODE_SPI        	(1 << 4)
#define MMC_MODE_HC         	(1 << 5)
#define MMC_MODE_DDR_52MHz  	(1 << 6)

#define SD_DATA_4BIT 		0x00040000

#define MMC_SWITCH_MODE_CMD_SET    0x00 // Change The Command Set
#define MMC_SWITCH_MODE_SET_BITS   0x01 // Set Bits In EXT_CSD Byte
                                        // Addressed By Index Which Are
                                        // 1 In Value Field
#define MMC_SWITCH_MODE_CLEAR_BITS 0x02 // Clear Bits In EXT_CSD Byte
                                        // Addressed By Index, Which Are
                                        // 1 In Value Field
#define MMC_SWITCH_MODE_WRITE_BYTE 0x03 // Set Target Byte To Value

#define SD_SWITCH_CHECK            0
#define SD_SWITCH_SWITCH    	1

#define MMC_DATA_READ              1
#define MMC_DATA_WRITE             2

#define IsSpi(Mmc)       	0

#define SD_VER_SD       	0x20000
#define SD_VER_3 		(SD_VER_SD | 0x300)
#define SD_VER_2 		(SD_VER_SD | 0x200)
#define SD_VER_1_0      	(SD_VER_SD | 0x100)
#define SD_VER_1_10     	(SD_VER_SD | 0x10a)
#define MMC_VER_MMC            0x10000
#define MMC_VER_UNKNOWN 	(MMC_VER_MMC)
#define MMC_VER_1_2            (MMC_VER_MMC | 0x102)
#define MMC_VER_1_4            (MMC_VER_MMC | 0x104)
#define MMC_VER_2_2            (MMC_VER_MMC | 0x202)
#define MMC_VER_3              (MMC_VER_MMC | 0x300)
#define MMC_VER_4              (MMC_VER_MMC | 0x400)
#define MMC_VER_4_1            (MMC_VER_MMC | 0x401)
#define MMC_VER_4_2            (MMC_VER_MMC | 0x402)
#define MMC_VER_4_3            (MMC_VER_MMC | 0x403)
#define MMC_VER_4_41    	(MMC_VER_MMC | 0x429)
#define MMC_VER_4_5            (MMC_VER_MMC | 0x405)
#define MMC_VER_5_0            (MMC_VER_MMC | 0x500)

#define IS_SD(X) 			(X->Version & SD_VER_SD)

/**
  Maximum Block Size for MMC
**/
#define MMC_MAX_BLOCK_LEN   512

#define MMCPART_NOAVAILABLE (0xff)
#define PART_ACCESS_MASK    (0x7)
#define PART_SUPPORT        (0x1)
#define PART_ENH_ATTRIB     (0x1f)

/**
  SCR Definitions In Different Words
**/
#define SD_HIGHSPEED_BUSY   	0x00020000
#define SD_HIGHSPEED_SUPPORTED     0x00020000

#define OCR_BUSY            	0x80000000
#define OCR_HCS                    0x40000000
#define OCR_VOLTAGE_MASK    	0x007FFF80
#define OCR_ACCESS_MODE            0x60000000

#define SECURE_ERASE        	0x80000000

#define SD_CMD_SEND_RELATIVE_ADDR  3
#define SD_CMD_SEND_OP_COND        5
#define SD_CMD_SWITCH_FUNC         6
#define SD_CMD_SEND_IF_COND        8
#define SD_CMD_APP_SET_BUS_WIDTH   6

#define SD_CMD_ERASE_WR_BLK_START  32
#define SD_CMD_ERASE_WR_BLK_END    33
#define SD_CMD_APP_SEND_OP_COND    41
#define SD_CMD_APP_SEND_SCR        51

/**
  EXT_CSD Fields
**/
#define EXT_CSD_GP_SIZE_MULT              143    // R/W
#define EXT_CSD_PARTITION_SETTING         155    // R/W
#define EXT_CSD_PARTITIONS_ATTRIBUTE      156    // R/W
#define EXT_CSD_PARTITIONING_SUPPORT      160    // RO
#define EXT_CSD_RST_N_FUNCTION            162    // R/W
#define EXT_CSD_RPMB_MULT          	168    // RO
#define EXT_CSD_ERASE_GROUP_DEF           175    // R/W
#define EXT_CSD_BOOT_BUS_WIDTH            177
#define EXT_CSD_PART_CONF          	179    // R/W
#define EXT_CSD_BUS_WIDTH          	183    // R/W
#define EXT_CSD_HS_TIMING          	185    // R/W
#define EXT_CSD_REV                	192    // RO
#define EXT_CSD_CARD_TYPE          	196    // RO
#define EXT_CSD_SEC_CNT                   212    // RO, 4 Bytes
#define EXT_CSD_HC_WP_GRP_SIZE            221    // RO
#define EXT_CSD_HC_ERASE_GRP_SIZE  	224    // RO
#define EXT_CSD_BOOT_MULT          	226    // RO

/* GENERIC_CMD6_TIME indicates the default maximum timeout for a SWITCH 
 * command (CMD6) unless a specific timeout is defined when accessing a specific
 * field.
 */
#define GENERIC_CMD6_TIME           248

#define EXT_CSD_CMD_SET_NORMAL            (1 << 0)
#define EXT_CSD_CMD_SET_SECURE            (1 << 1)
#define EXT_CSD_CMD_SET_CPSECURE   	(1 << 2)

#define EXT_CSD_CARD_TYPE_26       	(1 << 0)      // Card Can Run At 26MHz
#define EXT_CSD_CARD_TYPE_52       	(1 << 1)      // Card Can Run At 52MHz
#define EXT_CSD_CARD_TYPE_DDR_1_8V 	(1 << 2)
#define EXT_CSD_CARD_TYPE_DDR_1_2V 	(1 << 3)
#define EXT_CSD_CARD_TYPE_DDR_52   	(EXT_CSD_CARD_TYPE_DDR_1_8V \
                                   	| EXT_CSD_CARD_TYPE_DDR_1_2V)

#define EXT_CSD_BUS_WIDTH_1 		0      // Card Is In 1 Bit Mode
#define EXT_CSD_BUS_WIDTH_4 		1      // Card Is In 4 Bit Mode
#define EXT_CSD_BUS_WIDTH_8 		2      // Card Is In 8 Bit Mode
#define EXT_CSD_DDR_BUS_WIDTH_4    	5      // Card Is In 4 Bit DDR Mode
#define EXT_CSD_DDR_BUS_WIDTH_8    	6      // Card Is In 8 Bit DDR Mode

#define EXT_CSD_PARTITION_SETTING_COMPLETED      (1 << 0)

/**
  MMC Commands
**/
#define EMMC_CMD_GO_IDLE_STATE             0
#define EMMC_CMD_SEND_OP_COND              1
#define EMMC_CMD_ALL_SEND_CID              2
#define EMMC_CMD_SET_RELATIVE_ADDR  	 3
#define EMMC_CMD_SET_DSR                   4
#define EMMC_CMD_SWITCH                    6
#define EMMC_CMD_SELECT_CARD        	 7
#define EMMC_CMD_SEND_EXT_CSD              8
#define EMMC_CMD_SEND_CSD           	 9
#define EMMC_CMD_SEND_CID           	 10
#define EMMC_CMD_STOP_TRANSMISSION  	 12
#define EMMC_CMD_SEND_STATUS        	 13
#define EMMC_CMD_SET_BLOCKLEN              16
#define EMMC_CMD_READ_SINGLE_BLOCK  	 17
#define EMMC_CMD_READ_MULTIPLE_BLOCK       18
#define EMMC_CMD_SET_BLOCK_COUNT         	 23
#define EMMC_CMD_WRITE_SINGLE_BLOCK 	 24
#define EMMC_CMD_WRITE_MULTIPLE_BLOCK      25
#define EMMC_CMD_ERASE_GROUP_START  	 35
#define EMMC_CMD_ERASE_GROUP_END           36
#define EMMC_CMD_ERASE                     38
#define EMMC_CMD_APP_CMD                   55
#define EMMC_CMD_SPI_READ_OCR              58
#define EMMC_CMD_SPI_CRC_ON_OFF            59
#define EMMC_CMD_RES_MAN                   62

#define MMC_RSP_PRESENT 			(1 << 0)
#define MMC_RSP_136  			(1 << 1)             // 136 Bit Response
#define MMC_RSP_CRC  			(1 << 2)             // Expect Valid Crc
#define MMC_RSP_BUSY 			(1 << 3)             // Card May Send Busy
#define MMC_RSP_OPCODE      		(1 << 4)             // Response Contains Opcode

#define MMC_RSP_NONE 		(0)
#define MMC_RSP_R1   		(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1b  		(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE| \
                     		MMC_RSP_BUSY)
#define MMC_RSP_R2   		(MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3   		(MMC_RSP_PRESENT)
#define MMC_RSP_R4   		(MMC_RSP_PRESENT)
#define MMC_RSP_R5   		(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6   		(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7   		(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define WML_RD_MAX      	0x10
#define WML_WR_MAX      	0x80
#define WML_RD_MAX_VAL  	0x00
#define WML_WR_MAX_VAL  	0x80
#define WML_RD_MASK     	0xff
#define WML_WR_MASK     	0xff0000

#define XFERTYPE                    0x0002e00c
#define XFERTYPE_CMD(X)             ((X & 0x3f) << 24)
#define XFERTYPE_CMDTYP_NORMAL      0x0
#define XFERTYPE_CMDTYP_SUSPEND     0x00400000
#define XFERTYPE_CMDTYP_RESUME      0x00800000
#define XFERTYPE_CMDTYP_ABORT       0x00c00000
#define XFERTYPE_DPSEL              0x00200000
#define XFERTYPE_CICEN              0x00100000
#define XFERTYPE_CCCEN              0x00080000
#define XFERTYPE_RSPTYP_NONE 	0
#define XFERTYPE_RSPTYP_136  	0x00010000
#define XFERTYPE_RSPTYP_48   	0x00020000
#define XFERTYPE_RSPTYP_48_BUSY     0x00030000
#define XFERTYPE_MSBSEL             0x00000020
#define XFERTYPE_DTDSEL             0x00000010
#define XFERTYPE_AC12EN             0x00000004
#define XFERTYPE_BCEN        	0x00000002
#define XFERTYPE_DMAEN              0x00000001

#define CPU_POLL_TIMEOUT         1000000000

#define IRQSTAT             (0x0002e030)
#define IRQSTATE_DMAE        (0x10000000)
#define IRQSTATE_AC12E       (0x01000000)
#define IRQSTATE_DEBE        (0x00400000)
#define IRQSTATE_DCE         (0x00200000)
#define IRQSTATE_DTOE        (0x00100000)
#define IRQSTATE_CIE         (0x00080000)
#define IRQSTATE_CEBE        (0x00040000)
#define IRQSTATE_CCE         (0x00020000)
#define IRQSTATE_CTOE        (0x00010000)
#define IRQSTATE_CINT        (0x00000100)
#define IRQSTATE_CRM         (0x00000080)
#define IRQSTATE_CINS        (0x00000040)
#define IRQSTATE_BRR         (0x00000020)
#define IRQSTATE_BWR         (0x00000010)
#define IRQSTATE_DINT        (0x00000008)
#define IRQSTATE_BGE         (0x00000004)
#define IRQSTATE_TC          (0x00000002)
#define IRQSTATE_CC          (0x00000001)

#define CMD_ERR             (IRQSTATE_CIE | IRQSTATE_CEBE | IRQSTATE_CCE)
#define DATA_ERR     	(IRQSTATE_DEBE | IRQSTATE_DCE | IRQSTATE_DTOE | \
                            IRQSTATE_DMAE)
#define DATA_COMPLETE       (IRQSTATE_TC | IRQSTATE_DINT)

#define IRQSTATE_EN           (0x0002e034)
#define IRQSTATE_EN_DMAE      (0x10000000)
#define IRQSTATE_EN_AC12E     (0x01000000)
#define IRQSTATE_EN_DEBE      (0x00400000)
#define IRQSTATE_EN_DCE       (0x00200000)
#define IRQSTATE_EN_DTOE      (0x00100000)
#define IRQSTATE_EN_CIE       (0x00080000)
#define IRQSTATE_EN_CEBE      (0x00040000)
#define IRQSTATE_EN_CCE       (0x00020000)
#define IRQSTATE_EN_CTOE      (0x00010000)
#define IRQSTATE_EN_CINT      (0x00000100)
#define IRQSTATE_EN_CRM       (0x00000080)
#define IRQSTATE_EN_CINS      (0x00000040)
#define IRQSTATE_EN_BRR       (0x00000020)
#define IRQSTATE_EN_BWR       (0x00000010)
#define IRQSTATE_EN_DINT      (0x00000008)
#define IRQSTATE_EN_BGE       (0x00000004)
#define IRQSTATE_EN_TC        (0x00000002)
#define IRQSTATE_EN_CC        (0x00000001)

#define PRSSTATE             (0x0002e024)
#define PRSSTATE_DAT0        (0x01000000)
#define PRSSTATE_CLSL        (0x00800000)
#define PRSSTATE_WPSPL       (0x00080000)
#define PRSSTATE_CDPL        (0x00040000)
#define PRSSTATE_CINS        (0x00010000)
#define PRSSTATE_BREN        (0x00000800)
#define PRSSTATE_BWEN        (0x00000400)
#define PRSSTATE_DLA         (0x00000004)
#define PRSSTATE_CICHB       (0x00000002)
#define PRSSTATE_CIDHB       (0x00000001)

#define PRCTL              0x0002e028
#define PRCTL_INIT         0x00000020
#define PRCTL_DTW_4        0x00000002
#define PRCTL_DTW_8        0x00000004
#define PRCTL_BE           0x00000030

#define MMC_STATUS_MASK            (~0x0206BF7F)
#define MMC_STATUS_SWITCH_ERROR    (1 << 7)
#define MMC_STATUS_RDY_FOR_DATA 	(1 << 8)
#define MMC_STATUS_CURR_STATE      (0xf << 9)
#define MMC_STATUS_ERROR    	(1 << 19)
#define MMC_STATE_PRG              (7 << 9)

#define LBA                    7744512

struct SdxcRegs {
       UINT32    Dsaddr;      // SDMA System Address Register
       UINT32    Blkattr;     // Block Attributes Register
       UINT32    CmdArg;      // Command Argument Register
       UINT32    Xfertype;     // Transfer Type Register
       UINT32    Rspns0;     // Command Response 0 Register
       UINT32    Rspns1;     // Command Response 1 Register
       UINT32    Rspns2;     // Command Response 2 Register
       UINT32    Rspns3;     // Command Response 3 Register
       UINT32    Datport;     // Buffer Data Port Register
       UINT32    Prsstat;     // Present State Register
       UINT32    Proctl;      // Protocol Control Register
       UINT32    Sysctl;      // System Control Register
       UINT32    Irqstat;     // Interrupt Status Register
       UINT32    Irqstaten;   // Interrupt Status Enable Register
       UINT32    Irqsigen;    // Interrupt Signal Enable Register
       UINT32    Autoc12err;  // Auto CMD Error Status Register
       UINT32    Hostcapblt;  // Host Controller Capabilities Register
       UINT32    Wml;         // Watermark Level Register
       UINT32    Mixctrl;     // for USDHC
       CHAR8     Reserved1[4]; // Reserved
       UINT32    Fevt;        // Force Event Register
       UINT32    Admaes;      // ADMA Error Status Register
       UINT32    Adsaddr;     // ADMA System Address Register
       CHAR8     Reserved2[100];// Reserved
       UINT32    VendorSpec;  //Vendor Specific Register
       CHAR8     Reserved3[56];// Reserved
       UINT32    Hostver;     // Host Controller Version Register
       CHAR8     Reserved4[4];// Reserved
       UINT32    Dmaerraddr;  // DMA Error Address Register
       CHAR8     Reserved5[4];// Reserved
       UINT32    Dmaerrattr;  // DMA Error Attribute Register
       CHAR8     Reserved6[4];// Reserved
       UINT32    Hostcapblt2; // Host Controller Capabilities Register 2
       CHAR8     Reserved7[8];// Reserved
       UINT32    Tcr;         // Tuning Control Register
       CHAR8     Reserved8[28];// Reserved
       UINT32    Sddirctl;    // SD Direction Control Register
       CHAR8     Reserved9[712];// Reserved
       UINT32    Scr;         // SDXC Control Register
};

struct DmaData {
	VOID *DmaAddr;
	UINTN Bytes;
	VOID *Mapping; 
        DMA_MAP_OPERATION MapOperation;
};

struct SdData {
       union {
              CHAR8 *Dest;
              CONST CHAR8 *Src; // Src Buffers Don'T Get Written To
       };
       UINT32 Flags;
       UINT32 Blocks;
       UINT32 Blocksize;
};

struct SdCmd {
       UINT16 CmdIdx;
       UINT32 RespType;
       UINT32 CmdArg;
       UINT32 Response[4];
};

typedef struct BlockDev {
       UINT64 Lba;          // Number Of Blocks
       UINT64 Blksz;        // Block Size
       CHAR8  Vendor [41];       // IDE Model, SCSI Vendor
       CHAR8  Product[21];       // IDE Serial No, SCSI Product
       CHAR8  Revision[9];       // Firmware Revision
       VOID   *Private;        // Driver Privateate struct Pointer
}BlockDevT;

struct Mmc {
       struct MmcConfig *Cfg;      // Provided Configuration
       UINT32 Version;
       VOID   *Private;
       UINT32 HasInit;
       INT32  HighCapacity;
       UINT32 BusWidth;
       UINT32 Clock;
       UINT32 CardCaps;
       UINT32 Ocr;
       UINT32 Dsr;
       UINT32 DsrImp;
       UINT32 Scr[2];
       UINT32 Csd[4];
       UINT32 Cid[4];
       UINT16 Rca;
       CHAR8  PartConfig;
       CHAR8  PartNum;
       UINT32 TranSpeed;
       UINT32 ReadBlkLen;
       UINT32 WriteBlkLen;
       UINT32 EraseGrpSize;
       UINT64 Capacity;
       UINT64 CapacityUser;
       UINT64 CapacityBoot;
       UINT64 CapacityRpmb;
       UINT64 CapacityGp[4];
       BlockDevT BlockDev;
       CHAR8  OpCondPending;       // 1 if We Are Waiting On An OpCond Command
       CHAR8  InitInProgress;      // 1 if We Have Done SdStartInit()
       CHAR8  Preinit;        // Start Init As Early As Possible
	INT32  DdrMode;
};

struct SdxcOperations {
       EFI_STATUS (*SdxcSendCmd)(struct SdxcRegs *Regs, UINT32 Clock,
                     struct SdCmd *Cmd, struct SdData *Data);
       VOID (*SdxcSetIos)(struct Mmc *Mmc);
       INT32 (*SdxcInit)(struct Mmc *Mmc);
       INT32 (*SdxcGetcd)(struct Mmc *Mmc);
       INT32 (*SdxcGetwp)(struct Mmc *Mmc);
};

struct MmcConfig {
       CHAR8 *Name;
       struct SdxcOperations *Ops;
       UINT32 HostCaps;
       UINT32 Voltages;
       UINT32 FMin;
       UINT32 FMax;
       UINT32 BMax;
};

struct SdxcCfg {
       VOID *    SdxcBase;
       UINT32    SdhcClk;
       UINT8     MaxBusWidth;
       struct MmcConfig Cfg;
};

struct SdxcBoot {
       struct SdxcRegs	*SdxcBase;
       UINT32	SdxcClk;
       UINT32 HostCaps;
       UINT32 Voltages;
       UINT32 FMin;
       UINT32 FMax;
       UINT32 BMax;
       UINT32 Dsr;
       UINT32 DsrImp;
       UINT32 Clock;
       UINT32 BusWidth;
	UINT16 Rca;
	UINT32 HighCapacity;
       UINT32 CardCaps;
	UINT32 Ocr;
       UINT32 Csd[4];
       UINT32 Cid[4];
	UINT32 Version;
	UINT32 ReadBlkLen;
	UINT32 TranSpeed;
       UINT64 Lba;          // Number Of Blocks
};

struct Mmc *
CreateMmcNode (
  IN struct MmcConfig *Cfg,
  IN VOID *Private
  );

VOID
SdxcSetClock (
  IN  struct Mmc *Mmc,
  IN  UINT32 Clock
  );

EFI_STATUS
SdxcSendCmd (
  IN  struct SdxcRegs *Regs,
  IN  UINT32 Clock,
  IN  struct SdCmd *Cmd,
  IN  struct SdData *Data
  );

VOID
SdxcSetIos (
  IN  struct Mmc *Mmc
  );

INT32
SdxcInit (
  IN  struct Mmc *Mmc
  );

INT32
SdxcGetcd (
  IN  struct Mmc *Mmc
  );

EFI_STATUS
SdxcInitialize (
  IN struct SdxcCfg *Cfg
  );

EFI_STATUS
SdxcMmcInit (
  );

INT32
BoardInit (
  VOID
  );

EFI_STATUS
MmcInitialize (
  VOID
  );

EFI_STATUS
MmcInit (
  IN  struct Mmc *Mmc
  );

INT32
MmcGetwp (
  IN  struct Mmc *Mmc
  );

EFI_STATUS
DetectMmcPresence (
  OUT UINT8*  Data
  );

INT32
Getcd (
  VOID
  );

EFI_STATUS
DoMmcInfo (
  VOID
  );

EFI_STATUS
MmcSendCommand (
  IN  struct SdCmd *Cmd
  );

EFI_STATUS
MmcRcvResp (
  IN   UINT32   RespType,
  OUT  UINT32*  Buffer
  );

EFI_STATUS
RecvResp(
  IN  struct SdxcRegs *Regs,
  IN  struct SdData *Data,
  IN  UINT32 RespType,
  OUT UINT32 *Response
  );

EFI_STATUS
DoMmcWrite (
  IN  VOID * InAddr,
  IN  UINT32 StartBlk,
  IN  UINT32 Count
  );

EFI_STATUS
DoMmcRead (
  OUT VOID * InAddr,
  IN  UINT32 StartBlk,
  IN  UINT32 Count
  );

EFI_STATUS
MmcSendReset (
  VOID
  );

EFI_STATUS
SdxcGoIdle (
  IN  struct Mmc *Mmc
  );

EFI_STATUS
DoMmcErase (
  IN UINT32 StartBlk,
  IN UINT32 Count
  );

EFI_STATUS
InitMmc (
  IN EFI_BLOCK_IO_MEDIA *Media
  );

VOID
PrintSdxcInfo (
  IN  struct Mmc *Mmc
  );

VOID
DestroyMmc (
  IN VOID
  );

EFI_STATUS
SdxcBootRead (
  OUT  VOID * InAddr,
  IN  UINT64 StartBlk,
  IN  UINT64 Size
  );

EFI_STATUS
DoMmcInfo (
  VOID
  );

EFI_STATUS
SdxcMmcInit (
  IN VOID
  );

VOID
SdxcReset (
  IN struct SdxcRegs *Regs
  );

EFI_STATUS
SendCmd (
  IN  struct SdCmd *Cmd,
  IN  struct SdData *Data
  );

EFI_STATUS
SdxcBootInit (
  IN  BOOLEAN Init
  );

VOID
CreateBootStruct (
  IN struct SdxcBoot*
  );

VOID
UpdateBootStruct (
  IN VOID
  );

EFI_STATUS
SdxcChangeFreq (
  IN  struct Mmc *Mmc
  );


EFI_STATUS
MmcChangeFreq (
  IN  struct Mmc *Mmc
  );

UINT32
SdxcBlkErase (
  IN  UINT32 Start,
  IN  UINT32 Blkcnt
  );

UINT32
SdxcBlkWrite (
  IN  UINT32 Start,
  IN  UINT32 Blkcnt,
  IN  CONST VOID *Src
  );

UINT32
SdxcBlkRead (
  IN  UINT32 Start,
  IN  UINT32 Blkcnt,
  OUT VOID *Dest
  );

VOID *
GetDmaBuffer (
  IN struct DmaData *DmaData
  );

EFI_STATUS
FreeDmaBuffer (
  IN struct DmaData *DmaData
  );

#endif
