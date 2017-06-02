/** LsSerDes.c
 Provides the basic interfaces for SerDes Module

 Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found 
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/ 

#include <I2c.h>
#include <Ddr.h>
#include <Uefi.h>
#include <LS1043aRdb.h>
#include <LS1043aSocLib.h>
#include <Ls1043aSerDes.h>

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Ppi/ArmMpCoreInfo.h>
#include <Common.h>
#include <Bitops.h>

/**
 * Entry in the table of configurations for SerDes protocols
 * for a given SerDes instance
 */
typedef struct _SERDES_CONFIG {
  /**
   * SerDes protocol selector
   */
  UINT16 SerDesProtocol;

  /**
   * Array of SerDes lane protocols, one entry per SerDes lane.
   * Values of entries must be from the SERDES_LANE_PROTOCOL enum.
   */
  UINT8 SerDesLanes[SERDES_MAX_LANES];
} SERDES_CONFIG;

/**
 * SerDes1 instance configuration
 */
static const SERDES_CONFIG SerDes1ConfigTable[] = {
  {
    .SerDesProtocol = 0x1555,
    .SerDesLanes = { XFI_FM1_MAC9, PCIE1, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x2555,
    .SerDesLanes = { SGMII_2500_FM1_DTSEC9, PCIE1, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x4555,
    .SerDesLanes = { QSGMII_FM1_A, PCIE1, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x4558,
    .SerDesLanes =  { QSGMII_FM1_A, PCIE1, PCIE2, SATA }
  },
  {
    .SerDesProtocol = 0x1355,
    .SerDesLanes = { XFI_FM1_MAC9, SGMII_FM1_DTSEC2, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x2355,
    .SerDesLanes = { SGMII_2500_FM1_DTSEC9, SGMII_FM1_DTSEC2, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3335,
    .SerDesLanes = { SGMII_FM1_DTSEC9, SGMII_FM1_DTSEC2, SGMII_FM1_DTSEC5, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3355,
    .SerDesLanes = { SGMII_FM1_DTSEC9, SGMII_FM1_DTSEC2, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3358,
    .SerDesLanes = { SGMII_FM1_DTSEC9, SGMII_FM1_DTSEC2, PCIE2, SATA }
  },
  {
    .SerDesProtocol = 0x3555,
    .SerDesLanes = { SGMII_FM1_DTSEC9, PCIE1, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3558,
    .SerDesLanes = { SGMII_FM1_DTSEC9, PCIE1, PCIE2, SATA }
  },
  {
    .SerDesProtocol = 0x7000,
    .SerDesLanes = { PCIE1, PCIE1, PCIE1, PCIE1 }
  },
  {
    .SerDesProtocol = 0x9998,
    .SerDesLanes = { PCIE1, PCIE2, PCIE3, SATA }
  },
  {
    .SerDesProtocol = 0x6058,
    .SerDesLanes = { PCIE1, PCIE1, PCIE2, SATA }
  },
  {
    .SerDesProtocol = 0x1455,
    .SerDesLanes = { XFI_FM1_MAC9, QSGMII_FM1_A, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x2455,
    .SerDesLanes = { SGMII_2500_FM1_DTSEC9, QSGMII_FM1_A, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x2255,
    .SerDesLanes = { SGMII_2500_FM1_DTSEC9, SGMII_2500_FM1_DTSEC2, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3333,
    .SerDesLanes = { SGMII_FM1_DTSEC9, SGMII_FM1_DTSEC2, SGMII_FM1_DTSEC5, SGMII_FM1_DTSEC6 }
  },
  {
    .SerDesProtocol = 0x1460,
    .SerDesLanes = { XFI_FM1_MAC9, QSGMII_FM1_A, PCIE3, PCIE3 }
  },
  {
    .SerDesProtocol = 0x2460,
    .SerDesLanes = { SGMII_2500_FM1_DTSEC9, QSGMII_FM1_A, PCIE3, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3460,
    .SerDesLanes = { SGMII_FM1_DTSEC9, QSGMII_FM1_A, PCIE3, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3455,
    .SerDesLanes = { SGMII_FM1_DTSEC9, QSGMII_FM1_A, PCIE2, PCIE3 }
  },
  {
    .SerDesProtocol = 0x9960,
    .SerDesLanes = { PCIE1, PCIE2, PCIE3, PCIE3 }
  },
  {
    .SerDesProtocol = 0x2233,
    .SerDesLanes = { SGMII_2500_FM1_DTSEC9, SGMII_FM1_DTSEC2, SGMII_FM1_DTSEC5, SGMII_FM1_DTSEC6 }
  },
  {
    .SerDesProtocol = 0x2533,
    .SerDesLanes = { SGMII_2500_FM1_DTSEC9, PCIE1, SGMII_FM1_DTSEC5, SGMII_FM1_DTSEC6 }
  },
  {
    .SerDesProtocol = 0x0,
  }
};

/**
   Get the configuration for the given SerDes instance.

   @param[in] SerDesInstance    SerDes module instance

   @retval Pointer to configuration, if found
   @retval NULL, if not found

 **/
static
const SERDES_CONFIG *
GetSerdesInstanceConfig(
  IN SERDES_INSTANCE SerDesInstance
)
{
  UINT32 RegValue;
  UINT32 SerDesProtocol = 0x0;
  const SERDES_CONFIG *ConfigTable = NULL;
  const SERDES_CONFIG *Config;
  struct CcsrGur *Gur = (VOID *)(GUTS_ADDR);

  ASSERT(SerDesInstance < SERDES_INSTANCES_COUNT);

  /*
   * Get the SerDes instance's SerDes protocol selector from RCW:
   */
  RegValue = MmioReadBe32((UINTN)&Gur->rcwsr[4]);
  if (SerDesInstance == SERDES_INSTANCE_1) {
    SerDesProtocol = GET_BIT_FIELD32(RegValue,
                                     RCWSR4_SERDES1_PRTCL_SHIFT,
                                     RCWSR4_SERDES1_PRTCL_WIDTH);
  DEBUG((EFI_D_INFO, "Using SERDES1 Protocol: %d (0x%x)\n", SerDesProtocol, SerDesProtocol));

    ConfigTable = SerDes1ConfigTable;
  }

  for (Config = ConfigTable; Config && (Config->SerDesProtocol != 0); Config ++) {
    if (Config->SerDesProtocol == SerDesProtocol) {
      return Config;
    }
  }

  DEBUG((EFI_D_ERROR, "SERDES%d protocol %#x not found\n",
         SerDesInstance + 1, SerDesProtocol));
  return NULL;
}


/**
   Tell if the given SerDes lane protocol has been configured.

   @param[in] SerDes1ProtocolMap    Pointer to bitmap of configured SerDes lane
                                    protocols for SerDes1 instance.
   @param[in] Device                SerDes lane protocol representing a device

   @retval TRUE, if configured
   @retval FALSE, otherwise

 **/
BOOLEAN
IsSerDesLaneProtocolConfigured(
  IN UINT64 SerDes1ProtocolMap,
  IN SERDES_LANE_PROTOCOL Device
)
{
  ASSERT(Device > NONE && Device < SERDES_LANE_PROTOCOLS_COUNT);

  return (SerDes1ProtocolMap & BIT(Device)) != 0 ;
}


/**
   Get SerDes protocol Map for given SerDes Instance.

   @param[in] SerDesInstance        SerDes module instance
   @param[out] SerDesProtocolMap    Pointer to bitmap of configured SerDes lane
                                    protocols for the given SerDes instance.
**/
static
VOID
GetSerDesInstanceProtocolMap(
  IN SERDES_INSTANCE SerDesInstance,
  OUT UINT64 *SerDesProtocolMapPtr
  )
{
  UINTN Lane;
  const SERDES_CONFIG *Config;

  *SerDesProtocolMapPtr = 0x0;

  /*
   * Get the SerDes instance's SerDes protocol:
   */
  Config = GetSerdesInstanceConfig(SerDesInstance);
  if (Config == NULL) {
    return;
  }

  /*
   * Fill SerDes protocol Map:
   */
  for (Lane = 0; Lane < SERDES_MAX_LANES; Lane++) {
    SERDES_LANE_PROTOCOL LaneProtocol = Config->SerDesLanes[Lane];
    ASSERT(LaneProtocol < SERDES_LANE_PROTOCOLS_COUNT);
    if (LaneProtocol != NONE) {
      *SerDesProtocolMapPtr |= BIT(LaneProtocol);
    }
  }
}


/**
   Get SerDes protocol Maps for both SerDes Instances.

   @param[out] SerDes1ProtocolMapPtr    Pointer to bitmap of configured SerDes
                                        lane protocols for SerDes1 instance.
**/
VOID
GetSerdesProtocolMaps(
  OUT UINT64 *SerDes1ProtocolMapPtr
  )
{
  GetSerDesInstanceProtocolMap(SERDES_INSTANCE_1, SerDes1ProtocolMapPtr);
}


/**
   Probes the lanes of a given SerDes instance.

   @param[in] SerDesInstance          SerDes module instance
   @param[in] SerDesLaneProbeCallback Driver-specific callback to be invoked
                                      for every lane protocol configured in
                                      the given SerDes instance, according to
                                      the SerDes instance's RCW configuration.

   @param[in] Arg                     Callback argument or NULL
 **/
static VOID
SerDesInstanceProbeLanes(
  IN SERDES_INSTANCE SerDesInstance,
  IN SERDES_LANE_PROBE_CALLBACK *SerDesLaneProbeCallback,
  IN VOID *Arg
  )
{
  const SERDES_CONFIG *Config;
  UINTN Lane;

  Config = GetSerdesInstanceConfig(SerDesInstance);
  if (Config == NULL) {
    return;
  }

  /*
   * Invoke callback for all lanes in the SerDes instance:
   */
  for (Lane = 0; Lane < SERDES_MAX_LANES; Lane++) {
    SERDES_LANE_PROTOCOL LaneProtocol = Config->SerDesLanes[Lane];
    ASSERT(LaneProtocol < SERDES_LANE_PROTOCOLS_COUNT);
    if (LaneProtocol != NONE) {
      SerDesLaneProbeCallback(LaneProtocol, Arg);
    }
  }
}

/**
   Probes lanes in all SerDes instances enabled and calls
   the given callback for each lane.

   SerDesProbe() invokes the given callback for each
   SerDes lane protocol configured  according to the SerDes
   protocol configured in RCW, for SerDes instance.

   @param[in] SerDesLaneProbeCallback Driver-specific callback to be invoked
                                      for every lane protocol configured in
                                      each SerDes instance, according to
                                      the SerDes instance's RCW configuration.

   @param[in] Arg                     Callback argument or NULL
 **/
VOID
SerDesProbeLanes(
  IN SERDES_LANE_PROBE_CALLBACK *SerDesLaneProbeCallback,
  IN VOID *Arg
  )
{
    SerDesInstanceProbeLanes(SERDES_INSTANCE_1,
			     SerDesLaneProbeCallback,
			     Arg);
}
