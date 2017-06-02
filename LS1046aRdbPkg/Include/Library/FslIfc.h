/** @FslIfc.h

	Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __FLASH_H__
#define __FLASH_H__

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <LS1046aRdb.h>

#include <Protocol/BlockIo.h>
#include <Protocol/Cpu.h>

#define NOIBR

#define FSL_IFC_BANK_COUNT	4

#define NAND_PAGE_SIZE_2K		(2048)
#define NAND_SPARE_AREA_SIZE_64B	(64)
#define NAND_BLOCK_SIZE_128K		(128*1024)
#define NAND_BLOCK_COUNT		(2048)
#define NAND_LAST_BLOCK		(NAND_BLOCK_COUNT - 1)


//List of commands.
#define IFC_NAND_CMD_RESET		0xFF
#define IFC_NAND_CMD_READID		0x90

#define IFC_NAND_CMD_STATUS		0x70

#define IFC_NAND_CMD_READ0		0x00
#define IFC_NAND_CMD_READSTART	0x30

#define IFC_NAND_CMD_ERASE1		0x60
#define IFC_NAND_CMD_ERASE2 	0xD0

#define IFC_NAND_CMD_SEQIN		0x80
#define IFC_NAND_CMD_PAGEPROG	0x10

#define MAX_RETRY_COUNT		150000

#define FSL_IFC_REG_BASE	 	0x1530000
#define FSL_IFC_NAND_BUF_BASE	0x7E800000
#define FSL_IFC_NOR_BUF_BASE	0x60000000

#define FSL_IFC_NOR_RESERVED_REGION_BASE	0x60700000

#define FSL_IFC_CSPR_REG_LEN	148
#define FSL_IFC_AMASK_REG_LEN	144
#define FSL_IFC_CSOR_REG_LEN	144
#define FSL_IFC_FTIM_REG_LEN	576

#define FSL_IFC_NAND_SEQ_STRT_FIR_STRT	0x80000000

#define FSL_IFC_CSPR_USED_LEN	sizeof(FSL_IFC_CSPR) * \
					FSL_IFC_BANK_COUNT
#define FSL_IFC_AMASK_USED_LEN	sizeof(FSL_IFC_AMASK) * \
					FSL_IFC_BANK_COUNT
#define FSL_IFC_CSOR_USED_LEN	sizeof(FSL_IFC_CSOR) * \
					FSL_IFC_BANK_COUNT
#define FSL_IFC_FTIM_USED_LEN	sizeof(FSL_IFC_FTIM) * \
					FSL_IFC_BANK_COUNT

/*
 * NAND Event and Error Status Register (NAND_EVTER_STAT)
 */
/* Operation Complete */
#define FSL_IFC_NAND_EVTER_STAT_OPC		0x80000000
/* Flash Timeout Error */
#define FSL_IFC_NAND_EVTER_STAT_FTOER	0x08000000
/* Write Protect Error */
#define FSL_IFC_NAND_EVTER_STAT_WPER	0x04000000
/* ECC Error */
#define FSL_IFC_NAND_EVTER_STAT_ECCER	0x02000000

/*
 * NAND Flash Byte Count Register (NAND_BC)
 */
/* Byte Count for read/Write */
#define FSL_IFC_NAND_BC			0x000001FF

/*
 * NAND Flash Instruction Registers (NAND_FIR0/NAND_FIR1/NAND_FIR2)
 */
/* NAND Machine specific opcodes OP0-OP14*/
#define FSL_IFC_NAND_FIR0_OP0		0xFC000000
#define FSL_IFC_NAND_FIR0_OP0_SHIFT		26
#define FSL_IFC_NAND_FIR0_OP1		0x03F00000
#define FSL_IFC_NAND_FIR0_OP1_SHIFT		20
#define FSL_IFC_NAND_FIR0_OP2		0x000FC000
#define FSL_IFC_NAND_FIR0_OP2_SHIFT		14
#define FSL_IFC_NAND_FIR0_OP3		0x00003F00
#define FSL_IFC_NAND_FIR0_OP3_SHIFT		8
#define FSL_IFC_NAND_FIR0_OP4		0x000000FC
#define FSL_IFC_NAND_FIR0_OP4_SHIFT		2
#define FSL_IFC_NAND_FIR1_OP5		0xFC000000
#define FSL_IFC_NAND_FIR1_OP5_SHIFT		26
#define FSL_IFC_NAND_FIR1_OP6		0x03F00000
#define FSL_IFC_NAND_FIR1_OP6_SHIFT		20
#define FSL_IFC_NAND_FIR1_OP7		0x000FC000
#define FSL_IFC_NAND_FIR1_OP7_SHIFT		14
#define FSL_IFC_NAND_FIR1_OP8		0x00003F00
#define FSL_IFC_NAND_FIR1_OP8_SHIFT		8
#define FSL_IFC_NAND_FIR1_OP9		0x000000FC
#define FSL_IFC_NAND_FIR1_OP9_SHIFT		2
#define FSL_IFC_NAND_FIR2_OP10		0xFC000000
#define FSL_IFC_NAND_FIR2_OP10_SHIFT	26
#define FSL_IFC_NAND_FIR2_OP11		0x03F00000
#define FSL_IFC_NAND_FIR2_OP11_SHIFT	20
#define FSL_IFC_NAND_FIR2_OP12		0x000FC000
#define FSL_IFC_NAND_FIR2_OP12_SHIFT	14
#define FSL_IFC_NAND_FIR2_OP13		0x00003F00
#define FSL_IFC_NAND_FIR2_OP13_SHIFT	8
#define FSL_IFC_NAND_FIR2_OP14		0x000000FC
#define FSL_IFC_NAND_FIR2_OP14_SHIFT	2

/*
 * NAND Flash Command Registers (NAND_FCR0/NAND_FCR1)
 */
/* General purpose FCM flash command bytes CMD0-CMD7 */
#define FSL_IFC_NAND_FCR0_CMD0		0xFF000000
#define FSL_IFC_NAND_FCR0_CMD0_SHIFT	24
#define FSL_IFC_NAND_FCR0_CMD1		0x00FF0000
#define FSL_IFC_NAND_FCR0_CMD1_SHIFT	16
#define FSL_IFC_NAND_FCR0_CMD2		0x0000FF00
#define FSL_IFC_NAND_FCR0_CMD2_SHIFT	8
#define FSL_IFC_NAND_FCR0_CMD3		0x000000FF
#define FSL_IFC_NAND_FCR0_CMD3_SHIFT	0
#define FSL_IFC_NAND_FCR1_CMD4		0xFF000000
#define FSL_IFC_NAND_FCR1_CMD4_SHIFT	24
#define FSL_IFC_NAND_FCR1_CMD5		0x00FF0000
#define FSL_IFC_NAND_FCR1_CMD5_SHIFT	16
#define FSL_IFC_NAND_FCR1_CMD6		0x0000FF00
#define FSL_IFC_NAND_FCR1_CMD6_SHIFT	8
#define FSL_IFC_NAND_FCR1_CMD7		0x000000FF
#define FSL_IFC_NAND_FCR1_CMD7_SHIFT	0

/* Timing registers for NAND Flash */

#define FSL_IFC_FTIM0_NAND_TCCST_SHIFT	25
#define FSL_IFC_FTIM0_NAND_TCCST(n)	((n) << FSL_IFC_FTIM0_NAND_TCCST_SHIFT)
#define FSL_IFC_FTIM0_NAND_TWP_SHIFT	16
#define FSL_IFC_FTIM0_NAND_TWP(n)	((n) << FSL_IFC_FTIM0_NAND_TWP_SHIFT)
#define FSL_IFC_FTIM0_NAND_TWCHT_SHIFT	8
#define FSL_IFC_FTIM0_NAND_TWCHT(n)	((n) << FSL_IFC_FTIM0_NAND_TWCHT_SHIFT)
#define FSL_IFC_FTIM0_NAND_TWH_SHIFT	0
#define FSL_IFC_FTIM0_NAND_TWH(n)	((n) << FSL_IFC_FTIM0_NAND_TWH_SHIFT)
#define FSL_IFC_FTIM1_NAND_TADLE_SHIFT	24
#define FSL_IFC_FTIM1_NAND_TADLE(n)	((n) << FSL_IFC_FTIM1_NAND_TADLE_SHIFT)
#define FSL_IFC_FTIM1_NAND_TWBE_SHIFT	16
#define FSL_IFC_FTIM1_NAND_TWBE(n)	((n) << FSL_IFC_FTIM1_NAND_TWBE_SHIFT)
#define FSL_IFC_FTIM1_NAND_TRR_SHIFT	8
#define FSL_IFC_FTIM1_NAND_TRR(n)	((n) << FSL_IFC_FTIM1_NAND_TRR_SHIFT)
#define FSL_IFC_FTIM1_NAND_TRP_SHIFT	0
#define FSL_IFC_FTIM1_NAND_TRP(n)	((n) << FSL_IFC_FTIM1_NAND_TRP_SHIFT)
#define FSL_IFC_FTIM2_NAND_TRAD_SHIFT	21
#define FSL_IFC_FTIM2_NAND_TRAD(n)	((n) << FSL_IFC_FTIM2_NAND_TRAD_SHIFT)
#define FSL_IFC_FTIM2_NAND_TREH_SHIFT	11
#define FSL_IFC_FTIM2_NAND_TREH(n)	((n) << FSL_IFC_FTIM2_NAND_TREH_SHIFT)
#define FSL_IFC_FTIM2_NAND_TWHRE_SHIFT	0
#define FSL_IFC_FTIM2_NAND_TWHRE(n)	((n) << FSL_IFC_FTIM2_NAND_TWHRE_SHIFT)
#define FSL_IFC_FTIM3_NAND_TWW_SHIFT	24
#define FSL_IFC_FTIM3_NAND_TWW(n)	((n) << FSL_IFC_FTIM3_NAND_TWW_SHIFT)



/*
 * Flash ROW and COL Address Register (ROWn, COLn)
 */
/* Main/spare region locator */
#define FSL_IFC_NAND_COL_MS			0x80000000
/* Column Address */
#define FSL_IFC_NAND_COL_CA_MASK		0x00000FFF

#define NAND_STATUS_WP			0x80

/*
 * NAND Event and Error Enable Register (NAND_EVTER_EN)
 */
/* Operation complete event enable */
#define FSL_IFC_NAND_EVTER_EN_OPC_EN	0x80000000
/* Page read complete event enable */
#define FSL_IFC_NAND_EVTER_EN_PGRDCMPL_EN	0x20000000
/* Flash Timeout error enable */
#define FSL_IFC_NAND_EVTER_EN_FTOER_EN	0x08000000
/* Write Protect error enable */
#define FSL_IFC_NAND_EVTER_EN_WPER_EN	0x04000000
/* ECC error logging enable */
#define FSL_IFC_NAND_EVTER_EN_ECCER_EN	0x02000000

/*
 * CSPR - Chip Select Property Register
 */
#define IFC_CSPR_BA			0xFFFF0000
#define IFC_CSPR_BA_SHIFT 		16
#define IFC_CSPR_PORT_SIZE		0x00000180
#define IFC_CSPR_PORT_SIZE_SHIFT	7
/* Port Size 8 bit */
#define IFC_CSPR_PORT_SIZE_8	0x00000080
/* Port Size 16 bit */
#define IFC_CSPR_PORT_SIZE_16	0x00000100
/* Port Size 32 bit */
#define IFC_CSPR_PORT_SIZE_32	0x00000180
/* Write Protect */
#define IFC_CSPR_WP			0x00000040
#define IFC_CSPR_WP_SHIFT		6
/* Machine Select */
#define IFC_CSPR_MSEL		0x00000006
#define IFC_CSPR_MSEL_SHIFT		1
/* NOR */
#define IFC_CSPR_MSEL_NOR		0x00000000
/* NAND */
#define IFC_CSPR_MSEL_NAND		0x00000002
/* GPCM */
#define IFC_CSPR_MSEL_GPCM		0x00000004
/* Bank Valid */
#define IFC_CSPR_V			0x00000001
#define IFC_CSPR_V_SHIFT		0

/*
 * Chip Select Option Register - NOR Flash Mode
 */
/* Enable Address shift Mode */
#define IFC_CSOR_NOR_ADM_SHFT_MODE_EN	0x80000000
/* Page Read Enable from NOR device */
#define IFC_CSOR_NOR_PGRD_EN		0x10000000
/* AVD Toggle Enable during Burst Program */
#define IFC_CSOR_NOR_AVD_TGL_PGM_EN		0x01000000
/* Address Data Multiplexing Shift */
#define IFC_CSOR_NOR_ADM_MASK		0x0003E000
#define IFC_CSOR_NOR_ADM_SHIFT_SHIFT	13
#define IFC_CSOR_NOR_ADM_SHIFT(n)	((n) << IFC_CSOR_NOR_ADM_SHIFT_SHIFT)
/* Type of the NOR device hooked */
#define IFC_CSOR_NOR_NOR_MODE_AYSNC_NOR	0x00000000
#define IFC_CSOR_NOR_NOR_MODE_AVD_NOR	0x00000020
/* Time for Read Enable High to Output High Impedance */
#define IFC_CSOR_NOR_TRHZ_MASK		0x0000001C
#define IFC_CSOR_NOR_TRHZ_SHIFT		2
#define IFC_CSOR_NOR_TRHZ_20		0x00000000
#define IFC_CSOR_NOR_TRHZ_40		0x00000004
#define IFC_CSOR_NOR_TRHZ_60		0x00000008
#define IFC_CSOR_NOR_TRHZ_80		0x0000000C
#define IFC_CSOR_NOR_TRHZ_100		0x00000010
/* Buffer control disable */
#define IFC_CSOR_NOR_BCTLD			0x00000001

/*
 * Chip Select Option Register FSL_IFC_NAND Machine
 */
/* Enable ECC Encoder */
#define IFC_CSOR_NAND_ECC_ENC_EN	0x80000000
#define IFC_CSOR_NAND_ECC_MODE_MASK	0x30000000
/* 4 bit correction per 520 Byte sector */
#define IFC_CSOR_NAND_ECC_MODE_4	0x00000000
/* 8 bit correction per 528 Byte sector */
#define IFC_CSOR_NAND_ECC_MODE_8	0x10000000
/* Enable ECC Decoder */
#define IFC_CSOR_NAND_ECC_DEC_EN	0x04000000
/* Row Address Length */
#define IFC_CSOR_NAND_RAL_MASK	0x01800000
#define IFC_CSOR_NAND_RAL_SHIFT	20
#define IFC_CSOR_NAND_RAL_1		0x00000000
#define IFC_CSOR_NAND_RAL_2		0x00800000
#define IFC_CSOR_NAND_RAL_3		0x01000000
#define IFC_CSOR_NAND_RAL_4		0x01800000
/* Page Size 512b, 2k, 4k */
#define IFC_CSOR_NAND_PGS_MASK	0x00180000
#define IFC_CSOR_NAND_PGS_SHIFT	16
#define IFC_CSOR_NAND_PGS_512	0x00000000
#define IFC_CSOR_NAND_PGS_2K	0x00080000
#define IFC_CSOR_NAND_PGS_4K	0x00100000
#define IFC_CSOR_NAND_PGS_8K	0x00180000
/* Spare region Size */
#define IFC_CSOR_NAND_SPRZ_MASK		0x0000E000
#define IFC_CSOR_NAND_SPRZ_SHIFT		13
#define IFC_CSOR_NAND_SPRZ_16		0x00000000
#define IFC_CSOR_NAND_SPRZ_64		0x00002000
#define IFC_CSOR_NAND_SPRZ_128		0x00004000
#define IFC_CSOR_NAND_SPRZ_210		0x00006000
#define IFC_CSOR_NAND_SPRZ_218		0x00008000
#define IFC_CSOR_NAND_SPRZ_224		0x0000A000
#define IFC_CSOR_NAND_SPRZ_CSOR_EXT 0x0000C000
/* Pages Per Block */
#define IFC_CSOR_NAND_PB_MASK		0x00000700
#define IFC_CSOR_NAND_PB_SHIFT		8
#define IFC_CSOR_NAND_PB(n)			(n-5) << IFC_CSOR_NAND_PB_SHIFT
/* Time for Read Enable High to Output High Impedance */
#define IFC_CSOR_NAND_TRHZ_MASK		0x0000001C
#define IFC_CSOR_NAND_TRHZ_SHIFT		2
#define IFC_CSOR_NAND_TRHZ_20		0x00000000
#define IFC_CSOR_NAND_TRHZ_40		0x00000004
#define IFC_CSOR_NAND_TRHZ_60		0x00000008
#define IFC_CSOR_NAND_TRHZ_80		0x0000000C
#define IFC_CSOR_NAND_TRHZ_100		0x00000010
/* Buffer control disable */
#define IFC_CSOR_NAND_BCTLD			0x00000001

#define FSL_IFC_NAND_BUF_MASK		0xffff0000
#define FSL_IFC_NOR_BUF_MASK		0xffff0000

#define FSL_IFC_NAND_CSPR		((FSL_IFC_NAND_BUF_BASE & FSL_IFC_NAND_BUF_MASK)\
				| IFC_CSPR_PORT_SIZE_8 \
				| IFC_CSPR_MSEL_NAND \
				| IFC_CSPR_V)
#define FSL_IFC_NAND_CSPR_EXT	0x0
#define FSL_IFC_NAND_AMASK		0xFFFF0000
#define FSL_IFC_NAND_CSOR    	(IFC_CSOR_NAND_ECC_ENC_EN /* ECC on encode */ \
                                | IFC_CSOR_NAND_ECC_DEC_EN /* ECC on decode */ \
                                | IFC_CSOR_NAND_ECC_MODE_4 /* 4-bit ECC */ \
                                | IFC_CSOR_NAND_RAL_3       /* RAL = 3 Bytes */ \
                                | IFC_CSOR_NAND_PGS_2K      /* Page Size = 2K */ \
                                | IFC_CSOR_NAND_SPRZ_64     /* Spare size = 64 */ \
                                | IFC_CSOR_NAND_PB(6))     /* 2^6 Pages Per Block */

/*
 * FTIM0 - NOR Flash Mode
 */
#define FSL_IFC_FTIM0_NOR			0xF03F3F3F
#define FSL_IFC_FTIM0_NOR_TACSE_SHIFT	28
#define FSL_IFC_FTIM0_NOR_TACSE(n)	((n) << FSL_IFC_FTIM0_NOR_TACSE_SHIFT)
#define FSL_IFC_FTIM0_NOR_TEADC_SHIFT	16
#define FSL_IFC_FTIM0_NOR_TEADC(n)	((n) << FSL_IFC_FTIM0_NOR_TEADC_SHIFT)
#define FSL_IFC_FTIM0_NOR_TAVDS_SHIFT	8
#define FSL_IFC_FTIM0_NOR_TAVDS(n)	((n) << FSL_IFC_FTIM0_NOR_TAVDS_SHIFT)
#define FSL_IFC_FTIM0_NOR_TEAHC_SHIFT	0
#define FSL_IFC_FTIM0_NOR_TEAHC(n)	((n) << FSL_IFC_FTIM0_NOR_TEAHC_SHIFT)
/*
 * FTIM1 - NOR Flash Mode
 */
#define FSL_IFC_FTIM1_NOR			0xFF003F3F
#define FSL_IFC_FTIM1_NOR_TACO_SHIFT	24
#define FSL_IFC_FTIM1_NOR_TACO(n)	((n) << FSL_IFC_FTIM1_NOR_TACO_SHIFT)
#define FSL_IFC_FTIM1_NOR_TRAD_NOR_SHIFT	8
#define FSL_IFC_FTIM1_NOR_TRAD_NOR(n)	((n) << FSL_IFC_FTIM1_NOR_TRAD_NOR_SHIFT)
#define FSL_IFC_FTIM1_NOR_TSEQRAD_NOR_SHIFT	0
#define FSL_IFC_FTIM1_NOR_TSEQRAD_NOR(n)	((n) << FSL_IFC_FTIM1_NOR_TSEQRAD_NOR_SHIFT)
/*
 * FTIM2 - NOR Flash Mode
 */
#define FSL_IFC_FTIM2_NOR			0x0F3CFCFF
#define FSL_IFC_FTIM2_NOR_TCS_SHIFT		24
#define FSL_IFC_FTIM2_NOR_TCS(n)	((n) << FSL_IFC_FTIM2_NOR_TCS_SHIFT)
#define FSL_IFC_FTIM2_NOR_TCH_SHIFT		18
#define FSL_IFC_FTIM2_NOR_TCH(n)	((n) << FSL_IFC_FTIM2_NOR_TCH_SHIFT)
#define FSL_IFC_FTIM2_NOR_TWPH_SHIFT	10
#define FSL_IFC_FTIM2_NOR_TWPH(n)	((n) << FSL_IFC_FTIM2_NOR_TWPH_SHIFT)
#define FSL_IFC_FTIM2_NOR_TWP_SHIFT		0
#define FSL_IFC_FTIM2_NOR_TWP(n)	((n) << FSL_IFC_FTIM2_NOR_TWP_SHIFT)

/*
 * FTIM0 - Normal GPCM Mode
 */
#define FSL_IFC_FTIM0_GPCM			0xF03F3F3F
#define FSL_IFC_FTIM0_GPCM_TACSE_SHIFT	28
#define FSL_IFC_FTIM0_GPCM_TACSE(n)	((n) << FSL_IFC_FTIM0_GPCM_TACSE_SHIFT)
#define FSL_IFC_FTIM0_GPCM_TEADC_SHIFT	16
#define FSL_IFC_FTIM0_GPCM_TEADC(n)	((n) << FSL_IFC_FTIM0_GPCM_TEADC_SHIFT)
#define FSL_IFC_FTIM0_GPCM_TAVDS_SHIFT	8
#define FSL_IFC_FTIM0_GPCM_TAVDS(n)	((n) << FSL_IFC_FTIM0_GPCM_TAVDS_SHIFT)
#define FSL_IFC_FTIM0_GPCM_TEAHC_SHIFT	0
#define FSL_IFC_FTIM0_GPCM_TEAHC(n)	((n) << FSL_IFC_FTIM0_GPCM_TEAHC_SHIFT)
/*
 * FTIM1 - Normal GPCM Mode
 */
#define FSL_IFC_FTIM1_GPCM			0xFF003F00
#define FSL_IFC_FTIM1_GPCM_TACO_SHIFT	24
#define FSL_IFC_FTIM1_GPCM_TACO(n)	((n) << FSL_IFC_FTIM1_GPCM_TACO_SHIFT)
#define FSL_IFC_FTIM1_GPCM_TRAD_SHIFT	8
#define FSL_IFC_FTIM1_GPCM_TRAD(n)	((n) << FSL_IFC_FTIM1_GPCM_TRAD_SHIFT)
/*
 * FTIM2 - Normal GPCM Mode
 */
#define FSL_IFC_FTIM2_GPCM			0x0F3C00FF
#define FSL_IFC_FTIM2_GPCM_TCS_SHIFT	24
#define FSL_IFC_FTIM2_GPCM_TCS(n)	((n) << FSL_IFC_FTIM2_GPCM_TCS_SHIFT)
#define FSL_IFC_FTIM2_GPCM_TCH_SHIFT	18
#define FSL_IFC_FTIM2_GPCM_TCH(n)	((n) << FSL_IFC_FTIM2_GPCM_TCH_SHIFT)
#define FSL_IFC_FTIM2_GPCM_TWP_SHIFT	0
#define FSL_IFC_FTIM2_GPCM_TWP(n)	((n) << FSL_IFC_FTIM2_GPCM_TWP_SHIFT)


/**
 * fls - find last (most-significant) bit set
 * @x: the word to search
 *
 * This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */
static inline int generic_fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

static inline int __ilog2(unsigned int x)
{
	return generic_fls(x) - 1;
}

/*
 * Address Mask Register
 */
#define FSL_IFC_AMASK_MASK			0xFFFF0000
#define FSL_IFC_AMASK_SHIFT			16
#define FSL_IFC_AMASK(n)			(FSL_IFC_AMASK_MASK << \
					(__ilog2(n) - FSL_IFC_AMASK_SHIFT))
#define FSL_IFC_NOR_AMASK		FSL_IFC_AMASK(128*1024*1024)

#define FSL_IFC_NOR_CSPR    	((FSL_IFC_NOR_BUF_BASE & FSL_IFC_NOR_BUF_MASK)\
				| IFC_CSPR_PORT_SIZE_16 \
                                | IFC_CSPR_MSEL_NOR        \
                                | IFC_CSPR_V)
#define FSL_IFC_NOR_CSPR_EXT	0x0
#define FSL_IFC_NOR_CSOR	        (IFC_CSOR_NOR_ADM_SHIFT(4) | \
				 IFC_CSOR_NOR_TRHZ_80)
#define FSL_IFC_NOR_FTIM0	        (FSL_IFC_FTIM0_NOR_TACSE(0x1) | \
				 FSL_IFC_FTIM0_NOR_TEADC(0x1) | \
				 FSL_IFC_FTIM0_NOR_TAVDS(0x0) | \
				 FSL_IFC_FTIM0_NOR_TEAHC(0xc))
#define FSL_IFC_NOR_FTIM1	        (FSL_IFC_FTIM1_NOR_TACO(0x1c) | \
			 	 FSL_IFC_FTIM1_NOR_TRAD_NOR(0xb) |\
				 FSL_IFC_FTIM1_NOR_TSEQRAD_NOR(0x9))
#define FSL_IFC_NOR_FTIM2	        (FSL_IFC_FTIM2_NOR_TCS(0x1) | \
				 FSL_IFC_FTIM2_NOR_TCH(0x4) | \
				 FSL_IFC_FTIM2_NOR_TWPH(0x8) | \
				 FSL_IFC_FTIM2_NOR_TWP(0x10))
#define FSL_IFC_NOR_FTIM3     	0x0

#define FSL_IFC_NOR_CSPR0		FSL_IFC_NOR_CSPR
#define FSL_IFC_NOR_AMASK0		FSL_IFC_NOR_AMASK
#define FSL_IFC_NOR_CSOR0		FSL_IFC_NOR_CSOR

#define FSL_IFC_SRAM_BUF_SIZE	0x4000

/* CPLD */

/* Convert an address into the right format for the CSPR Registers */
#define IFC_CSPR_PHYS_ADDR(x)	(((UINTN)x) & 0xffff0000)

#define CPLD_BASE_PHYS		CONFIG_CPLD_BASE

#define FSL_IFC_CPLD_CSPR_EXT	(0x0)
#define FSL_IFC_CPLD_CSPR		(IFC_CSPR_PHYS_ADDR(CPLD_BASE_PHYS) | \
				IFC_CSPR_PORT_SIZE_8 | \
				IFC_CSPR_MSEL_GPCM | \
				IFC_CSPR_V)
#define FSL_IFC_CPLD_AMASK		FSL_IFC_AMASK(64 * 1024)
#define FSL_IFC_CPLD_CSOR		IFC_CSOR_NOR_ADM_SHIFT(16) 

/* CPLD Timing parameters for IFC GPCM */
#define FSL_IFC_CPLD_FTIM0		(FSL_IFC_FTIM0_GPCM_TACSE(0x0e) | \
				FSL_IFC_FTIM0_GPCM_TEADC(0x0e) | \
				FSL_IFC_FTIM0_GPCM_TEAHC(0x0e))
#define FSL_IFC_CPLD_FTIM1		(FSL_IFC_FTIM1_GPCM_TACO(0xff) | \
				FSL_IFC_FTIM1_GPCM_TRAD(0x3f))
#define FSL_IFC_CPLD_FTIM2		(FSL_IFC_FTIM2_GPCM_TCS(0xf) | \
				FSL_IFC_FTIM2_GPCM_TCH(0xf) | \
				FSL_IFC_FTIM2_GPCM_TWP(0x3E))
#define FSL_IFC_CPLD_FTIM3		0x0

typedef enum {
	FSL_IFC_CS0 = 0,
	FSL_IFC_CS1,
	FSL_IFC_CS2,
	FSL_IFC_CS3,
	FSL_IFC_CS4,
	FSL_IFC_CS5,
	FSL_IFC_CS6,
	FSL_IFC_CS7,
} FSL_IFC_CHIP_SEL;

typedef enum {
	FSL_IFC_FTIM0 = 0,
	FSL_IFC_FTIM1,
	FSL_IFC_FTIM2,
	FSL_IFC_FTIM3,
} FSL_IFC_FTIMS;

/*
 * Instruction opcodes to be programmed
 * in FIR registers- 6bits
 */

enum ifc_nand_fir_opcodes {
	FSL_IFC_FIR_OP_NOP,
	FSL_IFC_FIR_OP_CA0,
	FSL_IFC_FIR_OP_CA1,
	FSL_IFC_FIR_OP_CA2,
	FSL_IFC_FIR_OP_CA3,
	FSL_IFC_FIR_OP_RA0,
	FSL_IFC_FIR_OP_RA1,
	FSL_IFC_FIR_OP_RA2,
	FSL_IFC_FIR_OP_RA3,
	FSL_IFC_FIR_OP_CMD0,
	FSL_IFC_FIR_OP_CMD1,
	FSL_IFC_FIR_OP_CMD2,
	FSL_IFC_FIR_OP_CMD3,
	FSL_IFC_FIR_OP_CMD4,
	FSL_IFC_FIR_OP_CMD5,
	FSL_IFC_FIR_OP_CMD6,
	FSL_IFC_FIR_OP_CMD7,
	FSL_IFC_FIR_OP_CW0,
	FSL_IFC_FIR_OP_CW1,
	FSL_IFC_FIR_OP_CW2,
	FSL_IFC_FIR_OP_CW3,
	FSL_IFC_FIR_OP_CW4,
	FSL_IFC_FIR_OP_CW5,
	FSL_IFC_FIR_OP_CW6,
	FSL_IFC_FIR_OP_CW7,
	FSL_IFC_FIR_OP_WBCD,
	FSL_IFC_FIR_OP_RBCD,
	FSL_IFC_FIR_OP_BTRD,
	FSL_IFC_FIR_OP_RDSTAT,
	FSL_IFC_FIR_OP_NWAIT,
	FSL_IFC_FIR_OP_WFR,
	FSL_IFC_FIR_OP_SBRD,
	FSL_IFC_FIR_OP_UA,
	FSL_IFC_FIR_OP_RB,
};

typedef struct {
	UINT32 cspr_ext;
	UINT32 cspr;
	UINT32 res;
} FSL_IFC_CSPR;

typedef struct {
	UINT32 amask;
	UINT32 res[0x2];
} FSL_IFC_AMASK;

typedef struct {
	UINT32 csor;
	UINT32 csor_ext;
	UINT32 res;
} FSL_IFC_CSOR;

typedef struct {
	UINT32 ftim[4];
	UINT32 res[0x8];
}FSL_IFC_FTIM ;

typedef struct {
	UINT32 ncfgr;
	UINT32 res1[0x4];
	UINT32 nand_fcr0;
	UINT32 nand_fcr1;
	UINT32 res2[0x8];
	UINT32 row0;
	UINT32 res3;
	UINT32 col0;
	UINT32 res4;
	UINT32 row1;
	UINT32 res5;
	UINT32 col1;
	UINT32 res6;
	UINT32 row2;
	UINT32 res7;
	UINT32 col2;
	UINT32 res8;
	UINT32 row3;
	UINT32 res9;
	UINT32 col3;
	UINT32 res10[0x24];
	UINT32 nand_fbcr;
	UINT32 res11;
	UINT32 nand_fir0;
	UINT32 nand_fir1;
	UINT32 nand_fir2;
	UINT32 res12[0x10];
	UINT32 nand_csel;
	UINT32 res13;
	UINT32 nandseq_strt;
	UINT32 res14;
	UINT32 nand_evter_stat;
	UINT32 res15;
	UINT32 pgrdcmpl_evt_stat;
	UINT32 res16[0x2];
	UINT32 nand_evter_en;
	UINT32 res17[0x2];
	UINT32 nand_evter_intr_en;
	UINT32 res18[0x2];
	UINT32 nand_erattr0;
	UINT32 nand_erattr1;
	UINT32 res19[0x10];
	UINT32 nand_fsr;
	UINT32 res20;
	UINT32 nand_eccstat[4];
	UINT32 res21[0x20];
	UINT32 nanndcr;
	UINT32 res22[0x2];
	UINT32 nand_autoboot_trgr;
	UINT32 res23;
	UINT32 nand_mdr;
	UINT32 res24[0x5C];
} FSL_IFC_NAND;

/*
 * IFC controller NOR Machine registers
 */
typedef struct {
	UINT32 nor_evter_stat;
	UINT32 res1[0x2];
	UINT32 nor_evter_en;
	UINT32 res2[0x2];
	UINT32 nor_evter_intr_en;
	UINT32 res3[0x2];
	UINT32 nor_erattr0;
	UINT32 nor_erattr1;
	UINT32 nor_erattr2;
	UINT32 res4[0x4];
	UINT32 norcr;
	UINT32 res5[0xEF];
} FSL_IFC_NOR;

/*
 * IFC controller GPCM Machine registers
 */
typedef struct  {
	UINT32 gpcm_evter_stat;
	UINT32 res1[0x2];
	UINT32 gpcm_evter_en;
	UINT32 res2[0x2];
	UINT32 gpcm_evter_intr_en;
	UINT32 res3[0x2];
	UINT32 gpcm_erattr0;
	UINT32 gpcm_erattr1;
	UINT32 gpcm_erattr2;
	UINT32 gpcm_stat;
} FSL_IFC_GPCM;

/*
 * IFC Controller Registers
 */
typedef struct {
	UINT32 ifc_rev;
	UINT32 res1[0x2];
	FSL_IFC_CSPR cspr_cs[FSL_IFC_BANK_COUNT];
	UINT8 res2[FSL_IFC_CSPR_REG_LEN - FSL_IFC_CSPR_USED_LEN];
	FSL_IFC_AMASK amask_cs[FSL_IFC_BANK_COUNT];
	UINT8 res3[FSL_IFC_AMASK_REG_LEN - FSL_IFC_AMASK_USED_LEN];
	FSL_IFC_CSOR csor_cs[FSL_IFC_BANK_COUNT];
	UINT8 res4[FSL_IFC_CSOR_REG_LEN - FSL_IFC_CSOR_USED_LEN];
	FSL_IFC_FTIM ftim_cs[FSL_IFC_BANK_COUNT];
	UINT8 res5[FSL_IFC_FTIM_REG_LEN - FSL_IFC_FTIM_USED_LEN];
	UINT32 rb_stat;
	UINT32 rb_map;
	UINT32 wp_map;
	UINT32 ifc_gcr;
	UINT32 res7[0x2];
	UINT32 cm_evter_stat;
	UINT32 res8[0x2];
	UINT32 cm_evter_en;
	UINT32 res9[0x2];
	UINT32 cm_evter_intr_en;
	UINT32 res10[0x2];
	UINT32 cm_erattr0;
	UINT32 cm_erattr1;
	UINT32 res11[0x2];
	UINT32 ifc_ccr;
	UINT32 ifc_csr;
	UINT32 ddr_ccr_low;
	UINT32 res12[0x2EA];
	FSL_IFC_NAND ifc_nand;
	FSL_IFC_NOR ifc_nor;
	FSL_IFC_GPCM ifc_gpcm;
} FSL_IFC_REGS;

VOID IfcNorInit(VOID);

VOID IfcNandInit(VOID);

#define FSL_IFC_REGS_BASE \
	((FSL_IFC_REGS *)FSL_IFC_REG_BASE)

#endif //__FLASH_H__
