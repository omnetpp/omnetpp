//==========================================================================
//  RUNATTRIBUTES.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_RUNATTRIBUTES_H
#define __OMNETPP_RUNATTRIBUTES_H

#include <cstdio>
#include "envirdefs.h"

NAMESPACE_BEGIN

/**
 * Collects and writes the run attributes and parameters.
 *
 * @ingroup Envir
 */
struct RunData {
   bool initialized;        // true if the other fields are valid
   opp_string runId;        // id of the run
   opp_string_map attributes;    // attributes of the run
   opp_string_map moduleParams;  // module parameters in the current run

   RunData() : initialized(false) {}
   void initRun();
   void reset();
   void writeRunData(FILE *f, opp_string fname);
};

NAMESPACE_END

#endif

