/** SoCLib.h
*  Header defining the LSCH3 SoC specific constants (Base addresses, sizes, flags)
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

#ifndef __LS2088A_SOC_H__
#define __LS2088A_SOC_H__

#define NUM_CC_PLLS			6
#define HWA_CGA_M1_CLK_SEL		0xe0000000
#define HWA_CGA_M1_CLK_SHIFT		29
#define HWA_CGA_M2_CLK_SEL		0x00000007
#define HWA_CGA_M2_CLK_SHIFT		0

#define TP_ITYP_AV_MASK			0x00000001 /* Check Initiator availability */
#define TP_ITYP_TYPE_MASK(x)		(((x) & 0x6) >> 1) /* Check Initiator Type */
#define TP_ITYP_TYPE_ARM		0x0     /* Type ARM */
#define TP_ITYP_TYPE_POWERPC		0x1	/* Type PowerPC */
#define TP_ITYP_TYPE_OTHER		0x2	/* Type StarCore DSP */
#define TP_ITYP_TYPE_HW_ACC		0x3	/* Type HW Accelerator */
#define TP_ITYP_THREADS(x)		(((x) & 0x18) >> 3)
#define TP_ITYP_VERSION(x)		(((x) & 0xe0) >> 5)	/* Check Initiator Version */
#define TY_ITYP_VERSION_A7		0x1
#define TY_ITYP_VERSION_A53		0x2
#define TY_ITYP_VERSION_A57		0x3
#define TY_ITYP_VERSION_A72		0x4

#define TP_CLUSTER_EOC_MASK     0x80000000      /* Mask for End of clusters */
#define TP_CLUSTER_INITR_MASK	0x0000003f	/* Cluster initiator mask */
#define TP_INIT_PER_CLUSTER	4

#define FSL_CLUSTER_CLOCKS		{ 1, 1, 4, 4 } /* LS208x */

#define MAX_CPUS			16

//#define ERRATUM_A008336
#define ERRATUM_A008511
//#define ERRATUM_A008514
#define ERRATUM_A008585
#define ERRATUM_A008751
#define ERRATUM_A009635
#define ERRATUM_A009663
#define ERRATUM_A009942
#define ERRATUM_A009803
#define ERRATUM_A009801
#define ERRATUM_A010165
#define ERRATUM_A009008
#define ERRATUM_A009798

/* Defines for selecting Muxed device */
enum {
       MUX_SDXC,
       MUX_DSPI,
};

#define PIN_SEL_SDXC_MUX    0x00
#define PIN_SEL_DSPI_MUX    0x0a

#define SET_MUX_SEL(Reg, Value)      ((Reg & 0xf0) | Value)

/*
 * On LS2088 stream IDs are programmed in AMQ registers (32-bits) for
 * each of the different bus masters.  The relationship between
 * the AMQ registers and stream IDs is defined in the table below:
 *          AMQ bit    streamID bit
 *      ---------------------------
 *           PL[18]         9
 *          BMT[17]         8
 *           VA[16]         7
 *             [15]         -
 *         ICID[14:7]       -
 *         ICID[6:0]        6-0
 *     ----------------------------
 */
#define AMQ_PL_MASK			(0x1 << 18)   /* privilege bit */
#define AMQ_BMT_MASK			(0x1 << 17)   /* bypass bit */
#define BYPASS_AMQ_MASK			(AMQ_PL_MASK | AMQ_BMT_MASK)

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
	UINTN FreqProcessor[MAX_CPUS];
	UINTN FreqSystemBus;
	UINTN FreqDdrBus;
	UINTN FreqDdrBus2;
	UINTN FreqLocalBus;
	UINTN FreqSdhc;
	UINTN FreqQman;
	UINTN FreqPme;
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
	SEC_UNIT_CSLX_DSCR = 120,
};

struct CsuNsDev {
	UINTN Ind;
	UINT32 Val;
};

/* Device Configuration and Pin Control */
struct CcsrGur {
	UINT32     Por_Sr1;         /* POR status register 1 */
	UINT32     Por_Sr2;         /* POR status register 2 */
	UINT8      Reserve_008[0x20-0x8];
	UINT32     Gppor_Cr1;       /* General-purpose POR configuration register */
        UINT32     Gppor_Cr2;       /* General-purpose POR configuration register 2 */
        UINT32     DcfgFuseSr;    /* Fuse status register */
        UINT32     Gppor_Cr3;
        UINT32     Gppor_Cr4;
        UINT8      Reserve_034[0x70-0x34];
	UINT32     DevDisr;        /* Device disable control register */
	UINT32     DevDisr2;       /* Device disable control register 2 */
	UINT32     DevDisr3;       /* Device disable control register 3 */
	UINT32     DevDisr4;       /* Device disable control register 4 */
	UINT32     DevDisr5;       /* Device disable control register 5 */
	UINT32     DevDisr6;       /* Device disable control register 6 */
	UINT32     DevDisr7;       /* Device disable control register 7 */
	UINT8      Reserve_08c[0x90-0x8c];
	UINT32     CoreDisrUpper;      /* CORE DISR Uppper for support of 64 cores */
	UINT32     CoreDisrLower;      /* CORE DISR Lower for support of 64 cores */
	UINT8      Reserve_098[0xa0-0x98];
	UINT32     Pvr;            /* Processor version */
	UINT32     Svr;            /* System version */
	UINT32     Mvr;            /* Manufacturing version */
	UINT8	   Reserve_0ac[0x100-0xac];
	UINT32     Rcw_Sr[32];      /* Reset control word status */
#define FSL_CHASSIS3_RCWSR_0_SYS_PLL_RAT_SHIFT   2
#define FSL_CHASSIS3_RCWSR_0_SYS_PLL_RAT_MASK    0x1f
#define FSL_CHASSIS3_RCWSR_0_MEM_PLL_RAT_SHIFT   10
#define FSL_CHASSIS3_RCWSR_0_MEM_PLL_RAT_MASK    0x3f
#define FSL_CHASSIS3_RCWSR_0_MEM2_PLL_RAT_SHIFT  18
#define FSL_CHASSIS3_RCWSR_0_MEM2_PLL_RAT_MASK   0x3f
#define FSL_CHASSIS3_RCWSR_29_INDEX		 28
#define FSL_CHASSIS3_RCWSR_29_SERDES1_PRTCL_WIDTH  8
#define FSL_CHASSIS3_RCWSR_29_SERDES1_PRTCL_SHIFT  16
#define FSL_CHASSIS3_RCWSR_29_SERDES2_PRTCL_WIDTH  8
#define FSL_CHASSIS3_RCWSR_29_SERDES2_PRTCL_SHIFT  24
        UINT8      Reserve_180[0x200-0x180];
        UINT32     Scratch_Rw[32];  /* Scratch Read/Write */
        UINT8      Reserve_280[0x300-0x280];
        UINT32     Scratch_W1r[4];  /* Scratch Read (Write once) */
        UINT8      Reserve_310[0x400-0x310];
        UINT32     Boot_Loc_PtrL;    /* Low addr : Boot location pointer */
        UINT32     Boot_Loc_PtrH;    /* High addr : Boot location pointer */
        UINT8      Reserve_408[0x500-0x408];
        UINT8      Reserve_500[0x740-0x500];
        UINT32     TpItyp[64];
        struct {
                UINT32     Upper;
                UINT32     Lower;
        } TpCluster[3];
        UINT8      Reserve_858[0x1000-0x858];
};

struct CcsrReset {
        UINT32 rstcr;                      /* 0x000 */
        UINT32 rstcrsp;                    /* 0x004 */
        UINT8 res_008[0x10-0x08];          /* 0x008 */
        UINT32 rstrqmr1;                   /* 0x010 */
        UINT32 rstrqmr2;                   /* 0x014 */
        UINT32 rstrqsr1;                   /* 0x018 */
        UINT32 rstrqsr2;                   /* 0x01c */
        UINT32 rstrqwdtmrl;                /* 0x020 */
        UINT32 rstrqwdtmru;                /* 0x024 */
        UINT8 res_028[0x30-0x28];          /* 0x028 */
        UINT32 rstrqwdtsrl;                /* 0x030 */
        UINT32 rstrqwdtsru;                /* 0x034 */
        UINT8 res_038[0x60-0x38];          /* 0x038 */
        UINT32 brrl;                       /* 0x060 */
        UINT32 brru;                       /* 0x064 */
        UINT8 res_068[0x80-0x68];          /* 0x068 */
        UINT32 pirset;                     /* 0x080 */
        UINT32 pirclr;                     /* 0x084 */
        UINT8 res_088[0x90-0x88];          /* 0x088 */
        UINT32 brcorenbr;                  /* 0x090 */
        UINT8 res_094[0x100-0x94];         /* 0x094 */
        UINT32 rcw_reqr;                   /* 0x100 */
        UINT32 rcw_completion;             /* 0x104 */
        UINT8 res_108[0x110-0x108];        /* 0x108 */
        UINT32 pbi_reqr;                   /* 0x110 */
        UINT32 pbi_completion;             /* 0x114 */
        UINT8 res_118[0xa00-0x118];        /* 0x118 */
        UINT32 qmbm_warmrst;               /* 0xa00 */
        UINT32 soc_warmrst;                /* 0xa04 */
	UINT8 res_a08[0xbf8-0xa08];        /* 0xa08 */
        UINT32 ip_rev1;                    /* 0xbf8 */
        UINT32 ip_rev2;                    /* 0xbfc */
};

struct CcsrClkClusterGroup {
        struct {
                UINT8      res_00[0x10];
                UINT32     csr;
                UINT8      res_14[0x20-0x14];
        } hwncsr[3];
        UINT8      res_60[0x80-0x60];
        struct {
                UINT32     gsr;
                UINT8      res_84[0xa0-0x84];
        } pllngsr[3];
        UINT8      res_e0[0x100-0xe0];
};

/* Clocking */
struct CcsrClkCtrl {
        struct {
                UINT32 csr;        /* core cluster n clock control status */
                UINT8  res_04[0x20-0x04];
        } clkcncsr[8];
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

struct CpuType {
	char name[16];
	UINT32 soc_ver;
	UINT32 num_cores;
};

#define CPU_TYPE_ENTRY(n, v, nc) \
	{ .name = #n, .soc_ver = SVR_##v, .num_cores = (nc)}

#define SVR_WO_E		0xFFFFFE
#define SVR_LS2045		0x870120
#define SVR_LS2080		0x870110
#define SVR_LS2085		0x870100
#define SVR_LS2088		0x870901
#define SVR_LS2040          0x870130

#define SVR_MAJ(svr)		(((svr) >> 4) & 0xf)
#define SVR_MIN(svr)		(((svr) >> 0) & 0xf)
#define SVR_SOC_VER(svr)	(((svr) >> 8) & SVR_WO_E)
#define IS_E_PROCESSOR(svr)	(!((svr >> 8) & 0x1))

VOID EnableDevicesNsAccess(struct CsuNsDev *NonSecureDevices, UINT32 Num);

VOID GetSysInfo(struct SysInfo *PtrSysInfo);

VOID SocInit(VOID);

VOID FdtCpuSetup(VOID *Blob, UINTN BlobSize);

VOID DebugDumpFdt(VOID* FdtBlob);

INT32 QoriqCoreToCluster(UINT32 Core);

UINT32 CpuMask(VOID);

UINTN CpuNumCores(VOID);

VOID PrintCpuInfo (VOID);

INT32
SelectMuxDevice (
  IN  UINT8  Type
  );

INTN
TimerInit (
  VOID
  );

VOID CleanDcacheRange(UINT64 StartAddr, UINT64 EndAddr);

VOID CleanAllDcacheLevels(VOID);

#endif /* __LS2088A_SOC_H__ */
