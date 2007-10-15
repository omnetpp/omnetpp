//=========================================================================
//  COMMONUTIL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include "commonutil.h"


int DebugCall::depth;

DebugCall::DebugCall(const char *fmt,...)
{
    va_list va;
    va_start(va, fmt);
    char buf[1024];
    vsprintf(buf,fmt,va);
    va_end(va);

    funcname = buf;
    printf("%*s ++ %s\n", depth++*2, "", funcname.c_str());
}

DebugCall::~DebugCall()
{
    printf("%*s -- %s\n", --depth*2, "", funcname.c_str());
}


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


