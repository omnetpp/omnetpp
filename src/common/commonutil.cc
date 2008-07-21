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

USING_NAMESPACE


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
    printf("%*s ++ %s\n", depth++*2, "", funcname.c_str());
}

DebugCall::~DebugCall()
{
    printf("%*s -- %s\n", --depth*2, "", funcname.c_str());
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


