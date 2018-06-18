//==========================================================================
//  INTLIMITS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_INTLIMITS_H
#define __OMNETPP_INTLIMITS_H

//
// MIN/MAX macros for integral types, copied from <cstdint>.
// Note that we need to define them ourselves, because if <cstdint>
// is first included without __STDC_LIMIT_MACROS somewhere, these macros
// remain unavailable and nothing can be done about it anymore.
// Brilliant design from the C standards commitee.
//

// Minimum of signed integral types.
#ifndef INT8_MIN
#define INT8_MIN      (-128)
#endif

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

