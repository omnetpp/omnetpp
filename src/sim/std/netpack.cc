//=========================================================================
//
//  NETPACK.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Zoltan Vass, 1996
//   Rewritten:   Andras Varga, Oct.1996
//
//   Contents:
//      dummy version of netPack() and netUnpack() member functions.
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>

#include "omnetpp.h"

int is_started_as_master()
{
    return NONPARALLEL_MODE;
}

// Using this source, it is possible to build a simulator without the PVM libs.
// In this case, the network pack/unpack functions have no functionality
// (dummy functions).

#define DUMMY_NETPACK_NETUPACK( CLASSNAME ) \
        int CLASSNAME::netPack()  {return 0;} \
        int CLASSNAME::netUnpack() {return 0;}

DUMMY_NETPACK_NETUPACK( cObject )
DUMMY_NETPACK_NETUPACK( cPar )
DUMMY_NETPACK_NETUPACK( cBag )
DUMMY_NETPACK_NETUPACK( cQueue )
DUMMY_NETPACK_NETUPACK( cArray )
DUMMY_NETPACK_NETUPACK( cMessage )
DUMMY_NETPACK_NETUPACK( cLinkedList )
DUMMY_NETPACK_NETUPACK( cTransientDetection )
DUMMY_NETPACK_NETUPACK( cAccuracyDetection )
DUMMY_NETPACK_NETUPACK( cStatistic )
DUMMY_NETPACK_NETUPACK( cStdDev )
DUMMY_NETPACK_NETUPACK( cWeightedStdDev )
DUMMY_NETPACK_NETUPACK( cDensityEstBase )
DUMMY_NETPACK_NETUPACK( cHistogramBase )
DUMMY_NETPACK_NETUPACK( cEqdHistogramBase )
DUMMY_NETPACK_NETUPACK( cLongHistogram )
DUMMY_NETPACK_NETUPACK( cDoubleHistogram )
DUMMY_NETPACK_NETUPACK( cVarHistogram )
DUMMY_NETPACK_NETUPACK( cPSquare )
DUMMY_NETPACK_NETUPACK( cKSplit )
DUMMY_NETPACK_NETUPACK( cTopology )
DUMMY_NETPACK_NETUPACK( cFSM )
DUMMY_NETPACK_NETUPACK( cPacket )
DUMMY_NETPACK_NETUPACK( cChannel )
DUMMY_NETPACK_NETUPACK( cSimpleChannel )
