//=========================================================================
//  CPTRPASSINGMPICOMM.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2005
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "cparsimcomm.h"

#ifndef __CPTRPASSINGMPICOMM_H__
#define __CPTRPASSINGMPICOMM_H__


/**
 * MPI communications layer, to be used with 64-bit shared memory
 * superclusters like SGI Altix.
 *
 * @ingroup Parsim
 */
class cPtrPassingMPICommunications : public cMPICommunications
{
  public:
    /**
     * Constructor.
     */
    cPtrPassingMPICommunications();

    /**
     * Creates an empty buffer of type cPtrPassingMPICommBuffer.
     */
    virtual cMPICommBuffer *doCreateCommBuffer();
};

#endif


