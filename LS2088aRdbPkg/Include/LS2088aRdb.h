/** LS2088aRdb.h
*  Header defining the LS2088aRdb constants (Base addresses, sizes, flags)
*
*  Copyright 2015-2016, Freescale semiconductor, Inc.
*  Copyright 2017 NXP
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

#ifndef __LS2088aRDB_PLATFORM_H__
#define __LS2088aRDB_PLATFORM_H__

#include "Common.h"
#include <LS2088aSocLib.h>

// IS_XIP
extern UINTN mSystemMemoryEnd;

#define IS_XIP() (((UINT32)FixedPcdGet32 (PcdFdBaseAddress) > mSystemMemoryEnd) || \
                   ((FixedPcdGet32 (PcdFdBaseAddress) + FixedPcdGet32 (PcdFdSize)) < FixedPcdGet64 (PcdSystemMemoryBase)))

// DDR attributes
#define DDR_ATTRIBUTES_CACHED                ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK
#define DDR_ATTRIBUTES_UNCACHED              ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED

#define CACHELINE_SIZE				64

#define SECURE_BOOTROM_BASE_ADDR	0x00000000
#define SECURE_BOOTROM_SIZE		0x00100000 /* 1MB */
#define EXT_BOOTROM_BASE_ADDR	0x00100000
#define EXT_BOOTROM_SIZE		0x00F00000 /* 15MB */
#define CCSR_BASE_ADDR		0x01000000
#define CCSR_SIZE			0x0F000000 /* 240MB */
#define DDR_CTRL1_ADDR             0x01080000
#define DDR_CTRL2_ADDR             0x01090000
#define DDR_CTRL3_ADDR             0x08210000
	#define ADDR_IMMR			0x01000000
	#define SDXC_BASE_ADDR		0x02140000

	#define CONFIG_SYS_FSL_GUTS_ADDR	(ADDR_IMMR + 0x00E00000)
	#define FSL_CLK_CTRL_ADDR		(ADDR_IMMR + 0x00370000)
	#define FSL_CLK_GRPA_ADDR		(ADDR_IMMR + 0x00300000)
	#define FSL_CLK_GRPB_ADDR		(ADDR_IMMR + 0x00310000)

// DCSR
#define CONFIG_SYS_FSL_DCSR_DDR_ADDR            0x70012c000ULL
#define CONFIG_SYS_FSL_DCSR_DDR2_ADDR           0x70012d000ULL
#define CONFIG_SYS_FSL_DCSR_DDR3_ADDR           0x700132000ULL
#define CONFIG_SYS_FSL_DCSR_DDR4_ADDR           0x700133000ULL

	/* Generic Interrupt Controller Definitions */
	#define GICD_BASE			0x06000000
	#define GICR_BASE			0x06100000

	#define DUART1_BASE_ADDR		0x21C0000
	#define DUART1_SIZE			0x1000
	#define DUART2_BASE_ADDR		0x21D0000
	#define DUART2_SIZE			0x1000

	#define WDOG1_BASE_ADDR            0x0C000000
       #define WDOG2_BASE_ADDR            0x0C010000
       #define WDOG3_BASE_ADDR            0x0C100000
       #define WDOG4_BASE_ADDR            0x0C110000
       #define WDOG5_BASE_ADDR            0x0C200000
       #define WDOG6_BASE_ADDR            0x0C210000
       #define WDOG7_BASE_ADDR            0x0C300000
       #define WDOG8_BASE_ADDR            0x0C310000
       #define WDOG_SIZE                  0x1000
		#define WDOG1_PMU_BASE_ADDR     	0x1E30000
		#define FSL_PMU_REG_SIZE        	0xFFFF
		#define FSL_PMU_PCTBENR_OFFSET  	0x8A0
		#define PMU_PCTBEN0_WDT_BIT_MASK   0x000000FF

	#define I2C1_BASE_ADDRESS		0x02000000
	#define I2C2_BASE_ADDRESS		0x02010000
	#define I2C3_BASE_ADDRESS		0x02020000
	#define I2C4_BASE_ADDRESS		0x02030000
	#define I2C_SIZE			0x10000
	#define DDRC_MEMORY_SIZE		0x10000
	#define SDXC_MEMORY_SIZE		0x10000
	#define TIMER_BASE_ADDR		0x023e0000
	#define IFC_REG_BASE_ADDR          0x2240000
	#define IFC_REG_SIZE               0x0010000
	#define SCFG_BASE_ADDR             0x1FC0000
	#define SCFG_SIZE                  0x0010000
	#define DSPI_BASE_ADDR             0x2100000
	#define QSPI_BASE_ADDR             0x20C0000
	#define PMU_BASE_ADDR		0x01E30000
	#define TIME_BASE_ENABLE      	(PMU_BASE_ADDR + 0x18A0)

/* Supplemental Configuration */
#define SCFG_USB3PRM1CR                   0x000
#define SCFG_USB3PRM2CR                   0x004
#define SCFG_USB3PRM1CR_INIT              0x27672b2a
#define USB_TXVREFTUNE                    0x9
#define USB_SQRXTUNE               0xFC7FFFFF
#define USB_PCSTXSWINGFULL         0x47
#define DCSR_BASE           0x700000000ULL
#define DCSR_USB_PHY1                     0x4600000
#define DCSR_USB_PHY2                     0x4610000
#define DCSR_USB_PHY_RX_OVRD_IN_HI 0x1006
#define USB_PHY_RX_EQ_VAL_1        0x0000
#define USB_PHY_RX_EQ_VAL_2        0x0080
#define USB_PHY_RX_EQ_VAL_3        0x0380
#define USB_PHY_RX_EQ_VAL_4        0x0b80
#define SCFG_QSPICLKCTLR           0x10

/* TrustZone Protection Controller Definitions */
#define TZPC_BASE_ADDR                               0x02200000
#define TZPC_DCFG_RESET_BIT_MASK		0x08
/* SMMU Defintions */
#define SMMU_BASE_ADDR		0x05000000
#define SMMU_REG_SCR0		(SMMU_BASE_ADDR + 0x0)
#define SMMU_REG_SACR		(SMMU_BASE_ADDR + 0x10)
#define SMMU_REG_IDR1		(SMMU_BASE_ADDR + 0x24)
#define SMMU_REG_NSCR0		(SMMU_BASE_ADDR + 0x400)
#define SMMU_REG_NSACR		(SMMU_BASE_ADDR + 0x410)

#define SCR0_USFCFG_MASK		0x00000400
#define SCR0_CLIENTPD_MASK		0x00000001
#define SACR_PAGESIZE_MASK		0x00010000
#define IDR1_PAGESIZE_MASK		0x80000000

#define OCRAM_BASE_ADDR     0x18000000      /* initial RAM */
#define OCRAM_SIZE          0x00200000      /* 2M */

/* Device Configuration */
#define DCFG_BASE               0x01e00000
#define DCFG_PORSR1                     0x000
#define DCFG_PORSR1_RCW_SRC             0xff800000
#define DCFG_PORSR1_RCW_SRC_NOR         0x12f00000
#define DCFG_RCWSR13                    0x130
#define DCFG_RCWSR13_DSPI               (0 << 8)
#define DCFG_RCWSR15                    0x138
#define DCFG_RCWSR15_IFCGRPABASE_QSPI   0x3

#define CONFIG_STM_BASE_ADDR			0x11000000
#define CONFIG_STM_SIZE				0x01000000 /* 16MB */
#define CONFIG_DCSR_BASE_ADDR			0x700000000ULL
#define CONFIG_DCSR_SIZE			0x40000000 /* 1GB */
#define IFC_REGION1_BASE_ADDR		0x30000000
#define IFC_REGION1_BASE_SIZE		0x10000000 /* 256MB */
#define QSPI_REGION1_BASE_ADDR		0x20000000
#define QSPI_REGION1_BASE_SIZE		0x10000000 /* 256MB */
#define QSPI_REGION2_BASE_ADDR		0x400000000ULL
#define QSPI_REGION2_BASE_SIZE		0x10000000ULL /* 256MB */

/* To accommodate bigger NOR flash and other devices, we will map IFC
 * chip selects to as below:
 * 0x5_1000_0000..0x5_1fff_ffff	Memory Hole
 * 0x5_2000_0000..0x5_3fff_ffff	IFC CSx (FPGA, NAND and others 512MB)
 * 0x5_4000_0000..0x5_7fff_ffff	ASIC or others 1GB
 * 0x5_8000_0000..0x5_bfff_ffff	IFC CS0 1GB (NOR/Promjet)
 * 0x5_C000_0000..0x5_ffff_ffff	IFC CS1 1GB (NOR/Promjet)
 *
 * For e.g. NOR flash at CS0 will be mapped to 0x580000000 after relocation.
 */
#define IFC_REGION2_BASE_ADDR		0x510000000ULL
#define IFC_REGION2_BASE_SIZE		0xF0000000ULL /* 3.75GB */

#undef  DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM
#undef DPAA2_MC_IN_LOW_MEM

#ifdef DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM
  /* MC firmware private memory dynamically allocated */
# define DRAM1_BASE_ADDR		0x0080000000
# define DRAM1_SIZE			0x0080000000    /* 2GB */
# define DDR_MEM_SIZE			0x380000000ULL  /* 14GB */
#else
  /* MC firmware private memory statically allocated */
#  define MC_LOW_MEM_FIXED_SIZE 0x20000000
#  define DDR_TOTAL_SIZE        0x380000000ULL  // 14GB
# ifdef DPAA2_MC_IN_LOW_MEM
   /* MC firmware private memory uses the first 512MB of DRAM1 */
#  define DRAM1_BASE_ADDR		0x00A0000000    /* Actual base address + 512MB */
#  define DRAM1_SIZE        0x0060000000    /* 2GB - 512MB (same as PcdSystemMemorySize) */
#  define DDR_MEM_SIZE      DDR_TOTAL_SIZE

   /* For MC LOW_MEM support, MC region is fixed size of 512MB reserved before DRAM1_BASE_ADDR, that
    * comes out to be (DRAM1_BASE_ADDR - MC_LOW_MEM_FIXED_SIZE)
    */
#  define MC_ADDR   (DRAM1_BASE_ADDR - MC_LOW_MEM_FIXED_SIZE) // last 512 MB block required for MC
# else
   /* MC firmware private memory uses the last 512MB of DRAM2 */
#  define DRAM1_BASE_ADDR   0x0080000000
#  define DRAM1_SIZE        0x0080000000    /* 2GB */
#  define DDR_MEM_SIZE      (DDR_TOTAL_SIZE- PcdGet32(PcdDpaa2McPrivateRamSize))
                                        /* 14GB - 512MB (same as PcdSystemMemorySize) by default */
   /* For MC HIGH_MEM support, MC region is reserved from top of DRAM (DRAM2_BASE_ADDR + DDR_TOTAL_SIZE),
    * MC_ADDR should be the top 512MB from memory region allocated for MC, that comes
    * out to be (DRAM2_BASE_ADDR + DDR_TOTAL_SIZE - MC_LOW_MEM_FIXED_SIZE)
    */
#  define MC_ADDR  (DRAM2_BASE_ADDR + DDR_TOTAL_SIZE - MC_LOW_MEM_FIXED_SIZE)
# endif /* DPAA2_MC_IN_LOW_MEM */
#endif /* DPAA2_USE_UEFI_ALLOCATOR_FOR_MC_MEM */

#define DRAM2_BASE_ADDR			0x8080000000ULL
#define DRAM2_SIZE			0x8800000000ULL /* 510GB */

#define SERDES1_ADDR			(ADDR_IMMR + 0xEA0000)
#define SERDES2_ADDR			(ADDR_IMMR + 0xEB0000)

#define DPAA2_WRIOP1_ADDR               (ADDR_IMMR + 0x7B80000)
#define DPAA2_WRIOP1_MDIO1_ADDR	        (DPAA2_WRIOP1_ADDR + 0x16000)
#define DPAA2_WRIOP1_MDIO2_ADDR	        (DPAA2_WRIOP1_ADDR + 0x17000)

#define DPAA2_MC_PORTALS_BASE_ADDR              0x00080C000000ULL
#define DPAA2_MC_PORTALS_SIZE                   0x4000000
#define DPAA2_NI_PORTALS_BASE_ADDR	        0x000810000000ULL
#define DPAA2_NI_PORTALS_SIZE		        0x8000000
#define DPAA2_QBMAN_PORTALS_BASE_ADDR           0x000818000000ULL
#define DPAA2_QBMAN_PORTALS_SIZE                0x8000000
#define DPAA2_QBMAN_PORTALS_CACHE_ENABLED_SIZE  0x4000000

#define DPAA2_MC_RAM_SIZE               PcdGet32(PcdDpaa2McPrivateRamSize)

/* Cache Coherent Interconnect */
#define CCI_MN_BASE_ADDR             	0x04000000
#define CCI_MN_RNF_NODEID_LIST_OFFSET	0x180
#define CCI_MN_DVM_DOMAIN_CTL_OFFSET	0x200
#define CCI_MN_DVM_DOMAIN_CTL_SET       0x210

#define CCI_RN_I_0_BASE_ADDR                 (CCI_MN_BASE_ADDR + 0x800000)
#define CCI_RN_I_2_BASE_ADDR                 (CCI_MN_BASE_ADDR + 0x820000)
#define CCI_RN_I_6_BASE_ADDR                 (CCI_MN_BASE_ADDR + 0x860000)
#define CCI_RN_I_12_BASE_ADDR                (CCI_MN_BASE_ADDR + 0x8C0000)
#define CCI_RN_I_16_BASE_ADDR                (CCI_MN_BASE_ADDR + 0x900000)
#define CCI_RN_I_20_BASE_ADDR                (CCI_MN_BASE_ADDR + 0x940000)

#define CCI_S0_QOS_CNTRL_BASE(x) ((CCI_RN_I_0_BASE_ADDR + (x * 0x10000)) + 0x10)
#define CCI_S1_QOS_CNTRL_BASE(x) ((CCI_RN_I_0_BASE_ADDR + (x * 0x10000)) + 0x110)
#define CCI_S2_QOS_CNTRL_BASE(x) ((CCI_RN_I_0_BASE_ADDR + (x * 0x10000)) + 0x210)

#define CCI_AUX_CONTROL_BASE(x) ((CCI_RN_I_0_BASE_ADDR + (x * 0x10000)) + 0x0500)

/* QIXIS */
UINTN GetQixisAddr(VOID);
#define QIXIS_BASE			GetQixisAddr()

#define QIXIS_BASE_PHYS			0x20000000
#define QIXIS_BASE_PHYS_EARLY		0xC000000
#define QIXIS_RST_CTL_RESET		0x31
#define QIXIS_LBMAP_MASK		0x0f
#define QIXIS_LBMAP_SWITCH		0x06
#define QIXIS_LBMAP_SHIFT		0
#define QIXIS_LBMAP_DFLTBANK		0x00
#define QIXIS_LBMAP_ALTBANK		0x04
#define QIXIS_LBMAP_NAND		0x09
#define QIXIS_RST_CTL_RESET		0x31
#define QIXIS_RST_CTL_RESET_EN		0x30
#define QIXIS_RCFG_CTL_RECONFIG_IDLE	0x20
#define QIXIS_RCFG_CTL_RECONFIG_START	0x21
#define QIXIS_RCFG_CTL_WATCHDOG_ENBLE	0x08
#define QIXIS_RCW_SRC_NAND		0x119
#define	QIXIS_RST_FORCE_MEM		0x01



/*
 * board-specific DDR setting
 */
#define MEMORY_TYPE_DDR4
#define DDR_SDRAM_BASE_ADDR          0x80000000UL
#define DDR_BLOCK2_BASE_ADDR         0x8080000000ULL
#define DDR_SDRAM_BASE_PHY           0x0
#define DDR_SPD_TIMING
#define DDR_ECC_ENABLE
#define DDR_CTLRS_NUM                2
#define DDR_DIMM_SLOTS_PER_CTLR      2
#define DDR_CHIP_SELECTS_PER_CTRL    4
#define DDR_ECC_INIT_VIA_CONTROLLER
#define DDR_CTRL_INTLV_256B
#define DDR_VERY_BIG_RAM
#define DDR_SPD_I2C_ADDRESS1     0x51
#define DDR_SPD_I2C_ADDRESS2     0x52
#define DDR_SPD_I2C_ADDRESS3     0x53
#define DDR_SPD_I2C_ADDRESS4     0x54
#define DDR_SPD_I2C_ADDRESS      DDR_SPD_I2C_ADDRESS1
#define DDR_SPD_I2C_BUS          I2C1
#define DDR_SPD_I2C_BASE_ADDR    I2C1_BASE_ADDRESS

#define I2C_DEBUG_REG 0x6
#define I2C_GLITCH_EN 0x8

#define PCI_EXP1_BASE_ADDR               0x2000000000
#define PCI_EXP1_BASE_SIZE               0x800000000 /* 32GB */
#define PCI_EXP2_BASE_ADDR               0x2800000000
#define PCI_EXP2_BASE_SIZE               0x800000000 /* 32GB */
#define PCI_EXP3_BASE_ADDR               0x3000000000
#define PCI_EXP3_BASE_SIZE               0x800000000 /* 32GB */
#define PCI_EXP4_BASE_ADDR               0x3800000000
#define PCI_EXP4_BASE_SIZE               0x800000000 /* 32GB */

/*
 * Global defines
 */
#define CONFIG_LS2088A
#define CONFIG_LS2088ARDB

#define NOR_BOOT	0x0
#define NAND_BOOT	0x1
#define QSPI_BOOT	0x2

/* PCI controllers addresses */
#define PCIE1_PHYS_BASE              0x2000000000ULL
#define PCIE2_PHYS_BASE              0x2800000000ULL
#define PCIE3_PHYS_BASE              0x3000000000ULL
#define PCIE4_PHYS_BASE              0x3800000000ULL

#define PCIE1_PHYS_ADDR              0x2000000000ULL
#define PCIE2_PHYS_ADDR              0x2800000000ULL
#define PCIE3_PHYS_ADDR              0x3000000000ULL
#define PCIE4_PHYS_ADDR              0x3800000000ULL

#define PCIE1_BASE_ADDR                  0x03400000
#define PCIE2_BASE_ADDR                  0x03500000
#define PCIE3_BASE_ADDR                  0x03600000
#define PCIE4_BASE_ADDR                  0x03700000

#define LS_PCIE_CFG0_PHYS_OFF	   	0x00000000
#define LS_PCIE_CFG0_SIZE       	0x00001000      /* 4k */
#define LS_PCIE_CFG1_PHYS_OFF   	0x00001000
#define LS_PCIE_CFG1_SIZE       	0x00001000      /* 4k */

#define LS_PCIE_IO_BUS          0x00000000
#define LS_PCIE_IO_PHYS_OFF     0x00010000
#define LS_PCIE_IO_SIZE         0x00010000      /* 64k */

#define LS_PCIE_MEM_BUS         0x40000000
#define LS_PCIE_MEM_PHYS_OFF    0x40000000
#define LS_PCIE_MEM_SIZE        0x40000000      /* 1 GB */

/* SATA */
#define SATA1_REG_ADDR                        (ADDR_IMMR + 0x02200000)
#define SATA2_REG_ADDR                        (ADDR_IMMR + 0x02210000)

/* USB */
#define USB1_REG_ADDR                         (ADDR_IMMR + 0x02100000)
#define USB2_REG_ADDR                         (ADDR_IMMR + 0x02110000)

/*
 * DPAA2 Ethernet PHY MDIO addresses
 */
#define CORTINA_PHY_ADDR1	0x10
#define CORTINA_PHY_ADDR2	0x11
#define CORTINA_PHY_ADDR3	0x12
#define CORTINA_PHY_ADDR4	0x13
#define AQUANTIA_PHY_ADDR1	0x00
#define AQUANTIA_PHY_ADDR2	0x01
#define AQUANTIA_PHY_ADDR3	0x02
#define AQUANTIA_PHY_ADDR4	0x03
#define AQUANTIA405_IRQ_MASK	0x36

/* Interrupt Sampling Control */
#define INT_SAMPLING_CTRL_BASE	0x01F70000
#define IRQCR_OFFSET		0x14

/*
 * RPCM block
 */
#define RESET_BASE_ADDR		0x1E60000
#define RESET_SIZE              0x10000
#define	RST_CONTROL_REG_OFFSET	0x0
/* Hardware reset request. External hardware may then
 * decide to issue the desired reset signal (PORESET_B or HRESET_B)
 * to the device.
 */
#define RESET_REQ_MASK		0x02

#endif
