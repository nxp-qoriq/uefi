/** SoCLib.h
*  Header defining the LS1043a SoC specific constants (Base addresses, sizes, flags)
*
*  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#ifndef __LS1043A_SOC_H__
#define __LS1043A_SOC_H__

#define CONFIG_SYS_FSL_NUM_CC_PLLS	2
#define HWA_CGA_M1_CLK_SEL		0xe0000000
#define HWA_CGA_M1_CLK_SHIFT		29
#define HWA_CGA_M2_CLK_SEL		0x00000007
#define HWA_CGA_M2_CLK_SHIFT		0

#define TP_ITYP_AV			0x00000001	/* Initiator available */
#define TP_ITYP_TYPE(x)			(((x) & 0x6) >> 1) /* Initiator Type */
#define TP_ITYP_TYPE_ARM		0x0
#define TP_ITYP_TYPE_PPC		0x1	/* PowerPC */
#define TP_ITYP_TYPE_OTHER		0x2	/* StarCore DSP */
#define TP_ITYP_TYPE_HA			0x3	/* HW Accelerator */
#define TP_ITYP_THDS(x)			(((x) & 0x18) >> 3)	/* # threads */
#define TP_ITYP_VER(x)			(((x) & 0xe0) >> 5)	/* Initiator Version */
#define TY_ITYP_VER_A53			0x2

#define TP_CLUSTER_EOC_MASK	0xc0000000	/* end of clusters mask */
#define TP_CLUSTER_INIT_MASK	0x0000003f	/* initiator mask */
#define TP_INIT_PER_CLUSTER	4

#define CONFIG_SYS_CLK_FREQ		100000000
#define CONFIG_DDR_CLK_FREQ		100000000

#define CONFIG_MAX_CPUS				4
#define FMAN_V3
#define NUM_FMAN			1
#define NUM_FMAN1_DTSEC		7
#define NUM_FMAN1_10GEC		1

#define FMAN_MEM_SIZE	0x60000

/* Generic Interrupt Controller Definitions */
#define GICD_BASE_4K        0x01401000
#define GICD_SIZE_4K        0x1000
#define GICC_BASE_4K        0x01402000
#define GICC_SIZE_4K        0x2000
#define GICH_BASE_4K        0x01404000
#define GICH_SIZE_4K        0x2000
#define GICV_BASE_4K        0x01406000
#define GICV_SIZE_4K        0x2000

#define GICD_BASE_64K	0x01410000
#define GICD_SIZE_64K	0x10000
#define GICC_BASE_64K	0x01420000
#define GICC_SIZE_64K	0x20000
#define GICH_BASE_64K	0x01440000
#define GICH_SIZE_64K	0x20000
#define GICV_BASE_64K	0x01460000
#define GICV_SIZE_64K	0x20000

#define REV1_0                     0x10
#define REV1_1                     0x11
#define SCFG_GIC400_ALIGN   0x1570188

typedef struct CpuType {
	CHAR8  name[16];
	UINT32 soc_ver;
	UINT32 num_cores;
}CPU_TYPE;

#define CPU_TYPE_ENTRY(n, v, nc) \
       { .name = #n, .soc_ver = SVR_##v, .num_cores = (nc)}

#define SVR_WO_E            0xFFFFFE
#define SVR_MAJOR(svr)      (((svr) >> 4) & 0xf)
#define SVR_MINOR(svr)      (((svr) >> 0) & 0xf)
#define SVR_SOC_VER(svr)    (((svr) >> 8) & SVR_WO_E)

#define SVR_LS1043A		0x879200

#define IS_E_PROCESSOR(svr) (!((svr >> 8) & 0x1))

/*
 * Divide positive or negative dividend by positive divisor and round
 * to closest UINTNeger. Result is undefined for negative divisors and
 * for negative dividends if the divisor variable type is unsigned.
 */
#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
	typeof(x) __x = x;				\
	typeof(divisor) __d = divisor;			\
	(((typeof(x))-1) > 0 ||				\
	 ((typeof(divisor))-1) > 0 || (__x) > 0) ?	\
		(((__x) + ((__d) / 2)) / (__d)) :	\
		(((__x) - ((__d) / 2)) / (__d));	\
}							\
)

/*
 * HammingWeight32: returns the hamming weight (i.e. the number
 * of bits set) of a 32-bit word
 */
static inline UINTN HammingWeight32(UINTN w)
{
	UINTN Res = (w & 0x55555555) + ((w >> 1) & 0x55555555);
	Res = (Res & 0x33333333) + ((Res >> 2) & 0x33333333);
	Res = (Res & 0x0F0F0F0F) + ((Res >> 4) & 0x0F0F0F0F);
	Res = (Res & 0x00FF00FF) + ((Res >> 8) & 0x00FF00FF);
	return (Res & 0x0000FFFF) + ((Res >> 16) & 0x0000FFFF);
}

static inline UINTN CpuMaskNext(UINTN Cpu, UINTN Mask)
{
	for (Cpu++; !((1 << Cpu) & Mask); Cpu++)
		;

	return Cpu;
}

#define ForEachCpu(iter, cpu, num_cpus, mask) \
	for (iter = 0, cpu = CpuMaskNext(-1, mask); \
		iter < num_cpus; \
		iter++, cpu = CpuMaskNext(cpu, mask)) \

struct SysInfo {
	UINTN FreqProcessor[CONFIG_MAX_CPUS];
	UINTN FreqSystemBus;
	UINTN FreqDdrBus;
	UINTN FreqLocalBus;
	UINTN FreqSdhc;
	UINTN FreqFman[NUM_FMAN];
	UINTN FreqQman;
};

typedef struct SocClocks {
	UINTN CpuClk;	/* CPU clock in Hz! */
	UINTN BusClk;
	UINTN MemClk;
	UINTN PciClk;
	UINTN SdhcClk;
} SocClockInfo;

enum PeriphClock {
	ARM_CLK = 0,
	BUS_CLK,
	UART_CLK,
	ESDHC_CLK,
	I2C_CLK,
	DSPI_CLK,
};

enum CsuCslxAccess {
	SEC_UNIT_NS_SUP_R = 0x08,
	SEC_UNIT_NS_SUP_W = 0x80,
	SEC_UNIT_NS_SUP_RW = 0x88,
	SEC_UNIT_NS_USER_R = 0x04,
	SEC_UNIT_NS_USER_W = 0x40,
	SEC_UNIT_NS_USER_RW = 0x44,
	SEC_UNIT_S_SUP_R = 0x02,
	SEC_UNIT_S_SUP_W = 0x20,
	SEC_UNIT_S_SUP_RW = 0x22,
	SEC_UNIT_S_USER_R = 0x01,
	SEC_UNIT_S_USER_W = 0x10,
	SEC_UNIT_S_USER_RW = 0x11,
	SEC_UNIT_ALL_RW = 0xff,
};

enum CsuCslxInd {
	SEC_UNIT_CSLX_PCIE2_IO = 0,
	SEC_UNIT_CSLX_PCIE1_IO,
	SEC_UNIT_CSLX_MG2TPR_IP,
	SEC_UNIT_CSLX_IFC_MEM,
	SEC_UNIT_CSLX_OCRAM,
	SEC_UNIT_CSLX_GIC,
	SEC_UNIT_CSLX_PCIE1,
	SEC_UNIT_CSLX_OCRAM2,
	SEC_UNIT_CSLX_QSPI_MEM,
	SEC_UNIT_CSLX_PCIE2,
	SEC_UNIT_CSLX_SATA,
	SEC_UNIT_CSLX_USB1,
	SEC_UNIT_CSLX_QM_BM_SWPORTAL,
	SEC_UNIT_CSLX_PCIE3 = 16,
	SEC_UNIT_CSLX_PCIE3_IO,
	SEC_UNIT_CSLX_USB3 = 20,
	SEC_UNIT_CSLX_USB2,
	SEC_UNIT_CSLX_SERDES = 32,
	SEC_UNIT_CSLX_QDMA,
	SEC_UNIT_CSLX_LPUART2,
	SEC_UNIT_CSLX_LPUART1,
	SEC_UNIT_CSLX_LPUART4,
	SEC_UNIT_CSLX_LPUART3,
	SEC_UNIT_CSLX_LPUART6,
	SEC_UNIT_CSLX_LPUART5,
	SEC_UNIT_CSLX_DSPI1 = 41,
	SEC_UNIT_CSLX_QSPI,
	SEC_UNIT_CSLX_ESDHC,
	SEC_UNIT_CSLX_IFC = 45,
	SEC_UNIT_CSLX_I2C1,
	SEC_UNIT_CSLX_I2C3 = 48,
	SEC_UNIT_CSLX_I2C2,
	SEC_UNIT_CSLX_DUART2 = 50,
	SEC_UNIT_CSLX_DUART1,
	SEC_UNIT_CSLX_WDT2,
	SEC_UNIT_CSLX_WDT1,
	SEC_UNIT_CSLX_EDMA,
	SEC_UNIT_CSLX_SYS_CNT,
	SEC_UNIT_CSLX_DMA_MUX2,
	SEC_UNIT_CSLX_DMA_MUX1,
	SEC_UNIT_CSLX_DDR,
	SEC_UNIT_CSLX_QUICC,
	SEC_UNIT_CSLX_DCFG_CCU_RCPM = 60,
	SEC_UNIT_CSLX_SECURE_BOOTROM,
	SEC_UNIT_CSLX_SFP,
	SEC_UNIT_CSLX_TMU,
	SEC_UNIT_CSLX_SECURE_MONITOR,
	SEC_UNIT_CSLX_SCFG,
	SEC_UNIT_CSLX_FM = 66,
	SEC_UNIT_CSLX_SEC5_5,
	SEC_UNIT_CSLX_BM,
	SEC_UNIT_CSLX_QM,
	SEC_UNIT_CSLX_GPIO2 = 70,
	SEC_UNIT_CSLX_GPIO1,
	SEC_UNIT_CSLX_GPIO4,
	SEC_UNIT_CSLX_GPIO3,
	SEC_UNIT_CSLX_PLATFORM_CONT,
	SEC_UNIT_CSLX_SEC_UNIT,
	SEC_UNIT_CSLX_IIC4 = 77,
	SEC_UNIT_CSLX_WDT4,
	SEC_UNIT_CSLX_WDT3,
	SEC_UNIT_CSLX_WDT5 = 81,
	SEC_UNIT_CSLX_FTM2 = 86,
	SEC_UNIT_CSLX_FTM1,
	SEC_UNIT_CSLX_FTM4,
	SEC_UNIT_CSLX_FTM3,
	SEC_UNIT_CSLX_FTM6 = 90,
	SEC_UNIT_CSLX_FTM5,
	SEC_UNIT_CSLX_FTM8,
	SEC_UNIT_CSLX_FTM7,
	SEC_UNIT_CSLX_DSCR = 121,
};

struct CsuNsDev {
	UINTN Ind;
	UINT32 Val;
};

/* Device Configuration and Pin Control */
struct CcsrGur {
	UINT32     porsr1;         /* POR status 1 */
#define FSL_CHASSIS2_CCSR_PORSR1_RCW_MASK	0xFF800000
	UINT32     porsr2;         /* POR status 2 */
	UINT8      res_008[0x20-0x8];
	UINT32     gpporcr1;       /* General-purpose POR configuration */
	UINT32	   gpporcr2;
	UINT32     dcfg_fusesr;    /* Fuse status register */
	UINT8      res_02c[0x70-0x2c];
	UINT32     devdisr;        /* Device disable control */
#define DEVDISR2_DTSEC1_1	0x80000000
#define DEVDISR2_DTSEC1_2	0x40000000
#define DEVDISR2_DTSEC1_3	0x20000000
#define DEVDISR2_DTSEC1_4	0x10000000
#define DEVDISR2_DTSEC1_5	0x08000000
#define DEVDISR2_DTSEC1_6	0x04000000
#define DEVDISR2_DTSEC1_9	0x00800000
	UINT32     devdisr2;       /* Device disable control 2 */
	UINT32     devdisr3;       /* Device disable control 3 */
	UINT32     devdisr4;       /* Device disable control 4 */
	UINT32     devdisr5;       /* Device disable control 5 */
	UINT32     devdisr6;       /* Device disable control 6 */
	UINT32     devdisr7;       /* Device disable control 7 */
	UINT8      res_08c[0x94-0x8c];
	UINT32     coredisru;      /* uppper portion for support of 64 cores */
	UINT32     coredisrl;      /* lower portion for support of 64 cores */
	UINT8      res_09c[0xa0-0x9c];
	UINT32     pvr;            /* Processor version */
	UINT32     svr;            /* System version */
	UINT32     mvr;            /* Manufacturing version */
	UINT8	res_0ac[0xb0-0xac];
	UINT32	rstcr;		/* Reset control */
	UINT32	rstrqpblsr;	/* Reset request preboot loader status */
	UINT8	res_0b8[0xc0-0xb8];
	UINT32	rstrqmr1;	/* Reset request mask */
	UINT8	res_0c4[0xc8-0xc4];
	UINT32	rstrqsr1;	/* Reset request status */
	UINT8	res_0cc[0xd4-0xcc];
	UINT32	rstrqwdtmrl;	/* Reset request WDT mask */
	UINT8	res_0d8[0xdc-0xd8];
	UINT32	rstrqwdtsrl;	/* Reset request WDT status */
	UINT8	res_0e0[0xe4-0xe0];
	UINT32	brrl;		/* Boot release */
	UINT8      res_0e8[0x100-0xe8];
	UINT32     rcwsr[16];      /* Reset control word status */
#define FSL_CHASSIS2_RCWSR0_SYS_PLL_RAT_SHIFT	25
#define FSL_CHASSIS2_RCWSR0_SYS_PLL_RAT_MASK	0x1f
#define FSL_CHASSIS2_RCWSR0_MEM_PLL_RAT_SHIFT	16
#define FSL_CHASSIS2_RCWSR0_MEM_PLL_RAT_MASK	0x3f
#define FSL_CHASSIS2_RCWSR4_SRDS1_PRTCL_MASK	0xffff0000
#define FSL_CHASSIS2_RCWSR4_SRDS1_PRTCL_SHIFT	16
	UINT8      res_140[0x200-0x140];
	UINT32     scratchrw[4];  /* Scratch Read/Write */
	UINT8      res_210[0x300-0x210];
	UINT32     scratchw1r[4];  /* Scratch Read (Write once) */
	UINT8      res_310[0x400-0x310];
	UINT32	crstsr[12];
	UINT8	res_430[0x500-0x430];

	/* PCI Express n Logical I/O Device Number register */
	UINT32 dcfg_ccsr_pex1liodnr;
	UINT32 dcfg_ccsr_pex2liodnr;
	UINT32 dcfg_ccsr_pex3liodnr;
	UINT32 dcfg_ccsr_pex4liodnr;
	/* RIO n Logical I/O Device Number register */
	UINT32 dcfg_ccsr_rio1liodnr;
	UINT32 dcfg_ccsr_rio2liodnr;
	UINT32 dcfg_ccsr_rio3liodnr;
	UINT32 dcfg_ccsr_rio4liodnr;
	/* USB Logical I/O Device Number register */
	UINT32 dcfg_ccsr_usb1liodnr;
	UINT32 dcfg_ccsr_usb2liodnr;
	UINT32 dcfg_ccsr_usb3liodnr;
	UINT32 dcfg_ccsr_usb4liodnr;
	/* SD/MMC Logical I/O Device Number register */
	UINT32 dcfg_ccsr_sdmmc1liodnr;
	UINT32 dcfg_ccsr_sdmmc2liodnr;
	UINT32 dcfg_ccsr_sdmmc3liodnr;
	UINT32 dcfg_ccsr_sdmmc4liodnr;
	/* RIO Message Unit Logical I/O Device Number register */
	UINT32 dcfg_ccsr_riomaintliodnr;

	UINT8 res_544[0x550-0x544];
	UINT32 sataliodnr[4];
	UINT8 res_560[0x570-0x560];

	UINT32 dcfg_ccsr_misc1liodnr;
	UINT32 dcfg_ccsr_misc2liodnr;
	UINT32 dcfg_ccsr_misc3liodnr;
	UINT32 dcfg_ccsr_misc4liodnr;
	UINT32 dcfg_ccsr_dma1liodnr;
	UINT32 dcfg_ccsr_dma2liodnr;
	UINT32 dcfg_ccsr_dma3liodnr;
	UINT32 dcfg_ccsr_dma4liodnr;
	UINT32 dcfg_ccsr_spare1liodnr;
	UINT32 dcfg_ccsr_spare2liodnr;
	UINT32 dcfg_ccsr_spare3liodnr;
	UINT32 dcfg_ccsr_spare4liodnr;
	UINT8	res_5a0[0x600-0x5a0];
	UINT32 dcfg_ccsr_pblsr;

	UINT32	pamubypenr;
	UINT32	dmacr1;

	UINT8	res_60c[0x610-0x60c];
	UINT32 dcfg_ccsr_gensr1;
	UINT32 dcfg_ccsr_gensr2;
	UINT32 dcfg_ccsr_gensr3;
	UINT32 dcfg_ccsr_gensr4;
	UINT32 dcfg_ccsr_gencr1;
	UINT32 dcfg_ccsr_gencr2;
	UINT32 dcfg_ccsr_gencr3;
	UINT32 dcfg_ccsr_gencr4;
	UINT32 dcfg_ccsr_gencr5;
	UINT32 dcfg_ccsr_gencr6;
	UINT32 dcfg_ccsr_gencr7;
	UINT8 res_63c[0x658-0x63c];
	UINT32 dcfg_ccsr_cgensr1;
	UINT32 dcfg_ccsr_cgensr0;
	UINT8 res_660[0x678-0x660];
	UINT32 dcfg_ccsr_cgencr1;

	UINT32 dcfg_ccsr_cgencr0;
	UINT8 res_680[0x700-0x680];
	UINT32 dcfg_ccsr_sriopstecr;
	UINT32 dcfg_ccsr_dcsrcr;

	UINT8 res_708[0x740-0x708]; /* add more registers when needed */
	UINT32 tp_ityp[64]; /* Topology Initiator Type Register */
	struct {
		UINT32 upper;
		UINT32 lower;
	} tp_cluster[16];
	UINT8 res_8c0[0xa00-0x8c0]; /* add more registers when needed */
	UINT32 dcfg_ccsr_qmbm_warmrst;
	UINT8 res_a04[0xa20-0xa04]; /* add more registers when needed */
	UINT32 dcfg_ccsr_reserved0;
	UINT32 dcfg_ccsr_reserved1;
};

/* Supplemental Configuration Unit */
struct CcsrScfg {
	UINT8 res_000[0x070-0x000];
	UINT32 usb1prm1cr;
	UINT32 usb1prm2cr;
	UINT32 usb1prm3cr;
	UINT32 usb2prm1cr;
	UINT32 usb2prm2cr;
	UINT32 usb2prm3cr;
	UINT32 usb3prm1cr;
	UINT32 usb3prm2cr;
	UINT32 usb3prm3cr;
	UINT8 res_094[0x100-0x094];
	UINT32 usb2_icid;
	UINT32 usb3_icid;
	UINT8 res_108[0x114-0x108];
	UINT32 dma_icid;
	UINT32 sata_icid;
	UINT32 usb1_icid;
	UINT32 qe_icid;
	UINT32 sdhc_icid;
	UINT32 edma_icid;
	UINT32 etr_icid;
	UINT32 core0_sft_rst;
	UINT32 core1_sft_rst;
	UINT32 core2_sft_rst;
	UINT32 core3_sft_rst;
	UINT8 res_140[0x158-0x140];
	UINT32 altcbar;
	UINT32 qspi_cfg;
	UINT8 res_160[0x180-0x160];
	UINT32 dmamcr;
	UINT8 res_184[0x188-0x184];
	UINT32 gic_align;
	UINT32 debug_icid;
	UINT8 res_190[0x1a4-0x190];
	UINT32 snpcnfgcr;
#define CCSR_SCFG_SNPCNFGCR_SECRDSNP         BIT31
#define CCSR_SCFG_SNPCNFGCR_SECWRSNP         BIT30
#define CCSR_SCFG_SNPCNFGCR_SATARDSNP        BIT23
#define CCSR_SCFG_SNPCNFGCR_SATAWRSNP        BIT22
#define CCSR_SCFG_SNPCNFGCR_USB1RDSNP        BIT21
#define CCSR_SCFG_SNPCNFGCR_USB1WRSNP        BIT20
#define CCSR_SCFG_SNPCNFGCR_USB2RDSNP        BIT15
#define CCSR_SCFG_SNPCNFGCR_USB2WRSNP        BIT16
#define CCSR_SCFG_SNPCNFGCR_USB3RDSNP        BIT13
#define CCSR_SCFG_SNPCNFGCR_USB3WRSNP        BIT14
	UINT8 res_1a8[0x1ac-0x1a8];
	UINT32 intpcr;
	UINT8 res_1b0[0x204-0x1b0];
	UINT32 coresrencr;
	UINT8 res_208[0x220-0x208];
	UINT32 rvbar0_0;
	UINT32 rvbar0_1;
	UINT32 rvbar1_0;
	UINT32 rvbar1_1;
	UINT32 rvbar2_0;
	UINT32 rvbar2_1;
	UINT32 rvbar3_0;
	UINT32 rvbar3_1;
	UINT32 lpmcsr;
	UINT8 res_244[0x400-0x244];
	UINT32 qspidqscr;
	UINT32 ecgtxcmcr;
	UINT32 sdhciovselcr;
	UINT32 rcwpmuxcr0;
	UINT32 usbdrvvbus_selcr;
#define CCSR_SCFG_USBDRVVBUS_SELCR_USB1      0x00000000
#define CCSR_SCFG_USBDRVVBUS_SELCR_USB2      0x00000001
#define CCSR_SCFG_USBDRVVBUS_SELCR_USB3      0x00000003
	UINT32 usbpwrfault_selcr;
#define CCSR_SCFG_USBPWRFAULT_INACTIVE       0x00000000
#define CCSR_SCFG_USBPWRFAULT_SHARED         0x00000001
#define CCSR_SCFG_USBPWRFAULT_DEDICATED      0x00000002
#define CCSR_SCFG_USBPWRFAULT_USB3_SHIFT     4
#define CCSR_SCFG_USBPWRFAULT_USB2_SHIFT     2
#define CCSR_SCFG_USBPWRFAULT_USB1_SHIFT     0
	UINT32 usb_refclk_selcr1;
	UINT32 usb_refclk_selcr2;
	UINT32 usb_refclk_selcr3;
	UINT8 res_424[0x600-0x424];
	UINT32 scratchrw[4];
	UINT8 res_610[0x680-0x610];
	UINT32 corebcr;
	UINT8 res_684[0x1000-0x684];
	UINT32 pex1msiir;
	UINT32 pex1msir;
	UINT8 res_1008[0x2000-0x1008];
	UINT32 pex2;
	UINT32 pex2msir;
	UINT8 res_2008[0x3000-0x2008];
	UINT32 pex3msiir;
	UINT32 pex3msir;
};

#define USB_TXVREFTUNE        0x9
#define USB_SQRXTUNE          0xFC7FFFFF
#define USB_PCSTXSWINGFULL    0x47
/* TODO : make it generic */
#define USB_PHY_RX_EQ_VAL_1   0x0000
#define USB_PHY_RX_EQ_VAL_2   0x8000
#define USB_PHY_RX_EQ_VAL_3   0x8003
#define USB_PHY_RX_EQ_VAL_4   0x800b

/*USB_PHY_SS memory map*/
struct CcsrUsbPhySs {
  UINT16 IpIdcodeLo;
  UINT16 SupIdcodeHi;
  UINT8  Res4[0x0006-0x0004];
  UINT16 RtuneDebug;
  UINT16 RtuneStat;
  UINT16 SupSsPhase;
  UINT16 SsFreq;
  UINT8  ResE[0x0020-0x000e];
  UINT16 Ateovrd;
  UINT16 MpllOvrdInLo;
  UINT8  Res24[0x0026-0x0024];
  UINT16 SscOvrdIn;
  UINT8  Res28[0x002A-0x0028];
  UINT16 LevelOvrdIn;
  UINT8  Res2C[0x0044-0x002C];
  UINT16 ScopeCount;
  UINT8  Res46[0x0060-0x0046];
  UINT16 MpllLoopCtl;
  UINT8  Res62[0x006C-0x0062];
  UINT16 SscClkCntrl;
  UINT8  Res6E[0x2002-0x006E];
  UINT16 Lane0TxOvrdInHi;
  UINT16 Lane0TxOvrdDrvLo;
  UINT8  Res2006[0x200C-0x2006];
  UINT16 Lane0RxOvrdInHi;
  UINT8  Res200E[0x2022-0x200E];
  UINT16 Lane0TxCmWaitTimeOvrd;
  UINT8  Res2024[0x202A-0x2024];
  UINT16 Lane0TxLbertCtl;
  UINT16 Lane0RxLbertCtl;
  UINT16 Lane0RxLbertErr;
  UINT8  Res2030[0x205A-0x2030];
  UINT16 Lane0TxAltBlock;
};

/* Clocking */
struct CcsrClk {
	struct {
		UINT32 clkcncsr;	/* core cluster n clock control status */
		UINT8 res_004[0x0c];
		UINT32 clkcghwacsr; /* Clock generator n hardware accelerator */
		UINT8 res_014[0x0c];
	} clkcsr[4];
	UINT8	res_040[0x780]; /* 0x100 */
	struct {
		UINT32 pllcngsr;
		UINT8 res_804[0x1c];
	} pllcgsr[2];
	UINT8	res_840[0x1c0];
	UINT32	clkpcsr;	/* 0xa00 Platform clock domain control/status */
	UINT8	res_a04[0x1fc];
	UINT32	pllpgsr;	/* 0xc00 Platform PLL General Status */
	UINT8	res_c04[0x1c];
	UINT32	plldgsr;	/* 0xc20 DDR PLL General Status */
	UINT8	res_c24[0x3dc];
};

#define CCI400_CTRLORD_TERM_BARRIER	0x00000008
#define CCI400_CTRLORD_EN_BARRIER	0
#define CCI400_SHAORD_NON_SHAREABLE	0x00000002
#define CCI400_DVM_MESSAGE_REQ_EN	0x00000002
#define CCI400_SNOOP_REQ_EN		0x00000001

/* CCI-400 registers */
struct CcsrCci400 {
	UINT32 ctrl_ord;			/* Control Override */
	UINT32 spec_ctrl;			/* Speculation Control */
	UINT32 secure_access;		/* Secure Access */
	UINT32 status;			/* Status */
	UINT32 impr_err;			/* Imprecise Error */
	UINT8 res_14[0x100 - 0x14];
	UINT32 pmcr;			/* Performance Monitor Control */
	UINT8 res_104[0xfd0 - 0x104];
	UINT32 pid[8];			/* Peripheral ID */
	UINT32 cid[4];			/* Component ID */
	struct {
		UINT32 snoop_ctrl;		/* Snoop Control */
		UINT32 sha_ord;		/* Shareable Override */
		UINT8 res_1008[0x1100 - 0x1008];
		UINT32 rc_qos_ord;		/* read channel QoS Value Override */
		UINT32 wc_qos_ord;		/* read channel QoS Value Override */
		UINT8 res_1108[0x110c - 0x1108];
		UINT32 qos_ctrl;		/* QoS Control */
		UINT32 max_ot;		/* Max OT */
		UINT8 res_1114[0x1130 - 0x1114];
		UINT32 target_lat;		/* Target Latency */
		UINT32 latency_regu;	/* Latency Regulation */
		UINT32 qos_range;		/* QoS Range */
		UINT8 res_113c[0x2000 - 0x113c];
	} slave[5];			/* Slave Interface */
	UINT8 res_6000[0x9004 - 0x6000];
	UINT32 cycle_counter;		/* Cycle counter */
	UINT32 count_ctrl;			/* Count Control */
	UINT32 overflow_status;		/* Overflow Flag Status */
	UINT8 res_9010[0xa000 - 0x9010];
	struct {
		UINT32 event_select;	/* Event Select */
		UINT32 event_count;	/* Event Count */
		UINT32 counter_ctrl;	/* Counter Control */
		UINT32 overflow_status;	/* Overflow Flag Status */
		UINT8 res_a010[0xb000 - 0xa010];
	} pcounter[4];			/* Performance Counter */
	UINT8 res_e004[0x10000 - 0xe004];
};

VOID EnableDevicesNsAccess(struct CsuNsDev *NonSecureDevices, UINT32 Num);

VOID GetSysInfo(struct SysInfo *PtrSysInfo);

VOID SocInit(VOID);

VOID FdtCpuSetup(VOID *Blob, UINTN BlobSize);

INTN TimerInit (VOID);

#endif /* __LS1043A_SOC_H__ */
