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

// cQueue
#define peekTail()     tail()
#define peekHead()     head()

//XXX can typedefs be deprecated?

// following ones became inner classes:
typedef OPP::cKSplit::Iterator      cKSplitIterator;
typedef OPP::cKSplit::Grid          sGrid;
typedef OPP::cLinkedList::Iterator  cLinkedListIterator;
typedef OPP::cMessageHeap::Iterator cMessageHeapIterator;
typedef OPP::cTopology::Link        sTopoLink;
typedef OPP::cTopology::LinkIn      sTopoLinkIn;
typedef OPP::cTopology::LinkOut     sTopoLinkOut;
typedef OPP::cTopology::Node        sTopoNode;

// renamed classes:
typedef OPP::cDisplayString cDisplayStringParser;
typedef OPP::cBasicChannel  cSimpleChannel;
typedef OPP::cObject        cPolymorphic;

#endif

