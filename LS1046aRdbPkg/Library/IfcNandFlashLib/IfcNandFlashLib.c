/** @IfcNandFlashLib.c

  API for implementing BlockIo protocol over IFC NAND

  Based on NAND flash driver implementation in
  Omap35xxPkg/Flash/Flash.c

  Copyright (c) 2011-2013, ARM Limited. All rights reserved.
  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/FslIfc.h>
#include <Library/IfcNand.h>
#include <Library/TimerLib.h>

NAND_PART_INFO_TABLE gNandPartInfoTable[1] = {
  { 0x2C, 0xAC, 17, 11}
};

UINT32 NandCs;
NAND_FLASH_INFO NandFlashInfo;
NAND_FLASH_INFO *gNandFlashInfo = NULL;

UINT8           *gEccCode;
UINTN           gNum512BytesChunks = 0;

VOID IfcNandAlign2BlkSize(
		UINTN *Size
		)
{
	if(gNandFlashInfo) {
		if(*Size % gNandFlashInfo->BlockSize)
			*Size += (gNandFlashInfo->BlockSize - (*Size % gNandFlashInfo->BlockSize));
	}
}

/*
 * execute IFC NAND command and wait for it to complete
 */
INTN IfcRunCmd()
{
	UINT32 Status = 0;
	UINT32 Count;

	/* set the chip select for NAND Transaction */
	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_csel,
			(NandCs << 26));

	/* start read/write seq */
	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nandseq_strt,
		FSL_IFC_NAND_SEQ_STRT_FIR_STRT);

	/* wait for NAND Machine complete flag or timeout */

	Count = 0;
	while (Count++ < MAX_RETRY_COUNT) {
		Status = MmioReadBe32(
			(UINTN)
			&gNandFlashInfo->IfcRegs->ifc_nand.nand_evter_stat);

		if (Status & FSL_IFC_NAND_EVTER_STAT_OPC)
			break;

		MicroSecondDelay(100);
	}

	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_evter_stat,
			Status);

	if (Status & FSL_IFC_NAND_EVTER_STAT_FTOER)
		DEBUG ((EFI_D_ERROR, "Flash Time Out Error %x \n", Status));
	if (Status & FSL_IFC_NAND_EVTER_STAT_WPER)
		DEBUG ((EFI_D_ERROR, "Write Protect Error %x \n", Status));

	/* returns 0 on success otherwise non-zero) */
	return Status == FSL_IFC_NAND_EVTER_STAT_OPC ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

/*
 * Set up the IFC hardware block and page address fields, and the ifc nand
 * structure addr field to point to the correct IFC buffer in memory
 */
VOID SetAddressRegs(INTN column, INTN page_addr)
{
	/* Program ROW0/COL0 */
	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.row0, page_addr);
	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.col0, column);
}



EFI_STATUS IfcWait(
		VOID
		)
{
	EFI_STATUS Status;

	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir0,
		  (FSL_IFC_FIR_OP_CW0 << FSL_IFC_NAND_FIR0_OP0_SHIFT) |
		  (FSL_IFC_FIR_OP_RDSTAT << FSL_IFC_NAND_FIR0_OP1_SHIFT));
	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fcr0, IFC_NAND_CMD_STATUS <<
		  FSL_IFC_NAND_FCR0_CMD0_SHIFT);
	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fbcr, 1);
	SetAddressRegs(0, 0);

	Status = IfcRunCmd();

	return Status;
}

VOID IfcRead(
	VOID
)
{
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir0,
			(FSL_IFC_FIR_OP_CW0 << FSL_IFC_NAND_FIR0_OP0_SHIFT) |
			(FSL_IFC_FIR_OP_CA0 << FSL_IFC_NAND_FIR0_OP1_SHIFT) |
			(FSL_IFC_FIR_OP_RA0 << FSL_IFC_NAND_FIR0_OP2_SHIFT) |
			(FSL_IFC_FIR_OP_CMD1 << FSL_IFC_NAND_FIR0_OP3_SHIFT) |
			(FSL_IFC_FIR_OP_BTRD << FSL_IFC_NAND_FIR0_OP4_SHIFT));
		MmioWriteBe32(
			(UINTN)
			&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir1, 0x0);

		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fcr0,
			(IFC_NAND_CMD_READ0 << FSL_IFC_NAND_FCR0_CMD0_SHIFT) |
			(IFC_NAND_CMD_READSTART << FSL_IFC_NAND_FCR0_CMD1_SHIFT));
}

/* cmdfunc send commands to the IFC NAND Machine */
EFI_STATUS IfcNandCmdSend(UINTN command, INTN column, INTN page_addr)
{
	EFI_STATUS Status;
	UINT32 NandFcr0;
	switch (command) {
	case IFC_NAND_CMD_READ0:
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fbcr, 0);
		SetAddressRegs(0, page_addr);

		IfcRead();
		return IfcRunCmd();

	case IFC_NAND_CMD_READID:
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir0,
			(FSL_IFC_FIR_OP_CW0 << FSL_IFC_NAND_FIR0_OP0_SHIFT) |
			(FSL_IFC_FIR_OP_UA << FSL_IFC_NAND_FIR0_OP1_SHIFT) |
			(FSL_IFC_FIR_OP_RB << FSL_IFC_NAND_FIR0_OP2_SHIFT));
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fcr0,
			command << FSL_IFC_NAND_FCR0_CMD0_SHIFT);
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.row3, column);

			MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fbcr, 256);

		SetAddressRegs(0, 0);
		return IfcRunCmd();

	case IFC_NAND_CMD_ERASE1:
		SetAddressRegs(0, page_addr);

		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir0,
			(FSL_IFC_FIR_OP_CW0 << FSL_IFC_NAND_FIR0_OP0_SHIFT) |
			(FSL_IFC_FIR_OP_RA0 << FSL_IFC_NAND_FIR0_OP1_SHIFT) |
			(FSL_IFC_FIR_OP_CMD1 << FSL_IFC_NAND_FIR0_OP2_SHIFT));

		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fcr0,
			(IFC_NAND_CMD_ERASE1 << FSL_IFC_NAND_FCR0_CMD0_SHIFT) |
			(IFC_NAND_CMD_ERASE2 << FSL_IFC_NAND_FCR0_CMD1_SHIFT));

		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fbcr, 0);
		return IfcRunCmd();

	case IFC_NAND_CMD_SEQIN: 

			NandFcr0 =
				(IFC_NAND_CMD_SEQIN << FSL_IFC_NAND_FCR0_CMD0_SHIFT) |
				(IFC_NAND_CMD_STATUS << FSL_IFC_NAND_FCR0_CMD1_SHIFT) |
				(IFC_NAND_CMD_PAGEPROG << FSL_IFC_NAND_FCR0_CMD2_SHIFT);

			MmioWriteBe32(
				(UINTN)
				&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir0,
				(FSL_IFC_FIR_OP_CW0 << FSL_IFC_NAND_FIR0_OP0_SHIFT) |
				(FSL_IFC_FIR_OP_CA0 << FSL_IFC_NAND_FIR0_OP1_SHIFT) |
				(FSL_IFC_FIR_OP_RA0 << FSL_IFC_NAND_FIR0_OP2_SHIFT) |
				(FSL_IFC_FIR_OP_WBCD <<
						FSL_IFC_NAND_FIR0_OP3_SHIFT) |
				(FSL_IFC_FIR_OP_CMD2 << FSL_IFC_NAND_FIR0_OP4_SHIFT));
			MmioWriteBe32((UINTN)
				&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir1,
				(FSL_IFC_FIR_OP_CW1 << FSL_IFC_NAND_FIR1_OP5_SHIFT) |
				(FSL_IFC_FIR_OP_RDSTAT <<
					FSL_IFC_NAND_FIR1_OP6_SHIFT) |
				(FSL_IFC_FIR_OP_NOP << FSL_IFC_NAND_FIR1_OP7_SHIFT));
		
		MmioWriteBe32(
		(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fcr0, NandFcr0);
		SetAddressRegs(column, page_addr);
		return EFI_SUCCESS;

	case IFC_NAND_CMD_PAGEPROG:
		MmioWriteBe32(
		(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fbcr, 0);

		return IfcRunCmd();

	case IFC_NAND_CMD_STATUS:
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir0,
			(FSL_IFC_FIR_OP_CW0 << FSL_IFC_NAND_FIR0_OP0_SHIFT) |
			(FSL_IFC_FIR_OP_RB << FSL_IFC_NAND_FIR0_OP1_SHIFT));
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fcr0,
			IFC_NAND_CMD_STATUS << FSL_IFC_NAND_FCR0_CMD0_SHIFT);
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fbcr, 1);
		SetAddressRegs(0, 0);

		Status  = IfcRunCmd();

		MmioWrite8(
			(UINTN)gNandFlashInfo->BufBase,
			MmioRead8(
			(UINTN)gNandFlashInfo->BufBase) | NAND_STATUS_WP);
		return Status;

	case IFC_NAND_CMD_RESET:
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fir0,
			FSL_IFC_FIR_OP_CW0 << FSL_IFC_NAND_FIR0_OP0_SHIFT);
		MmioWriteBe32(
			(UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_fcr0,
			IFC_NAND_CMD_RESET << FSL_IFC_NAND_FCR0_CMD0_SHIFT);
	return	IfcRunCmd();

	default:
		DEBUG ((EFI_D_ERROR, "Unsupported Command 0x%x.\n",
			command));
		return EFI_UNSUPPORTED;
	}
}

UINTN
GetActualPageAddress (
  UINTN BlockIndex,
  UINTN PageIndex
)
{
  return ((BlockIndex * gNandFlashInfo->NumPagesPerBlock) + PageIndex);
}

EFI_STATUS
NandDetectPart (
  VOID
)
{
    UINT8      PartInfo[8];
    UINTN      Index;
    BOOLEAN    Found = FALSE;
		EFI_STATUS Status;

  //Send READ ID command
  Status = IfcNandCmdSend(IFC_NAND_CMD_READID, 0, 0);
	if(EFI_ERROR(Status))
		return Status;

  //Read 5-bytes to idenfity code programmed into the NAND flash devices.
  //BYTE 0 = Manufacture ID
  //Byte 1 = Device ID
  //Byte 2, 3, 4 = Nand part specific information (Page size, Block size etc)

  for(Index = 0; Index < sizeof(PartInfo); Index++) {
		PartInfo[Index] = MmioRead8((UINTN)gNandFlashInfo->BufBase + Index);
  }

  //Check if the ManufactureId and DeviceId are part of the currently
  //supported nand parts.
  for (Index = 0;
	Index < sizeof(gNandPartInfoTable)/sizeof(NAND_PART_INFO_TABLE);
	Index++) {
    if (gNandPartInfoTable[Index].ManufactureId == PartInfo[0] &&
		gNandPartInfoTable[Index].DeviceId == PartInfo[1]) {
	gNandFlashInfo->BlockAddressStart =
		gNandPartInfoTable[0].BlockAddressStart;
	gNandFlashInfo->PageAddressStart =
		gNandPartInfoTable[0].PageAddressStart;
	gNandFlashInfo->PageSize = NAND_PAGE_SIZE_2K;
	gNandFlashInfo->SparePageSize = NAND_SPARE_AREA_SIZE_64B;
	gNandFlashInfo->BlockSize = NAND_BLOCK_SIZE_128K;
	Found = TRUE;
	break;
    }
  }

  if (Found == FALSE) {
    DEBUG ((EFI_D_ERROR, "Nand part is not currently supported.\
			Manufacture id: %x, Device id: %x\n",
			PartInfo[0], PartInfo[1]));
    return EFI_NOT_FOUND;
  }

  //Populate NAND_FLASH_INFO based on the result of READ ID command.
  gNandFlashInfo->ManufactureId = PartInfo[0];
  gNandFlashInfo->DeviceId = PartInfo[1];

  //Calculate total number of blocks.
  gNandFlashInfo->NumPagesPerBlock = DivU64x32(gNandFlashInfo->BlockSize,
		gNandFlashInfo->PageSize);

  return EFI_SUCCESS;
}

VOID
NandConfigureEcc (
  VOID
  )
{
  //Define ECC size 0 and size 1 to 512 bytes
}

VOID
NandEnableEcc (
  VOID
  )
{
  //Clear all the ECC result registers and select ECC result register 1

  //Enable ECC engine on CS0
}

VOID
NandDisableEcc (
  VOID
  )
{
  //Turn off ECC engine.
}

VOID
NandCalculateEcc (
  VOID
  )
{
}

EFI_STATUS
NandReadPage (
  IN  UINTN                         BlockIndex,
  IN  UINTN                         PageIndex,
  OUT VOID                          *Buffer
)
{
	EFI_STATUS Status;
  UINTN      PageAddr;
	VOID			*SrcAddr;
	UINT8			*SpareArea;

  //Generate device address in bytes to access specific block and page index
  PageAddr = GetActualPageAddress(BlockIndex, PageIndex);

  //Send READ command
  Status = IfcNandCmdSend(IFC_NAND_CMD_READ0, 0, PageAddr);
	if(EFI_ERROR(Status))
		return Status;
	SrcAddr = (VOID*)(gNandFlashInfo->BufBase +
			(gNandFlashInfo->PageSize << 1) * (PageIndex & 0x3));
	SpareArea = (UINT8*)SrcAddr + gNandFlashInfo->PageSize;
	if(*SpareArea != 0xff)
		return EFI_DEVICE_ERROR;
  CopyMem(Buffer, SrcAddr, gNandFlashInfo->PageSize);
  return EFI_SUCCESS;
}

EFI_STATUS
NandWritePage (
  IN  UINTN                         BlockIndex,
  IN  UINTN                         PageIndex,
  OUT VOID                          *Buffer,
  IN  UINT8                         *SpareBuffer
)
{
  UINTN      Address;
	VOID			*DestAddr;

  //Generate device address in bytes to access specific block and page index
  Address = GetActualPageAddress(BlockIndex, PageIndex);

  //Send SERIAL DATA INPUT command
  IfcNandCmdSend(IFC_NAND_CMD_SEQIN, 0, Address);

	DestAddr = (VOID*)(gNandFlashInfo->BufBase +  
			(gNandFlashInfo->PageSize << 1) * (PageIndex & 0x3));
  //Data input from Buffer
  CopyMem(DestAddr, (VOID*) Buffer, gNandFlashInfo->PageSize);

	//Send PROGRAM command
  return IfcNandCmdSend(IFC_NAND_CMD_PAGEPROG, 0, Address);
}

EFI_STATUS
NandEraseBlock (
  IN UINTN BlockIndex
)
{
  UINTN      Address;

  //Generate device address in bytes to access specific block and page index
  Address = GetActualPageAddress(BlockIndex, 0);

  //Send ERASE command
  return IfcNandCmdSend(IFC_NAND_CMD_ERASE1, 0, Address);
}

EFI_STATUS
NandReadBlock (
  IN UINTN                          BlockIndex,
  OUT VOID                          *Buffer
)
{
  UINTN      PageIndex;
  EFI_STATUS Status = EFI_SUCCESS;

  for (PageIndex = 0; PageIndex < gNandFlashInfo->NumPagesPerBlock;
		PageIndex++) {
      Status = NandReadPage(BlockIndex, PageIndex, Buffer);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      Buffer = ((UINT8 *)Buffer + gNandFlashInfo->PageSize);
  }
  return Status;
}

EFI_STATUS
NandWriteBlock (
  IN UINTN                          StartBlockIndex,
  IN UINTN                          EndBlockIndex,
  OUT VOID                          *Buffer,
  OUT VOID                          *SpareBuffer
  )
{
  UINTN      BlockIndex;
  UINTN      PageIndex;
  EFI_STATUS Status = EFI_SUCCESS;

  for (BlockIndex = StartBlockIndex; BlockIndex <= EndBlockIndex; BlockIndex++){
    //Page programming.
    for (PageIndex = 0; PageIndex < gNandFlashInfo->NumPagesPerBlock;
		PageIndex++) {
      Status = NandWritePage(BlockIndex, PageIndex, Buffer, SpareBuffer);
      if (EFI_ERROR(Status)) {
				DEBUG((EFI_D_ERROR,"NandWritePage Failed\n"));
        return Status;
      }
			Status = IfcWait();
      if (EFI_ERROR(Status)) {
				DEBUG((EFI_D_ERROR,"IfcWait Failed\n"));
        return Status;
      }
      Buffer = ((UINT8 *)Buffer + gNandFlashInfo->PageSize);
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
IfcNandFlashReset (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  return IfcNandCmdSend(IFC_NAND_CMD_RESET, 0, 0);
}

EFI_STATUS
EFIAPI
IfcNandFlashReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  OUT VOID                          *Buffer
  )
{
  UINTN			NumBlocks;
  UINTN			Index;
  UINTN			BlockCount;
	VOID			*TBuf;
  EFI_STATUS Status = EFI_SUCCESS;

  if (Buffer == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto exit;
  }

  if (Lba > NAND_LAST_BLOCK) {
    Status = EFI_INVALID_PARAMETER;
    goto exit;
  }

  if ((BufferSize % gNandFlashInfo->BlockSize) != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
    goto exit;
  }

  NumBlocks = DivU64x32(BufferSize, gNandFlashInfo->BlockSize);

	TBuf = Buffer;
	Index = Lba;
	BlockCount = 0;
	while(BlockCount < NumBlocks) {
		Status = NandReadBlock((UINTN)Index, TBuf);
	  if (EFI_ERROR(Status)) {
		  DEBUG((EFI_D_ERROR, "Read block failed, skipping %x\n", Status));
			Index++;
			continue;
		}
		TBuf+=gNandFlashInfo->BlockSize;
		BlockCount++;
		Index++;
	}

exit:
  return Status;
}

EFI_STATUS
EFIAPI
IfcNandFlashWriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  IN VOID                           *Buffer
  )
{
  UINTN      BlockIndex;
  UINTN      NumBlocks;
  UINTN      EndBlockIndex;
  EFI_STATUS Status;
  UINT8      *SpareBuffer = NULL;

  if (Buffer == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto exit;
  }

  if (Lba > NAND_LAST_BLOCK) {
    Status = EFI_INVALID_PARAMETER;
    goto exit;
  }

  if ((BufferSize % gNandFlashInfo->BlockSize) != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
    goto exit;
  }

  NumBlocks = DivU64x32(BufferSize, gNandFlashInfo->BlockSize);
  EndBlockIndex = ((UINTN)Lba + NumBlocks) - 1;

  // Erase block
  for (BlockIndex = (UINTN)Lba; BlockIndex <= EndBlockIndex; BlockIndex++) {
    Status = NandEraseBlock(BlockIndex);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Erase block failed. Status: %x\n", Status));
      goto exit;
    }
		Status = IfcWait();
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Wait on Erase block failed. Status: %x\n", Status));
      goto exit;
    }
  }

  // Program data
  Status = NandWriteBlock((UINTN)Lba, EndBlockIndex, Buffer, SpareBuffer);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Block write fails: %x\n", Status));
    goto exit;
  }

exit:
  if (SpareBuffer != NULL) {
    FreePool (SpareBuffer);
  }

  return Status;
}

EFI_STATUS
EFIAPI
IfcNandFlashFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

VOID IfcNandInit(
	VOID
)
{
	if(PcdGet32(PcdBootMode) == NAND_BOOT)
		NandCs = FSL_IFC_CS0;
	else
		NandCs = FSL_IFC_CS1;
  gNandFlashInfo = &NandFlashInfo;
  gNandFlashInfo->IfcRegs = (FSL_IFC_REGS*) FSL_IFC_REG_BASE;
	gNandFlashInfo->BufBase = (VOID*) FSL_IFC_NAND_BUF_BASE;

  /* clear event registers */

	MmioWriteBe32(
		(UINTN)
		&gNandFlashInfo->IfcRegs->ifc_nand.pgrdcmpl_evt_stat, ~0U);

  MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->ifc_nand.nand_evter_stat, ~0U);

        /* Enable error and event for any detected errors */
  MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.nand_evter_en,
                  FSL_IFC_NAND_EVTER_EN_OPC_EN |
                  FSL_IFC_NAND_EVTER_EN_PGRDCMPL_EN |
                  FSL_IFC_NAND_EVTER_EN_FTOER_EN |
                  FSL_IFC_NAND_EVTER_EN_WPER_EN);
	MmioWriteBe32((UINTN)&gNandFlashInfo->IfcRegs->ifc_nand.ncfgr, 0x0);

	MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->ftim_cs[NandCs].ftim[FSL_IFC_FTIM0],
		NAND_FTIM0);

	MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->ftim_cs[NandCs].ftim[FSL_IFC_FTIM1],
		NAND_FTIM1);

	MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->ftim_cs[NandCs].ftim[FSL_IFC_FTIM2],
		NAND_FTIM2);

	MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->ftim_cs[NandCs].ftim[FSL_IFC_FTIM3],
		NAND_FTIM3);

	MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->cspr_cs[NandCs].cspr,
		FSL_IFC_NAND_CSPR);

	MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->cspr_cs[NandCs].cspr_ext,
		FSL_IFC_NAND_CSPR_EXT);

	MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->amask_cs[NandCs].amask,
		FSL_IFC_NAND_AMASK);

	MmioWriteBe32((UINTN)
		&gNandFlashInfo->IfcRegs->csor_cs[NandCs].csor,
		FSL_IFC_NAND_CSOR);
}

EFI_STATUS
IfcNandFlashInit (
	EFI_BLOCK_IO_MEDIA *gNandFlashMedia
)
{
  EFI_STATUS  Status;
	if(PcdGet32(PcdBootMode) == NAND_BOOT)
		NandCs = FSL_IFC_CS0;
	else
		NandCs = FSL_IFC_CS1;
  gNandFlashInfo = &NandFlashInfo;
	gNandFlashInfo->IfcRegs = (FSL_IFC_REGS*) FSL_IFC_REG_BASE;
	gNandFlashInfo->BufBase = (VOID*) FSL_IFC_NAND_BUF_BASE;

  //Reset NAND part
  IfcNandFlashReset(NULL, FALSE);

  //Detect NAND part and populate gNandFlashInfo structure
  Status = NandDetectPart ();
  if (EFI_ERROR(Status)) {
    DEBUG((
	EFI_D_ERROR, "Nand part id detection failure: Status: %x\n", Status));
    return Status;
  }

  //Patch EFI_BLOCK_IO_MEDIA structure.
  if(gNandFlashMedia) {
	  gNandFlashMedia->BlockSize = gNandFlashInfo->BlockSize;
	  gNandFlashMedia->LastBlock = NAND_LAST_BLOCK;
  }
  return Status;
}

