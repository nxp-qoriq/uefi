/** Ls1043aSerDes.h
 The Header file of SerDes Module 

 Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found 
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/ 

#ifndef __LS1043A_SERDES_H
#define __LS1043A_SERDES_H

#include <LS1043aRdb.h>
#include <Common.h>

#define	SERDES_MAX_LANES	4

#define RCWSR4_SERDES1_PRTCL_SHIFT 16
#define RCWSR4_SERDES1_PRTCL_WIDTH 16

typedef enum _SERDES_LANE_PROTOCOL {
  NONE = 0,
  PCIE1,
  PCIE2,
  PCIE3,
  SATA,
  SGMII_FM1_DTSEC1,
  SGMII_FM1_DTSEC2,
  SGMII_FM1_DTSEC5,
  SGMII_FM1_DTSEC6,
  SGMII_FM1_DTSEC9,
  QSGMII_FM1_A,        /* A indicates MACs 1,2,5,6 */
  XFI_FM1_MAC9,
  SGMII_2500_FM1_DTSEC2,
  SGMII_2500_FM1_DTSEC9,
  /*
   * Number of entries in this enum
   */
  SERDES_LANE_PROTOCOLS_COUNT
} SERDES_LANE_PROTOCOL;

C_ASSERT(SERDES_LANE_PROTOCOLS_COUNT <= sizeof(UINT64) * 8);

/**
 * SerDes module instances
 */
typedef enum _SERDES_INSTANCES {
  SERDES_INSTANCE_1  = 0,

  /*
   * Number of entries in this enum
   */
  SERDES_INSTANCES_COUNT
} SERDES_INSTANCE;


VOID
GetSerdesProtocolMaps(
  OUT UINT64 *SerDes1ProtocolMapPtr
);

BOOLEAN
IsSerDesLaneProtocolConfigured(
  IN UINT64 SerDes1ProtocolMap,
  IN SERDES_LANE_PROTOCOL Device
);

typedef VOID
SERDES_LANE_PROBE_CALLBACK(
  IN SERDES_LANE_PROTOCOL LaneProtocol,
  IN VOID *Arg
);

VOID
SerDesProbeLanes(
  IN SERDES_LANE_PROBE_CALLBACK *SerDesLaneProbeCallback,
  IN VOID *Arg
);

#endif /* __LS1043A_SERDES_H */
