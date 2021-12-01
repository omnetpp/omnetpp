//==========================================================================
//  STATISTICSOURCEPARSER.H - part of
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

#ifndef __OMNETPP_STATISTICSOURCEPARSER_H
#define __OMNETPP_STATISTICSOURCEPARSER_H

#include "omnetpp/cresultrecorder.h"
#include "omnetpp/cstatisticbuilder.h"
#include "expressionfilter.h"

namespace omnetpp {

typedef cStatisticBuilder::TristateBool TristateBool;

class SIM_API StatisticSourceParser
{
  public:
    SignalSource parse(cComponent *component, cProperty *statisticProperty, const char *statisticName, const char *sourceSpec, TristateBool checkSignalDecl, bool needWarmupFilter);
    static void checkSignalDeclaration(cComponent *component, const char *signalName, TristateBool checkSignalDecl);
};

}  // namespace omnetpp

#endif


