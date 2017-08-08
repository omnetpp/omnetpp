//==========================================================================
//  STATISTICPARSING.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_STATISTICPARSING_H
#define __OMNETPP_STATISTICPARSING_H

#include "common/expression.h"
#include "sim/resultrecorders.h"

namespace omnetpp {

class FilterOrRecorderReference;

typedef cStatisticBuilder::TristateBool TristateBool;

/**
 * Data structure used when setting up chains of result filters and result recorders
 */
class SIM_API SignalSource
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

class SIM_API StatisticSourceParser
{
  protected:
    typedef omnetpp::common::Expression Expression;
  protected:
    // create and install a cResultFilter; its input is the expression at the top of the stack (length 'len').
    // filterRef may be nullptr; in that case, only the expression filter needs to be created
    SignalSource createFilter(FilterOrRecorderReference *filterRef, const std::vector<Expression::Elem>& stack, int len, cComponent *component, cProperty *statisticProperty);
  public:
    StatisticSourceParser() {}
    SignalSource parse(cComponent *component, cProperty *statisticProperty, const char *statisticName, const char *sourceSpec, TristateBool checkSignalDecl, bool needWarmupFilter);
    static void checkSignalDeclaration(cComponent *component, const char *signalName, TristateBool checkSignalDecl);
};

class SIM_API StatisticRecorderParser
{
  protected:
    typedef omnetpp::common::Expression Expression;
  protected:
    // create and install a cResultFilter or a cResultRecorder, depending on the makeRecorder flag;
    // its input is the expression at the top of the stack (length 'len').
    // filterRef may be nullptr; in that case, only the expression filter/recorder needs to be created.
    // returns SignalSource(nullptr) if called with makeRecorder==true, meaning that no chaining is possible
    SignalSource createFilterOrRecorder(FilterOrRecorderReference *filterOrRecorderRef, bool makeRecorder, const std::vector<Expression::Elem>& stack, int len, const SignalSource& source, cComponent *component, const char *statisticName, const char *recordingMode, cProperty *attrsProperty);
  public:
    StatisticRecorderParser() {}
    void parse(const SignalSource& source, const char *recordingMode, cComponent *component, const char *statisticName, cProperty *attrsProperty);
};

}  // namespace omnetpp

#endif


