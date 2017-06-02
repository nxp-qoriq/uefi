/** @Common.h
  Header defining the General Purpose Utilities

  Copyright (c) 2015, Freescale Semiconductor, Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __COMMON_H__
#define __COMMON_H__

/*
 * Compile-time assertion macro
 */
#if __STDC_VERSION__ >= 201112L
#include <assert.h>
#define C_ASSERT(_cond) \
	static_assert(_cond, #_cond)
#else
#define C_ASSERT(_cond) \
        extern CONST CHAR8 c_assert_dummy_decl[(_cond) ? 1 : -1]
#endif

#define MASK_LOWER_16		0xFFFF0000
#define MASK_UPPER_16		0x0000FFFF
#define MASK_LOWER_8			0xFF000000
#define MASK_UPPER_8			0x000000FF

/**
  Utility to return minimum among X and Y
**/
#define Min(X, Y)				\
	({ typeof(X) __x = (X);			\
		typeof(Y) __y = (Y);		\
		(__x < __y) ? __x : __y; })

/**
  Utility to return maximum among X and Y
**/
#define Max(X, Y)				\
	({ typeof(X) __x = (X);			\
		typeof(Y) __y = (Y);		\
		(__x > __y) ? __x : __y; })

/**
  Utility to perform division (N must be 64bit)
**/
# define DoDiv(N,Base) ({				\
	UINT32 __Base = (Base);			\
	UINT32 __Rem;					\
	(VOID)(((typeof((N)) *)0) == ((UINT64 *)0));	\
	if (((N) >> 32) == 0) {			\
		__Rem = (UINT32)(N) % __Base;		\
		(N) = (UINT32)(N) / __Base;		\
	} else						\
		__Rem = __Div64_32(&(N), __Base);	\
	__Rem;						\
 })

#define Uswap32(X) \
       ((((X) & 0xff000000) >> 24) | \
        (((X) & 0x00ff0000) >>  8) | \
        (((X) & 0x0000ff00) <<  8) | \
        (((X) & 0x000000ff) << 24))

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define RoundUp(X, Y)       ((((X) + ((Y) - 1)) / (Y)) * (Y))

#define DMA_MINALIGN   64

#define offsetof(TYPE, MEMBER) ((UINTN) &((TYPE *)0)->MEMBER)

/**
 * Upper32Bits - return bits 32-63 of a number
 * @N: the number we're accessing
 *
 * A basic shift-right of a 64- or 32-bit quantity.  Use this to suppress
 * the "right shift count >= width of type" warning when that quantity is
 * 32-bits.
 */
#define Upper32Bits(N) ((UINT32)(((N) >> 16) >> 16))

/**
 * Lower32Bits - return bits 0-31 of a number
 * @N: the number we're accessing
 */
#define Lower32Bits(N) ((UINT32)(N))

//
// Stores a value for a given bit field in _container
//
#define SET_BIT_FIELD(_container, _bit_mask, _bit_shift, _value) \
    do {                                                                    \
        (_container) &= ~(_bit_mask);                                       \
        if ((_value) != 0) {                                                \
            ASSERT(((UINT32)(_value) << (_bit_shift)) <= (_bit_mask));      \
            (_container) |=                                                 \
                ((UINT32)(_value) << (_bit_shift)) & (_bit_mask);           \
        }                                                                   \
    } while (0)

//
// Returns the number of elements of an array
//
#define ARRAY_NUM_ENTRIES(_array)   (sizeof(_array) / sizeof((_array)[0]))

//
// Converts a 32-bit big endian value to the corresponding value in CPU
// byte-order, assuming the CPU byte order is little endian.
//
#define BE32_TO_CPU(_x) ByteSwap32(_x)


/*
 * Returns the number of elements of an array
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_Array)   (sizeof(_Array) / sizeof((_Array)[0]))
#endif

typedef UINTN PhysAddrT;
typedef UINTN PhysSizeT;

/*
 * The ALLOC_CACHE_ALIGN_BUF macro is used to allocate a buffer
 * that meets the minimum architecture alignment requirements for DMA.
 *
 * The resulting buffer is aligned to the value of DMA_MINALIGN.
 *
 * The buffer variable created is a pointer to the specified type, and
 * NOT an array.
 *
 * The size parameter is the number of array elements to allocate.
 */

#define ALIGN_BUFF(x,a)          ALIGN_MSK((x),(typeof(x))(a)-1)
#define ALIGN_MSK(x,mask)       (((x)+(mask))&~(mask))

#define PAD_CNT(S, Pad) (((S) - 1) / (Pad) + 1)
#define PAD_SIZE(S, Pad) (PAD_CNT(S, Pad) * Pad)

#define ROUND_OFF(a,b)          (((a) + (b) - 1) & ~((b) - 1))

#define DMA_MINALIGN   64

#define ALLOC_ALIGN_BUF_PAD(Type, Name, Size, Align, Pad)		\
	INT8 __##Name[ROUND_OFF(PAD_SIZE((Size) * sizeof(Type), Pad), Align)  \
		      + (Align - 1)];					\
									\
	Type *Name = (Type *) ALIGN_BUFF((UINTN)__##Name, Align)
#define ALLOC_ALIGN_BUF(Type, Name, Size, Align)		\
	ALLOC_ALIGN_BUF_PAD(Type, Name, Size, Align, 1)
#define ALLOC_CACHE_ALIGN_BUF_PAD(Type, Name, Size, Pad)		\
	ALLOC_ALIGN_BUF_PAD(Type, Name, size, DMA_MINALIGN, Pad)
#define ALLOC_CACHE_ALIGN_BUF(Type, Name, Size)			\
	ALLOC_ALIGN_BUF(Type, Name, Size, DMA_MINALIGN)

STATIC inline PhysAddrT VirtToPhys(VOID * VAddr)
{
       return (PhysAddrT)(VAddr);
}

//
// Invert byte order of a 32-bit value
//
STATIC inline
UINT32
ByteSwap32(UINT32 Value)
{
    UINT32 SwappedVal;

    asm volatile (
            "rev32 %[SwappedVal], %[Value]\n\t"
            : [SwappedVal] "=r" (SwappedVal)
            : [Value] "r" (Value)
    );

    return SwappedVal;
}

/**
  Funtion to divide N to base Base

  @param   N         Dividend
  @param   Base      Base

**/
UINT32
__Div64_32 (
  IN   UINT64        *N,
  IN   UINT32        Base
  );

VOID
PrintSize (
  IN  UINT64 Size,
  IN  CONST INT8 *S
  );

#endif
