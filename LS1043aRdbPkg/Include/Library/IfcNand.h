/** @IfcNand.h

	Based on BlockIo protocol implementation available in
	MdePkg/Include/Protocol/BlockIo.h

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DebugLib.h>
#include <Protocol/BlockIo.h>
#include <Library/FslIfc.h>

/**
 * Align Size to NAND block size
**/

VOID IfcNandAlign2BlkSize(
		UINTN *Size
);

/**
  Initialize IFC Nand Flash interface
**/

EFI_STATUS
IfcNandFlashInit (
  OUT EFI_BLOCK_IO_MEDIA *gNandFlashMedia;
);

/**
  Write BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    The media ID that the write request is for.
  @param  Lba        The starting logical block address to be written.
		     The caller is responsible for writing to only legitimate
		     locations.
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
				the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size
				of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not
				valid, or the buffer is not on proper alignment.

**/

EFI_STATUS
EFIAPI
IfcNandFlashWriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  IN VOID                           *Buffer
  );

/**
  Read BufferSize bytes from Lba into Buffer.

  @param  This		Indicates a pointer to the calling context.
  @param  MediaId    	Id of the media, changes every time media is replaced.
  @param  Lba        	The starting Logical Block Address to read from
  @param  BufferSize	Size of Buffer, must be a multiple of device block size.
  @param  Buffer	A pointer to the destination buffer for the data. The
			caller is responsible for either having implicit or
			explicit ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
				the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size
				of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not
				valid, or the buffer is not on proper alignment.
**/

EFI_STATUS
EFIAPI
IfcNandFlashReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  OUT VOID                          *Buffer
  );

/**
  Reset the Block Device.

  @param  This			Indicates a pointer to the calling context.
  @param  ExtendedVerification	Driver may perform diagnostics on reset.

  @retval EFI_SUCCESS		The device was reset.
  @retval EFI_DEVICE_ERROR	The device is not functioning properly and could
				not be reset.
**/

EFI_STATUS
EFIAPI
IfcNandFlashReset (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN BOOLEAN                        ExtendedVerification
  );

/**
  Flush the Block Device.

  @param  This			Indicates a pointer to the calling context.

  @retval EFI_SUCCESS		All outstanding data was written to the device
  @retval EFI_DEVICE_ERROR	The device reported an error while writting back
				the data
  @retval EFI_NO_MEDIA		There is no media in the device.
**/

EFI_STATUS
EFIAPI
IfcNandFlashFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  );

typedef struct {
  UINT8 ManufactureId;
  UINT8 DeviceId;
  UINT8 BlockAddressStart; //Start of the Block address in actual NAND
  UINT8 PageAddressStart; //Start of the Page address in actual NAND
} NAND_PART_INFO_TABLE;

typedef struct {
  UINT8		    ManufactureId;
  UINT8		    DeviceId;
  UINT8		    Organization; //x8 or x16
  UINT32	    PageSize;
  UINT32	    SparePageSize;
  UINT32	    BlockSize;
  UINT32	    NumPagesPerBlock;
  UINT8		    BlockAddressStart;//Start of Block address in actual NAND
  UINT8		    PageAddressStart; //Start of Page address in actual NAND
  VOID*		    BufBase;
  FSL_IFC_REGS*     IfcRegs;
  UINT32	    cs_nand;
} NAND_FLASH_INFO;


#define NAND_FTIM0   (FSL_IFC_FTIM0_NAND_TCCST(0x7) | \
		          FSL_IFC_FTIM0_NAND_TWP(0x18)   | \
		          FSL_IFC_FTIM0_NAND_TWCHT(0x7) | \
		          FSL_IFC_FTIM0_NAND_TWH(0xa))
#define NAND_FTIM1   (FSL_IFC_FTIM1_NAND_TADLE(0x32) | \
		          FSL_IFC_FTIM1_NAND_TWBE(0x39)  | \
		          FSL_IFC_FTIM1_NAND_TRR(0xe)   | \
		          FSL_IFC_FTIM1_NAND_TRP(0x18))
#define NAND_FTIM2   (FSL_IFC_FTIM2_NAND_TRAD(0xf) | \
		          FSL_IFC_FTIM2_NAND_TREH(0xa) | \
		          FSL_IFC_FTIM2_NAND_TWHRE(0x1e))
#define NAND_FTIM3   0x0

