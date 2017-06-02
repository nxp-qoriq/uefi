#ifndef __NXP_STREAM_ID_H
#define __NXP_STREAM_ID_H

/* Stream IDs on ls2088a devices are not hardwired and are
 * programmed by sw.  There are a limited number of stream IDs
 * available, and the partitioning of them is scenario dependent.
 * This header defines the partitioning between legacy, PCI,
 * and DPAA2 devices.
 *
 * This partitiong can be customized in this file depending
 * on the specific hardware config-- e.g. perhaps not all
 * PEX controllers are in use.
 *
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

#define NXP_AMQ_PL_MASK			(0x1 << 18)   /* priviledge bit */
#define NXP_AMQ_BMT_MASK			(0x1 << 17)   /* bypass bit */

#define NXP_INVALID_STREAM_ID		0

#define NXP_BYPASS_AMQ			(NXP_AMQ_PL_MASK | NXP_AMQ_BMT_MASK)

/* legacy devices */
#define NXP_USB1_STREAM_ID		1
#define NXP_USB2_STREAM_ID		2
#define NXP_SDMMC_STREAM_ID		3
#define NXP_SATA1_STREAM_ID		4
#define NXP_SATA2_STREAM_ID		5
#define NXP_DMA_STREAM_ID		6

/* PCI - programmed in PEXn_LUT by OS */
/*   4 IDs per controller */
#define NXP_PEX1_STREAM_ID_START	7
#define NXP_PEX1_STREAM_ID_END		10
#define NXP_PEX2_STREAM_ID_START	11
#define NXP_PEX2_STREAM_ID_END		14
#define NXP_PEX3_STREAM_ID_START	15
#define NXP_PEX3_STREAM_ID_END		18
#define NXP_PEX4_STREAM_ID_START	19
#define NXP_PEX4_STREAM_ID_END		22

/* DPAA2 - set in MC DPC and alloced by MC */
#define NXP_DPAA2_STREAM_ID_START	23
#define NXP_DPAA2_STREAM_ID_END		63

#endif
