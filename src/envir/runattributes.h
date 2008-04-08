//==========================================================================
//  RUNATTRIBUTES.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __RUNATTRIBUTES_H
#define __RUNATTRIBUTES_H

#include <stdio.h>
#include "envirdefs.h"

NAMESPACE_BEGIN

/**
 * Collects and writes the run attributes and parameters.
 * 
 * @ingroup Envir
 */
struct sRunData {
   bool initialized;        // true if the other fields are valid
   opp_string runId;        // id of the run
   opp_string_map attributes;    // attributes of the run
   opp_string_map moduleParams;  // module parameters in the current run

   sRunData() : initialized(false) {}
   void initRun();
   void reset();
   void writeRunData(FILE *f, opp_string fname);
};

NAMESPACE_END

#endif

