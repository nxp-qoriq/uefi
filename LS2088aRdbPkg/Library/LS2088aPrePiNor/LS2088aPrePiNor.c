/** @LS2088aPrePiNor.c
#
#  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
**/

#include <Library/ArmLib.h>
#include <Library/PcdLib.h>
#include <Library/FslDdr.h>
#include <Library/SerialPortLib.h>
#include <LS2088aRdb.h>


UINTN mGlobalVariableBase = 0;

VOID CopyImage(UINT8* Dest, UINT8* Src, UINTN Size)
{
	UINTN Count;
		
	for(Count = 0; Count < Size; Count++) {
		Dest[Count] = Src[Count];
	}

	for(Count = 0; Count < Size; Count++) {
		if (Dest[Count] != Src[Count])
		 {
			DEBUG((EFI_D_ERROR, "Comparision failed at Count %d \n", Count ));
			break;
		 }
	}
}

/*
 * This erratum improves clock stability by setting glitch_en bit
 * which enable digital glitch filter.
 */
static
VOID I2cErratumA009203 (
		VOID
		)
{
	MmioWrite8((UINTN)(I2C1_BASE_ADDRESS + I2C_DEBUG_REG), I2C_GLITCH_EN);
	MmioWrite8((UINTN)(I2C2_BASE_ADDRESS + I2C_DEBUG_REG), I2C_GLITCH_EN);
	MmioWrite8((UINTN)(I2C3_BASE_ADDRESS + I2C_DEBUG_REG), I2C_GLITCH_EN);
	MmioWrite8((UINTN)(I2C4_BASE_ADDRESS + I2C_DEBUG_REG), I2C_GLITCH_EN);

}

static void ErratumA009008(void)
{
#ifdef ERRATUM_A009008
	UINT32 *Scfg = (VOID *)(SCFG_BASE_ADDR);
       UINT32 Val = MmioRead32((UINTN)(Scfg + SCFG_USB3PRM1CR / 4));
       Val &= ~(0xF << 6);
       MmioWrite32((UINTN)(Scfg + SCFG_USB3PRM1CR / 4), Val|(USB_TXVREFTUNE << 6));
#endif
}

static void ErratumA009798(void)
{
#ifdef ERRATUM_A009798
	UINT32 *Scfg = (VOID *)(SCFG_BASE_ADDR);
	UINT32 Val = MmioRead32((UINTN)(Scfg + SCFG_USB3PRM1CR / 4));
       MmioWrite32((UINTN)(Scfg + SCFG_USB3PRM1CR / 4), Val & USB_SQRXTUNE);
#endif
}

static void ErratumA008751(void)
{
#ifdef ERRATUM_A008751
	struct CcsrGur *GurBase = (VOID *)(CONFIG_SYS_FSL_GUTS_ADDR);
       UINTN Svr, Ver;

	Svr = MmioRead32((UINTN)&GurBase->Svr);
       Ver = SVR_SOC_VER(Svr);
       switch (Ver) {
       case SVR_LS2080A:
       case SVR_LS2085A:
       case SVR_LS2040A:
       case SVR_LS2045A:
              MmioWrite32((UINTN)(SCFG_BASE_ADDR + SCFG_USB3PRM1CR / 4), 0x27672b2a);
              break;
       default:
              break;
       }
#endif
}


/*
 * This erratum requires a register write before being Memory
 * controller 3 being enabled.
 */
static void ErratumA008514(void)
{
	UINT32 *eddrtqcr1;

	eddrtqcr1 = (void *)CONFIG_SYS_FSL_DCSR_DDR3_ADDR + 0x800;
	MmioWrite32((UINTN)eddrtqcr1, 0x63b20002);
}

/*
 * This erratum requires setting a value to eddrtqcr1 to
 * optimal the DDR performance.
 */
static void ErratumA008336(void)
{
	UINT32 *eddrtqcr1;

	eddrtqcr1 = (void *)CONFIG_SYS_FSL_DCSR_DDR_ADDR + 0x800;
	MmioWrite32((UINTN)eddrtqcr1, 0x63b30002);
	eddrtqcr1 = (void *)CONFIG_SYS_FSL_DCSR_DDR2_ADDR + 0x800;
	MmioWrite32((UINTN)eddrtqcr1, 0x63b30002);
}

/*
* A-009007: USB3PHY observing intermittent failure in receive compliance tests at
* higher jitter frequency using default register values
* Affects: USB
* Description: Receive compliance tests may fail intermittently at high jitter frequencies using default register
* values.
* Impact: Receive compliance test fails at default register setting.
*/
STATIC VOID ErratumA009007 (VOID)
{
  VOID *UsbPhyRxOvrdInHi = (VOID *)(DCSR_BASE + DCSR_USB_PHY1 + DCSR_USB_PHY_RX_OVRD_IN_HI);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)UsbPhyRxOvrdInHi, USB_PHY_RX_EQ_VAL_1);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)UsbPhyRxOvrdInHi, USB_PHY_RX_EQ_VAL_2);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)UsbPhyRxOvrdInHi, USB_PHY_RX_EQ_VAL_3);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)UsbPhyRxOvrdInHi, USB_PHY_RX_EQ_VAL_4);
  UsbPhyRxOvrdInHi = (VOID *)(DCSR_BASE + DCSR_USB_PHY2 + DCSR_USB_PHY_RX_OVRD_IN_HI);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)UsbPhyRxOvrdInHi, USB_PHY_RX_EQ_VAL_1);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)UsbPhyRxOvrdInHi, USB_PHY_RX_EQ_VAL_2);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)UsbPhyRxOvrdInHi, USB_PHY_RX_EQ_VAL_3);
  ArmDataMemoryBarrier();
  MmioWrite16((UINTN)UsbPhyRxOvrdInHi, USB_PHY_RX_EQ_VAL_4);
}

VOID CEntryPoint(
		UINTN	UefiMemoryBase,
		UINTN	UefiNorBase,
		UINTN	UefiMemorySize
		)
{ 
	VOID	(*PrePiStart)(VOID);

	//SerialPortInitialize();
	// Data Cache enabled on Primary core when MMU is enabled.
	ArmDisableDataCache ();
	// Invalidate Data cache
	//ArmInvalidateDataCache ();
	// Invalidate instruction cache
	ArmInvalidateInstructionCache ();
	// Enable Instruction Caches on all cores.
	ArmEnableInstructionCache ();
	
	if (PcdGetBool(PcdI2cErratumA009203))
       	I2cErratumA009203();

	ErratumA008751();
       ErratumA009008();
       ErratumA009798();

	ErratumA008514();
	ErratumA008336();
	if (PcdGetBool(PcdUsbErratumA009007))
	  ErratumA009007();

	TimerInit();
	DramInit();

	DEBUG((EFI_D_INFO, "UefiMemoryBase:0x%llx UefiNorBase:0x%llx, UefiMemorySize 0x%x\n", UefiMemoryBase, UefiNorBase, UefiMemorySize));
	CopyImage((VOID*)UefiMemoryBase, (VOID*)UefiNorBase, UefiMemorySize);
	PrePiStart = (VOID (*)())((UINT64)PcdGet64(PcdFvBaseAddress));
	PrePiStart();
}
