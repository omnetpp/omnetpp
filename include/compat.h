//==========================================================================
//  COMPAT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Backwards compatibility
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COMPAT_H
#define __COMPAT_H

//
// This header needs to be included *last* in omnetpp.h.
//

// renamed classes:
typedef _OPPDEPRECATED OPP::cBasicChannel cSimpleChannel;
typedef /*_OPPDEPRECATED*/ OPP::cObject cPolymorphic;

#endif

