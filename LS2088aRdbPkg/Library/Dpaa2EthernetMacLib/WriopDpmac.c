/** WriopDpmac.c
  DPAA2 WRIOP MAC services implementation

  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <LS2088aSocLib.h>
#include <Bitops.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/Dpaa2DebugLib.h>
#include <Library/Dpaa2EthernetMacLib.h>

const CHAR8 *const gWriopDpmacStrings[] = {
  [WRIOP_DPMAC1] = "DPMAC1",
  [WRIOP_DPMAC2] = "DPMAC2",
  [WRIOP_DPMAC3] = "DPMAC3",
  [WRIOP_DPMAC4] = "DPMAC4",
  [WRIOP_DPMAC5] = "DPMAC5",
  [WRIOP_DPMAC6] = "DPMAC6",
  [WRIOP_DPMAC7] = "DPMAC7",
  [WRIOP_DPMAC8] = "DPMAC8",
  [WRIOP_DPMAC9] = "DPMAC9",
  [WRIOP_DPMAC10] = "DPMAC10",
  [WRIOP_DPMAC11] = "DPMAC11",
  [WRIOP_DPMAC12] = "DPMAC12",
  [WRIOP_DPMAC13] = "DPMAC13",
  [WRIOP_DPMAC14] = "DPMAC14",
  [WRIOP_DPMAC15] = "DPMAC15",
  [WRIOP_DPMAC16] = "DPMAC16",
  [WRIOP_DPMAC17] = "DPMAC17",
  [WRIOP_DPMAC18] = "DPMAC18",
  [WRIOP_DPMAC19] = "DPMAC19",
  [WRIOP_DPMAC20] = "DPMAC20",
  [WRIOP_DPMAC21] = "DPMAC21",
  [WRIOP_DPMAC22] = "DPMAC22",
  [WRIOP_DPMAC23] = "DPMAC23",
  [WRIOP_DPMAC24] = "DPMAC24",
};

C_ASSERT(ARRAY_SIZE(gWriopDpmacStrings) == NUM_WRIOP_DPMACS);

static const CHAR8 *const gPhyInterfaceTypeStrings[] = {
  [PHY_INTERFACE_XGMII] = "xgmii",
  [PHY_INTERFACE_SGMII] = "sgmii",
  [PHY_INTERFACE_QSGMII] = "qsgmii",
};

C_ASSERT(ARRAY_SIZE(gPhyInterfaceTypeStrings) == NUM_PHY_INTERFACE_TYPES);

#define WRIOP_DPMAC_INITIALIZER(_DpmacId) \
        [_DpmacId] = {                                                  \
          .Signature = WRIOP_DPMAC_SIGNATURE,                           \
          .Id = _DpmacId,                                               \
          .Enabled = FALSE,                                             \
          .Phy = {                                                      \
            .Signature = DPAA2_PHY_SIGNATURE,                           \
            .PhyInterfaceType = PHY_INTERFACE_NONE,                     \
          },                                                            \
        }

static WRIOP_DPMAC gWriopDpmacs[] = {
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC1),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC2),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC3),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC4),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC5),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC6),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC7),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC8),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC9),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC10),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC11),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC12),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC13),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC14),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC15),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC16),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC17),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC18),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC19),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC20),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC21),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC22),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC23),
  WRIOP_DPMAC_INITIALIZER(WRIOP_DPMAC24),
};

C_ASSERT(ARRAY_SIZE(gWriopDpmacs) == NUM_WRIOP_DPMACS);

static BOOLEAN
IsDpmacEnabled(WRIOP_DPMAC_ID DpmacId)
{
  UINT32 RegValue;
  struct CcsrGur *Gur = (VOID *)(CONFIG_SYS_FSL_GUTS_ADDR);

  ASSERT(DpmacId >= WRIOP_DPMAC1 && DpmacId < NUM_WRIOP_DPMACS);

  RegValue = MmioRead32((UINTN)&Gur->DevDisr2);
  return ((RegValue & BIT(DpmacId - 1)) == 0);
}


static const CHAR8 *
SerDesLaneProtocolToString(SERDES_LANE_PROTOCOL LaneProtocol)
{
  static const CHAR8* const Strings[] = {
    "XFI1", "XFI2", "XFI3", "XFI4", "XFI5", "XFI6", "XFI7", "XFI8"
  };

  if (LaneProtocol >= XFI1 && LaneProtocol <= XFI8) {
    return Strings[LaneProtocol - XFI1];
  } else {
    return "Unknown";
  }
}


/**
   Initializes a given WRIOP DPAA2 MAC. If the DPMAc is enabled,
   it is added to the specified list of enabled DPMACs

   @param[in] SerDesInstance	    SerDes module instance
   @param[in] DpMacId	            DPAA2 MAC Id
   @param[in] LaneProtocol	    SerDes lane protocol for the PHY associated
                                    with the given DPAA2 MAC
   @param[in] PhyInterfaceType      Ethernet PHY interface type
   @param[in] MdioBus               Pointer to the MDIO bus that connects the DMPAC to a PHY
   @param[in] PhyAddress            PHY address on the MDIO bus
   @param[in] EnabledDpmacList      Pointer to the head of the list of enabled DPMACs

   @retval None

 **/
VOID
WriopDpmacInit(SERDES_INSTANCE SerDesInstance,
               WRIOP_DPMAC_ID DpmacId,
	       SERDES_LANE_PROTOCOL LaneProtocol,
               PHY_INTERFACE_TYPE PhyInterfaceType,
               DPAA2_PHY_MDIO_BUS *MdioBus,
               UINT8 PhyAddress,
               PHY_MEDIA_TYPE PhyMediaType,
               LIST_ENTRY *EnabledDpmacsList)
{
  static WRIOP_DPMAC *Dpmac;

  ASSERT(DpmacId >= WRIOP_DPMAC1 && DpmacId < NUM_WRIOP_DPMACS);
  ASSERT(LaneProtocol < SERDES_LANE_PROTOCOLS_COUNT);
  ASSERT(PhyInterfaceType < NUM_PHY_INTERFACE_TYPES);

  Dpmac = &gWriopDpmacs[DpmacId];
  ASSERT(Dpmac->Id == DpmacId);
  Dpmac->Enabled = IsDpmacEnabled(DpmacId);
  Dpmac->Phy.PhyInterfaceType = PhyInterfaceType;
  Dpmac->Phy.MdioBus = MdioBus;
  Dpmac->Phy.PhyAddress = PhyAddress;
  Dpmac->Phy.PhyMediaType = PhyMediaType;

  DPAA2_INFO_MSG(
    "%a discovered on SerDes%d, Lane protocol %a, PHY type %a, %a\n",
    gWriopDpmacStrings[DpmacId],
    SerDesInstance + 1,
    SerDesLaneProtocolToString(LaneProtocol),
    gPhyInterfaceTypeStrings[PhyInterfaceType],
    Dpmac->Enabled ? "Enabled" : "Disabled");

  if (Dpmac->Enabled) {
    InitializeListHead(&Dpmac->ListNode);
    InsertTailList(EnabledDpmacsList, &Dpmac->ListNode);
  }
}

const CHAR8 *
PhyInterfaceTypeToString(PHY_INTERFACE_TYPE PhyInterfaceType)
{
  if (PhyInterfaceType < NUM_PHY_INTERFACE_TYPES) {
    return gPhyInterfaceTypeStrings[PhyInterfaceType];
  } else {
    return "Unknown";
  }
}
