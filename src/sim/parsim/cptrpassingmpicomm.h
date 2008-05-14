//=========================================================================
//  CPTRPASSINGMPICOMM.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2005
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPTRPASSINGMPICOMM_H__
#define __CPTRPASSINGMPICOMM_H__

#include "cparsimcomm.h"

NAMESPACE_BEGIN


/**
 * MPI communications layer, to be used with 64-bit shared memory
 * superclusters like SGI Altix.
 *
 * @ingroup Parsim
 */
class SIM_API cPtrPassingMPICommunications : public cMPICommunications
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

NAMESPACE_END


#endif


