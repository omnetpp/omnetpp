//==========================================================================
//  PACKING.H - part of
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

#ifndef __PACKING_H
#define __PACKING_H

#include "ccommbuffer.h"

NAMESPACE_BEGIN

//
// This file contains methods for netPack/netUnpack methods generated in _m.cc files
//

//
// pack/unpack functions for primitive types
//

#define DOPACKING(T,R) \
          inline void doPacking(cCommBuffer *b, const T R a) {b->pack(a);}  \
          inline void doPacking(cCommBuffer *b, const T *a, int n) {b->pack(a,n);}  \
          inline void doUnpacking(cCommBuffer *b, T& a) {b->unpack(a);}  \
          inline void doUnpacking(cCommBuffer *b, T *a, int n) {b->unpack(a,n);}
#define _
DOPACKING(char,_)
DOPACKING(unsigned char,_)
DOPACKING(bool,_)
DOPACKING(short,_)
DOPACKING(unsigned short,_)
DOPACKING(int,_)
DOPACKING(unsigned int,_)
DOPACKING(long,_)
DOPACKING(unsigned long,_)
DOPACKING(float,_)
DOPACKING(double,_)
DOPACKING(long double,_)
DOPACKING(char *,_)
//FIXME DOPACKING(char *,_)
DOPACKING(SimTime,_)
DOPACKING(opp_string,&)
#undef _
#undef DOPACKING

//
// Default pack/unpack function for arrays
//
template<typename T>
void doPacking(cCommBuffer *b, const T *t, int n) {
    for (int i=0; i<n; i++)
        doPacking(b, t[i]);
}

template<typename T>
void doUnpacking(cCommBuffer *b, T *t, int n) {
    for (int i=0; i<n; i++)
        doUnpacking(b, t[i]);
}

NAMESPACE_END

#endif

