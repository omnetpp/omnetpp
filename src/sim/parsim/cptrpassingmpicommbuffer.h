//=========================================================================
//  CPTRPASSINGMPICOMMBUFFER.H - part of
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

#ifndef __CPTRPASSINGMPICOMMBUFFER_H__
#define __CPTRPASSINGMPICOMMBUFFER_H__

#include "cmpicommbuffer.h"


/**
 * A variant of cMPICommBuffer. packObject() only packs the pointer
 * and does not serialize object fields. This class can only be
 * used with 64-bit shared memory superclusters like SGI Altix.
 *
 * @ingroup Parsim
 */
class cPtrPassingMPICommBuffer : public cMPICommBuffer
{
  public:
    /**
     * Constructor.
     */
    cPtrPassingMPICommBuffer();

    /**
     * Packs an object.
     */
    virtual void packObject(cObject *obj);

    /**
     * Unpacks and returns an object.
     */
    virtual cObject *unpackObject();
};

#endif

