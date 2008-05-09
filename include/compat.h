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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COMPAT_H
#define __COMPAT_H

//
// This header needs to be included *last* in omnetpp.h.
//

// following ones became inner classes:
typedef _OPPDEPRECATED  OPP::cKSplit::Iterator      cKSplitIterator;
typedef _OPPDEPRECATED  OPP::cKSplit::Grid          sGrid;
typedef _OPPDEPRECATED  OPP::cLinkedList::Iterator  cLinkedListIterator;
typedef _OPPDEPRECATED  OPP::cMessageHeap::Iterator cMessageHeapIterator;
typedef _OPPDEPRECATED  OPP::cTopology::Link        sTopoLink;
typedef _OPPDEPRECATED  OPP::cTopology::LinkIn      sTopoLinkIn;
typedef _OPPDEPRECATED  OPP::cTopology::LinkOut     sTopoLinkOut;
typedef _OPPDEPRECATED  OPP::cTopology::Node        sTopoNode;

// renamed classes:
typedef _OPPDEPRECATED  OPP::cDisplayString   cDisplayStringParser;
typedef _OPPDEPRECATED  OPP::cBasicChannel    cSimpleChannel;
typedef /*TBD:_OPPDEPRECATED*/  OPP::cObject  cPolymorphic;

#endif

