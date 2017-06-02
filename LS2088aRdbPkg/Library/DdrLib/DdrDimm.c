/** @DdrDimm.c

  Copyright (c)) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Library/FslDdr.h>
#include <I2c.h>
#include <Common.h>


INT32
Crc16 (
  IN   UINT8      *Ptr,
  IN   INT32      Count
  )
{
  INT32 Crc, I;

  Crc = 0;
  while (--Count >= 0) {
    Crc = Crc ^ (INT32)*Ptr++ << 8;
    for (I = 0; I < 8; ++I)
      if (Crc & 0x8000)
        Crc = Crc << 1 ^ 0x1021;
      else
        Crc = Crc << 1;
  }
  return Crc & 0xffff;
}


#if defined(DDR_SPD_I2C_ADDRESS) || \
    defined(DDR_SPD_I2C_ADDRESS1) || defined(DDR_SPD_I2C_ADDRESS2) || \
    defined(DDR_SPD_I2C_ADDRESS3) || defined(DDR_SPD_I2C_ADDRESS4)
#if (DDR_CTLRS_NUM == 1) && (DDR_DIMM_SLOTS_PER_CTLR == 1)
UINT8 SpdI2cAddress[DDR_CTLRS_NUM][DDR_DIMM_SLOTS_PER_CTLR] = {
        [0][0] = DDR_SPD_I2C_ADDRESS,
};
#elif (DDR_CTLRS_NUM == 1) && (DDR_DIMM_SLOTS_PER_CTLR == 2)
UINT8 SpdI2cAddress[DDR_CTLRS_NUM][DDR_DIMM_SLOTS_PER_CTLR] = {
        [0][0] = DDR_SPD_I2C_ADDRESS1,
        [0][1] = DDR_SPD_I2C_ADDRESS2,
};
#elif (DDR_CTLRS_NUM == 2) && (DDR_DIMM_SLOTS_PER_CTLR == 1)
UINT8 SpdI2cAddress[DDR_CTLRS_NUM][DDR_DIMM_SLOTS_PER_CTLR] = {
        [0][0] = DDR_SPD_I2C_ADDRESS1,
        [1][0] = DDR_SPD_I2C_ADDRESS2,
};
#elif (DDR_CTLRS_NUM == 2) && (DDR_DIMM_SLOTS_PER_CTLR == 2)
UINT8 SpdI2cAddress[DDR_CTLRS_NUM][DDR_DIMM_SLOTS_PER_CTLR] = {
        [0][0] = DDR_SPD_I2C_ADDRESS1,
        [0][1] = DDR_SPD_I2C_ADDRESS2,
        [1][0] = DDR_SPD_I2C_ADDRESS3,
        [1][1] = DDR_SPD_I2C_ADDRESS4,
};
#elif (DDR_CTLRS_NUM == 3) && (DDR_DIMM_SLOTS_PER_CTLR == 1)
UINT8 SpdI2cAddress[DDR_CTLRS_NUM][DDR_DIMM_SLOTS_PER_CTLR] = {
        [0][0] = DDR_SPD_I2C_ADDRESS1,
        [1][0] = DDR_SPD_I2C_ADDRESS2,
        [2][0] = DDR_SPD_I2C_ADDRESS3,
};
#elif (DDR_CTLRS_NUM == 3) && (DDR_DIMM_SLOTS_PER_CTLR == 2)
UINT8 SpdI2cAddress[DDR_CTLRS_NUM][DDR_DIMM_SLOTS_PER_CTLR] = {
        [0][0] = DDR_SPD_I2C_ADDRESS1,
        [0][1] = DDR_SPD_I2C_ADDRESS2,
        [1][0] = DDR_SPD_I2C_ADDRESS3,
        [1][1] = DDR_SPD_I2C_ADDRESS4,
        [2][0] = DDR_SPD_I2C_ADDRESS5,
        [2][1] = DDR_SPD_I2C_ADDRESS6,
};
#endif


//temp compare to check I2C relibility of read
static VOID
DdrCompareSpd (
   IN OUT  GenericSpdEepromT    *Spd,
   IN UINT32                    Type
)
{
  static CONST UINT8 SpdData[512] = {
    0x23, 0x10, 0x0C, 0x02, 0x84, 0x19, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x09, 0x0B, 0x80, 0x00,
    0x00, 0x00, 0x08, 0x0C, 0xF4, 0x1B, 0x00, 0x00, 0x6C, 0x6C, 0x6C, 0x11, 0x08, 0x74, 0x20, 0x08,
    0x00, 0x05, 0x70, 0x03, 0x00, 0xA8, 0x1E, 0x2B, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x2C, 0x15, 0x35,
    0x15, 0x35, 0x0B, 0x2C, 0x15, 0x35, 0x0B, 0x35, 0x0B, 0x2C, 0x0B, 0x35, 0x15, 0x36, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xEC, 0xB5, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC2, 0x30, 0x0E,
    0x11, 0x11, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x2E,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x2C, 0x0F, 0x15, 0x15, 0x0F, 0x3E, 0xB1, 0x98, 0x31, 0x38, 0x41, 0x53, 0x46, 0x31, 0x47,
    0x37, 0x32, 0x41, 0x5A, 0x2D, 0x32, 0x47, 0x31, 0x41, 0x31, 0x20, 0x20, 0x20, 0x31, 0x80, 0x2C,
    0x41, 0x44, 0x50, 0x41, 0x46, 0x32, 0x4B, 0x36, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  UINT32 I;
  UINT8 *Ptr = (VOID *) Spd;
  UINT8 SpdError = 0;

  if (Type == 0) {
    if (*Ptr == 0x0)
      return;

    for (I = 0; I < 512; I++, Ptr++) {
      if ( *Ptr == SpdData[I]) {
        continue;
      }
      SpdError += 1;
      DEBUG((EFI_D_INFO, "SPD Byte[%d]=0x%02x, should be 0x%02x\n", I, *Ptr, SpdData[I]));
      *Ptr = SpdData[I];
    }

    if (SpdError) {
      DEBUG((EFI_D_INFO,"I2C read 256 Bytes SPD Data, %d errors fixed!\n", SpdError));
    } else {
     DEBUG((EFI_D_INFO,"I2C read SPD 512-Bytes correctly!\n"));
    }
  } else {
    for (I = 0; I < 512; I++, Ptr++) {
      *Ptr = SpdData[I];
    }
  }

  return;
}

void Ddr4SpdDump(const struct Ddr4SpdEepromS *spd)
{
       UINT32 i;

       /* General Section: Bytes 0-127 */

#define DEBUG_DXA(x, y, z...) DEBUG((EFI_D_INFO,"%-3d    : %02x " z "\n", x, (UINT8)y));
#define DEBUG_DDXXA(n0, n1, x0, x1, s) \
       DEBUG((EFI_D_INFO,"%-3d-%3d: %02x %02x " s "\n", n0, n1, x0, x1));

       DEBUG_DXA(0, spd->InfoSizeCrc,
                "info_size_crc  bytes written into serial memory, CRC coverage");
       DEBUG_DXA(1, spd->SpdRev,
                "spd_rev        SPD Revision");
       DEBUG_DXA(2, spd->MemType,
                "mem_type       Key Byte / DRAM Device Type");
       DEBUG_DXA(3, spd->ModuleType,
                "module_type    Key Byte / Module Type");
       DEBUG_DXA(4, spd->DensityBanks,
                "density_banks  SDRAM Density and Banks");
       DEBUG_DXA(5, spd->Addressing,
                "addressing     SDRAM Addressing");
       DEBUG_DXA(6, spd->PackageType,
                "package_type   Package type");
       DEBUG_DXA(7, spd->OptFeature,
                "opt_feature    Optional features");
       DEBUG_DXA(8, spd->ThermalRef,
                "thermal_ref    Thermal and Refresh options");
       DEBUG_DXA(9, spd->OthOptFeatures,
                "oth_opt_features Other SDRAM optional features");
       DEBUG_DXA(10, spd->Res10,
                "res_10         Reserved");
       DEBUG_DXA(11, spd->ModuleVdd,
                "module_vdd     Module Nominal Voltage, VDD");
       DEBUG_DXA(12, spd->Organization,
                "organization Module Organization");
       DEBUG_DXA(13, spd->BusWidth,
                "bus_width      Module Memory Bus Width");
       DEBUG_DXA(14, spd->ThermSensor,
                "therm_sensor   Module Thermal Sensor");
       DEBUG_DXA(15, spd->ExtType,
                "ext_type       Extended module type");
       DEBUG_DXA(16, spd->Res16,
                "res_16       Reserved");
       DEBUG_DXA(17, spd->Timebases,
                "timebases    MTb and FTB");
       DEBUG_DXA(18, spd->TckMin,
                "tck_min      tCKAVGmin");
       DEBUG_DXA(19, spd->TckMax,
                "tck_max      TCKAVGmax");
       DEBUG_DXA(20, spd->CaslatB1,
                "caslat_b1    CAS latencies, 1st byte");
       DEBUG_DXA(21, spd->CaslatB2,
                "caslat_b2    CAS latencies, 2nd byte");
       DEBUG_DXA(22, spd->CaslatB3,
                "caslat_b3    CAS latencies, 3rd byte ");
       DEBUG_DXA(23, spd->CaslatB4,
                "caslat_b4    CAS latencies, 4th byte");
       DEBUG_DXA(24, spd->TaaMin,
                "taaMin      Min CAS Latency Time");
       DEBUG_DXA(25, spd->TrcdMin,
                "trcdMin     Min RAS# to CAS# Delay Time");
       DEBUG_DXA(26, spd->TrpMin,
                "trpMin      Min Row Precharge Delay Time");
       DEBUG_DXA(27, spd->TrasTrcExt,
                "tras_trc_ext Upper Nibbles for tRAS and tRC");
       DEBUG_DXA(28, spd->TrasMinLsb,
                "trasMin_lsb tRASmin, lsb");
       DEBUG_DXA(29, spd->TrcMinLsb,
                "trcMin_lsb  tRCmin, lsb");
       DEBUG_DXA(30, spd->Trfc1MinLsb,
                "trfc1Min_lsb  Min Refresh Recovery Delay Time, LSB");
       DEBUG_DXA(31, spd->Trfc1MinMsb,
                "trfc1Min_msb  Min Refresh Recovery Delay Time, MSB ");
       DEBUG_DXA(32, spd->Trfc2MinLsb,
                "trfc2Min_lsb  Min Refresh Recovery Delay Time, LSB");
       DEBUG_DXA(33, spd->Trfc2MinMsb,
                "trfc2Min_msb  Min Refresh Recovery Delay Time, MSB");
       DEBUG_DXA(34, spd->Trfc4MinLsb,
                "trfc4Min_lsb Min Refresh Recovery Delay Time, LSB");
       DEBUG_DXA(35, spd->Trfc4MinMsb,
                "trfc4Min_msb Min Refresh Recovery Delay Time, MSB");
       DEBUG_DXA(36, spd->TfawMsb,
                "tfaw_msb      Upper Nibble for tFAW");
       DEBUG_DXA(37, spd->TfawMin,
                "tfawMin      tFAW, lsb");
       DEBUG_DXA(38, spd->TrrdsMin,
                "trrdsMin     tRRD_Smin, MTB");
       DEBUG_DXA(39, spd->TrrdlMin,
                "trrdlMin     tRRD_Lmin, MTB");
       DEBUG_DXA(40, spd->TccdlMin,
                "tccdlMin     tCCS_Lmin, MTB");

       DEBUG((EFI_D_INFO,"%-3d-%3d: ", 41, 59));  /* Reserved, General Section */
       for (i = 41; i <= 59; i++)
              DEBUG((EFI_D_INFO,"%02x ", spd->Res41[i - 41]));

       DEBUG((EFI_D_INFO,"\n"));
       DEBUG((EFI_D_INFO,"%-3d-%3d: ", 60, 77));
       for (i = 60; i <= 77; i++)
              DEBUG((EFI_D_INFO,"%02x ", spd->Mapping[i - 60]));
       DEBUG((EFI_D_INFO,"   mapping[] Connector to SDRAM bit map\n"));

       DEBUG_DXA(117, spd->FineTccdlMin,
                "fineTccdlMin Fine offset of tCCD_Lmin");
       DEBUG_DXA(118, spd->FineTrrdlMin,
                "fineTrrdlMin Fine offset of tRRD_Lmin");
       DEBUG_DXA(119, spd->FineTrrdsMin,
                "fineTrrdsMin Fine offset of tRRD_Smin");
       DEBUG_DXA(120, spd->FineTrcMin,
                "fineTrcMin   Fine offset of tRCmin");
       DEBUG_DXA(121, spd->FineTrpMin,
                "fineTrpMin   Fine offset of tRPmin");
       DEBUG_DXA(122, spd->FineTrcdMin,
                "fineTrcdMin  Fine offset of tRCDmin");
       DEBUG_DXA(123, spd->FineTaaMin,
                "fineTaaMin   Fine offset of tAAmin");
       DEBUG_DXA(124, spd->FineTckMax,
                "fineTckMax   Fine offset of tCKAVGmax");
       DEBUG_DXA(125, spd->FineTckMin,
                "fineTckMin   Fine offset of tCKAVGmin");

       /* CRC Bytes 126-127 */
       DEBUG_DDXXA(126, 127, spd->Crc[0], spd->Crc[1], "  SPD CRC");

       switch (spd->ModuleType) {
	/* RDIMM */
       case 1:
              DEBUG_DXA(128, spd->ModSection.Registered.ModHeight,
                       "mod_height    Registered : Module Nominal Height");
              DEBUG_DXA(129, spd->ModSection.Registered.ModThickness,
                       "mod_thickness Registered : Module Maximum Thickness");
              DEBUG_DXA(130, spd->ModSection.Registered.RefRawCard,
                       "ref_raw_card  Registered : Reference Raw Card Used");
              DEBUG_DXA(131, spd->ModSection.Registered.ModuAttr,
                       "modu_attr     Registered : DIMM Module Attributes");
              DEBUG_DXA(132, spd->ModSection.Registered.Thermal,
                       "thermal       Registered : Thermal Heat Spreader Solution");
              DEBUG_DXA(133, spd->ModSection.Registered.RegIdLo,
                       "Regid_lo      Registered : Register Manufacturer ID Code, LSB");
              DEBUG_DXA(134, spd->ModSection.Registered.RegIdHi,
                       "Regid_hi      Registered : Register Manufacturer ID Code, MSB");
              DEBUG_DXA(135, spd->ModSection.Registered.RegRev,
                       "Regrev        Registered : Register Revision Number");
              DEBUG_DXA(136, spd->ModSection.Registered.RegMap,
                       "Regmap        Registered : Address mapping");
              DEBUG_DDXXA(254, 255, spd->ModSection.Registered.Crc[0],
                         spd->ModSection.Registered.Crc[1], "  Module CRC");
              break;
	/* UDIMM */
       case 2:
	/* SO-DIMM */
       case 3:
              DEBUG_DXA(128, spd->ModSection.Unbuffered.ModHeight,
                       "mod_height    Unbuffered : Module Nominal Height");
              DEBUG_DXA(129, spd->ModSection.Unbuffered.ModThickness,
                       "mod_thickness Unbuffered : Module Maximum Thickness");
              DEBUG_DXA(130, spd->ModSection.Unbuffered.RefRawCard,
                       "ref_raw_card  Unbuffered : Reference Raw Card Used");
              DEBUG_DXA(131, spd->ModSection.Unbuffered.AddrMapping,
                       "addr_mapping  Unbuffered : Address mapping from Edge Connector to DRAM");
              DEBUG_DDXXA(254, 255, spd->ModSection.Unbuffered.Crc[0],
                         spd->ModSection.Unbuffered.Crc[1], "  Module CRC");
              break;
	/* LRDIMM */
       case 4:
              DEBUG_DXA(128, spd->ModSection.Loadreduced.ModHeight,
                       "mod_height    Loadreduced : Module Nominal Height");
              DEBUG_DXA(129, spd->ModSection.Loadreduced.ModThickness,
                       "mod_thickness Loadreduced : Module Maximum Thickness");
              DEBUG_DXA(130, spd->ModSection.Loadreduced.RefRawCard,
                       "ref_raw_card  Loadreduced : Reference Raw Card Used");
              DEBUG_DXA(131, spd->ModSection.Loadreduced.ModuAttr,
                       "modu_attr     Loadreduced : DIMM Module Attributes");
              DEBUG_DXA(132, spd->ModSection.Loadreduced.Thermal,
                       "thermal       Loadreduced : Thermal Heat Spreader Solution");
              DEBUG_DXA(133, spd->ModSection.Loadreduced.RegIdLo,
                       "Regid_lo     Loadreduced : Register Manufacturer ID Code, LSB");
              DEBUG_DXA(134, spd->ModSection.Loadreduced.RegIdHi,
                       "Regid_hi     Loadreduced : Register Manufacturer ID Code, MSB");
              DEBUG_DXA(135, spd->ModSection.Loadreduced.RegRev,
                       "Regrev       Loadreduced : Register Revision Number");
              DEBUG_DXA(136, spd->ModSection.Loadreduced.RegMap,
                       "Regmap       Loadreduced : Address mapping");
              DEBUG_DXA(137, spd->ModSection.Loadreduced.RegDrv,
                       "Regdrv       Loadreduced : Reg output drive strength");
              DEBUG_DXA(138, spd->ModSection.Loadreduced.RegDrvCk,
                       "Regdrv_ck    Loadreduced : Reg output drive strength for CK");
              DEBUG_DXA(139, spd->ModSection.Loadreduced.DataBufRev,
                       "data_buf_rev  Loadreduced : Data Buffer Revision Numbe");
              DEBUG_DXA(140, spd->ModSection.Loadreduced.VrefqeR0,
                       "VrefqeR0     Loadreduced : DRAM VrefDQ for Package Rank 0");
              DEBUG_DXA(141, spd->ModSection.Loadreduced.VrefqeR1,
                       "VrefqeR1     Loadreduced : DRAM VrefDQ for Package Rank 1");
              DEBUG_DXA(142, spd->ModSection.Loadreduced.VrefqeR2,
                       "VrefqeR2     Loadreduced : DRAM VrefDQ for Package Rank 2");
              DEBUG_DXA(143, spd->ModSection.Loadreduced.VrefqeR3,
                       "VrefqeR3     Loadreduced : DRAM VrefDQ for Package Rank 3");
              DEBUG_DXA(144, spd->ModSection.Loadreduced.DataIntf,
                       "data_intf     Loadreduced : Data Buffer VrefDQ for DRAM Interface");
              DEBUG_DXA(145, spd->ModSection.Loadreduced.DataDrv1866,
                       "DataDrv1866 Loadreduced : Data Buffer MDQ Drive Strength and RTT");
              DEBUG_DXA(146, spd->ModSection.Loadreduced.DataDrv2400,
                       "DataDrv2400 Loadreduced : Data Buffer MDQ Drive Strength and RTT");
              DEBUG_DXA(147, spd->ModSection.Loadreduced.DataDrv3200,
                       "data_drv_3200 Loadreduced : Data Buffer MDQ Drive Strength and RTT");
              DEBUG_DXA(148, spd->ModSection.Loadreduced.DramDrv,
                       "dram_drv      Loadreduced : DRAM Drive Strength");
              DEBUG_DXA(149, spd->ModSection.Loadreduced.DramOdt1866,
                       "DramOdt1866 Loadreduced : DRAM ODT (RTT_WR, RTT_NOM)");
              DEBUG_DXA(150, spd->ModSection.Loadreduced.DramOdt2400,
                       "DramOdt2400 Loadreduced : DRAM ODT (RTT_WR, RTT_NOM)");
              DEBUG_DXA(151, spd->ModSection.Loadreduced.DramOdt3200,
                       "DramOdt3200 Loadreduced : DRAM ODT (RTT_WR, RTT_NOM)");
              DEBUG_DXA(152, spd->ModSection.Loadreduced.DramOdtPark1866,
                       "DramOdtpark_1866 Loadreduced : DRAM ODT (RTT_PARK)");
              DEBUG_DXA(153, spd->ModSection.Loadreduced.DramOdtPark2400,
                       "DramOdtpark_2400 Loadreduced : DRAM ODT (RTT_PARK)");
              DEBUG_DXA(154, spd->ModSection.Loadreduced.DramOdtPark3200,
                       "dram_odt_park_3200 Loadreduced : DRAM ODT (RTT_PARK)");
              DEBUG_DDXXA(254, 255, spd->ModSection.Loadreduced.Crc[0],
                         spd->ModSection.Loadreduced.Crc[1],
                         "  Module CRC");
              break;
       default:
              /* Module-specific Section, Unsupported Module Type */
              DEBUG((EFI_D_INFO,"%-3d-%3d: ", 128, 255));

              for (i = 128; i <= 255; i++)
                     DEBUG((EFI_D_INFO,"%02x", spd->ModSection.Uc[i - 128]));

              break;
       }

       /* Unique Module ID: Bytes 320-383 */
       DEBUG_DXA(320, spd->MmidLsb, "Module MfgID Code LSB - JEP-106");
       DEBUG_DXA(321, spd->MmidMsb, "Module MfgID Code MSB - JEP-106");
       DEBUG_DXA(322, spd->Mloc,     "Mfg Location");
       DEBUG_DDXXA(323, 324, spd->Mdate[0], spd->Mdate[1], "Mfg Date");

       DEBUG((EFI_D_INFO,"%-3d-%3d: ", 325, 328));

       for (i = 325; i <= 328; i++)
              DEBUG((EFI_D_INFO,"%02x ", spd->Sernum[i - 325]));
       DEBUG((EFI_D_INFO,"   Module Serial Number\n"));

       DEBUG((EFI_D_INFO,"%-3d-%3d: ", 329, 348));
       for (i = 329; i <= 348; i++)
              DEBUG((EFI_D_INFO,"%02x ", spd->Mpart[i - 329]));
       DEBUG((EFI_D_INFO,"   Mfg's Module Part Number\n"));

       DEBUG_DXA(349, spd->Mrev, "Module Revision code");
       DEBUG_DXA(350, spd->DmidLsb, "DRAM MfgID Code LSB - JEP-106");
       DEBUG_DXA(351, spd->DmidMsb, "DRAM MfgID Code MSB - JEP-106");
       DEBUG_DXA(352, spd->Stepping, "DRAM stepping");

       DEBUG((EFI_D_INFO,"%-3d-%3d: ", 353, 381));
       for (i = 353; i <= 381; i++)
              DEBUG((EFI_D_INFO,"%02x ", spd->Msd[i - 353]));
       DEBUG((EFI_D_INFO,"   Mfg's Specific Data\n"));
}

VOID
ReadSpdInfo (
  OUT  GenericSpdEepromT    *Spd,
  IN   UINT8                I2cAddr
  )
{
  UINT32 Ret;

#ifdef MEMORY_TYPE_DDR4
#define SPD_SPA0_ADDR  0x36
#define SPD_SPA1_ADDR  0x37
  UINT8 Dummy = 0;
  VOID *I2cBaseAddr = (VOID *)DDR_SPD_I2C_BASE_ADDR;

  I2cBusInit(DDR_SPD_I2C_BUS, I2C_SPEED);
  I2cDataWrite(I2cBaseAddr, SPD_SPA0_ADDR, 0, 1, &Dummy, 1);
  Ret = I2cDataRead(I2cBaseAddr, I2cAddr, 0, 1, (UINT8 *)Spd, 256);

  if (Ret == EFI_SUCCESS) {
     Dummy = 1;
     I2cDataWrite(I2cBaseAddr, SPD_SPA1_ADDR, 0, 1, &Dummy, 1);
     Ret = I2cDataRead(I2cBaseAddr, I2cAddr, 0, 1, (UINT8 *)((UINTN)Spd + 256),
                       Min(256, (INT32)sizeof(GenericSpdEepromT) - 256));
  }
#else
  Ret = I2cDataRead(I2cBaseAddr, I2cAddr, 0, 1, (UINT8 *)Spd,
                    sizeof(GenericSpdEepromT));
#endif

  if (Ret != EFI_SUCCESS) {
     if (I2cAddr == DDR_SPD_I2C_ADDRESS1) {
        DEBUG((EFI_D_ERROR, "Failed to read SPD from address %u\n", I2cAddr));
     }
     //InternalMemZeroMem(Spd, sizeof(GenericSpdEepromT));
     DEBUG((EFI_D_ERROR, "using static SPD data to fix I2C stability\n"));
     DdrCompareSpd(Spd, 1);
     return;
  }
//  DdrCompareSpd(Spd, 0);
  return;
}


VOID
DdrReadSpd (
  OUT  GenericSpdEepromT  *DimmSpd,
  IN   UINT32             CtrlNum
  )
{
  UINT32 I;

  for (I = 0; I < 1; I++) {
     ReadSpdInfo(&(DimmSpd[I]), SpdI2cAddress[CtrlNum][I]);
  }
}
#endif /* DDR_SPD_I2C_ADDRESS */

#if defined(MEMORY_TYPE_DDR3)
UINT32
Ddr3SpdCheck (
  IN   CONST Ddr3SpdEepromT   *Spd
  )
{
  UINT8 *P = (UINT8 *)Spd;
  INT32 Csum16;
  INT32 Len;
  UINT8 CrcLsb;
  UINT8 CrcMsb;

  Len = !(Spd->InfoSizeCrc & 0x80) ? 126 : 117;
  Csum16 = Crc16(P, Len);

  CrcLsb = (UINT8) (Csum16 & 0xff);
  CrcMsb = (UINT8) (Csum16 >> 8);

  if (Spd->Crc[0] == CrcLsb && Spd->Crc[1] == CrcMsb) {
     return 0;
  } else {
     DEBUG((EFI_D_INFO,"DDR3 SPD checksum error\n"));
     return 1;
  }
}

#elif defined(MEMORY_TYPE_DDR4)
UINT32
Ddr4SpdCheck (
  IN  CONST Ddr4SpdEepromT *Spd
  )
{
   UINT8 *P = (UINT8 *)Spd;
   INT32 Csum16;
   INT32 Len;
   UINT8 CrcLsb;
   UINT8 CrcMsb;

   Len = 126;
   Csum16 = Crc16(P, Len);

   CrcLsb = (UINT8) (Csum16 & 0xff);
   CrcMsb = (UINT8) (Csum16 >> 8);

   if (Spd->Crc[0] != CrcLsb || Spd->Crc[1] != CrcMsb) {
      DEBUG((EFI_D_ERROR, "DDR4 SPD checksum error\n"));
      return 1;
   }

   P = (UINT8 *)((UINTN)Spd + 128);
   Len = 126;
   Csum16 = Crc16(P, Len);

   CrcLsb = (UINT8) (Csum16 & 0xff);
   CrcMsb = (UINT8) (Csum16 >> 8);

   if (Spd->ModSection.Uc[126] != CrcLsb ||
       Spd->ModSection.Uc[127] != CrcMsb) {
      DEBUG((EFI_D_ERROR, "DDR4 SPD checksum unexpected\n"));
      return 1;
   }

   return 0;
}
#endif


#ifdef MEMORY_TYPE_DDR3
STATIC UINT64
ComputeRanksize (
  IN  CONST struct Ddr3SpdEepromS  *Spd
  )
{
  UINT64 BankSize;

  INT32 NbitSdramCapBsize = 0;
  INT32 NbitPrimaryBusWidth = 0;
  INT32 NbitSdramWidth = 0;

  if ((Spd->DensityBanks & 0xf) < 7) {
    NbitSdramCapBsize = (Spd->DensityBanks & 0xf) + 28;
  }

  if ((Spd->BusWidth & 0x7) < 4) {
    NbitPrimaryBusWidth = (Spd->BusWidth & 0x7) + 3;
  }

  if ((Spd->Organization & 0x7) < 4) {
    NbitSdramWidth = (Spd->Organization & 0x7) + 2;
  }

  BankSize = 1ULL << (NbitSdramCapBsize - 3 +
             NbitPrimaryBusWidth - NbitSdramWidth);

  DEBUG((EFI_D_INFO,"DDR3 Rank density = 0x%llx\n", BankSize));

  return BankSize;
}


UINT32
DdrComputeDimmParameters (
  IN  CONST GenericSpdEepromT   *Spd,
  OUT  DimmParamsT              *Pdimm,
  IN   UINT32                   DimmNumber
  )
{
  UINT32 Retval;
  UINT32 MtbPs;
  INT32 Ftb10thPs;
  INT32 I;

  if (Spd->MemType) {
    if (Spd->MemType != SPD_MEMTYPE_DDR3) {
      return 1;
    }
  } else {
    InternalMemZeroMem(Pdimm, sizeof(DimmParamsT));
    return 1;
  }

  Retval = Ddr3SpdCheck(Spd);
  if (Retval) {
    return 2;
  }

  InternalMemZeroMem(Pdimm->Mpart, sizeof(Pdimm->Mpart));
  if ((Spd->InfoSizeCrc & 0xF) > 1) {
    InternalMemCopyMem(Pdimm->Mpart, Spd->Mpart, sizeof(Pdimm->Mpart) - 1);
  }

  Pdimm->NRanks = ((Spd->Organization >> 3) & 0x7) + 1;
  Pdimm->RankDensity = ComputeRanksize(Spd);
  Pdimm->Capacity = Pdimm->NRanks * Pdimm->RankDensity;
  Pdimm->PrimarySdramWidth = 1 << (3 + (Spd->BusWidth & 0x7));
  if ((Spd->BusWidth >> 3) & 0x3) {
    Pdimm->EcSdramWidth = 8;
  } else {
    Pdimm->EcSdramWidth = 0;
  }
  Pdimm->DataWidth = Pdimm->PrimarySdramWidth + Pdimm->EcSdramWidth;
  Pdimm->DeviceWidth = 1 << ((Spd->Organization & 0x7) + 2);
  Pdimm->MirroredDimm = 0;
  Pdimm->RegisteredDimm = 0;

  switch (Spd->ModuleType & DDR_MODULETYPE_MASK) {
    case DDR_MODULETYPE_RDIMM:
    case DDR_MODULETYPE_MINI_RDIMM:
    case DDR_MODULETYPE_72B_SO_RDIMM:
      Pdimm->RegisteredDimm = 1;
      for (I = 0; I < 16; I += 2) {
         UINT8 Rcw = Spd->ModSection.Registered.Rcw[I/2];
         Pdimm->Rcw[I]   = (Rcw >> 0) & 0x0F;
         Pdimm->Rcw[I+1] = (Rcw >> 4) & 0x0F;
      }
      break;
    case DDR_MODULETYPE_UDIMM:
    case DDR_MODULETYPE_SO_DIMM:
    case DDR_MODULETYPE_MINI_UDIMM:
    case DDR_MODULETYPE_MINI_CDIMM:
    case DDR_MODULETYPE_LRDIMM:
    case DDR_MODULETYPE_16B_SO_DIMM:
    case DDR_MODULETYPE_32B_SO_DIMM:
    case DDR_MODULETYPE_72B_SO_CDIMM:
    case DDR_MODULETYPE_72B_SO_UDIMM:
      if (Spd->ModSection.Unbuffered.AddrMapping & 0x1) {
        Pdimm->MirroredDimm = 1;
      }
      break;
    default:
      return 1;
  }

  Pdimm->NRowAddr = ((Spd->Addressing >> 3) & 0x7) + 12;
  Pdimm->NColAddr = (Spd->Addressing & 0x7) + 9;
  Pdimm->NBanksPerSdramDevice = 8 << ((Spd->DensityBanks >> 4) & 0x7);

  if (Pdimm->EcSdramWidth) {
    Pdimm->EdcConfig = 0x02;
  } else {
    Pdimm->EdcConfig = 0x00;
  }

  Pdimm->BurstLengthsBitmask = 0x0c;
  Pdimm->RowDensity = __ILog2(Pdimm->RankDensity);
  MtbPs = (Spd->MtbDividend * 1000) /Spd->MtbDivisor;
  Pdimm->MtbPs = MtbPs;
  Ftb10thPs = ((Spd->FtbDiv & 0xf0) >> 4) * 10 / (Spd->FtbDiv & 0x0f);
  Pdimm->Ftb10thPs = Ftb10thPs;
  Pdimm->TckminXPs = Spd->TckMin * MtbPs + (Spd->FineTckMin * Ftb10thPs) / 10;
  Pdimm->CaslatX  = ((Spd->CaslatMsb << 8) | Spd->CaslatLsb) << 4;
  Pdimm->TaaPs = Spd->TaaMin * MtbPs + (Spd->FineTaaMin * Ftb10thPs) / 10;
  Pdimm->TwrPs = Spd->TwrMin * MtbPs;
  Pdimm->TrcdPs = Spd->TrcdMin * MtbPs + (Spd->FineTrcdMin * Ftb10thPs) / 10;
  Pdimm->TrrdPs = Spd->TrrdMin * MtbPs;
  Pdimm->TrpPs = Spd->TrpMin * MtbPs + (Spd->FineTrpMin * Ftb10thPs) / 10;
  Pdimm->TrasPs = (((Spd->TrasTrcExt & 0xf) << 8) | Spd->TrasMinLsb) * MtbPs;
  Pdimm->TrcPs = (((Spd->TrasTrcExt & 0xf0) << 4) | Spd->TrcMinLsb) * MtbPs +
                   (Spd->FinetrcMin * Ftb10thPs) / 10;

  Pdimm->TrfcPs = ((Spd->TrfcMinMsb << 8) | Spd->TrfcMinLsb) * MtbPs;
  Pdimm->TwtrPs = Spd->TwtrMin * MtbPs;
  Pdimm->TrtpPs = Spd->TrtpMin * MtbPs;
  Pdimm->RefreshRatePs = 7800000;
  if ((Spd->ThermRefOpt & 0x1) && !(Spd->ThermRefOpt & 0x2)) {
    Pdimm->RefreshRatePs = 3900000;
    Pdimm->ExtendedOpSrt = 1;
  }

  Pdimm->TfawPs = (((Spd->TfawMsb & 0xf) << 8) | Spd->TfawMin) * MtbPs;

  return 0;
}

#elif defined(MEMORY_TYPE_DDR4)
STATIC UINT64
ComputeRanksize  (
  IN  CONST struct Ddr4SpdEepromS  *Spd
  )
{
  UINT64 Bsize;

  INT32 NbitSdramCapBsize = 0;
  INT32 NbitPrimaryBusWidth = 0;
  INT32 NbitSdramWidth = 0;
  INT32 DieCount = 0;
  BOOLEAN Package3ds;

  if ((Spd->DensityBanks & 0xf) <= 7)
    NbitSdramCapBsize = (Spd->DensityBanks & 0xf) + 28;
  if ((Spd->BusWidth & 0x7) < 4)
    NbitPrimaryBusWidth = (Spd->BusWidth & 0x7) + 3;
  if ((Spd->Organization & 0x7) < 4)
    NbitSdramWidth = (Spd->Organization & 0x7) + 2;
    Package3ds = (Spd->PackageType & 0x3) == 0x2;
  if (Package3ds)
    DieCount = (Spd->PackageType >> 4) & 0x7;

  Bsize = 1ULL << (NbitSdramCapBsize - 3 +
         NbitPrimaryBusWidth - NbitSdramWidth +
         DieCount);

  DEBUG((EFI_D_INFO,"DDR: Rank Density = 0x%llx\n", Bsize));

  return Bsize;
}

#define SpdToPs(Mtb, Ftb) \
        (Mtb * Pdimm->MtbPs + (Ftb * Pdimm->Ftb10thPs) / 10)

UINT32
DdrComputeDimmParameters (
  IN  CONST GenericSpdEepromT    *Spd,
  OUT DimmParamsT                *Pdimm,
  IN  UINT32                      DimmNumber
  )
{
  UINT32 Retval;
  INT32 I;
  STATIC CONST UINT8 UdimmRcEDq[18] = {
        0x0c, 0x2c, 0x15, 0x35, 0x15, 0x35, 0x0b, 0x2c, 0x15,
        0x35, 0x0b, 0x35, 0x0b, 0x2c, 0x0b, 0x35, 0x15, 0x36
  };
  INT32 SpdError = 0;
  UINT8 *Ptr;

  if (Spd->MemType) {
    if (Spd->MemType != SPD_MEMTYPE_DDR4) {
      DEBUG((EFI_D_ERROR,"DIMM %d: is not a DDR4 SPD.\n", DimmNumber));
      return 1;
    }
  } else {
    InternalMemZeroMem(Pdimm, sizeof(DimmParamsT));
    return 1;
  }

  Retval = Ddr4SpdCheck(Spd);
  if (Retval != 0) {
        DEBUG((EFI_D_ERROR,"DIMM %d: failed checksum\n", DimmNumber));
        return 2;
  }

  InternalMemZeroMem(Pdimm->Mpart, sizeof(Pdimm->Mpart));
  if ((Spd->InfoSizeCrc & 0xF) > 2)
    InternalMemCopyMem(Pdimm->Mpart, Spd->Mpart, sizeof(Pdimm->Mpart) - 1);

  Pdimm->NRanks = ((Spd->Organization >> 3) & 0x7) + 1;
  Pdimm->RankDensity = ComputeRanksize(Spd);
  Pdimm->Capacity = Pdimm->NRanks * Pdimm->RankDensity;
  Pdimm->PrimarySdramWidth = 1 << (3 + (Spd->BusWidth & 0x7));
  if ((Spd->BusWidth >> 3) & 0x3) {
    Pdimm->EcSdramWidth = 8;
  } else {
    Pdimm->EcSdramWidth = 0;
  }

  Pdimm->DataWidth = Pdimm->PrimarySdramWidth + Pdimm->EcSdramWidth;
  Pdimm->DeviceWidth = 1 << ((Spd->Organization & 0x7) + 2);

  Pdimm->MirroredDimm = 0;
  Pdimm->RegisteredDimm = 0;
  switch (Spd->ModuleType & DDR_MODULETYPE_MASK) {
    case DDR_MODULETYPE_RDIMM:
      Pdimm->RegisteredDimm = 1;
      break;

    case DDR_MODULETYPE_UDIMM:
    case DDR_MODULETYPE_SO_DIMM:
      if (Spd->ModSection.Unbuffered.AddrMapping & 0x1) {
        Pdimm->MirroredDimm = 1;
      }
      if ((Spd->ModSection.Unbuffered.ModHeight & 0xe0) == 0 &&
         (Spd->ModSection.Unbuffered.RefRawCard == 0x04)) {
         for (I = 0; I < 18; I++) {
           if (Spd->Mapping[I] == UdimmRcEDq[I]) {
             continue;
           }
           SpdError = 1;
           DEBUG((EFI_D_ERROR, "SPD Byte %d: 0x%x, should be 0x%x\n",
                  60 + I, Spd->Mapping[I], UdimmRcEDq[I]));
           Ptr = (UINT8 *)&Spd->Mapping[I];
           *Ptr = UdimmRcEDq[I];
         }
         if (SpdError) {
           DEBUG((EFI_D_INFO,"SPD DQ mapping error fixed\n"));
         }
      }
      break;

    default:
      DEBUG((EFI_D_ERROR,"unknown module type 0x%02x\n", Spd->ModuleType));
      return 1;
  }

  Pdimm->NRowAddr = ((Spd->Addressing >> 3) & 0x7) + 12;
  Pdimm->NColAddr = (Spd->Addressing & 0x7) + 9;
  Pdimm->BankAddrBits = (Spd->DensityBanks >> 4) & 0x3;
  Pdimm->BankGroupBits = (Spd->DensityBanks >> 6) & 0x3;

  if (Pdimm->EcSdramWidth) {
    Pdimm->EdcConfig = 0x02;
  } else {
    Pdimm->EdcConfig = 0x00;
  }

  Pdimm->BurstLengthsBitmask = 0x0c;
  Pdimm->RowDensity = __ILog2(Pdimm->RankDensity);

  if ((Spd->Timebases & 0xf) == 0x0) {
    Pdimm->MtbPs = 125;
    Pdimm->Ftb10thPs = 10;
  } else {
    DEBUG((EFI_D_ERROR,"unknown timebase\n"));
  }

  Pdimm->TckminXPs = SpdToPs(Spd->TckMin, Spd->FineTckMin);
  Pdimm->TckmaxPs = SpdToPs(Spd->TckMax, Spd->FineTckMax);
  Pdimm->CaslatX  = (Spd->CaslatB1 << 7) |
                    (Spd->CaslatB2 << 15) |
                    (Spd->CaslatB3 << 23);
  ASSERT(Spd->CaslatB4 == 0);

  Pdimm->TaaPs = SpdToPs(Spd->TaaMin, Spd->FineTaaMin);
  Pdimm->TrcdPs = SpdToPs(Spd->TrcdMin, Spd->FineTrcdMin);
  Pdimm->TrpPs = SpdToPs(Spd->TrpMin, Spd->FineTrpMin);
  Pdimm->TrasPs = (((Spd->TrasTrcExt & 0xf) << 8) +
                     Spd->TrasMinLsb) * Pdimm->MtbPs;

  Pdimm->TrcPs = SpdToPs((((Spd->TrasTrcExt & 0xf0) << 4) +
                            Spd->TrcMinLsb), Spd->FineTrcMin);
  Pdimm->Trfc1Ps = ((Spd->Trfc1MinMsb << 8) |
                    (Spd->Trfc1MinLsb)) * Pdimm->MtbPs;
  Pdimm->Trfc2Ps = ((Spd->Trfc2MinMsb << 8) |
                    (Spd->Trfc2MinLsb)) * Pdimm->MtbPs;
  Pdimm->Trfc4Ps = ((Spd->Trfc4MinMsb << 8) |
                    (Spd->Trfc4MinLsb)) * Pdimm->MtbPs;
  Pdimm->TfawPs = (((Spd->TfawMsb & 0xf) << 8) |
                     Spd->TfawMin) * Pdimm->MtbPs;

  Pdimm->TrrdsPs = SpdToPs(Spd->TrrdsMin, Spd->FineTrrdsMin);
  Pdimm->TrrdlPs = SpdToPs(Spd->TrrdlMin, Spd->FineTrrdlMin);
  Pdimm->TccdlPs = SpdToPs(Spd->TccdlMin, Spd->FineTccdlMin);
  Pdimm->RefreshRatePs = 7800000;

  for (I = 0; I < 18; I++) {
    Pdimm->DqMapping[I] = Spd->Mapping[I];
  }
  Pdimm->DqMappingOrs = ((Spd->Mapping[0] >> 6) & 0x3) == 0 ? 1 : 0;

  return 0;
}
#endif
