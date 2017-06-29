/** @Qspi.h
  Header Defining The Qspi Flash Controller Constants (Base Addresses, Sizes,
  Flags), Function Prototype, Structures etc

  Based on NOR flash access APIs used in ArmPlatformPkg/Drivers/NorFlashDxe/NorFlashDxe.h

  Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright 2016- 2017 NXP

  This Program And The Accompanying Materials
  Are Licensed And Made Available Under The Terms And Conditions Of The BSD
  License Which Accompanies This Distribution. The Full Text Of The License
  May Be Found At
  http://Opensource.Org/Licenses/Bsd-License.Php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __QSPI_FLASH_LIB_H__
#define __QSPI_FLASH_LIB_H__

#include <Bitops.h>
#include <LS1043aRdb.h>
#include <PiPei.h>
#include <Spi.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/MemoryAllocationLib.h>

#define DEFAULT_SPEED        1000000
#define DEFAULT_MODE         SPI_COMMON_MODE_0
#define DEFAULT_CS           0
#define DEFAULT_BUS          0

#define TX_BUFFER_SIZE       0x40
#define RX_BUFFER_SIZE       0x80

#define OFFSET_MASK          0x00ffffff
#define STATUS_WEL           0x02

#define SPEED_HZ             0x2faf080
//#define AMBA_BASE          0x20000000
#define AMBA_BASE            0x40000000
#define AMBA_TOTAL_SIZE      0x4000000
//#define AMBA_TOTAL_SIZE    0x10000000
#define FLASH_NUM   0x1
#define MAX_CHIPSELECT_NUM   2

#define QSPI_FLASH_SIZE      (1 << 24) /* 16MB */

#define QSPI_LAST_BLOCK      256  /* (16M / 64K) */

#define LCKCR_LOCK           0x1
#define LCKCR_UNLOCK         0x2
#define LUT_KEY              0x5af05af0

/*
 * The definition of the LUT register shows below:
 *
 *  ---------------------------------------------------
 *  | INSTR1 | PAD1 | OPRND1 | INSTR0 | PAD0 | OPRND0 |
 *  ---------------------------------------------------
 */
#define LUT_OPRND0_SHIFT        0
#define LUT_PAD0_SHIFT          8
#define LUT_INSTR0_SHIFT        10
#define LUT_OPRND1_SHIFT        16

/* Macros for constructing the LUT register. */
#define LUT_0(ins, pad, opr)						\
		(((opr) << LUT_OPRND0_SHIFT) | ((LUT_##pad) << LUT_PAD0_SHIFT) | \
		((LUT_##ins) << LUT_INSTR0_SHIFT))

#define LUT_1(ins, pad, opr)	(LUT_0(ins, pad, opr) << LUT_OPRND1_SHIFT)

#define CLEAR_LUT(LutBase, Lut_ID) \
  do { \
    MmioWriteBe32((UINTN)&LutBase[Lut_ID], 0); \
    MmioWriteBe32((UINTN)&LutBase[Lut_ID + 1], 0); \
    MmioWriteBe32((UINTN)&LutBase[Lut_ID + 2], 0); \
    MmioWriteBe32((UINTN)&LutBase[Lut_ID + 3], 0); \
  } while(0)

#define LUT_CMD              1
#define LUT_ADDR             2
#define LUT_DUMMY            3
#define LUT_MODE             4
#define LUT_READ             7
#define LUT_WRITE            8

#define LUT_PAD_1            0
#define LUT_PAD_2            1
#define LUT_PAD_4            2

#define ADDR_24BIT           0x18
#define ADDR_32BIT           0x20

/*
 * SEQID
 */
/* Common to all flash, prepare before flash probe */
#define SEQ_ID_RDID          1
#define SEQ_ID_WREN          2
#define SEQ_ID_RDSR          3
#define SEQ_ID_WRSR          4
#define SEQ_ID_RDCR          5
/* Common to all flash, prepare after flash probe */
#define SEQ_ID_READ          6
#define SEQ_ID_SECTOR_ERASE  7
#define SEQ_ID_CHIP_ERASE    8
#define SEQ_ID_WRITE         9
/* Used for Spansion S25FS-s Family Flash Only */
#define SEQ_ID_WRAR          10
#define SEQ_ID_RDAR          11

/* QSPI CMD */
#define CMD_PP               0x02   /* Page program (up to 256 bytes) */
#define CMD_RDSR             0x05   /* Read status register */
#define CMD_WREN             0x06   /* Write enable */
#define CMD_FAST_READ        0x0b   /* Read data bytes (high frequency) */
#define CMD_CHIP_ERASE       0xc7   /* Erase whole flash chip */
#define CMD_SE               0xd8   /* Sector erase (usually 64KiB) */
#define CMD_RDID             0x9f   /* Read JEDEC ID */

/* 4-byte address QSPI CMD - used on Spansion and some Macronix flashes */
#define CMD_FAST_READ_4B     0x0c    /* Read data bytes (high frequency) */
#define CMD_PP_4B            0x12    /* Page program (up to 256 bytes) */
#define CMD_SE_4B            0xdc    /* Sector erase (usually 64KiB) */

/* Used for Micron, winbond and Macronix flashes */
#define  CMD_WREAR           0xc5   /* EAR register write */
#define  CMD_RDEAR           0xc8   /* EAR reigster read */

/* Used for Spansion flashes only. */
#define  CMD_BRRD            0x16   /* Bank register read */
#define  CMD_BRWR            0x17   /* Bank register write */

/* Used for Spansion S25FS-S family flash only. */
#define  CMD_RDAR            0x65   /* Read any device register */
#define  CMD_WRAR            0x71   /* Write any device register */

#define IPCR_SEQID_SHIFT     24
#define IPCR_SEQID_MASK      (0xf << IPCR_SEQID_SHIFT)

#define RBSR_RDBFL_SHIFT     8
#define RBSR_RDBFL_MASK      (0x1f << RBSR_RDBFL_SHIFT)

#define BYTES_IN_RX_BUFFER   4

/* Module Configuration */
#define MCR_CLR_RXF_SHIFT    10
#define MCR_CLR_RXF_MASK     (1 << MCR_CLR_RXF_SHIFT)
#define MCR_CLR_TXF_SHIFT    11
#define MCR_CLR_TXF_MASK     (1 << MCR_CLR_TXF_SHIFT)
#define MCR_MDIS_SHIFT       14
#define MCR_MDIS_MASK        (1 << MCR_MDIS_SHIFT)
#define MCR_RESERVED_SHIFT   16
#define MCR_RESERVED_MASK    (0xf << MCR_RESERVED_SHIFT)
#define MCR_END_CFD_SHIFT    2
#define MCR_END_CFD_MASK     (3 << MCR_END_CFD_SHIFT)
#define MCR_END_CFD_LE       (1 << MCR_END_CFD_SHIFT)


#define SMPR_HSENA_SHIFT     0
#define SMPR_HSENA_MASK      (1 << SMPR_HSENA_SHIFT)
#define SMPR_FSPHS_SHIFT     5
#define SMPR_FSPHS_MASK      (1 << SMPR_FSPHS_SHIFT)
#define SMPR_FSDLY_SHIFT     6
#define SMPR_FSDLY_MASK      (1 << SMPR_FSDLY_SHIFT)
#define SMPR_DDRSMP_SHIFT    16
#define SMPR_DDRSMP_MASK     (7 << SMPR_DDRSMP_SHIFT)

#define RBCT_RXBRD_SHIFT     8
#define RBCT_RXBRD_USEIPS    (1 << RBCT_RXBRD_SHIFT)

#define SR_BUSY_SHIFT        0
#define SR_BUSY_MASK         (1 << SR_BUSY_SHIFT)
#define SR_TXFULL            BIT27

#define TBSR_TRBL_MASK       0x1F00
#define TBSR_TRBL_SHIFT      8

#define QSPI_FLASH_DEVICE_COUNT          1
#define QSPI_FLASH_DATA_ALIGNMENT        8

/* Sf Param Flags */
enum {
  SECT_4K    = 1 << 0,
  SECT_32K   = 1 << 1,
  E_FSR      = 1 << 2,
  SST_BP     = 1 << 3,
  SST_WP     = 1 << 4,
  WR_QPP     = 1 << 5,
  AT45DB_CMD = 1 << 6
};

/**
 * struct QspiRegs - Qspi Controller Registers
 */
struct QspiRegs {
  UINT32 Mcr;
  UINT32 Rsvd0[1];
  UINT32 Ipcr;
  UINT32 Flshcr;
  UINT32 Buf0cr;
  UINT32 Buf1cr;
  UINT32 Buf2cr;
  UINT32 Buf3cr;
  UINT32 Bfgencr;
  UINT32 Soccr;
  UINT32 Rsvd1[2];
  UINT32 Buf0ind;
  UINT32 Buf1ind;
  UINT32 Buf2ind;
  UINT32 Rsvd2[49];
  UINT32 Sfar;
  UINT32 Rsvd3[1];
  UINT32 Smpr;
  UINT32 Rbsr;
  UINT32 Rbct;
  UINT32 Rsvd4[15];
  UINT32 Tbsr;
  UINT32 Tbdr;
  UINT32 Rsvd5[1];
  UINT32 Sr;
  UINT32 Fr;
  UINT32 Rser;
  UINT32 Spndst;
  UINT32 Sptrclr;
  UINT32 Rsvd6[4];
  UINT32 Sfa1ad;
  UINT32 Sfa2ad;
  UINT32 Sfb1ad;
  UINT32 Sfb2ad;
  UINT32 Rsvd7[28];
  UINT32 Rbdr[32];
  UINT32 Rsvd8[32];
  UINT32 Lutkey;
  UINT32 Lckcr;
  UINT32 Rsvd9[2];
  UINT32 Lut[64];
};


/**
 * struct QspiSlave - Container of all parameters required to communicate
 * with connected slave device.
 *
 * @Slave:  Instance of structure SpiSlave, represent slave device
    connected to controller
 * @Regs:   Pointer to Qspi Registers structure
 * @Baudrate: Baudrate
 * @BusClk: QSPI input clk frequency
 * @Charbit:  Charbit
 * @SpeedHz: Default SCK frequency
 * @CurSeqid: current LUT table sequence id
 * @SfAddr: flash access offset
 * @AmbaBase: Base address of QSPI memory mapping of every CS
 * @AmbaTotalSize: size of QSPI memory mapping
 * @CurAmbaBase: Base address of QSPI memory mapping of current CS
 * @FlashNum: Number of active slave devices
 * @NumChipselect: Number of QSPI chipselect signals
 */
struct QspiSlave {
  struct SpiSlave Slave;
  struct QspiRegs *Regs;
  UINT32 Baudrate;
  UINT32 BusClk;
  INT32  Charbit;
  UINT32 SpeedHz;
  UINT32 CurSeqid;
  UINT32 SfAddr;
  UINT32 AmbaBase[MAX_CHIPSELECT_NUM];
  UINT32 AmbaTotalSize;
  UINT32 CurAmbaBase;
  UINT32 FlashNum;
  UINT32 NumChipselect;
};

/**
 * struct QspiFlash - Structure to keep command ids and functions
 *       used for read, write and erase operations.
 *
 * @Qspi:          Pointer to structure containing information about
 *         QSPI controller register and slave device connected
 *         to controller.
 * @Name:          Name Of SPI Flash
 * @DualFlash:     Indicates Dual Flash Memories - Dual Stacked, Parallel
 * @Shift:         Flash Shift Useful In Dual Parallel
 * @Size:          Total Flash Size
 * @PageSize:      Write (Page) Size
 * @SectorSize:    Sector Size
 * @EraseSize:     Erase Size
 * @BlockSize:     Block Size
 * @BankReadCmd:   Bank Read Cmd
 * @BankWriteCmd:  Bank Write Cmd
 * @BankCurr:      Current Flash Bank
 * @PollCmd:       Poll Cmd - for Flash Erase/Program
 * @EraseCmd:      Erase Cmd 4K, 32K, 64K
 * @ReadCmd:       Read Cmd - Array Fast, Extn Read And Quad Read.
 * @WriteCmd:      Write Cmd - Page And Quad Program.
 * @DummyByte:     Dummy Cycles for Read Operation.
 * @MemoryMap:     Address Of Read-Only SPI Flash Access
 */
struct QspiFlash {
  struct QspiSlave *Qspi;
  CONST CHAR8 *Name;
  UINT8 DualFlash;
  UINT8 Shift;
  UINT32 Size;
  UINT32 PageSize;
  UINT32 SectorSize;
  UINT32 EraseSize;
  UINT32 BlockSize;
  UINT8 BankReadCmd;
  UINT8 BankWriteCmd;
  UINT8 BankCurr;
  UINT8 PollCmd;
  UINT8 EraseCmd;
  UINT8 ReadCmd;
  UINT8 WriteCmd;
  UINT8 DummyByte;
  VOID *MemoryMap;
};

/* Function Prototypes */
EFI_STATUS
QspiCommonRead (
  IN  struct QspiFlash *Flash,
  IN  CONST UINT8 *Cmd,
  IN  UINT32 CmdLen,
  OUT VOID *Data,
  IN  UINT64 DataLen
);

EFI_STATUS
QspiCommonWrite (
  IN  struct QspiFlash *Flash,
  IN  CONST UINT8 *Cmd,
  IN  UINT32 CmdLen,
  IN  CONST VOID *Buf,
  IN  UINT32 BufLen
);

EFI_STATUS
QspiWriteOps (
  IN  struct QspiFlash *Flash,
  IN  UINT32 Offset,
  IN  UINT64 Len,
  IN  CONST VOID *Buf
);

EFI_STATUS
QspiEraseOps (
  IN  struct QspiFlash *Flash,
  IN  UINT32 Offset,
  IN  UINT64 Len
);

EFI_STATUS
QspiReadOps (
  IN  struct QspiFlash *Flash,
  IN  UINT32 Offset,
  IN  UINT64 Len,
  OUT VOID *Data
);

INT32
BoardSpiIsDataflash (
  IN  UINT32 Bus,
  IN  UINT32 Cs
);

EFI_STATUS
QspiClaimBus (
  IN  struct QspiSlave *Slave
);

EFI_STATUS
QspiReadWrite (
  IN  struct QspiSlave *Qspi,
  IN  CONST UINT8 *Cmd,
  IN  UINT32 CmdLen,
  IN  CONST UINT8 *DataOut,
  OUT UINT8 *DataIn,
  IN  UINT64 DataLen
);

EFI_STATUS
QspiSetQeb (
  IN  struct QspiFlash *Flash,
  IN  UINT8 Idcode0
);

VOID
PrintMemorySize (
  IN  UINT64 Size,
  IN  CONST INT8 *S
);

VOID
QspiReleaseBus (
  IN  struct QspiSlave *Slave
);

VOID
QspiFreeSlave (
  IN  struct QspiSlave *Slave
);

struct QspiFlash *
QspiProbeDevice (
  IN  struct QspiSlave *Qspi
);

struct QspiSlave *
QspiSetupSlave (
  IN  UINT32 Bus,
  IN  UINT32 Cs,
  IN  UINT32 MaxHz,
  IN  UINT32 Mode
);

VOID
QspiPrepareLutPostProbe (
  IN  struct QspiFlash *Flash
);

VOID
SelectQspi (
  IN VOID
);

VOID
QspiCreateAddr (
  IN  UINT32 Addr,
  OUT UINT8 *Cmd
);

EFI_STATUS
QspiXfer (
  IN  struct QspiSlave *QspiSlave,
  IN  UINT32 Bitlen,
  IN  CONST VOID *OutData,
  OUT VOID *InData,
  IN  UINT64 Flags
);

#endif //__QSPI_FLASH_LIB_H__
