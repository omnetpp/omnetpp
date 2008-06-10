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

//FIXME consider using <stdint.h> [not available in VC++ though]

#ifdef _MSC_VER
    typedef __int8              int8;
    typedef __int16             int16;
    typedef __int32             int32;
    typedef __int64             int64;
    typedef unsigned __int8     uint8;
    typedef unsigned __int16    uint16;
    typedef unsigned __int32    uint32;
    typedef unsigned __int64    uint64;
#else
    // the following should work on 32-bit and 64-bit architectures too
    // note: cenvir.cc contains asserts to ensure that these typedefs are correct
    typedef char                int8;
    typedef short               int16;
    typedef int                 int32;
    typedef long long           int64;
    typedef unsigned char       uint8;
    typedef unsigned short      uint16;
    typedef unsigned int        uint32;
    typedef unsigned long long  uint64;
#endif

#define INT64_MAX   (int64)((((uint64)1)<<63)-1)


// Note: %I64d is only used with VC++ 7.1 and MinGW gcc 3.4.x; once we
// drop support for these compilers, this macro can be dropped altogether
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

