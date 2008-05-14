//=========================================================================
//  CPTRPASSINGMPICOMM.CC - part of
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

#include "cptrpassingmpicomm.h"
#include "cptrpassingmpicommbuffer.h"
#include "globals.h"
#include "cconfigkey.h"
#include "regmacros.h"

USING_NAMESPACE

Register_Class(cPtrPassingMPICommunications);

cPtrPassingMPICommunications::cPtrPassingMPICommunications()
{
}

cMPICommBuffer *cPtrPassingMPICommunications::doCreateCommBuffer()
{
    return new cPtrPassingMPICommBuffer();
}


