/** QixisLib.h
*  Header defining QIXIS FPGA/CPLD specific constants (Base addresses, sizes, flags)
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

#ifndef ___QIXIS_H__
#define ___QIXIS_H__

#include <Common.h>

/* SYSCLK */
#define QIXIS_SYSCLK_66			0x0
#define QIXIS_SYSCLK_83			0x1
#define QIXIS_SYSCLK_100		0x2
#define QIXIS_SYSCLK_125		0x3
#define QIXIS_SYSCLK_133		0x4
#define QIXIS_SYSCLK_150		0x5
#define QIXIS_SYSCLK_160		0x6
#define QIXIS_SYSCLK_166		0x7

struct QixisRegSet {
        UINT8 id;      /* ID value uniquely identifying each QorIQ board type */
        UINT8 arch;    /* Board version information */
        UINT8 scver;   /* QIXIS Version Register */
        UINT8 model;   /* Information of software programming model version */
        UINT8 tagdata;
        UINT8 ctl_sys;
        UINT8 aux;         /* Auxiliary Register,0x06 */
        UINT8 clk_spd;
        UINT8 stat_dut;
        UINT8 stat_sys;
        UINT8 stat_alrm;
        UINT8 present;
        UINT8 present2;    /* Presence Status Register 2,0x0c */
        UINT8 rcw_ctl;
        UINT8 ctl_led;
        UINT8 i2cblk;
        UINT8 rcfg_ctl;    /* Reconfig Control Register,0x10 */
        UINT8 rcfg_st;
        UINT8 dcm_ad;
        UINT8 dcm_da;
        UINT8 dcmd;
        UINT8 dmsg;
        UINT8 gdc;
        UINT8 gdd;         /* DCM Debug Data Register,0x17 */
        UINT8 dmack;
        UINT8 res1[6];
        UINT8 watch;       /* Watchdog Register,0x1F */
        UINT8 pwr_ctl[2];  /* Power Control Register,0x20 */
        UINT8 res2[2];
        UINT8 pwr_stat[4]; /* Power Status Register,0x24 */
        UINT8 res3[8];
        UINT8 clk_spd2[2];  /* SYSCLK clock Speed Register,0x30 */
        UINT8 res4[2];
        UINT8 sclk[3];  /* Clock Configuration Registers,0x34 */
        UINT8 res5;
        UINT8 dclk[3];
        UINT8 res6;
        UINT8 clk_dspd[3];
        UINT8 res7;
        UINT8 rst_ctl;     /* Reset Control Register,0x40 */
        UINT8 rst_stat;    /* Reset Status Register */
        UINT8 rst_rsn;     /* Reset Reason Register */
        UINT8 rst_frc[2];  /* Reset Force Registers,0x43 */
        UINT8 res8[11];
        UINT8 brdcfg[16];  /* Board Configuration Register,0x50 */
        UINT8 dutcfg[16];
        UINT8 rcw_ad[2];   /* RCW SRAM Address Registers,0x70 */
        UINT8 rcw_data;
        UINT8 res9[5];
        UINT8 post_ctl;
        UINT8 post_stat;
        UINT8 post_dat[2];
        UINT8 pi_d[4];
        UINT8 gpio_io[4];
        UINT8 gpio_dir[4];
        UINT8 res10[20];
        UINT8 rjtag_ctl;
        UINT8 rjtag_dat;
        UINT8 res11[2];
        UINT8 trig_src[4];
        UINT8 trig_dst[4];
        UINT8 trig_stat;
        UINT8 res12[3];
        UINT8 trig_ctr[4];
        UINT8 res13[16];
        UINT8 clk_freq[6]; /* Clock Measurement Registers */
        UINT8 res_c6[8];
        UINT8 clk_base[2]; /* Clock Frequency Base Reg */
        UINT8 res_d0[8];
        UINT8 cms[2];      /* Core Management Space Address Register, 0xD8 */
        UINT8 res_c0[6];
        UINT8 aux2[4];     /* Auxiliary Registers,0xE0 */
        UINT8 res14[10];
        UINT8 aux_ad;
        UINT8 aux_da;
        UINT8 res15[16];
};

#define QIXIS_RCFG_CTL_RECONFIG_IDLE  0x20
#define QIXIS_RCFG_CTL_RECONFIG_START 0x21
#define QIXIS_RST_CTL_RESET_EN        0x30

#ifndef DEFAULT_ALTBANK
#define DEFAULT_ALTBANK   0x04 /* default alternate bank to vbank4 */
#endif

/*
 * Reset the board, Reset to alternate bank or Dump registers:
 * RESET - reset to default bank
 * RESET_ALTBANK - reset to alternate bank
 * DUMP_REGISTERS - display the CPLD/FPGA registers
 */
typedef enum {
	RESET = 0,
	RESET_ALTBANK,
	DUMP_REGISTERS
} QixisCmd;

UINT8 QixisRead(UINTN Reg);
VOID QixisWrite(UINTN Reg, UINT8 Value);
VOID QixisRevBit(UINT8 *Value);
VOID DoQixis (QixisCmd Cmd);
VOID QixisInit (VOID);

#define QIXIS_READ(Reg) QixisRead(offsetof(struct QixisRegSet, Reg))
#define QIXIS_WRITE(Reg, Value)  \
                QixisWrite(offsetof(struct QixisRegSet, Reg), Value)

#endif /* ___QIXIS_H__ */
