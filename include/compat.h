//==========================================================================
//   COMPAT.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Backwards compatibility
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COMPAT_H
#define __COMPAT_H


//
// This header needs to be included *last* in omnetpp.h.
//

// cObject:
#define isA()          className()
// cQueue:
#define insertHead(a)  insert(a)
#define peekTail()     tail()
#define peekHead()     head()
#define getTail()      pop()
// cSimulation
#define lastModuleIndex() lastModuleId()
// cModule
#define displayStringAsParent()  backgroundDisplayString()
#define setDisplayStringAsParent setBackgroundDisplayString

// following ones became inner classes:
typedef cKSplit::Iterator      cKSplitIterator;
typedef cKSplit::Grid          sGrid;
typedef cQueue::Iterator       cQueueIterator;
typedef cLinkedList::Iterator  cLinkedListIterator;
typedef cPar::ExprElem         sXElem;
typedef cMessageHeap::Iterator cMessageHeapIterator;
typedef cTopology::Link        sTopoLink;
typedef cTopology::LinkIn      sTopoLinkIn;
typedef cTopology::LinkOut     sTopoLinkOut;
typedef cTopology::Node        sTopoNode;

// renamed classes:
typedef cDisplayString cDisplayStringParser;
typedef cBasicChannel  cSimpleChannel;

#endif

