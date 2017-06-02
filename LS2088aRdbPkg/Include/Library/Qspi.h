/** @Qspi.h
  Header Defining The Qspi Flash Controller Constants (Base Addresses, Sizes,
  Flags), Function Prototype, Structures etc

  Based on BlockIo Protocol available in MdePkg/Include/Protocol/BlockIo.h

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.
  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This Program And The Accompanying Materials
  Are Licensed And Made Available Under The Terms And Conditions Of The BSD
  License Which Accompanies This Distribution. The Full Text Of The License
  May Be Found At
  http://Opensource.Org/Licenses/Bsd-License.Php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __QSPI_H__
#define __QSPI_H__

#include <LS2088aRdb.h>
#include <LS2088aSocLib.h>
#include <Common.h>
#include <Bitops.h>
#include <QixisLib.h>
#include <Spi.h>
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


#define DEFAULT_SPEED	1000000
#define DEFAULT_MODE 	SPI_COMMON_MODE_3
#define DEFAULT_CS 		0
#define DEFAULT_BUS 		0

#define TX_BUFFER_SIZE      0x40
#define RX_BUFFER_SIZE      0x80

#define OFFSET_MASK    	0x00ffffff
#define STATUS_WEL    	0x02

#define SPEED_HZ		0x2faf080
#define AMBA_BASE		0x20000000
#define AMBA_TOTAL_SIZE	0x10000000
#define FLASH_NUM		0x1
#define MAX_CHIPSELECT_NUM  4

#define QSPI_FLASH_SIZE     (1 << 24) /* 16MB */

#define QSPI_LAST_BLOCK	256	   /* (16M / 64K) */

#define LCKCR_LOCK                 0x1
#define LCKCR_UNLOCK          	0x2
#define LUT_KEY                  	0x5af05af0

#define OPRND_0_SHIFT               0
#define OPRND_0(x)                  ((x) << OPRND_0_SHIFT)
#define PAD_0_SHIFT                 8
#define PAD_0(x)                    ((x) << PAD_0_SHIFT)
#define INSTR_0_SHIFT               10
#define INSTR_0(x)                  ((x) << INSTR_0_SHIFT)
#define OPRND_1_SHIFT               16
#define OPRND_1(x)                  ((x) << OPRND_1_SHIFT)
#define PAD_1_SHIFT                 24
#define PAD_1(x)                    ((x) << PAD_1_SHIFT)
#define INSTR_1_SHIFT               26
#define INSTR_1(x)                  ((x) << INSTR_1_SHIFT)

#define LUT_CMD                    1
#define LUT_ADDR                   2
#define LUT_DUMMY                  3
#define LUT_READ                   7
#define LUT_WRITE                  8

#define LUT_PAD_1                   0
#define LUT_PAD_2                   1
#define LUT_PAD_4                   2

#define ADDR_24BIT                  0x18
#define ADDR_32BIT                  0x20

/* SEQID */
#define SEQ_ID_WREN          1
#define SEQ_ID_FAST_READ     2
#define SEQ_ID_RDSR          3
#define SEQ_ID_SE            4
#define SEQ_ID_CHIP_ERASE    5
#define SEQ_ID_PP            6
#define SEQ_ID_RDID          7
#define SEQ_ID_BE_4K         8
#define SEQ_ID_WRAR          13
#define SEQ_ID_RDAR          14

/* QSPI CMD */
#define CMD_PP         	0x02   /* Page program (up to 256 bytes) */
#define CMD_RDSR            0x05   /* Read status register */
#define CMD_WREN            0x06   /* Write enable */
#define CMD_FAST_READ  	0x0b   /* Read data bytes (high frequency) */
#define CMD_BE_4K           0x20    /* 4K erase */
#define CMD_CHIP_ERASE 	0xc7   /* Erase whole flash chip */
#define CMD_SE         	0xd8   /* Sector erase (usually 64KiB) */
#define CMD_RDID            0x9f   /* Read JEDEC ID */

/* 4-byte address QSPI CMD - used on Spansion and some Macronix flashes */
#define CMD_FAST_READ_4B      0x0c    /* Read data bytes (high frequency) */
#define CMD_PP_4B             0x12    /* Page program (up to 256 bytes) */
#define CMD_SE_4B             0xdc    /* Sector erase (usually 64KiB) */

/* Used for Micron, winbond and Macronix flashes */
#define  CMD_WREAR       	0xc5   /* EAR register write */
#define  CMD_RDEAR       	0xc8   /* EAR reigster read */

/* Used for Spansion flashes only. */
#define  CMD_BRRD        	0x16   /* Bank register read */
#define  CMD_BRWR        	0x17   /* Bank register write */

/* Used for Spansion S25FS-S family flash only. */
#define  CMD_RDAR        	0x65   /* Read any device register */
#define  CMD_WRAR        	0x71   /* Write any device register */

#define IPCR_SEQID_SHIFT   	24
#define IPCR_SEQID_MASK     (0xf << IPCR_SEQID_SHIFT)

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

/* Sf Param Flags */
enum {
  SECT_4K    = 0 << 0,
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
 * @Slave: 	Instance of structure SpiSlave, represent slave device
		connected to controller
 * @Regs: 	Pointer to Qspi Registers structure
 * @Baudrate: Baudrate
 * @BusClk: QSPI input clk frequency
 * @Charbit: 	Charbit
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
 *			 used for read, write and erase operations.
 *
 * @Qspi:          Pointer to structure containing information about
 *		     QSPI controller register and slave device connected
 *		     to controller.
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
  CONST INT8 *Name;
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

/**
  This API detect the slave device connected to qspi controller and
  initializes slave device structure.

  @retval EFI_SUCCESS       	Slave device is attached to QSPI Controller.
  @retval EFI_OUT_OF_RESOURCES	Failed to allocate memory.
**/
EFI_STATUS
QspiDetect(
  VOID
  );

/**
  This API read the length bytes of data starting from specified offset
  from connected flash memory device to an input memory buffer

  @param[in]  Offset    	Offset of Flash memory to start with.
  @param[in]  Len	 	Length of data to be read.
  @param[out] Buf		Pointer to memory buffer to keep read data.

  @retval EFI_SUCCESS       	Read operation is successful.
  @retval EFI_INVALID_PARAMETER 	Input Parameter is invalid.
**/
EFI_STATUS
QspiFlashRead (
  IN  UINT32 Offset,
  IN  UINT64 Len,
  OUT VOID *Buf
  );

/**
  This API write the length bytes of data from inputted memory
  buffer to connected flash memory device starting from specified offset.

  @param[in]  Offset    	Offset of Flash memory to start with.
  @param[in]  Len	 	Length of data to write.
  @param[in]  Buf		Pointer to memory buffer to write data from.

  @retval EFI_SUCCESS       	Write operation is successful.
  @retval EFI_INVALID_PARAMETER 	Input Parameter is invalid.
**/
EFI_STATUS
QspiFlashWrite (
  IN  UINT32 Offset,
  IN  UINT64 Len,
  IN  CONST VOID *Buf
  );

/**
  This API erase length bytes of flash memory device starting from
  input offset value.

  @param[in]  Offset    	Offset of Flash memory to start with.
  @param[in]  Len	 	Length of flash memory need to erase.

  @retval EFI_SUCCESS       	Erase operation is successful.
  @retval EFI_INVALID_PARAMETER 	Input Parameter is invalid.
**/
EFI_STATUS
QspiFlashErase (
  IN  UINT32 Offset,
  IN  UINT64 Len
  );

/**
  This API detect the slave device connected to qspi controller
  and initialize block IO parameter.

  @param[in]  gQspiMedia	Indicates a pointer to the block IO media
				device.

  @retval EFI_SUCCESS       	Slave device is attached to QSPI Controller.
  @retval EFI_OUT_OF_RESOURCES	Failed to allocate memory.
**/
EFI_STATUS
QspiInit (
  EFI_BLOCK_IO_MEDIA *gQspiMedia
  );

/**
  This API Erases BufferSize bytes starting from Lba.

  @param[in]  This    	Indicates a pointer to the calling context.
  @param[in]  MediaId 	Id of the media, changes every time media is replaced.
  @param[in]  Lba		The starting Logical Block Address to be erased
  @param[in]  BufferSize	Size of Buffer, must be a multiple of device
  				block size.

  @retval EFI_SUCCESS       	The data was erased successfully.
  @retval EFI_WRITE_PROTECTED	The device cannot be written to.
  @retval EFI_DEVICE_ERROR   	The device reported an error while performing
					the erase.
  @retval EFI_NO_MEDIA         	There is no media in the device.
  @retval EFI_MEDIA_CHANGED     	The MediaId does not matched the current
  					device.
  @retval EFI_BAD_BUFFER_SIZE   	Buffer not a multiple of the block size
  					of the device.
  @retval EFI_INVALID_PARAMETER 	The erase request contains LBAs that are
  					not valid.
**/
EFI_STATUS
EFIAPI
QspiErase (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize
);

/**
  This API Writes BufferSize bytes from Lba into Buffer.

  @param[in]  This    	Indicates a pointer to the calling context.
  @param[in]  MediaId 	Id of the media, changes every time media is replaced.
  @param[in]  Lba		The starting Logical Block Address to be written
				The caller is responsible for writing to only
				legitimate locations.
  @param[in]  BufferSize	Size of Buffer, must be a multiple of device
  				block size.
  @param[in]  Buffer		A pointer to the source buffer for the data.

  @retval EFI_SUCCESS       	The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED	The device cannot be written to.
  @retval EFI_DEVICE_ERROR   	The device reported an error while performing
					the write.
  @retval EFI_NO_MEDIA         	There is no media in the device.
  @retval EFI_MEDIA_CHANGED     	The MediaId does not matched the current
  					device.
  @retval EFI_BAD_BUFFER_SIZE   	Buffer not a multiple of the block size
  					of the device.
  @retval EFI_INVALID_PARAMETER 	The write request contains LBAs that are
  					not valid, or the buffer is not on
					proper alignment
**/
EFI_STATUS
EFIAPI
QspiWrite (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize,
  IN  VOID                   *Buffer
);

/**
  This API read the BuferSize bytes of from Lba into Buffer.

  @param[in]  This    	Indicates a pointer to the calling context.
  @param[in]  MediaId 	Id of the media, changes every time media is replaced.
  @param[in]  Lba		The starting Logical Block Address to read from.
  @param[in]  BufferSize	Size of Buffer, must be a multiple of device
  				block size.
  @param[out] Buffer		A pointer to the destination buffer for the
  				data. The caller is responsible for either
				having implicit or explicit ownership of the
				buffer.

  @retval EFI_SUCCESS       	The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR   	The device reported an error while performing
					the read.
  @retval EFI_NO_MEDIA         	There is no media in the device.
  @retval EFI_MEDIA_CHANGED     	The MediaId does not matched the current
  					device.
  @retval EFI_BAD_BUFFER_SIZE   	Buffer not a multiple of the block size
  					of the device.
  @retval EFI_INVALID_PARAMETER 	The read request contains LBAs that are
  					not valid, or the buffer is not on
					proper alignment
**/
EFI_STATUS
EFIAPI
QspiRead (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  OUT VOID                          *Buffer
  );

/**
  This API free the global Qspi flash memory structure(QspiFlash).
**/
VOID
QspiFlashFree (
  VOID
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

#endif
