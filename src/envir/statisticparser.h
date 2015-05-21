//==========================================================================
//  STATISTICPARSER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_STATISTICPARSER_H
#define __OMNETPP_ENVIR_STATISTICPARSER_H

#include "common/expression.h"
#include "sim/resultrecorders.h"
#include "envirdefs.h"

NAMESPACE_BEGIN


class FilterOrRecorderReference;

/**
 * Data structure used when setting up chains of result filters and result recorders
 */
class ENVIR_API SignalSource
{
  private:
    cResultFilter *filter;
    cComponent *component;
    simsignal_t signalID;
  public:
    SignalSource() {filter=nullptr; component=nullptr; signalID=-1;}
    SignalSource(cResultFilter *prevFilter) {filter=prevFilter; component=nullptr; signalID=-1;}
    SignalSource(cComponent *comp, simsignal_t sigID) {filter=nullptr; component=comp; signalID=sigID;}
    bool isNull() const {return !filter && !component;}
    void subscribe(cResultListener *listener) const;
    cResultFilter *getFilter() const {return filter;}
    cComponent *getComponent() const {return component;}
    simsignal_t getSignalID() const {return signalID;}
};

class ENVIR_API StatisticSourceParser
{
  protected:
    // create and install a cResultFilter; its input is the expression at the top of the stack (length 'len').
    // filterRef may be NULL; in that case, only the expression filter needs to be created
    SignalSource createFilter(FilterOrRecorderReference *filterRef, const std::vector<Expression::Elem>& stack, int len);
  public:
    StatisticSourceParser() {}
    SignalSource parse(cComponent *component, const char *statisticName, const char *sourceSpec, bool needWarmupFilter);
};

class ENVIR_API StatisticRecorderParser
{
  protected:
    // create and install a cResultFilter or a cResultRecorder, depending on the makeRecorder flag;
    // its input is the expression at the top of the stack (length 'len').
    // filterRef may be NULL; in that case, only the expression filter/recorder needs to be created.
    // returns SignalSource(NULL) if called with makeRecorder==true, meaning that no chaining is possible
    SignalSource createFilterOrRecorder(FilterOrRecorderReference *filterOrRecorderRef, bool makeRecorder, const std::vector<Expression::Elem>& stack, int len, const SignalSource& source, cComponent *component, const char *statisticName, const char *recordingMode, cProperty *attrsProperty);
  public:
    StatisticRecorderParser() {}
    void parse(const SignalSource& source, const char *recordingMode, bool scalarsEnabled, bool vectorsEnabled, cComponent *component, const char *statisticName, cProperty *attrsProperty);
};

NAMESPACE_END

#endif


