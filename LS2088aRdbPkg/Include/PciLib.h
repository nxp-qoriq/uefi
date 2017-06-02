/** PciLib.h
  Pci Library containing Defines for Pci Controller configuration etc

  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PCI_LIB_H__
#define __PCI_LIB_H__

/*
 * In PCI, devices have 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */
#define LS_PCI_VENDOR_ID           0x00    /* 16 bits */
#define LS_PCI_DEVICE_ID           0x02    /* 16 bits */
#define LS_PCI_COMMAND             0x04    /* 16 bits */
#define  LS_PCI_COMMAND_IO         0x1     /* Enable response in I/O space */
#define  LS_PCI_COMMAND_MEMORY     0x2     /* Enable response in Memory space */
#define  LS_PCI_COMMAND_MASTER     0x4     /* Enable bus mastering */
#define  LS_PCI_COMMAND_SPECIAL    0x8     /* Enable response to special cycles */
#define  LS_PCI_COMMAND_INVALIDATE 0x10    /* Use memory write and invalidate */
#define  LS_PCI_COMMAND_VGA_PALETTE 0x20   /* Enable palette snooping */
#define  LS_PCI_COMMAND_PARITY     0x40    /* Enable parity checking */
#define  LS_PCI_COMMAND_WAIT       0x80    /* Enable address/data stepping */
#define  LS_PCI_COMMAND_SERR       0x100   /* Enable SERR */
#define  LS_PCI_COMMAND_FAST_BACK  0x200   

#define LS_PCI_STATUS              0x06    /* 16 bits */
#define  LS_PCI_STATUS_CAP_LIST    0x10    /* Support Capability List */
#define  LS_PCI_STATUS_66MHZ       0x20  
#define  LS_PCI_STATUS_UDF         0x40  
#define  LS_PCI_STATUS_FAST_BACK   0x80  
#define  LS_PCI_STATUS_PARITY      0x100 
#define  LS_PCI_STATUS_DEVSEL_MASK 0x600 
#define  LS_PCI_STATUS_DEVSEL_FAST 0x000
#define  LS_PCI_STATUS_DEVSEL_MEDIUM 0x200
#define  LS_PCI_STATUS_DEVSEL_SLOW 0x400
#define  LS_PCI_STATUS_SIG_TARGET_ABORT 0x800 
#define  LS_PCI_STATUS_REC_TARGET_ABORT 0x1000
#define  LS_PCI_STATUS_REC_MASTER_ABORT 0x2000 
#define  LS_PCI_STATUS_SIG_SYSTEM_ERROR 0x4000 
#define  LS_PCI_STATUS_DETECTED_PARITY 0x8000 

#define LS_PCI_CLASS_REVISION      0x08    
#define LS_PCI_REVISION_ID         0x08    /* Revision ID */
#define LS_PCI_CLASS_PROG          0x09    
#define LS_PCI_CLASS_DEVICE        0x0a    /* Device class */
#define LS_PCI_CLASS_CODE          0x0b    /* Device class code */
#define  LS_PCI_CLASS_CODE_TOO_OLD 0x00
#define  LS_PCI_CLASS_CODE_STORAGE 0x01
#define  LS_PCI_CLASS_CODE_NETWORK 0x02
#define  LS_PCI_CLASS_CODE_DISPLAY 0x03
#define  LS_PCI_CLASS_CODE_MULTIMEDIA 0x04
#define  LS_PCI_CLASS_CODE_MEMORY  0x05
#define  LS_PCI_CLASS_CODE_BRIDGE  0x06
#define  LS_PCI_CLASS_CODE_COMM    0x07
#define  LS_PCI_CLASS_CODE_PERIPHERAL 0x08
#define  LS_PCI_CLASS_CODE_INPUT   0x09
#define  LS_PCI_CLASS_CODE_DOCKING 0x0A
#define  LS_PCI_CLASS_CODE_PROCESSOR 0x0B
#define  LS_PCI_CLASS_CODE_SERIAL  0x0C
#define  LS_PCI_CLASS_CODE_WIRELESS 0x0D
#define  LS_PCI_CLASS_CODE_I2O     0x0E
#define  LS_PCI_CLASS_CODE_SATELLITE 0x0F
#define  LS_PCI_CLASS_CODE_CRYPTO  0x10
#define  LS_PCI_CLASS_CODE_DATA    0x11
/* Base Class 0x12 - 0xFE is reserved */
#define  LS_PCI_CLASS_CODE_OTHER   0xFF

#define LS_PCI_CLASS_SUB_CODE      0x0a    /* Device sub-class code */
#define  LS_PCI_CLASS_SUB_CODE_TOO_OLD_NOTVGA      0x00
#define  LS_PCI_CLASS_SUB_CODE_TOO_OLD_VGA         0x01
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_SCSI        0x00
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_IDE         0x01
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_FLOPPY      0x02
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_IPIBUS      0x03
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_RAID        0x04
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_ATA         0x05
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_SATA        0x06
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_SAS         0x07
#define  LS_PCI_CLASS_SUB_CODE_STORAGE_OTHER       0x80
#define  LS_PCI_CLASS_SUB_CODE_NETWORK_ETHERNET    0x00
#define  LS_PCI_CLASS_SUB_CODE_NETWORK_TOKENRING   0x01
#define  LS_PCI_CLASS_SUB_CODE_NETWORK_FDDI        0x02
#define  LS_PCI_CLASS_SUB_CODE_NETWORK_ATM         0x03
#define  LS_PCI_CLASS_SUB_CODE_NETWORK_ISDN        0x04
#define  LS_PCI_CLASS_SUB_CODE_NETWORK_WORLDFIP    0x05
#define  LS_PCI_CLASS_SUB_CODE_NETWORK_PICMG       0x06
#define  LS_PCI_CLASS_SUB_CODE_NETWORK_OTHER       0x80
#define  LS_PCI_CLASS_SUB_CODE_DISPLAY_VGA         0x00
#define  LS_PCI_CLASS_SUB_CODE_DISPLAY_XGA         0x01
#define  LS_PCI_CLASS_SUB_CODE_DISPLAY_3D          0x02
#define  LS_PCI_CLASS_SUB_CODE_DISPLAY_OTHER       0x80
#define  LS_PCI_CLASS_SUB_CODE_MULTIMEDIA_VIDEO    0x00
#define  LS_PCI_CLASS_SUB_CODE_MULTIMEDIA_AUDIO    0x01
#define  LS_PCI_CLASS_SUB_CODE_MULTIMEDIA_PHONE    0x02
#define  LS_PCI_CLASS_SUB_CODE_MULTIMEDIA_OTHER    0x80
#define  LS_PCI_CLASS_SUB_CODE_MEMORY_RAM          0x00
#define  LS_PCI_CLASS_SUB_CODE_MEMORY_FLASH        0x01
#define  LS_PCI_CLASS_SUB_CODE_MEMORY_OTHER        0x80
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_HOST         0x00
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_ISA          0x01
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_EISA         0x02
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_MCA          0x03
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_LS_PCI          0x04
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_PCMCIA       0x05
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_NUBUS        0x06
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_CARDBUS      0x07
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_RACEWAY      0x08
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_SEMI_LS_PCI     0x09
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_INFINIBAND   0x0A
#define  LS_PCI_CLASS_SUB_CODE_BRIDGE_OTHER        0x80
#define  LS_PCI_CLASS_SUB_CODE_COMM_SERIAL         0x00
#define  LS_PCI_CLASS_SUB_CODE_COMM_PARALLEL       0x01
#define  LS_PCI_CLASS_SUB_CODE_COMM_MULTIPORT      0x02
#define  LS_PCI_CLASS_SUB_CODE_COMM_MODEM          0x03
#define  LS_PCI_CLASS_SUB_CODE_COMM_GPIB           0x04
#define  LS_PCI_CLASS_SUB_CODE_COMM_SMARTCARD      0x05
#define  LS_PCI_CLASS_SUB_CODE_COMM_OTHER          0x80
#define  LS_PCI_CLASS_SUB_CODE_PERIPHERAL_PIC      0x00
#define  LS_PCI_CLASS_SUB_CODE_PERIPHERAL_DMA      0x01
#define  LS_PCI_CLASS_SUB_CODE_PERIPHERAL_TIMER    0x02
#define  LS_PCI_CLASS_SUB_CODE_PERIPHERAL_RTC      0x03
#define  LS_PCI_CLASS_SUB_CODE_PERIPHERAL_HOTPLUG  0x04
#define  LS_PCI_CLASS_SUB_CODE_PERIPHERAL_SD       0x05
#define  LS_PCI_CLASS_SUB_CODE_PERIPHERAL_OTHER    0x80
#define  LS_PCI_CLASS_SUB_CODE_INPUT_KEYBOARD      0x00
#define  LS_PCI_CLASS_SUB_CODE_INPUT_DIGITIZER     0x01
#define  LS_PCI_CLASS_SUB_CODE_INPUT_MOUSE         0x02
#define  LS_PCI_CLASS_SUB_CODE_INPUT_SCANNER       0x03
#define  LS_PCI_CLASS_SUB_CODE_INPUT_GAMEPORT      0x04
#define  LS_PCI_CLASS_SUB_CODE_INPUT_OTHER         0x80
#define  LS_PCI_CLASS_SUB_CODE_DOCKING_GENERIC     0x00
#define  LS_PCI_CLASS_SUB_CODE_DOCKING_OTHER       0x80
#define  LS_PCI_CLASS_SUB_CODE_PROCESSOR_386       0x00
#define  LS_PCI_CLASS_SUB_CODE_PROCESSOR_486       0x01
#define  LS_PCI_CLASS_SUB_CODE_PROCESSOR_PENTIUM   0x02
#define  LS_PCI_CLASS_SUB_CODE_PROCESSOR_ALPHA     0x10
#define  LS_PCI_CLASS_SUB_CODE_PROCESSOR_POWERPC   0x20
#define  LS_PCI_CLASS_SUB_CODE_PROCESSOR_MIPS      0x30
#define  LS_PCI_CLASS_SUB_CODE_PROCESSOR_COPROC    0x40
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_1394         0x00
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_ACCESSBUS    0x01
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_SSA          0x02
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_USB          0x03
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_FIBRECHAN    0x04
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_SMBUS        0x05
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_INFINIBAND   0x06
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_IPMI         0x07
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_SERCOS       0x08
#define  LS_PCI_CLASS_SUB_CODE_SERIAL_CANBUS       0x09
#define  LS_PCI_CLASS_SUB_CODE_WIRELESS_IRDA       0x00
#define  LS_PCI_CLASS_SUB_CODE_WIRELESS_IR         0x01
#define  LS_PCI_CLASS_SUB_CODE_WIRELESS_RF         0x10
#define  LS_PCI_CLASS_SUB_CODE_WIRELESS_BLUETOOTH  0x11
#define  LS_PCI_CLASS_SUB_CODE_WIRELESS_BROADBAND  0x12
#define  LS_PCI_CLASS_SUB_CODE_WIRELESS_80211A     0x20
#define  LS_PCI_CLASS_SUB_CODE_WIRELESS_80211B     0x21
#define  LS_PCI_CLASS_SUB_CODE_WIRELESS_OTHER      0x80
#define  LS_PCI_CLASS_SUB_CODE_I2O_V1_0            0x00
#define  LS_PCI_CLASS_SUB_CODE_SATELLITE_TV        0x01
#define  LS_PCI_CLASS_SUB_CODE_SATELLITE_AUDIO     0x02
#define  LS_PCI_CLASS_SUB_CODE_SATELLITE_VOICE     0x03
#define  LS_PCI_CLASS_SUB_CODE_SATELLITE_DATA      0x04
#define  LS_PCI_CLASS_SUB_CODE_CRYPTO_NETWORK      0x00
#define  LS_PCI_CLASS_SUB_CODE_CRYPTO_ENTERTAINMENT 0x10
#define  LS_PCI_CLASS_SUB_CODE_CRYPTO_OTHER        0x80
#define  LS_PCI_CLASS_SUB_CODE_DATA_DPIO           0x00
#define  LS_PCI_CLASS_SUB_CODE_DATA_PERFCNTR       0x01
#define  LS_PCI_CLASS_SUB_CODE_DATA_COMMSYNC       0x10
#define  LS_PCI_CLASS_SUB_CODE_DATA_MGMT           0x20
#define  LS_PCI_CLASS_SUB_CODE_DATA_OTHER          0x80

#define LS_PCI_CACHE_LINE_SIZE     0x0c    /* 8 bits */
#define LS_PCI_LATENCY_TIMER       0x0d    /* 8 bits */
#define LS_PCI_HEADER_TYPE         0x0e    /* 8 bits */
#define  LS_PCI_HEADER_TYPE_NORMAL 0
#define  LS_PCI_HEADER_TYPE_BRIDGE 1
#define  LS_PCI_HEADER_TYPE_CARDBUS 2

#define LS_PCI_BIST                0x0f    /* 8 bits */
#define LS_PCI_BIST_CODE_MASK      0x0f    /* Return result */
#define LS_PCI_BIST_START          0x40
#define LS_PCI_BIST_CAPABLE        0x80  

#define LS_PCI_BASE_ADDRESS_0      0x10    /* 32 bits */
#define LS_PCI_BASE_ADDRESS_1      0x14
#define LS_PCI_BASE_ADDRESS_2      0x18
#define LS_PCI_BASE_ADDRESS_3      0x1c    /* 32 bits */
#define LS_PCI_BASE_ADDRESS_4      0x20    /* 32 bits */
#define LS_PCI_BASE_ADDRESS_5      0x24    /* 32 bits */
#define  LS_PCI_BASE_ADDRESS_SPACE 0x01 
#define  LS_PCI_BASE_ADDRESS_SPACE_IO 0x01
#define  LS_PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define  LS_PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06
#define  LS_PCI_BASE_ADDRESS_MEM_TYPE_32   0x00    /* 32 bit address */
#define  LS_PCI_BASE_ADDRESS_MEM_TYPE_1M   0x02    /* Below 1M [obsolete] */
#define  LS_PCI_BASE_ADDRESS_MEM_TYPE_64   0x04    /* 64 bit address */
#define  LS_PCI_BASE_ADDRESS_MEM_PREFETCH  0x08    /* prefetchable? */
#define  LS_PCI_BASE_ADDRESS_MEM_MASK      (~0x0fULL)
#define  LS_PCI_BASE_ADDRESS_IO_MASK       (~0x03ULL)

/* Header type 0 (normal devices) */
#define LS_PCI_CARDBUS_CIS         0x28
#define LS_PCI_SUBSYSTEM_VENDOR_ID 0x2c
#define LS_PCI_SUBSYSTEM_ID        0x2e
#define LS_PCI_ROM_ADDRESS         0x30
#define  LS_PCI_ROM_ADDRESS_ENABLE 0x01
#define LS_PCI_ROM_ADDRESS_MASK    (~0x7ffULL)

#define LS_PCI_CAPABILITY_LIST     0x34    /* Offset of first capability list entry */

#define LS_PCI_INTERRUPT_LINE      0x3c    /* 8 bits */
#define LS_PCI_INTERRUPT_PIN       0x3d    /* 8 bits */
#define LS_PCI_MIN_GNT             0x3e    /* 8 bits */
#define LS_PCI_MAX_LAT             0x3f    /* 8 bits */

/* Header type 1 (PCI-to-PCI bridges) */
#define LS_PCI_PRIMARY_BUS         0x18    /* Primary bus number */
#define LS_PCI_SECONDARY_BUS       0x19    /* Secondary bus number */
#define LS_PCI_SUBORDINATE_BUS     0x1a    
#define LS_PCI_SEC_LATENCY_TIMER   0x1b    
#define LS_PCI_IO_BASE             0x1c    /* I/O range behind the bridge */
#define LS_PCI_IO_LIMIT            0x1d
#define  LS_PCI_IO_RANGE_TYPE_MASK 0x0f    /* I/O bridging type */
#define  LS_PCI_IO_RANGE_TYPE_16   0x00
#define  LS_PCI_IO_RANGE_TYPE_32   0x01
#define  LS_PCI_IO_RANGE_MASK      ~0x0f
#define LS_PCI_SEC_STATUS          0x1e 
#define LS_PCI_MEMORY_BASE         0x20    /* Memory range behind */
#define LS_PCI_MEMORY_LIMIT        0x22
#define  LS_PCI_MEMORY_RANGE_TYPE_MASK 0x0f
#define  LS_PCI_MEMORY_RANGE_MASK  ~0x0f
#define LS_PCI_PREF_MEMORY_BASE    0x24    /* Prefetchable memory range behind */
#define LS_PCI_PREF_MEMORY_LIMIT   0x26
#define  LS_PCI_PREF_RANGE_TYPE_MASK 0x0f
#define  LS_PCI_PREF_RANGE_TYPE_32 0x00
#define  LS_PCI_PREF_RANGE_TYPE_64 0x01
#define  LS_PCI_PREF_RANGE_MASK    ~0x0f
#define LS_PCI_PREF_BASE_UPPER32   0x28    /* Upper half of prefetchable memory range */
#define LS_PCI_PREF_LIMIT_UPPER32  0x2c
#define LS_PCI_IO_BASE_UPPER16     0x30    /* Upper half of I/O addresses */
#define LS_PCI_IO_LIMIT_UPPER16    0x32
#define LS_PCI_ROM_ADDRESS1        0x38    /* Same as LS_PCI_ROM_ADDRESS, but for htype 1 */
#define LS_PCI_BRIDGE_CONTROL      0x3e
#define  LS_PCI_BRIDGE_CTL_PARITY  0x01    
#define  LS_PCI_BRIDGE_CTL_SERR    0x02    
#define  LS_PCI_BRIDGE_CTL_NO_ISA  0x04    
#define  LS_PCI_BRIDGE_CTL_VGA     0x08    
#define  LS_PCI_BRIDGE_CTL_MASTER_ABORT 0x20  /* Report master aborts */
#define  LS_PCI_BRIDGE_CTL_BUS_RESET 0x40  /* Secondary bus reset */
#define  LS_PCI_BRIDGE_CTL_FAST_BACK 0x80  

#define LS_PCI_ERREN       0x48     /* Error Enable */
#define LS_PCI_ERRSTS      0x49     /* Error Status */
#define LS_PCI_BRDGOPT1    0x4A     
#define LS_PCI_PLBSESR0    0x4C     
#define LS_PCI_PLBSESR1    0x50     
#define LS_PCI_PLBSEAR     0x54     
#define LS_PCI_CAPID       0x58     
#define LS_PCI_NEXTITEMPTR 0x59     
#define LS_PCI_PMC         0x5A    
#define LS_PCI_PMCSR       0x5C     
#define LS_PCI_PMCSRBSE    0x5E     
#define LS_PCI_BRDGOPT2    0x60     /* PCI Bridge Options 2 */
#define LS_PCI_PMSCRR      0x64    

/* Header type 2 (CardBus bridges) */
#define LS_PCI_CB_CAPABILITY_LIST  0x14
/* 0x15 reserved */
#define LS_PCI_CB_SEC_STATUS       0x16    /* Secondary status */
#define LS_PCI_CB_PRIMARY_BUS      0x18    /* PCI bus number */
#define LS_PCI_CB_CARD_BUS         0x19   
#define LS_PCI_CB_SUBORDINATE_BUS  0x1a    /* Subordinate bus number */
#define LS_PCI_CB_LATENCY_TIMER    0x1b   
#define LS_PCI_CB_MEMORY_BASE_0    0x1c
#define LS_PCI_CB_MEMORY_LIMIT_0   0x20
#define LS_PCI_CB_MEMORY_BASE_1    0x24
#define LS_PCI_CB_MEMORY_LIMIT_1   0x28
#define LS_PCI_CB_IO_BASE_0        0x2c
#define LS_PCI_CB_IO_BASE_0_HI     0x2e
#define LS_PCI_CB_IO_LIMIT_0       0x30
#define LS_PCI_CB_IO_LIMIT_0_HI    0x32
#define LS_PCI_CB_IO_BASE_1        0x34
#define LS_PCI_CB_IO_BASE_1_HI     0x36
#define LS_PCI_CB_IO_LIMIT_1       0x38
#define LS_PCI_CB_IO_LIMIT_1_HI    0x3a
#define  LS_PCI_CB_IO_RANGE_MASK   ~0x03
#define LS_PCI_CB_BRIDGE_CONTROL   0x3e
#define  LS_PCI_CB_BRIDGE_CTL_PARITY       0x01    /* Similar to standard bridge control register */
#define  LS_PCI_CB_BRIDGE_CTL_SERR         0x02
#define  LS_PCI_CB_BRIDGE_CTL_ISA          0x04
#define  LS_PCI_CB_BRIDGE_CTL_VGA          0x08
#define  LS_PCI_CB_BRIDGE_CTL_MASTER_ABORT 0x20
#define  LS_PCI_CB_BRIDGE_CTL_CB_RESET     0x40    /* CardBus reset */
#define  LS_PCI_CB_BRIDGE_CTL_16BIT_INT    0x80    
#define  LS_PCI_CB_BRIDGE_CTL_PREFETCH_MEM0 0x100  /* Prefetch enable for both memory regions */
#define  LS_PCI_CB_BRIDGE_CTL_PREFETCH_MEM1 0x200
#define  LS_PCI_CB_BRIDGE_CTL_POST_WRITES  0x400
#define LS_PCI_CB_SUBSYSTEM_VENDOR_ID 0x40
#define LS_PCI_CB_SUBSYSTEM_ID     0x42
#define LS_PCI_CB_LEGACY_MODE_BASE 0x44    /* 16-bit PC Card legacy mode base address (ExCa) */

/* Defines of Capability lists */

#define LS_PCI_CAP_LIST_ID         0       /* Capability ID */
#define  LS_PCI_CAP_ID_PM          0x01    /* Power Management */
#define  LS_PCI_CAP_ID_AGP         0x02    
#define  LS_PCI_CAP_ID_VPD         0x03    
#define  LS_PCI_CAP_ID_SLOTID      0x04    
#define  LS_PCI_CAP_ID_MSI         0x05    
#define  LS_PCI_CAP_ID_CHSWP       0x06    
#define  LS_PCI_CAP_ID_EXP         0x10    
#define LS_PCI_CAP_LIST_NEXT       1       /* Next capability in the list */
#define LS_PCI_CAP_FLAGS           2       /* Capability defined flags (16 bits) */
#define LS_PCI_CAP_SIZEOF          4

/* Power Management Registers */

#define  LS_PCI_PM_CAP_VER_MASK    0x0007  
#define  LS_PCI_PM_CAP_PME_CLOCK   0x0008 
#define  LS_PCI_PM_CAP_AUX_POWER   0x0010 
#define  LS_PCI_PM_CAP_DSI         0x0020 
#define  LS_PCI_PM_CAP_D1          0x0200 
#define  LS_PCI_PM_CAP_D2          0x0400 
#define  LS_PCI_PM_CAP_PME         0x0800 
#define LS_PCI_PM_CTRL             4      
#define  LS_PCI_PM_CTRL_STATE_MASK 0x0003  
#define  LS_PCI_PM_CTRL_PME_ENABLE 0x0100  
#define  LS_PCI_PM_CTRL_DATA_SEL_MASK      0x1e00  
#define  LS_PCI_PM_CTRL_DATA_SCALE_MASK    0x6000  
#define  LS_PCI_PM_CTRL_PME_STATUS 0x8000  
#define LS_PCI_PM_PPB_EXTENSIONS   6       
#define  LS_PCI_PM_PPB_B2_B3       0x40    
#define  LS_PCI_PM_BPCC_ENABLE     0x80    
#define LS_PCI_PM_DATA_REGISTER    7       
#define LS_PCI_PM_SIZEOF           8

/* AGP registers defines */

#define LS_PCI_AGP_VERSION         2       
#define LS_PCI_AGP_RFU             3      
#define LS_PCI_AGP_STATUS          4      
#define  LS_PCI_AGP_STATUS_RQ_MASK 0xff000000
#define  LS_PCI_AGP_STATUS_SBA     0x0200  
#define  LS_PCI_AGP_STATUS_64BIT   0x0020  
#define  LS_PCI_AGP_STATUS_FW      0x0010 
#define  LS_PCI_AGP_STATUS_RATE4   0x0004  
#define  LS_PCI_AGP_STATUS_RATE2   0x0002  
#define  LS_PCI_AGP_STATUS_RATE1   0x0001  
#define LS_PCI_AGP_COMMAND         8       
#define  LS_PCI_AGP_COMMAND_RQ_MASK 0xff000000 
#define  LS_PCI_AGP_COMMAND_SBA    0x0200  
#define  LS_PCI_AGP_COMMAND_AGP    0x0100  
#define  LS_PCI_AGP_COMMAND_64BIT  0x0020  
#define  LS_PCI_AGP_COMMAND_FW     0x0010  
#define  LS_PCI_AGP_COMMAND_RATE4  0x0004  
#define  LS_PCI_AGP_COMMAND_RATE2  0x0002
#define  LS_PCI_AGP_COMMAND_RATE1  0x0001 
#define LS_PCI_AGP_SIZEOF          12

/* Defines for PCI-X registers */

#define  LS_PCI_X_CMD_DPERR_E      0x0001  
#define  LS_PCI_X_CMD_ERO          0x0002  
#define  LS_PCI_X_CMD_MAX_READ     0x0000  
#define  LS_PCI_X_CMD_MAX_SPLIT    0x0030  
#define  LS_PCI_X_CMD_VERSION(x)   (((x) >> 12) & 3) 


/* Register of Slot Identification */

#define LS_PCI_SID_ESR             2       /* Expansion Slot Register */
#define  LS_PCI_SID_ESR_NSLOTS     0x1f    /* Number of expansion slots available */
#define  LS_PCI_SID_ESR_FIC        0x20    /* First In Chassis Flag */
#define LS_PCI_SID_CHASSIS_NR      3       /* Chassis Number */

#define LS_PCI_MSI_FLAGS           2   
#define  LS_PCI_MSI_FLAGS_64BIT    0x80
#define  LS_PCI_MSI_FLAGS_QSIZE    0x70
#define  LS_PCI_MSI_FLAGS_QMASK    0x0e
#define  LS_PCI_MSI_FLAGS_ENABLE   0x01
#define LS_PCI_MSI_RFU             3 
#define LS_PCI_MSI_ADDRESS_LO      4
#define PCI_MSI_ADDRESS_HI      8 
#define LS_PCI_MSI_DATA_32         8      
#define LS_PCI_MSI_DATA_64         12     

#define LS_PCI_MAX_PCI_DEVICES     32
#define LS_PCI_MAX_PCI_FUNCTIONS   8

#define LS_PCI_FIND_CAP_TTL 0x48
#define CAP_START_POS 0x40

#define LS_PCI_REGION_MEM		0x00000000	/* PCI memory space */
#define LS_PCI_REGION_IO		0x00000001	/* PCI IO space */
#define LS_PCI_REGION_TYPE		0x00000001
#define LS_PCI_REGION_PREFETCH	0x00000008	/* prefetchable PCI memory */

#define LS_PCI_REGION_SYS_MEMORY	0x00000100	/* System memory */
#define LS_PCI_REGION_RO		0x00000200	/* Read-only memory */

#define PCI_BUS(Dev)	(((Dev) >> 16) & 0xff)
#define PCI_DEV(Dev)	(((Dev) >> 11) & 0x1f)
#define PCI_FUNC(Dev)	(((Dev) >> 8) & 0x7)
#define PCI_BDF(B,D,F)	((B) << 16 | (D) << 11 | (F) << 8)

#define PCI_ANY_ID (~0)

#define INDIRECT_TYPE_NO_PCIE_LINK	1

#endif /* _PCI_LIB_H */
