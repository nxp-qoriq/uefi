/** Dpaa2DebugLib.h
  DPAA2 debug macros

  Copyright (c) 2016, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DPAA2_DEBUG_LIB_H__
#define __DPAA2_DEBUG_LIB_H__

#include <Uefi.h>
#include <Library/DebugLib.h>

/*
 * DPAA2 debugging flags
 */
#define DPAA2_DEBUG_MESSAGES_ON             0x1     /* Enable DPAA2 debugging messages */
#define DPAA2_DEBUG_DUMP_VALUES             0x2     /* Dump values of RAM words or registers */
#define DPAA2_DEBUG_TRACE_MC_COMMANDS       0x4     /* Trace commands sent to the MC */
#define DPAA2_DEBUG_DUMP_MC_LOG_FRAGMENT    0x8     /* Dump MC log fragment */
#define DPAA2_DEBUG_DUMP_ROOT_DPRC          0x10    /* Dump contents of the root DPRC */
#define DPAA2_DEBUG_EXTRA_CHECKS            0x20    /* Perform extra checks */
#define DPAA2_DEBUG_TRACE_NET_PACKETS       0x40    /* Trace sent/received network packets */

/**
 * Print a debug message with prefix, if DPAA2_DEBUG_MESSAGES_ON set
 */
#define DPAA2_DEBUG_MSG(_Fmt, ...) \
  do {                                                                  \
    if (gDpaa2DebugFlags & DPAA2_DEBUG_MESSAGES_ON) {                   \
        DEBUG((EFI_D_RELEASE, "DPAA2 DBG: " _Fmt, ##__VA_ARGS__));      \
    }                                                                   \
  } while (0)

/**
 * Print a debug message with no prefix, if DPAA2_DEBUG_MESSAGES_ON set
 */
#define DPAA2_DEBUG_MSG_NO_PREFIX(_Fmt, ...) \
  do {                                                              \
    if (gDpaa2DebugFlags & DPAA2_DEBUG_MESSAGES_ON) {               \
        DEBUG((EFI_D_RELEASE, _Fmt, ##__VA_ARGS__));                \
    }                                                               \
  } while (0)

/**
 * Print an error message with prefix
 */
#define DPAA2_ERROR_MSG(_Fmt, ...) \
  do {                                                              \
    DEBUG((EFI_D_ERROR, "DPAA2 ERROR: " _Fmt, ##__VA_ARGS__));      \
  } while (0)

/**
 * Print a warning message with prefix
 */
#define DPAA2_WARN_MSG(_Fmt, ...) \
  do {                                                              \
    DEBUG((EFI_D_RELEASE, "DPAA2 WARNING: " _Fmt, ##__VA_ARGS__));     \
  } while (0)

/**
 * Print a production informational message with prefix
 */
#define DPAA2_INFO_MSG(_Fmt, ...) \
  do {                                                              \
    DEBUG((EFI_D_RELEASE, "DPAA2: " _Fmt, ##__VA_ARGS__));          \
  } while (0)

/**
 * Print a production informational message with no prefix
 */
#define DPAA2_INFO_MSG_NO_PREFIX(_Fmt, ...) \
  do {                                                              \
    DEBUG((EFI_D_RELEASE, _Fmt, ##__VA_ARGS__));                    \
  } while (0)

extern UINT32 gDpaa2DebugFlags;

#endif /* __DPAA2_DEBUG_LIB_H__ */
