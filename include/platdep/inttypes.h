//==========================================================================
//  INTTYPES.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __INTTYPES_H
#define __INTTYPES_H

//
// Make intX_t and uintX_t types available
//
#ifndef _MSC_VER

  // include C99 header, enabling INT64_MAX etc.
  #ifndef __STDC_LIMIT_MACROS
    #define __STDC_LIMIT_MACROS 1
  #endif

  #include <stdint.h>

#else
  // MSVC has no <stdint.h>
  typedef __int8   int8_t;
  typedef __int16  int16_t;
  typedef __int32  int32_t;
  typedef __int64  int64_t;
  typedef unsigned __int8  uint8_t;
  typedef unsigned __int16 uint16_t;
  typedef unsigned __int32 uint32_t;
  typedef unsigned __int64 uint64_t;

  // Minimum of signed integral types.
  #ifndef INT8_MIN
  #endif
  #define INT8_MIN      (-128)

  #ifndef INT16_MIN
  #define INT16_MIN     (-32767-1)
  #endif

  #ifndef INT32_MIN
  #define INT32_MIN     (-2147483647-1)
  #endif

  #ifndef INT64_MIN
  #define INT64_MIN     (-9223372036854775807LL-1)
  #endif

  // Maximum of signed integral types.
  #ifndef INT8_MAX
  #define INT8_MAX      (127)
  #endif

  #ifndef INT16_MAX
  #define INT16_MAX     (32767)
  #endif

  #ifndef INT32_MAX
  #define INT32_MAX     (2147483647)
  #endif

  #ifndef INT64_MAX
  #define INT64_MAX     (9223372036854775807LL)
  #endif

  // Maximum of unsigned integral types.
  #ifndef UINT8_MAX
  #define UINT8_MAX     (255)
  #endif

  #ifndef UINT16_MAX
  #define UINT16_MAX    (65535)
  #endif

  #ifndef UINT32_MAX
  #define UINT32_MAX    (4294967295U)
  #endif

  #ifndef UINT64_MAX
  #define UINT64_MAX    (18446744073709551615ULL)
  #endif
#endif

//
// We prefer shorter names for ints. If they collide with existing code,
// #undef them after #including omnetpp.h.
//
#ifndef int8
#define int8     int8_t
#endif

#ifndef int16
#define int16    int16_t
#endif

#ifndef int32
#define int32    int32_t
#endif

#ifndef int64
#define int64    int64_t
#endif

#ifndef uint8
#define uint8    uint8_t
#endif

#ifndef uint16
#define uint16   uint16_t
#endif

#ifndef uint32
#define uint32   uint32_t
#endif

#ifndef uint64
#define uint64   uint64_t
#endif


//
// printf type specifier for int64.
//
// Note: %I64d is only used with VC++ 7.1 and MinGW gcc 3.4.x; once we
// drop support for these compilers, this macro can be dropped altogether
//
#if defined(_MSC_VER) || defined(__MINGW32__)
#   define INT64_PRINTF_FORMAT   "I64"
#else
#   define INT64_PRINTF_FORMAT   "ll"
#endif

//
// Parse int64 strings
//
#ifdef _MSC_VER
#define strtoi64 _strtoi64
#else
#define strtoi64 strtoll
#endif

#endif

