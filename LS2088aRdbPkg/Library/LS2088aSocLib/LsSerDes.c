/** LsSerDes.c
 Provides the basic interfaces for SerDes Module

 Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution. The full text of the license may be found
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <LS2088aRdb.h>
#include <LS2088aSocLib.h>
#include <Library/DebugLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Ls2088aSerDes.h>
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
  UINT8 SerDesProtocol;

  /**
   * Array of SerDes lane protocols, one entry per SerDes lane.
   * Values of entries must be from the SERDES_LANE_PROTOCOL enum.
   */
  UINT8 SerDesLanes[SERDES_MAX_LANES];
} SERDES_CONFIG;

/**
 * SerDes1 instance configuration
 *
 * NOTE: Lanes for SerDes1 are inverted. (See LS2088A reference manual
 * section 24.1.2.1
 */
static const SERDES_CONFIG SerDes1ConfigTable[] = {
  {
    .SerDesProtocol = 0x03,
    .SerDesLanes = { PCIE2, PCIE2, PCIE2, PCIE2, PCIE1, PCIE1, PCIE1, PCIE1 }
  },
  {
    .SerDesProtocol = 0x05,
    .SerDesLanes = { PCIE2, PCIE2, PCIE2, PCIE2, SGMII4, SGMII3, SGMII2,
                     SGMII1 }
  },
  {
    .SerDesProtocol = 0x07,
    .SerDesLanes = { SGMII8, SGMII7, SGMII6, SGMII5, SGMII4, SGMII3, SGMII2,
                     SGMII1 }
  },
  {
    .SerDesProtocol = 0x09,
    .SerDesLanes =  { SGMII8, SGMII7, SGMII6, SGMII5, SGMII4, SGMII3, SGMII2,
                      SGMII1 }
  },
  {
     .SerDesProtocol = 0x0A,
     .SerDesLanes =  { SGMII8, SGMII7, SGMII6, SGMII5, SGMII4, SGMII3, SGMII2,
                       SGMII1 }
  },
  {
    .SerDesProtocol = 0x0C,
    .SerDesLanes = { SGMII8, SGMII7, SGMII6, SGMII5, SGMII4, SGMII3, SGMII2,
                     SGMII1 }
  },
  {
    .SerDesProtocol = 0x0E,
    .SerDesLanes =  { SGMII8, SGMII7, SGMII6, SGMII5, SGMII4, SGMII3, SGMII2,
                      SGMII1 }
  },
  {
    .SerDesProtocol =  0x26,
    .SerDesLanes = { SGMII8, SGMII7, SGMII6, SGMII5, SGMII4, SGMII3, XFI2,
                     XFI1 }
  },
  {
     .SerDesProtocol = 0x28,
     .SerDesLanes = { SGMII8, SGMII7, SGMII6, SGMII5, XFI4, XFI3, XFI2, XFI1 }
  },
  {
    .SerDesProtocol = 0x2A,
    .SerDesLanes = { XFI8, XFI7, XFI6, XFI5, XFI4, XFI3, XFI2, XFI1 }
  },
  {
    .SerDesProtocol = 0x2B,
    .SerDesLanes = { SGMII8, SGMII7, SGMII6, SGMII5, XAUI1, XAUI1, XAUI1,
                     XAUI1 }
  },
  {
    .SerDesProtocol = 0x32,
    .SerDesLanes = { XAUI2, XAUI2, XAUI2, XAUI2, XAUI1, XAUI1, XAUI1, XAUI1 }
  },
  {
    .SerDesProtocol = 0x33,
    .SerDesLanes = { PCIE2, PCIE2, PCIE2, PCIE2, QSGMII_D, QSGMII_C, QSGMII_B,
                     QSGMII_A }
  },
  {
    .SerDesProtocol = 0x35,
    .SerDesLanes = { QSGMII_D, QSGMII_C, QSGMII_B, PCIE2, XFI4, XFI3, XFI2,
                     XFI1 }
  },
  {
    .SerDesProtocol = 0x0,
  }
};

/**
 * SerDes2 instance configuration
 */
static const SERDES_CONFIG SerDes2ConfigTable[] = {
  {
    .SerDesProtocol = 0x07,
    .SerDesLanes = { SGMII9, SGMII10, SGMII11, SGMII12, SGMII13, SGMII14,
                     SGMII15, SGMII16 }
  },
  {
    .SerDesProtocol = 0x09,
    .SerDesLanes = { SGMII9, SGMII10, SGMII11, SGMII12, SGMII13, SGMII14,
                     SGMII15, SGMII16 }
  },
  {
    .SerDesProtocol = 0x0A,
    .SerDesLanes = { SGMII9, SGMII10, SGMII11, SGMII12, SGMII13, SGMII14,
                     SGMII15, SGMII16 }
  },
  {
    .SerDesProtocol = 0x0C,
    .SerDesLanes = { SGMII9, SGMII10, SGMII11, SGMII12, SGMII13, SGMII14,
                     SGMII15, SGMII16 }
  },
  {
    .SerDesProtocol = 0x0E,
    .SerDesLanes = { SGMII9, SGMII10, SGMII11, SGMII12, SGMII13, SGMII14,
                     SGMII15, SGMII16 }
  },
  {
    .SerDesProtocol = 0x3D,
    .SerDesLanes = { PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3E,
    .SerDesLanes = { PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3 }
  },
  {
    .SerDesProtocol = 0x3F,
    .SerDesLanes = { PCIE3, PCIE3, PCIE3, PCIE3, PCIE4, PCIE4, PCIE4, PCIE4 }
  },
  {
    .SerDesProtocol = 0x40,
    .SerDesLanes = { PCIE3, PCIE3, PCIE3, PCIE3, PCIE4, PCIE4, PCIE4, PCIE4 }
  },
  {
    .SerDesProtocol = 0x41,
    .SerDesLanes = { PCIE3, PCIE3, PCIE3, PCIE3, PCIE4, PCIE4, SATA1, SATA2 }
  },
  {
    .SerDesProtocol = 0x42,
    .SerDesLanes = { PCIE3, PCIE3, PCIE3, PCIE3, PCIE4, PCIE4, SATA1, SATA2 }
  },
  {
    .SerDesProtocol = 0x43,
    .SerDesLanes = { PCIE3, PCIE3, PCIE3, PCIE3, NONE, NONE, SATA1, SATA2 }
  },
  {
    .SerDesProtocol = 0x44,
    .SerDesLanes = { PCIE3, PCIE3, PCIE3, PCIE3, NONE, NONE, SATA1, SATA2 }
  },

  {
    .SerDesProtocol = 0x45,
    .SerDesLanes = { SGMII9, SGMII10, SGMII11, SGMII12, PCIE4, PCIE4, PCIE4,
	             PCIE4 }
  },
  {
    .SerDesProtocol = 0x47,
    .SerDesLanes = { PCIE3, SGMII10, SGMII11, SGMII12, PCIE4, SGMII14, SGMII15,
	             SGMII16 }
  },
  {
    .SerDesProtocol = 0x49,
    .SerDesLanes = { SGMII9, SGMII10, SGMII11, SGMII12, PCIE4, PCIE4, SATA1,
	             SATA2 }
  },
  {
    .SerDesProtocol = 0x4A,
    .SerDesLanes = { SGMII9, SGMII10, SGMII11, SGMII12, PCIE4, PCIE4, SATA1,
	             SATA2 }
  },
  {
     .SerDesProtocol = 0x0,
  }
};


/**
   Get the configuration for the given SerDes instance.

   @param[in] SerDesInstance	SerDes module instance

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
  struct CcsrGur *Gur = (VOID *)(CONFIG_SYS_FSL_GUTS_ADDR);

  ASSERT(SerDesInstance < SERDES_INSTANCES_COUNT);

  /*
   * Get the SerDes instance's SerDes protocol selector from RCW:
   */
  RegValue = MmioRead32((UINTN)&Gur->Rcw_Sr[FSL_CHASSIS3_RCWSR_29_INDEX]);
  if (SerDesInstance == SERDES_INSTANCE_1) {
    SerDesProtocol = GET_BIT_FIELD32(RegValue,
                                     FSL_CHASSIS3_RCWSR_29_SERDES1_PRTCL_SHIFT,
                                     FSL_CHASSIS3_RCWSR_29_SERDES1_PRTCL_WIDTH);

    ConfigTable = SerDes1ConfigTable;
  } else {
    SerDesProtocol = GET_BIT_FIELD32(RegValue,
                                     FSL_CHASSIS3_RCWSR_29_SERDES2_PRTCL_SHIFT,
                                     FSL_CHASSIS3_RCWSR_29_SERDES2_PRTCL_WIDTH);

    ConfigTable = SerDes2ConfigTable;
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
   @param[in] SerDes2ProtocolMap    Pointer to bitmap of configured SerDes lane
                                    protocols for SerDes2 instance.
   @param[in] Device		    SerDes lane protocol representing a device

   @retval TRUE, if configured
   @retval FALSE, otherwise

 **/
BOOLEAN
IsSerDesLaneProtocolConfigured(
  IN UINT64 SerDes1ProtocolMap,
  IN UINT64 SerDes2ProtocolMap,
  IN SERDES_LANE_PROTOCOL Device
)
{
  ASSERT(Device > NONE && Device < SERDES_LANE_PROTOCOLS_COUNT);

  return (SerDes1ProtocolMap & BIT(Device)) != 0 ||
         (SerDes2ProtocolMap & BIT(Device)) != 0;
}


/**
   Get SerDes protocol Map for given SerDes Instance.

   @param[in] SerDesInstance	    SerDes module instance
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

   @param[out] SerDes1ProtocolMapPtr	Pointer to bitmap of configured SerDes
					lane protocols for SerDes1 instance.
   @param[out] SerDes2ProtocolMapPtr	Pointer to bitmap of configured SerDes
					lane protocols for SerDes2 instance.
**/
VOID
GetSerdesProtocolMaps(
  OUT UINT64 *SerDes1ProtocolMapPtr,
  OUT UINT64 *SerDes2ProtocolMapPtr
  )
{
  GetSerDesInstanceProtocolMap(SERDES_INSTANCE_1, SerDes1ProtocolMapPtr);
  GetSerDesInstanceProtocolMap(SERDES_INSTANCE_2, SerDes2ProtocolMapPtr);
}


/**
   Probes the lanes of a given SerDes instance.

   @param[in] SerDesInstance	      SerDes module instance
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
      SerDesLaneProbeCallback(SerDesInstance, LaneProtocol, Arg);
    }
  }
}


/**
   Probes lanes in all SerDes instances enabled and calls
   the given callback for each lane.

   SerDesProbeLanes() invokes the given callback for each
   SerDes lane protocol configured  according to the SerDes
   protocol configured in RCW, for each SerDes instance.

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
  if (PcdGetBool(PcdSerDes1Initialize)) {
    SerDesInstanceProbeLanes(SERDES_INSTANCE_1,
	                     SerDesLaneProbeCallback,
                             Arg);
  }

  if (PcdGetBool(PcdSerDes2Initialize)) {
    SerDesInstanceProbeLanes(SERDES_INSTANCE_2,
                             SerDesLaneProbeCallback,
		             Arg);
  }
}
