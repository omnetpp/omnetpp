//=========================================================================
//  CPTRPASSINGMPICOMM.CC - part of
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

#include "cptrpassingmpicomm.h"
#include "cptrpassingmpicommbuffer.h"
#include "macros.h"

Register_Class(cPtrPassingMPICommunications);

cPtrPassingMPICommunications::cPtrPassingMPICommunications()
{
}

cMPICommBuffer *cPtrPassingMPICommunications::doCreateCommBuffer()
{
    return new cPtrPassingMPICommBuffer();
}


