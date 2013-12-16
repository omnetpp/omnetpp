//=========================================================================
//  COMMONUTIL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <locale.h>
#include "commonutil.h"

NAMESPACE_BEGIN


static double zero = 0.0;
double const NaN = zero / zero;
double const POSITIVE_INFINITY = 1.0 / zero;
double const NEGATIVE_INFINITY = -1.0 / zero;

//----

void setPosixLocale()
{
    setlocale(LC_ALL, "C");
}

//----

int DebugCall::depth;

DebugCall::DebugCall(const char *fmt,...)
{
    char buf[1024];
    VSNPRINTF(buf, 1024, fmt);
    funcname = buf;
    std::cout << std::setw(depth++*2) << "" << " ++ " << funcname << std::endl;
}

DebugCall::~DebugCall()
{
    std::cout << std::setw(--depth*2) << "" << " -- " << funcname;
    if (!result.empty())
    	std::cout << ", result: " << result;
    std::cout << std::endl;
}

void DebugCall::printf(const char *fmt, ...)
{
    char buf[1024];
    VSNPRINTF(buf, 1024, fmt);
    std::cout << std::setw(depth*2) << "" << "    " << buf << std::endl;
}

//----

#ifdef _MSC_VER
// source: http://en.wikipedia.org/wiki/RDTSC
__declspec(naked)
uint64 __cdecl readCPUTimeStampCounter()
{
   __asm
   {
      rdtsc
      ret       ; return value at EDX:EAX
   }
}
#else
uint64 readCPUTimeStampCounter()
{
    return 0;
}
#endif

NAMESPACE_END

