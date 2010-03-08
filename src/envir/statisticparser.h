//==========================================================================
//  STATISTICPARSER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVIR_STATISTICPARSER_H
#define __ENVIR_STATISTICPARSER_H

#include "envirdefs.h"
#include "expression.h"
#include "resultrecorders.h"

class FilterOrRecorderReference;

class ENVIR_API StatisticSourceParser
{
  protected:
    // create and install a ResultFilter; its input is the expression at the top of the stack (length 'len').
    // filterRef may be NULL; in that case, only the expression needs to be created
    SignalSource createFilter(FilterOrRecorderReference *filterRef, const std::vector<Expression::Elem>& stack, int len, cComponent *component, bool needWarmupFilter);
  public:
    StatisticSourceParser() {}
    SignalSource parse(cComponent *component, const char *statisticName, const char *sourceSpec, bool needWarmupFilter);
};

class ENVIR_API StatisticRecorderParser
{
  public:
    StatisticRecorderParser() {}
    void parse(const SignalSource& source, const char *mode, bool scalarsEnabled, bool vectorsEnabled, cComponent *component, const char *statisticName, const char *where);
};

#endif


