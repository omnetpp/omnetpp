//==========================================================================
//  STATISTICRECORDERPARSER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_STATISTICRECORDERPARSER_H
#define __OMNETPP_STATISTICRECORDERPARSER_H

#include "omnetpp/cresultrecorder.h"
#include "omnetpp/cstatisticbuilder.h"
#include "expressionfilter.h"

namespace omnetpp {

class SIM_API StatisticRecorderParser
{
  public:
    void parse(const SignalSource& source, const char *recordingMode, cComponent *component, const char *statisticName, cProperty *attrsProperty);
};

}  // namespace omnetpp

#endif


