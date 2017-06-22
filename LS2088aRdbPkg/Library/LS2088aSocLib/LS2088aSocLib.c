/** @SoCLib.c
  SoC specific Library for LS2088A SoC, containing functions to initialize various SoC components

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>

#include <Library/PrePiLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Library/FslIfc.h>
#include <Library/FslDdr.h>
#include <ArmTrustzone.h>
#include <Drivers/Mmc.h>

#include <LS2088aRdb.h>
#include <Ls2088aSerDes.h>
#include <StreamId.h>
#include <LS2088aSocLib.h>
#include <QixisLib.h>

#include <libfdt.h>

#include <Protocol/PciRootBridgeIo.h>
#include <PciRootBridge.h>
#include <PciHostBridge.h>

#define FDT_EXTRA_SPACE 4096

/* Global Clock Information pointer */
static SocClockInfo gClkInfo;

UINTN get_board_sys_clk(void)
{
	UINT8 sysclk_conf = QIXIS_READ(brdcfg[1]);

	switch (sysclk_conf & 0x0F) {
	case QIXIS_SYSCLK_83:
		return 83333333;
	case QIXIS_SYSCLK_100:
		return 100000000;
	case QIXIS_SYSCLK_125:
		return 125000000;
	case QIXIS_SYSCLK_133:
		return 133333333;
	case QIXIS_SYSCLK_150:
		return 150000000;
	case QIXIS_SYSCLK_160:
		return 160000000;
	case QIXIS_SYSCLK_166:
		return 166666666;
	}
	return 66666666;
}

char *StringToMHz (
  char *Buf,
  unsigned long Hz
  )
{
	long l, m, n;

	n = DIV_ROUND_CLOSEST(Hz, 1000) / 1000L;
	l = AsciiSPrint (Buf, sizeof(Buf), "%ld", n);

	Hz -= n * 1000000L;
	m = DIV_ROUND_CLOSEST(Hz, 1000L);
	if (m != 0)
		AsciiSPrint (Buf + l, sizeof(Buf), ".%03ld", m);
	return (Buf);
}

INT32
SelectMuxDevice (
  IN  UINT8  Type
  )
{
  if (PcdGet32(PcdBootMode) == QSPI_BOOT)
    return EFI_SUCCESS;
  else {
    UINT8 Reg5;

    Reg5 = QIXIS_READ(brdcfg[5]);

    switch (Type) {
    case MUX_SDXC:
           Reg5 = SET_MUX_SEL(Reg5, PIN_SEL_SDXC_MUX);
           break;
    case MUX_DSPI:
           Reg5 = SET_MUX_SEL(Reg5, PIN_SEL_DSPI_MUX);
           break;
    default:
           DEBUG((EFI_D_ERROR,"Wrong mux interface type\n"));
           return -1;
    }
 
    QIXIS_WRITE(brdcfg[5], Reg5);
 
    return EFI_SUCCESS;
  }
}

/**
  Initialize/Set the Secure peripheral : TZPC_BP147

  If Trustzone is supported by platform then this function
  makes the required setting of the secure peripheral

  Set Non Secure access for all devices protected via TZPC

**/
VOID
TzpcBp147Init (
  VOID
  )
{
  // Decode Protection-0 Set Reg
  // Enable non-secure access to shared
  // on-chip RAM by programming TZPC
  // Setting DCFG bit 3 for it
  TZPCSetDecProtBits(TZPC_BASE_ADDR, 0, TZPC_DCFG_RESET_BIT_MASK);

}

VOID
GetSysInfo (
  OUT struct SysInfo *PtrSysInfo
  )
{
	struct CcsrGur *GurBase = (void *)(CONFIG_SYS_FSL_GUTS_ADDR);
	struct CcsrClkCtrl *ClkBase = (void *)(FSL_CLK_CTRL_ADDR);
        struct CcsrClkClusterGroup  *ClkGrp[2] = {
                   (void *)(FSL_CLK_GRPA_ADDR),
                   (void *)(FSL_CLK_GRPB_ADDR)
               };

        UINT32 Index;
        const UINT8 CoreCplxPll[16] = {
                [0] = 0,        /* CC1 PPL / 1 */
                [1] = 0,        /* CC1 PPL / 2 */
                [2] = 0,        /* CC1 PPL / 4 */
                [4] = 1,        /* CC2 PPL / 1 */
                [5] = 1,        /* CC2 PPL / 2 */
                [6] = 1,        /* CC2 PPL / 4 */
                [8] = 2,        /* CC3 PPL / 1 */
                [9] = 2,        /* CC3 PPL / 2 */
                [10] = 2,       /* CC3 PPL / 4 */
                [12] = 3,       /* CC4 PPL / 1 */
                [13] = 3,       /* CC4 PPL / 2 */
                [14] = 3,       /* CC4 PPL / 4 */
	};

	const UINT8 CoreCplxPllDivisor[16] = {
                [0] = 1,        /* CC1 PPL / 1 */
                [1] = 2,        /* CC1 PPL / 2 */
                [2] = 4,        /* CC1 PPL / 4 */
                [4] = 1,        /* CC2 PPL / 1 */
                [5] = 2,        /* CC2 PPL / 2 */
                [6] = 4,        /* CC2 PPL / 4 */
                [8] = 1,        /* CC3 PPL / 1 */
                [9] = 2,        /* CC3 PPL / 2 */
                [10] = 4,       /* CC3 PPL / 4 */
                [12] = 1,       /* CC4 PPL / 1 */
                [13] = 2,       /* CC4 PPL / 2 */
                [14] = 4,       /* CC4 PPL / 4 */
	};

	UINTN PllCount, Cluster;
	UINTN FreqCPll[NUM_CC_PLLS];
	UINTN PllRatio[NUM_CC_PLLS];
	UINTN SysClk;

	INT32 CcGroup[12] = FSL_CLUSTER_CLOCKS;
        UINT32 Cpu, CPllSel, CplxPll;
        VOID *offset;

  	if (PcdGet32(PcdBootMode) != QSPI_BOOT)
		SysClk = get_board_sys_clk();
	else
		SysClk = PcdGet64(PcdSysClk);

	PtrSysInfo->FreqSystemBus = SysClk;
	PtrSysInfo->FreqDdrBus = PcdGet64(PcdDdrClk);
	PtrSysInfo->FreqDdrBus2 = PcdGet64(PcdDdrClk);

	PtrSysInfo->FreqSystemBus *= (MmioRead32((UINTN)&GurBase->Rcw_Sr[0]) >>
				      FSL_CHASSIS3_RCWSR_0_SYS_PLL_RAT_SHIFT) &
                                      FSL_CHASSIS3_RCWSR_0_SYS_PLL_RAT_MASK;

	/* Platform clock is half of platform PLL */
        PtrSysInfo->FreqSystemBus /= 2;

	PtrSysInfo->FreqDdrBus *= (MmioRead32((UINTN)&GurBase->Rcw_Sr[0]) >>
					FSL_CHASSIS3_RCWSR_0_MEM_PLL_RAT_SHIFT) &
					FSL_CHASSIS3_RCWSR_0_MEM_PLL_RAT_MASK;
	PtrSysInfo->FreqDdrBus2 *= (MmioRead32((UINTN)&GurBase->Rcw_Sr[0]) >>
					FSL_CHASSIS3_RCWSR_0_MEM2_PLL_RAT_SHIFT) &
					FSL_CHASSIS3_RCWSR_0_MEM2_PLL_RAT_MASK;

	for (PllCount = 0; PllCount < NUM_CC_PLLS; PllCount++) {
                offset = (void *)((size_t)ClkGrp[PllCount/3] +
                         offsetof(struct CcsrClkClusterGroup, pllngsr[PllCount%3].gsr));
                PllRatio[PllCount] = (MmioRead32((UINTN)offset) >> 1) & 0x3f;
                FreqCPll[PllCount] = SysClk * PllRatio[PllCount];
	}

	ForEachCpu(Index, Cpu, CpuNumCores(), CpuMask()) {
		Cluster = QoriqCoreToCluster(Cpu);
		ASSERT_EFI_ERROR (Cluster);
                CPllSel = (MmioRead32((UINTN)&ClkBase->clkcncsr[Cluster].csr) >> 27) & 0xf;
                CplxPll = CoreCplxPll[CPllSel];
                CplxPll += CcGroup[Cluster] - 1;
                PtrSysInfo->FreqProcessor[Cpu] = FreqCPll[CplxPll] / CoreCplxPllDivisor[CPllSel];
        }
	PtrSysInfo->FreqSdhc = PtrSysInfo->FreqSystemBus/2;
}

VOID
ClockInit (
  VOID
  )
{
	struct SysInfo SocSysInfo;

	GetSysInfo(&SocSysInfo);
	gClkInfo.CpuClk = SocSysInfo.FreqProcessor[0];
	gClkInfo.BusClk = SocSysInfo.FreqSystemBus;
	gClkInfo.MemClk = SocSysInfo.FreqDdrBus;
	gClkInfo.SdhcClk = gClkInfo.BusClk/2;
}

INTN
TimerInit (
  VOID
  )
{
	UINT32 *TimerBase = (UINT32 *)TIMER_BASE_ADDR;

	if (PcdGetBool(PcdCounterFrequencyReal)) {
		UINTN cntfrq = PcdGet32(PcdCounterFrequency);

		/* Update with accurate clock frequency */
		asm volatile("msr cntfrq_el0, %0" : : "r" (cntfrq) : "memory");
	}

	/* Enable timebase for the cluster */
	MmioWrite32((UINTN)TIME_BASE_ENABLE, 0xF);
	/* Set the bit corresponding to our watchDog-id in the
	 * PMU-Physical Core Time Base Enable Register (PCTBENR)
	 * to allow the WDT counter to decrement and raise a reset
	 * request (if configured in the WDTCONTROL register).
	 */
	MmioWrite32((WDOG1_PMU_BASE_ADDR + FSL_PMU_PCTBENR_OFFSET), PMU_PCTBEN0_WDT_BIT_MASK);

	/* Enable clock for timer. This is a global setting. */
	MmioWrite32((UINTN)TimerBase, 0x1);

	return 0;
}

static inline
UINT32
InitiatorType (
  IN UINT32 Cluster,
  IN UINTN InitId
  )
{
	struct CcsrGur *GurBase = (void *)(CONFIG_SYS_FSL_GUTS_ADDR);
	UINT32 Idx = (Cluster >> (InitId * 8)) & TP_CLUSTER_INITR_MASK;
	UINT32 Type = MmioRead32((UINTN)&GurBase->TpItyp[Idx]);

	if (Type & TP_ITYP_AV_MASK)
		return Type;

	return 0;
}

UINT32
CpuMask (
  VOID
  )
{
	struct CcsrGur *GurBase = (void *)(CONFIG_SYS_FSL_GUTS_ADDR);
	UINTN ClusterIndex = 0, Count = 0;
	UINT32 Cluster, Type, Mask = 0;

	do {
		UINTN InitiatorIndex;
		Cluster = MmioRead32((UINTN)&GurBase->TpCluster[ClusterIndex].Lower);
		for (InitiatorIndex = 0; InitiatorIndex < TP_INIT_PER_CLUSTER; InitiatorIndex++) {
			Type = InitiatorType(Cluster, InitiatorIndex);
			if (Type) {
				if (TP_ITYP_TYPE_MASK(Type) == TP_ITYP_TYPE_ARM)
					Mask |= 1 << Count;
				Count++;
			}
		}
		ClusterIndex++;
	} while ((Cluster & TP_CLUSTER_EOC_MASK) != TP_CLUSTER_EOC_MASK);

	return Mask;
}

/*
 * Return the number of cores on this SOC.
 */
UINTN
CpuNumCores (
  VOID
  )
{
	return HammingWeight32(CpuMask());
}

UINT32
QoriqCoreToType (
  IN UINTN Core
  )
{
	struct CcsrGur *GurBase = (VOID *)(CONFIG_SYS_FSL_GUTS_ADDR);
	UINTN ClusterIndex = 0, Count = 0;
	UINT32 Cluster, Type;

	do {
		UINTN InitiatorIndex;
		Cluster = MmioRead32((UINTN)&GurBase->TpCluster[ClusterIndex].Lower);
		for (InitiatorIndex = 0; InitiatorIndex < TP_INIT_PER_CLUSTER; InitiatorIndex++) {
			Type = InitiatorType(Cluster, InitiatorIndex);
			if (Type) {
				if (Count == Core)
					return Type;
				Count++;
			}
		}
		ClusterIndex++;
	} while ((Cluster & TP_CLUSTER_EOC_MASK) != TP_CLUSTER_EOC_MASK);

	return -1;      /* cannot identify the cluster */
}

INT32
QoriqCoreToCluster (
    UINT32 Core
    )
{
        struct CcsrGur *Gur = (VOID *) CONFIG_SYS_FSL_GUTS_ADDR;
        INT32 i = 0, j, Count = 0;
        UINT32 Cluster;

        /* LS Chassis Gen3 */
        do {
                Cluster = MmioRead32((UINTN)&Gur->TpCluster[i].Lower);
                for (j = 0; j < TP_INIT_PER_CLUSTER; j++) {
                        if (InitiatorType(Cluster, j)) {
                                if (Count == Core)
                                        return i;
                                Count++;
                        }
                }
                i++;
        } while ((Cluster & TP_CLUSTER_EOC_MASK) != TP_CLUSTER_EOC_MASK);

        return -1;      /* cannot identify the Cluster */
}

VOID
PrintCpuInfo (
  VOID
  )
{
	struct SysInfo SysInfo;
	UINTN CoreIndex, Core;
	UINT32 Type;
	CHAR8 Buf[32];
	CHAR8 Buffer[100];
	UINTN CharCount;

	GetSysInfo(&SysInfo);
	CharCount = AsciiSPrint (Buffer, sizeof (Buffer), "Clock Configuration:");
	SerialPortWrite ((UINT8 *) Buffer, CharCount);

	ForEachCpu(CoreIndex, Core, CpuNumCores(), CpuMask()) {
		 if (!(CoreIndex % 3)) {
                       CharCount = AsciiSPrint (Buffer, sizeof (Buffer), "\n      ");
                       SerialPortWrite ((UINT8 *) Buffer, CharCount);
                 }

		Type = TP_ITYP_VERSION(QoriqCoreToType(Core));
		CharCount = AsciiSPrint (Buffer, sizeof (Buffer), "CPU%d(%a):%-4a MHz  ", Core,
			Type == TY_ITYP_VERSION_A7 ? "A7 " :
                     (Type == TY_ITYP_VERSION_A53 ? "A53" :
                     (Type == TY_ITYP_VERSION_A57 ? "A57" :
                     (Type == TY_ITYP_VERSION_A72 ? "A72" : " Unknown Core "))),
		       StringToMHz(Buf, SysInfo.FreqProcessor[Core]));
		SerialPortWrite ((UINT8 *) Buffer, CharCount);
	}

	CharCount = AsciiSPrint (Buffer, sizeof (Buffer), "\n      Bus:      %-4a  MHz  ",
	 		StringToMHz(Buf, SysInfo.FreqSystemBus));
	SerialPortWrite ((UINT8 *) Buffer, CharCount);
	CharCount = AsciiSPrint (Buffer, sizeof (Buffer), "DDR:      %-4a MHz", StringToMHz(Buf, SysInfo.FreqDdrBus));
	SerialPortWrite ((UINT8 *) Buffer, CharCount);
	CharCount = AsciiSPrint (Buffer, sizeof (Buffer), "\n");
	SerialPortWrite ((UINT8 *) Buffer, CharCount);
}

VOID
PrintSocPersonality (
  VOID
  )
{
}

VOID
IfcInit (
  VOID
  )
{
  	if (PcdGet32(PcdBootMode) != QSPI_BOOT) {
		/* NOR Init */
		IfcNorInit();

		/* NAND Init */
		IfcNandInit();
	}

	/* Qixis Init */
	QixisInit();
}

static struct CpuType CpuTypeList[] = {
	CPU_TYPE_ENTRY(LS2080, LS2080, 8),
	CPU_TYPE_ENTRY(LS2085, LS2085, 8),
	CPU_TYPE_ENTRY(LS2088, LS2088, 8),
	CPU_TYPE_ENTRY(LS2045, LS2045, 4),
	CPU_TYPE_ENTRY(LS2040, LS2040, 4),
};

VOID
PrintBoardPersonality (
  VOID
  )
{
	UINT8 SwitchConf;
	char Buf[16];
	struct CcsrGur *GurBase = (VOID *)(CONFIG_SYS_FSL_GUTS_ADDR);
	UINTN Count, Svr, Ver;

	Buf[0] = L'\0';
	Svr = MmioRead32((UINTN)&GurBase->Svr);
	Ver = SVR_SOC_VER(Svr);

	for (Count = 0; Count < ARRAY_SIZE(CpuTypeList); Count++)
		if ((CpuTypeList[Count].soc_ver & SVR_WO_E) == Ver) {
			AsciiStrCpy(Buf, (CONST CHAR8 *)CpuTypeList[Count].name);

			if (IS_E_PROCESSOR(Svr))
				AsciiStrCat(Buf, (CONST CHAR8 *)"E");
			break;
		}

	if (Count == ARRAY_SIZE(CpuTypeList)) {
		AsciiStrCpy(Buf, (CONST CHAR8 *)"unknown");
	}

	DEBUG((EFI_D_RELEASE, "SoC: %a (0x%x)\n", Buf, Svr));
	DEBUG((EFI_D_RELEASE, "Board: LS2088A-RDB\n"));

	SwitchConf = QIXIS_READ(arch);
	DEBUG((EFI_D_RELEASE, "Board Arch: V%d, ", SwitchConf >> 4));
	DEBUG((EFI_D_RELEASE, "Board version: %c, boot from ",
		(SwitchConf & 0xf) + 'A'));

	SwitchConf = QIXIS_READ(brdcfg[0]);
	SwitchConf = (SwitchConf & QIXIS_LBMAP_MASK) >> QIXIS_LBMAP_SHIFT;

	if (SwitchConf < 0x8)
		DEBUG((EFI_D_RELEASE,  "vBank: %d\n", SwitchConf));
	else if (SwitchConf == 0x9)
		DEBUG((EFI_D_RELEASE, "NAND\n"));
	else
		DEBUG((EFI_D_RELEASE, "invalid setting of SW%u\n",
			QIXIS_LBMAP_SWITCH));

	DEBUG((EFI_D_RELEASE, "FPGA: v%d.%d\n", QIXIS_READ(scver),
		QIXIS_READ(tagdata)));

	DEBUG((EFI_D_RELEASE, "SERDES1 Reference : "));
	DEBUG((EFI_D_RELEASE, "Clock1 = 156.25MHz "));
	DEBUG((EFI_D_RELEASE, "Clock2 = 156.25MHz"));

	DEBUG((EFI_D_RELEASE, "\nSERDES2 Reference : "));
	DEBUG((EFI_D_RELEASE, "Clock1 = 100MHz "));
	DEBUG((EFI_D_RELEASE, "Clock2 = 100MHz\n"));
}

VOID
PrintRCW (
  VOID
  )
{
	struct CcsrGur *Base = (void *)(CONFIG_SYS_FSL_GUTS_ADDR);
	UINTN Count;
	CHAR8 Buffer[100];
	UINTN CharCount;

	/*
	 * Display the RCW, so that no one gets confused as to what RCW
	 * we're actually using for this boot.
	 */

	CharCount = AsciiSPrint (Buffer, sizeof (Buffer),
				 "Reset Configuration Word (RCW):");
	SerialPortWrite ((UINT8 *) Buffer, CharCount);
	for (Count = 0; Count < ARRAY_SIZE(Base->Rcw_Sr); Count++) {
		UINT32 Rcw = MmioRead32((UINTN)&Base->Rcw_Sr[Count]);

		if ((Count % 4) == 0) {
			CharCount = AsciiSPrint (Buffer, sizeof (Buffer),
						 "\n       %08x:", Count * 4);
		 	SerialPortWrite ((UINT8 *) Buffer, CharCount);
		}

		CharCount = AsciiSPrint (Buffer, sizeof (Buffer), " %08x", Rcw);
		SerialPortWrite ((UINT8 *) Buffer, CharCount);
	}
	CharCount = AsciiSPrint (Buffer, sizeof (Buffer), "\n");
	SerialPortWrite ((UINT8 *) Buffer, CharCount);
}

VOID
SmmuInit (
  VOID
  )
{
	UINT32 Value;

	/* set pagesize as 64K and ssmu-500 in bypass mode */
	Value = (MmioRead32((UINTN)SMMU_REG_SACR) | SACR_PAGESIZE_MASK);
	MmioWrite32((UINTN)SMMU_REG_SACR, Value);

	Value = (MmioRead32((UINTN)SMMU_REG_SCR0) | SCR0_CLIENTPD_MASK) & ~(SCR0_USFCFG_MASK);
	MmioWrite32((UINTN)SMMU_REG_SCR0, Value);

	Value = (MmioRead32((UINTN)SMMU_REG_NSCR0) | SCR0_CLIENTPD_MASK) & ~(SCR0_USFCFG_MASK);
	MmioWrite32((UINTN)SMMU_REG_NSCR0, Value);
}

VOID
FixAquantiaPhyIntPolarity (
   VOID
   )
{
	UINT32 *IRQ_CCSR = (UINT32 *)INT_SAMPLING_CTRL_BASE;

	/* invert AQR405 IRQ pins polarity */
	MmioWrite32((UINTN)(IRQ_CCSR + IRQCR_OFFSET / 4), AQUANTIA405_IRQ_MASK);
}

/**
  Function to initialize SoC specific constructs
  // ClockInit
  // TimerInit
  // CPU Info
  // SoC Personality
  // Board Personality
  // RCW prints
 **/
VOID
SocInit (
  VOID
  )
{

  CHAR8 Buffer[100];
  UINTN CharCount;


  // LS2088A SoC has a CSU (Central Security Unit)

  if (PcdGetBool(PcdTzpcBp147Initialize))
          TzpcBp147Init();

  if (PcdGetBool(PcdClockInitialize))
	  ClockInit();

  SmmuInit();
  TimerInit();

  /* IFC Init is needed earlier, as we need to setup QIXIS to provide us a
   * correct Sysclk value.
   */
  IfcInit();
  
  // Initialize the Serial Port
  SerialPortInitialize ();
  CharCount = AsciiSPrint (Buffer, sizeof (Buffer), "\nUEFI firmware (version %s built at %a on %a)\n\r",
    (CHAR16*)PcdGetPtr(PcdFirmwareVersionString), __TIME__, __DATE__);
  SerialPortWrite ((UINT8 *) Buffer, CharCount);

  QIXIS_WRITE(rst_ctl, QIXIS_RST_CTL_RESET_EN);

  PrintCpuInfo();
  PrintRCW();
  PrintDdrInfo(0);
  PrintSocPersonality();

  PrintBoardPersonality();
  FixAquantiaPhyIntPolarity();

  return;
}



/* FDT fixup for LS2088A */
/* --------------------- */

#define ALIGN(x, a)     (((x) + ((a) - 1)) & ~((a) - 1))
#define PALIGN(p, a)    ((void *)(ALIGN((unsigned long)(p), (a))))
#define GET_CELL(p)     (p += 4, *((const UINT32 *)(p-4)))

#define FDT_CHECK_HEADER(fdt) \
	{ \
		UINTN __err; \
		if ((__err = fdt_check_header(fdt)) != 0) \
			return __err; \
	}

#define FDT_SW_MAGIC		(~FDT_MAGIC)

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

#define ___swab32(x) \
	((UINT32)( \
		(((UINT32)(x) & (UINT32)0x000000ffUL) << 24) | \
		(((UINT32)(x) & (UINT32)0x0000ff00UL) <<  8) | \
		(((UINT32)(x) & (UINT32)0x00ff0000UL) >>  8) | \
		(((UINT32)(x) & (UINT32)0xff000000UL) >> 24) ))

/* Max address size we deal with */
#define OF_MAX_ADDR_CELLS	4
#define OF_BAD_ADDR	((UINTN)-1)
#define OF_CHECK_COUNTS(na, ns)	((na) > 0 && (na) <= OF_MAX_ADDR_CELLS && \
			(ns) > 0)

/* Errors in device tree content */
#define FDT_ERR_BADNCELLS	14
	/* FDT_ERR_BADNCELLS: Device tree has a #address-cells, #size-cells
	 * or similar property with a bad format or value */

/**
 * FDT_MAX_NCELLS - maximum value for #address-cells and #size-cells
 *
 * This is the maximum value for #address-cells, #size-cells and
 * similar properties that will be processed by libfdt.  IEE1275
 * requires that OF implementations handle values up to 4.
 * Implementations may support larger values, but in practice higher
 * values aren't used.
 */
#define FDT_MAX_NCELLS		4

enum fdt_status {
	FDT_STATUS_OKAY,
	FDT_STATUS_DISABLED,
	FDT_STATUS_FAIL,
	FDT_STATUS_FAIL_ERROR_CODE,
};

typedef struct {
  UINTN   Base;
  UINTN   Size;
} FdtRegion;

STATIC
UINTN
IsPrintableString (
  IN CONST VOID* data,
  IN UINTN len
  )
{
  CONST CHAR8 *s = data;
  CONST CHAR8 *ss;

  // Zero length is not
  if (len == 0) {
    return 0;
  }

  // Must terminate with zero
  if (s[len - 1] != '\0') {
    return 0;
  }

  ss = s;
  while (*s/* && isprint(*s)*/) {
    s++;
  }

  // Not zero, or not done yet
  if (*s != '\0' || (s + 1 - ss) < len) {
    return 0;
  }

  return 1;
}

STATIC
VOID
PrintData (
  IN CONST CHAR8* data,
  IN UINTN len
  )
{
  UINTN i;
  CONST CHAR8 *p = data;

  // No data, don't print
  if (len == 0)
    return;

  if (IsPrintableString (data, len)) {
    DEBUG((EFI_D_INFO, " = \"%a\"", (const char *)data));
  } else if ((len % 4) == 0) {
    DEBUG((EFI_D_INFO, " = <"));
    for (i = 0; i < len; i += 4) {
      DEBUG((EFI_D_INFO, "0x%08x%a", fdt32_to_cpu(GET_CELL(p)),i < (len - 4) ? " " : ""));
    }
    DEBUG((EFI_D_INFO, ">"));
  } else {
    DEBUG((EFI_D_INFO, " = ["));
    for (i = 0; i < len; i++)
      DEBUG((EFI_D_INFO, "%02x%a", *p++, i < len - 1 ? " " : ""));
    DEBUG((EFI_D_INFO, "]"));
  }
}

VOID
DebugDumpFdt (
  IN VOID*                FdtBlob
  )
{
  struct fdt_header *bph;
  UINT32 off_dt;
  UINT32 off_str;
  CONST CHAR8* p_struct;
  CONST CHAR8* p_strings;
  CONST CHAR8* p;
  CONST CHAR8* s;
  CONST CHAR8* t;
  UINT32 tag;
  UINTN sz;
  UINTN depth;
  UINTN shift;
  UINT32 version;
  {
    // Can 'memreserve' be printed by below code?
    INTN num = fdt_num_mem_rsv(FdtBlob);
    INTN i, err;
    UINT64 addr = 0,size = 0;

    for (i = 0; i < num; i++) {
      err = fdt_get_mem_rsv(FdtBlob, i, &addr, &size);
      if (err) {
        DEBUG((EFI_D_ERROR, "Error (%d) : Cannot get memreserve section (%d)\n", err, i));
      }
      else {
        DEBUG((EFI_D_INFO, "/memreserve/ \t0x%lx \t0x%lx;\n",addr,size));
      }
    }
  }

  depth = 0;
  shift = 4;

  bph = FdtBlob;
  off_dt = fdt32_to_cpu(bph->off_dt_struct);
  off_str = fdt32_to_cpu(bph->off_dt_strings);
  p_struct = (CONST CHAR8*)FdtBlob + off_dt;
  p_strings = (CONST CHAR8*)FdtBlob + off_str;
  version = fdt32_to_cpu(bph->version);

  p = p_struct;
  while ((tag = fdt32_to_cpu(GET_CELL(p))) != FDT_END) {
    if (tag == FDT_BEGIN_NODE) {
      s = p;
      p = PALIGN(p + AsciiStrLen (s) + 1, 4);

      if (*s == '\0')
              s = "/";

      DEBUG((EFI_D_INFO, "%*a%a {\n", depth * shift, " ", s));

      depth++;
      continue;
    }

    if (tag == FDT_END_NODE) {
      depth--;

      DEBUG((EFI_D_INFO, "%*a};\n", depth * shift, " "));
      continue;
    }

    if (tag == FDT_NOP) {
      DEBUG((EFI_D_INFO, "%*a// [NOP]\n", depth * shift, " "));
      continue;
    }

    if (tag != FDT_PROP) {
      DEBUG((EFI_D_INFO, "%*a ** Unknown tag 0x%08x\n", depth * shift, " ", tag));
      break;
    }
    sz = fdt32_to_cpu(GET_CELL(p));
    s = p_strings + fdt32_to_cpu(GET_CELL(p));
    if (version < 16 && sz >= 8)
            p = PALIGN(p, 8);
    t = p;

    p = PALIGN(p + sz, 4);

    DEBUG((EFI_D_INFO, "%*a%a", depth * shift, " ", s));
    PrintData(t, sz);
    DEBUG((EFI_D_INFO, ";\n"));
  }
}

/*
 * fdt_set_node_status: Set status for the given node
 *
 * @fdt: ptr to device tree
 * @nodeoffset: node to update
 * @status: FDT_STATUS_OKAY, FDT_STATUS_DISABLED,
 *	    FDT_STATUS_FAIL, FDT_STATUS_FAIL_ERROR_CODE
 * @error_code: optional, only used if status is FDT_STATUS_FAIL_ERROR_CODE
 */
int fdt_set_node_status(void *fdt, int nodeoffset,
			enum fdt_status status, unsigned int error_code)
{
	int ret = 0;
	CHAR8 Buffer[100];

	if (nodeoffset < 0)
		return nodeoffset;

	switch (status) {
	case FDT_STATUS_OKAY:
		ret = fdt_setprop_string(fdt, nodeoffset, "status", "okay");
		break;
	case FDT_STATUS_DISABLED:
		ret = fdt_setprop_string(fdt, nodeoffset, "status", "disabled");
		break;
	case FDT_STATUS_FAIL:
		ret = fdt_setprop_string(fdt, nodeoffset, "status", "fail");
		break;
	case FDT_STATUS_FAIL_ERROR_CODE:
		AsciiSPrint (Buffer, sizeof (Buffer), "fail-%d", error_code);
		ret = fdt_setprop_string(fdt, nodeoffset, "status", Buffer);
		break;
	default:
		DEBUG((EFI_D_ERROR, "Invalid fdt status: %x\n", status));
		ret = -1;
		break;
	}

	return ret;
}

static inline int fdt_status_disabled(void *fdt, int nodeoffset)
{
	return fdt_set_node_status(fdt, nodeoffset, FDT_STATUS_DISABLED, 0);
}

/**
 * fdt_alloc_phandle: Return next free phandle value
 *
 * @blob: ptr to device tree
 */
int fdt_alloc_phandle(void *blob)
{
	int offset;
	uint32_t phandle = 0;

	for (offset = fdt_next_node(blob, -1, NULL); offset >= 0;
	     offset = fdt_next_node(blob, offset, NULL)) {
		phandle = max(phandle, fdt_get_phandle(blob, offset));
	}

	return phandle + 1;
}


/*
 * fdt_set_phandle: Create a phandle property for the given node
 *
 * @fdt: ptr to device tree
 * @nodeoffset: node to update
 * @phandle: phandle value to set (must be unique)
 */
int fdt_set_phandle(void *fdt, int nodeoffset, uint32_t phandle)
{
	int ret;

	ret = fdt_setprop_cell(fdt, nodeoffset, "phandle", phandle);
	if (ret < 0)
		return ret;

	/*
	 * For now, also set the deprecated "linux,phandle" property, so that we
	 * don't break older kernels.
	 */
	ret = fdt_setprop_cell(fdt, nodeoffset, "linux,phandle", phandle);

	return ret;
}

/*
 * fdt_create_phandle: Create a phandle property for the given node
 *
 * @fdt: ptr to device tree
 * @nodeoffset: node to update
 */
unsigned int fdt_create_phandle(void *fdt, int nodeoffset)
{
	/* see if there is a phandle already */
	int phandle = fdt_get_phandle(fdt, nodeoffset);

	/* if we got 0, means no phandle so create one */
	if (phandle == 0) {
		int ret;

		phandle = fdt_alloc_phandle(fdt);
		ret = fdt_set_phandle(fdt, nodeoffset, phandle);
		if (ret < 0) {
			DEBUG((EFI_D_ERROR,"Can't set phandle %u: %s\n", phandle,
			       fdt_strerror(ret)));
			return 0;
		}
	}

	return phandle;
}

int fdt_address_cells(const void *fdt, int nodeoffset)
{
	const fdt32_t *ac;
	int val;
	int len;

	ac = fdt_getprop(fdt, nodeoffset, "#address-cells", &len);
	if (!ac)
		return 2;

	if (len != sizeof(*ac))
		return -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*ac);
	if ((val <= 0) || (val > FDT_MAX_NCELLS))
			return -FDT_ERR_BADNCELLS;

	return val;
}

int fdt_size_cells(const void *fdt, int nodeoffset)
{
	const fdt32_t *sc;
	int val;
	int len;

	sc = fdt_getprop(fdt, nodeoffset, "#size-cells", &len);
	if (!sc)
		return 2;

	if (len != sizeof(*sc))
		return -FDT_ERR_BADNCELLS;

	val = fdt32_to_cpu(*sc);
	if ((val < 0) || (val > FDT_MAX_NCELLS))
			return -FDT_ERR_BADNCELLS;

	return val;
}

static int fdt_pack_reg(const void *fdt, void *buf, UINT64 *address, UINT64 *size,
		      int n)
{
	int i;
	int address_cells = fdt_address_cells(fdt, 0);
	int size_cells = fdt_size_cells(fdt, 0);
	char *p = buf;

	for (i = 0; i < n; i++) {
		if (address_cells == 2)
			*(fdt64_t *)p = cpu_to_fdt64(address[i]);
		else
			*(fdt32_t *)p = cpu_to_fdt32(address[i]);
		p += 4 * address_cells;

		if (size_cells == 2)
			*(fdt64_t *)p = cpu_to_fdt64(size[i]);
		else
			*(fdt32_t *)p = cpu_to_fdt32(size[i]);
		p += 4 * size_cells;
	}

	return p - (char *)buf;
}

VOID
FixupByCompatibleField (
  VOID *Fdt,
  CONST char *Compat,
  CONST char *Prop,
  CONST VOID *Val,
  INTN Len,
  INTN Create
  )
{
	INTN Offset = -1;
	Offset = fdt_node_offset_by_compatible(Fdt, -1, Compat);
	while (Offset != -FDT_ERR_NOTFOUND) {
		if (Create || (fdt_get_property(Fdt, Offset, Prop, NULL) != NULL))
			fdt_setprop(Fdt, Offset, Prop, Val, Len);
		Offset = fdt_node_offset_by_compatible(Fdt, Offset, Compat);
	}
}

VOID
FixupByCompatibleField32 (
  VOID *Fdt,
  CONST char *Compat,
  CONST char *Prop,
  UINT32 Val,
  INTN Create
  )
{
	fdt32_t Tmp = cpu_to_fdt32(Val);
	FixupByCompatibleField(Fdt, Compat, Prop, &Tmp, 4, Create);
}

/*
 * the burden is on the the caller to not request a count
 * exceeding the bounds of the stream_ids[] array
 */
void alloc_stream_ids(UINTN start_id, UINTN count, UINT32 *stream_ids, UINTN max_cnt)
{
	UINTN i;

	if (count > max_cnt) {
		DEBUG((EFI_D_ERROR, "\nERROR: max per-device stream ID count exceed\n"));
		return;
	}

	for (i = 0; i < count; i++)
		stream_ids[i] = start_id++;
}

/*
 * This function updates the mmu-masters property on the SMMU
 * node as per the SMMU binding-- phandle and list of stream IDs
 * for each MMU master.
 */
void append_mmu_masters(void *blob, const char *smmu_path,
			const char *master_name, UINT32 *stream_ids, UINTN count)
{
	UINT32 phandle;
	UINTN smmu_nodeoffset;
	UINTN master_nodeoffset;
	UINTN i;

	/* get phandle of mmu master device */
	master_nodeoffset = fdt_path_offset(blob, master_name);
	if (master_nodeoffset < 0) {
		DEBUG((EFI_D_ERROR, "\nERROR: master not found\n"));
		return;
	}
	phandle = fdt_get_phandle(blob, master_nodeoffset);
	if (!phandle) { /* if master has no phandle, create one */
		phandle = fdt_create_phandle(blob, master_nodeoffset);
		if (!phandle) {
			DEBUG((EFI_D_ERROR, "\nERROR: unable to create phandle\n"));
			return;
		}
	}

	/* append it to mmu-masters */
	smmu_nodeoffset = fdt_path_offset(blob, smmu_path);
	if (fdt_appendprop_u32(blob, smmu_nodeoffset, "mmu-masters",
			       phandle) < 0) {
		DEBUG((EFI_D_ERROR, "\n%s: ERROR: unable to update SMMU node\n"));
		return;
	}

	/* for each stream ID, append to mmu-masters */
	for (i = 0; i < count; i++) {
		fdt_appendprop_u32(blob, smmu_nodeoffset, "mmu-masters",
				   stream_ids[i]);
	}

	/* fix up #stream-id-cells with stream ID count */
	if (fdt_setprop_u32(blob, master_nodeoffset, "#stream-id-cells",
			    count) < 0)
		DEBUG((EFI_D_ERROR, "\nERROR: unable to update #stream-id-cells\n"));
}

void pcie_set_available_streamids(void *blob, const char *pcie_path,
				  UINT32 *stream_ids, UINTN count)
{
	UINTN nodeoffset;
	UINTN i;

	nodeoffset = fdt_path_offset(blob, pcie_path);
	if (nodeoffset < 0) {
		DEBUG((EFI_D_ERROR,"\nERROR: unable to update PCIe node\n"));
		return;
	}

	/* for each stream ID, append to mmu-masters */
	for (i = 0; i < count; i++) {
		fdt_appendprop_u32(blob, nodeoffset, "available-stream-ids",
				   stream_ids[i]);
	}
}

#define MAX_STREAM_IDS 4
void fdt_fixup_smmu_pcie(void *blob)
{
	UINTN count;
	UINT32 stream_ids[MAX_STREAM_IDS];
	UINT32 ctlr_streamid = 0x300;

	/* PEX1 stream ID fixup */
	count =	NXP_PEX1_STREAM_ID_END - NXP_PEX1_STREAM_ID_START + 1;
	alloc_stream_ids(NXP_PEX1_STREAM_ID_START, count, stream_ids,
			 MAX_STREAM_IDS);
	pcie_set_available_streamids(blob, "/pcie@3400000", stream_ids, count);
	append_mmu_masters(blob, "/iommu@5000000", "/pcie@3400000",
			   &ctlr_streamid, 1);

	/* PEX2 stream ID fixup */
	count =	NXP_PEX2_STREAM_ID_END - NXP_PEX2_STREAM_ID_START + 1;
	alloc_stream_ids(NXP_PEX2_STREAM_ID_START, count, stream_ids,
			 MAX_STREAM_IDS);
	pcie_set_available_streamids(blob, "/pcie@3500000", stream_ids, count);
	append_mmu_masters(blob, "/iommu@5000000", "/pcie@3500000",
			   &ctlr_streamid, 1);

	/* PEX3 stream ID fixup */
	count =	NXP_PEX3_STREAM_ID_END - NXP_PEX3_STREAM_ID_START + 1;
	alloc_stream_ids(NXP_PEX3_STREAM_ID_START, count, stream_ids,
			 MAX_STREAM_IDS);
	pcie_set_available_streamids(blob, "/pcie@3600000", stream_ids, count);
	append_mmu_masters(blob, "/iommu@5000000", "/pcie@3600000",
			   &ctlr_streamid, 1);

	/* PEX4 stream ID fixup */
	count =	NXP_PEX4_STREAM_ID_END - NXP_PEX4_STREAM_ID_START + 1;
	alloc_stream_ids(NXP_PEX4_STREAM_ID_START, count, stream_ids,
			 MAX_STREAM_IDS);
	pcie_set_available_streamids(blob, "/pcie@3700000", stream_ids, count);
	append_mmu_masters(blob, "/iommu@5000000", "/pcie@3700000",
			   &ctlr_streamid, 1);
}

/*
 * The info below summarizes how streamID partitioning works
 * for ls2088a and how it is conveyed to the OS via the device tree.
 *
 *  -non-PCI legacy, platform devices (USB, SD/MMC, SATA, DMA)
 *     -all legacy devices get a unique ICID assigned and programmed in
 *      their AMQR registers by u-boot
 *     -u-boot updates the hardware device tree with streamID properties
 *      for each platform/legacy device (smmu-masters property)
 *
 *  -PCIe
 *     -for each PCI controller that is active (as per RCW settings),
 *      u-boot will allocate a range of ICID and convey that to Linux via
 *      the device tree (smmu-masters property)
 *
 *  -DPAA2
 *     -u-boot will allocate a range of ICIDs to be used by the Management
 *      Complex for containers and will set these values in the MC DPC image.
 *     -the MC is responsible for allocating and setting up ICIDs
 *      for all DPAA2 devices.
 *
 */
static void fdt_fixup_smmu(void *blob)
{
	UINTN nodeoffset;

	nodeoffset = fdt_path_offset(blob, "/iommu@5000000");
	if (nodeoffset < 0) {
		DEBUG((EFI_D_ERROR,"\nWARNING: no SMMU node found\n"));
		return;
	}

	/* fixup for all PCI controllers */
	fdt_fixup_smmu_pcie(blob);
}

/* Helper to read a big number; size is in cells (not bytes) */
static inline UINTN of_read_number(const fdt32_t *cell, int size)
{
	UINTN r = 0;
	while (size--)
		r = (r << 32) | fdt32_to_cpu(*(cell++));
	return r;
}

/* Callbacks for bus specific translators */
struct of_bus {
	const char	*name;
	const char	*addresses;
	void		(*count_cells)(void *blob, int parentoffset,
				int *addrc, int *sizec);
	UINTN		(*map)(fdt32_t *addr, const fdt32_t *range,
				int na, int ns, int pna);
	int		(*translate)(fdt32_t *addr, UINTN offset, int na);
};

/* Default translator (generic bus) */
void of_bus_default_count_cells(void *blob, int parentoffset,
					int *addrc, int *sizec)
{
	const fdt32_t *prop;

	if (addrc)
		*addrc = fdt_address_cells(blob, parentoffset);

	if (sizec) {
		prop = fdt_getprop(blob, parentoffset, "#size-cells", NULL);
		if (prop)
			*sizec = ___swab32(*prop);
		else
			*sizec = 1;
	}
}

static UINTN of_bus_default_map(fdt32_t *addr, const fdt32_t *range,
		int na, int ns, int pna)
{
	UINTN cp, s, da;

	cp = of_read_number(range, na);
	s  = of_read_number(range + na + pna, ns);
	da = of_read_number(addr, na);

	if (da < cp || da >= (cp + s))
		return OF_BAD_ADDR;
	return da - cp;
}

static int of_bus_default_translate(fdt32_t *addr, UINTN offset, int na)
{
	UINTN a = of_read_number(addr, na);
	memset(addr, 0, na * 4);
	a += offset;
	if (na > 1)
		addr[na - 2] = cpu_to_fdt32(a >> 32);
	addr[na - 1] = cpu_to_fdt32(a & 0xffffffffu);

	return 0;
}

/* Array of bus specific translators */
static struct of_bus of_busses[] = {
	/* Default */
	{
		.name = "default",
		.addresses = "reg",
		.count_cells = of_bus_default_count_cells,
		.map = of_bus_default_map,
		.translate = of_bus_default_translate,
	},
};

static int of_translate_one(void * blob, int parent, struct of_bus *bus,
			    struct of_bus *pbus, fdt32_t *addr,
			    int na, int ns, int pna, const char *rprop)
{
	const fdt32_t *ranges;
	int rlen;
	int rone;
	UINTN offset = OF_BAD_ADDR;

	/* Normally, an absence of a "ranges" property means we are
	 * crossing a non-translatable boundary, and thus the addresses
	 * below the current not cannot be converted to CPU physical ones.
	 * Unfortunately, while this is very clear in the spec, it's not
	 * what Apple understood, and they do have things like /uni-n or
	 * /ht nodes with no "ranges" property and a lot of perfectly
	 * useable mapped devices below them. Thus we treat the absence of
	 * "ranges" as equivalent to an empty "ranges" property which means
	 * a 1:1 translation at that level. It's up to the caller not to try
	 * to translate addresses that aren't supposed to be translated in
	 * the first place. --BenH.
	 */
	ranges = fdt_getprop(blob, parent, rprop, &rlen);
	if (ranges == NULL || rlen == 0) {
		offset = of_read_number(addr, na);
		memset(addr, 0, pna * 4);
		goto finish;
	}


	/* Now walk through the ranges */
	rlen /= 4;
	rone = na + pna + ns;
	for (; rlen >= rone; rlen -= rone, ranges += rone) {
		offset = bus->map(addr, ranges, na, ns, pna);
		if (offset != OF_BAD_ADDR)
			break;
	}
	if (offset == OF_BAD_ADDR) {
		return 1;
	}
	memcpy(addr, ranges + na, 4 * pna);

 finish:

	/* Translate it into parent bus space */
	return pbus->translate(addr, offset, pna);
}

/*
 * Translate an address from the device-tree into a CPU physical address,
 * this walks up the tree and applies the various bus mappings on the
 * way.
 *
 * Note: We consider that crossing any level with #size-cells == 0 to mean
 * that translation is impossible (that is we are not dealing with a value
 * that can be mapped to a cpu physical address). This is not really specified
 * that way, but this is traditionally the way IBM at least do things
 */
static UINT64 __of_translate_address(void *blob, int node_offset, const fdt32_t *in_addr,
				  const char *rprop)
{
	int parent;
	struct of_bus *bus, *pbus;
	fdt32_t addr[OF_MAX_ADDR_CELLS];
	int na, ns, pna, pns;
	UINTN result = OF_BAD_ADDR;

	/* Get parent & match bus type */
	parent = fdt_parent_offset(blob, node_offset);
	if (parent < 0)
		goto bail;
	bus = &of_busses[0];

	/* Cound address cells & copy address locally */
	bus->count_cells(blob, parent, &na, &ns);
	if (!OF_CHECK_COUNTS(na, ns)) {
		DEBUG((EFI_D_ERROR,"%s: Bad cell count for %s\n", __FUNCTION__,
		       fdt_get_name(blob, node_offset, NULL)));
		goto bail;
	}
	memcpy(addr, in_addr, na * 4);

	/* Translate */
	for (;;) {
		/* Switch to parent bus */
		node_offset = parent;
		parent = fdt_parent_offset(blob, node_offset);

		/* If root, we have finished */
		if (parent < 0) {
			result = of_read_number(addr, na);
			break;
		}

		/* Get new parent bus and counts */
		pbus = &of_busses[0];
		pbus->count_cells(blob, parent, &pna, &pns);
		if (!OF_CHECK_COUNTS(pna, pns)) {
			DEBUG((EFI_D_ERROR,"%s: Bad cell count for %s\n", __FUNCTION__,
				fdt_get_name(blob, node_offset, NULL)));
			break;
		}

		/* Apply bus translation */
		if (of_translate_one(blob, node_offset, bus, pbus,
					addr, na, ns, pna, rprop))
			break;

		/* Complete the move up one level */
		na = pna;
		ns = pns;
		bus = pbus;
	}
 bail:

	return result;
}

UINTN fdt_translate_address(void *blob, int node_offset, const fdt32_t *in_addr)
{
	return __of_translate_address(blob, node_offset, in_addr, "ranges");
}

/**
 * fdt_node_offset_by_compat_reg: Find a node that matches compatiable and
 * who's reg property matches a physical cpu address
 *
 * @blob: ptr to device tree
 * @compat: compatiable string to match
 * @compat_off: property name
 *
 */
int fdt_node_offset_by_compat_reg(void *blob, const char *compat,
					UINT32 compat_off)
{
	int len, off = fdt_node_offset_by_compatible(blob, -1, compat);
	while (off != -FDT_ERR_NOTFOUND) {
		const fdt32_t *reg = fdt_getprop(blob, off, "reg", &len);
		if (reg) {
			if (compat_off == fdt_translate_address(blob, off, reg))
				return off;
		}
		off = fdt_node_offset_by_compatible(blob, off, compat);
	}

	return -FDT_ERR_NOTFOUND;
}

static void ft_pcie_ls_setup(void *blob, const char *pci_compat,
			     unsigned long ctrl_addr, enum _SERDES_LANE_PROTOCOL dev)
{
	UINTN off;
	UINT64 SerDes1ProtocolMap = 0x0;
	UINT64 SerDes2ProtocolMap = 0x0;

	off = fdt_node_offset_by_compat_reg(blob, pci_compat,
					    (UINT32)ctrl_addr);
	if (off < 0)
		return;

	GetSerdesProtocolMaps(&SerDes1ProtocolMap, &SerDes2ProtocolMap);
	if (!IsSerDesLaneProtocolConfigured(SerDes1ProtocolMap, SerDes2ProtocolMap, dev))
		fdt_set_node_status(blob, off, FDT_STATUS_DISABLED, 0);
       else
		fdt_set_node_status(blob, off, FDT_STATUS_OKAY, 0);
}

#define FSL_PCIE_COMPAT "fsl,ls2088a-pcie"
void ft_pci_setup(void *blob)
{
	ft_pcie_ls_setup(blob, FSL_PCIE_COMPAT, PCIE1_BASE_ADDR, PCIE1);

	ft_pcie_ls_setup(blob, FSL_PCIE_COMPAT, PCIE2_BASE_ADDR, PCIE2);

	ft_pcie_ls_setup(blob, FSL_PCIE_COMPAT, PCIE3_BASE_ADDR, PCIE3);

	ft_pcie_ls_setup(blob, FSL_PCIE_COMPAT, PCIE4_BASE_ADDR, PCIE4);
}

UINT8
SdxcSupported (
  VOID
  )
{
  EFI_MMC_HOST_PROTOCOL *Host;
  EFI_STATUS 			Status;

  Status = gBS->LocateProtocol (&gEfiMmcHostProtocolGuid,
		NULL,
		(VOID **)&Host);

  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR,"Failed to locate Mmc host protocol\n"));
    return Status;
  }

  Status = Host->IsCardPresent(Host);

  return Status;
}

void fdt_fixup_esdhc(void *blob)
{
	struct SysInfo SocSysInfo;

	const char *compat = "fsl,esdhc";

	if (!SdxcSupported()) {
		FixupByCompatibleField(blob, compat, "status", "disabled",
			   4 + 1, 1);
		return;
	}
	
	GetSysInfo(&SocSysInfo);

	FixupByCompatibleField32(blob, compat, "clock-frequency",
			       SocSysInfo.FreqSdhc, 1);

	FixupByCompatibleField(blob, compat, "status", "okay",
			   4 + 1, 1);
}

#define SYS_CLK_FREQ_533MHZ 	533

void fdt_fixup_usb(void *blob)
{
	UINTN off;
	UINTN Sysclk;

	struct SysInfo SocSysInfo;
	GetSysInfo(&SocSysInfo);

	Sysclk = SocSysInfo.FreqSystemBus / 1000000;
	if (SYS_CLK_FREQ_533MHZ == Sysclk) {
		off = fdt_node_offset_by_compatible(blob, -1, "snps,dwc3");
		while (off != -FDT_ERR_NOTFOUND) {
			fdt_status_disabled(blob, off);
			off = fdt_node_offset_by_compatible(blob, off, "snps,dwc3");
		}
	}

}

void fdt_fixup_psci(void *blob)
{
		INTN node, n, Status;
		node = fdt_subnode_offset (blob, 0, "cpus");
		if(node < 0) {
			DEBUG((EFI_D_ERROR, "Fdt: No cpus node found!!\n"));
			ASSERT(0);
		}
		n = fdt_first_subnode (blob, node);
		if(n<0) {
				DEBUG((EFI_D_ERROR, "No cpu nodes found!!!\n"));
				ASSERT(0);
		}

		while (1) {
			Status = fdt_setprop_string(blob, n, "enable-method", "psci");
			if(Status) {
				DEBUG((EFI_D_ERROR, "fdt_setprop/psci: Could not add enable-method, %a!!\n", fdt_strerror(Status)));
				ASSERT(0);
			}
			n = fdt_next_subnode(blob, n);
			if(n < 0)
				break;
		}

		// Create the /psci node if it doesn't exist
		node = fdt_subnode_offset (blob, 0, "psci");
		if (node < 0) {
			node = fdt_add_subnode(blob, 0, "psci");
			if(node < 0) {
				DEBUG((EFI_D_ERROR, "fdt_add_node: Could not add psci!!, %a\n", fdt_strerror(Status)));
				ASSERT(0);
			}

			Status = fdt_setprop_string(blob, node, "compatible", "arm,psci-0.2");
			if(Status) {
				DEBUG((EFI_D_ERROR, "fdt_setprop/psci: Could not add compatiblity, %a!!\n", fdt_strerror(Status)));
				ASSERT(0);
			}

			Status = fdt_setprop_string(blob, node, "method", "smc");
			if(Status) {
				DEBUG((EFI_D_ERROR, "fdt_setprop/psci: Could not add method, %a!!\n", fdt_strerror(Status)));
				ASSERT(0);
			}

			if(node < 0) {
				DEBUG((EFI_D_ERROR, "Fdt: Could not add psci node!!\n"));
				ASSERT(0);
			}
		}

		DEBUG((EFI_D_INFO, "PSCI fixup done!!!!\n"));
}

void fdt_fixup_memory(void *blob) {
	INTN node, Status, Length;
	UINT8 *Reg[32];
	UINT64 MemStart[] = {DRAM1_BASE_ADDR, DRAM2_BASE_ADDR};
	UINT64 MemSize[] = {DRAM1_SIZE, DDR_MEM_SIZE};
	node = fdt_subnode_offset (blob, 0, "memory");
	if(node < 0) {
			DEBUG((EFI_D_ERROR, "fdt_fixup_memory: Could not find memory node!!\n"));
			ASSERT(0);
	}
	Length = fdt_pack_reg (blob, Reg, MemStart, MemSize, 2);
	Status = fdt_setprop(blob, node, "reg", Reg, Length);
	if(Status < 0) {
			DEBUG((EFI_D_ERROR, "fdt_fixup_memory: Could not set memory regs!!\n"));
			ASSERT(0);
	}
	DEBUG((EFI_D_INFO, "Memory fixup done!!!!\n"));
}

#define JR3_OFFSET     0x40000

VOID FdtFixupJR (VOID *Blob)
{
  INT32 JRNode = 0, Parent = 0;
  INT32  AddrCell, Length, CryptoPath;
  UINT64 JROffset;
  fdt32_t *Reg;

  CryptoPath = fdt_path_offset(Blob, "crypto");
  if (CryptoPath < 0) {
    DEBUG((EFI_D_ERROR, "Crypto node not found \n"));
    return;
  }
  Parent = CryptoPath;

  of_busses[0].count_cells(Blob, CryptoPath, &AddrCell, NULL);

  for (;;) {
    JRNode = fdt_node_offset_by_compatible(Blob, Parent,
                    "fsl,sec-v4.0-job-ring");
    if (JRNode == -FDT_ERR_NOTFOUND) {
      DEBUG((EFI_D_ERROR, "WARNING : sec node not found \n"));
      return;
    }
    Reg = (fdt32_t *)fdt_getprop(Blob, JRNode, "reg", &Length);
    JROffset = of_read_number(Reg, AddrCell);
    if (JROffset == JR3_OFFSET) {
      fdt_del_node(Blob, JRNode);
      DEBUG ((EFI_D_INFO, "Job Ring 0x%x deleted successfully\n",
             JR3_OFFSET));
      return;
    }
    Parent = JRNode;
  }
}

/*
 * msi-map is a property to be added to the pci controller
 * node.  It is a table, where each entry consists of 4 fields
 * e.g.:
 *
 *      msi-map = <[devid] [phandle-to-msi-ctrl] [stream-id] [count]
 *                 [devid] [phandle-to-msi-ctrl] [stream-id] [count]>;
 */
void SetMsiMapEntry(void *blob, UINTN PciBase,
                       UINT32 devid, UINT32 streamid)
{
    UINT32 *prop;
    UINT32 phandle;
    int nodeoffset;
    char *compat = NULL;

    /* find pci controller node */
    nodeoffset = fdt_node_offset_by_compat_reg(blob, "fsl,ls-pcie",
            PciBase);

    if (nodeoffset < 0) {
        compat = FSL_PCIE_COMPAT;

        nodeoffset = fdt_node_offset_by_compat_reg(blob,
                compat, PciBase);
        if (nodeoffset < 0) {
            DEBUG((EFI_D_ERROR, "PCI %a node not found \n", compat));
            return;
        }
    }

    prop = (UINT32 *)fdt_getprop(blob, nodeoffset, "msi-parent", 0);
    if (prop == NULL) {
        DEBUG((EFI_D_ERROR, "missing msi-parent: PCIe 0x%x\n", PciBase));
        return;
    }
    phandle = fdt32_to_cpu(*prop);

    /* set one msi-map row */
    fdt_appendprop_u32(blob, nodeoffset, "msi-map", devid);
    fdt_appendprop_u32(blob, nodeoffset, "msi-map", phandle);
    fdt_appendprop_u32(blob, nodeoffset, "msi-map", streamid);
    fdt_appendprop_u32(blob, nodeoffset, "msi-map", 1);
}

void LutUpdate(UINTN LutBase, UINT32 Value, UINT32 Offset)
{
        MmioWrite32((UINTN)(LutBase + Offset), Value);
}

/*
 *  * Program a single LUT entry
 *   */
void SetPcieLutMapping(UINTN PciBase, UINT32 Index, UINT32 DevId,
        UINT32 StreamId)
{
    UINTN LutBase = PciBase + LS_PCIE_LUT_BASE;

    LutUpdate(LutBase, DevId << 16, PCIE_LUT_UDR(Index));
    LutUpdate(LutBase, StreamId | PCIE_LUT_ENABLE, PCIE_LUT_LDR(Index));
}

struct PciChildInfo {
    UINT8 HasChild;
    UINT8 SeqNum;
}PciChildInfo[MAX_PCI_DEVICES];

void UpdatePciChildInfo ()
{
    EFI_STATUS Status;
    UINTN Size;
    UINT32 Id = 0;
    EFI_HANDLE *Handle;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciProtocol;

    Status = gBS->LocateHandleBuffer (ByProtocol,
            &gEfiPciRootBridgeIoProtocolGuid,
            NULL, &Size, &Handle);

    if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR, "Fail to locate PciRootBridgeIoProtocolGuid 0x%x\n",
                    Status));
        return;
    }

    if (Size == 0)
        return;

    for (Id = 0; Id < Size; Id++) {
        Status = gBS->OpenProtocol (Handle[Id],
                &gEfiPciRootBridgeIoProtocolGuid,
                (VOID **)&PciProtocol, Handle[Id],
                Handle[Id], EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        if (EFI_ERROR(Status)) {
            DEBUG((EFI_D_ERROR, "OpenProtocol failed for handle 0x%x \n",
                        Handle[Id]));
            continue;
        }

        PciChildInfo[PciProtocol->SegmentNumber -1].HasChild = 1;
    }
}

void fdt_fixup_pcie(void *blob)
{
    UINT32 StreamId = NXP_PEX1_STREAM_ID_START;
    UINT32 Bdf = 0, PciNo, SeqNum;
    UINT32 NextLutIndex = 0;
    UINTN PciBaseAddress[] = {PCIE1_BASE_ADDR,
                              PCIE2_BASE_ADDR,
                              PCIE3_BASE_ADDR,
                              PCIE4_BASE_ADDR};

    UpdatePciChildInfo();

    /* Scan all known buses */
    for (PciNo = 0, SeqNum = 0; PciNo < MAX_PCI_DEVICES; PciNo++, SeqNum++) {
        if (!IsPcieEnabled(PciBaseAddress[PciNo]))
            continue;

        if (StreamId > NXP_PEX4_STREAM_ID_END) {
            DEBUG((EFI_D_ERROR, "No free stream ids available\n"));
            return;
        }

        /* map PCI b.d.f to streamID in LUT */
        SetPcieLutMapping(PciBaseAddress[PciNo], NextLutIndex, Bdf >> 8,
                StreamId);

        /* update msi-map in device tree */
        DEBUG((EFI_D_RELEASE, "0x%x : 0x%x, %d \n", PciBaseAddress[PciNo], Bdf, StreamId));
        SetMsiMapEntry(blob, PciBaseAddress[PciNo], Bdf >> 8,
                StreamId);

        /* Check for child */
        if (PciChildInfo[PciNo].HasChild) {
            SeqNum++;
            PciChildInfo[PciNo].SeqNum = SeqNum;
        }
        StreamId++;
    }

    /* Scan all children */
    NextLutIndex = 1;
    for (PciNo = 0; PciNo < MAX_PCI_DEVICES; PciNo++) {
        if (!PciChildInfo[PciNo].HasChild)
            continue;

        if (StreamId > NXP_PEX4_STREAM_ID_END) {
            DEBUG((EFI_D_ERROR, "No free stream ids available\n"));
            return;
        }

        /* the DT fixup must be relative to the hose first_busno */
        DEBUG((EFI_D_RELEASE, "PciChildInfo[PciNo].SeqNum %d \n", PciChildInfo[PciNo].SeqNum));
        Bdf = (PciChildInfo[PciNo].SeqNum << 16) - 
                     PCI_BDF(PciChildInfo[PciNo].SeqNum - 1, 0, 0);

        /* map PCI b.d.f to streamID in LUT */
        SetPcieLutMapping(PciBaseAddress[PciNo], NextLutIndex, Bdf >> 8,
                StreamId);

        /* update msi-map in device tree */
        DEBUG((EFI_D_RELEASE, "0x%x : 0x%x, %d \n", PciBaseAddress[PciNo], Bdf, StreamId));
        SetMsiMapEntry(blob, PciBaseAddress[PciNo], Bdf >> 8, StreamId);

        StreamId++;
    }
}

void FdtCpuSetup(void *blob, UINTN blob_size)
{
  if (PcdGet32(PcdBootMode) != QSPI_BOOT) {
	struct SysInfo SocSysInfo;
	GetSysInfo(&SocSysInfo);
	UINTN SysClk = get_board_sys_clk();

	fdt_open_into(blob, (VOID *)blob, blob_size);

       /* Job ring 3 is used by PPA, so remove it from dtb */
       FdtFixupJR(blob);

	FixupByCompatibleField32(blob, "fsl,ns16550",
			       "clock-frequency", SocSysInfo.FreqSystemBus/2, 1);

	FixupByCompatibleField32(blob, "fixed-clock",
			       "clock-frequency", SysClk, 1);

	ft_pci_setup(blob);

       fdt_fixup_pcie(blob);

	fdt_fixup_esdhc(blob);

	fdt_fixup_smmu(blob);

	fdt_fixup_usb(blob);

	fdt_fixup_psci(blob);

	fdt_fixup_memory(blob);
  }
}
