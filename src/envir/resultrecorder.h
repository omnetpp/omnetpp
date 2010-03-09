//==========================================================================
//  RESULTRECORDER.H - part of
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

#ifndef __ENVIR_RESULTRECORDER_H
#define __ENVIR_RESULTRECORDER_H

#include "envirdefs.h"
#include "resultlistener.h"
#include "ccomponent.h"
#include "commonutil.h"
#include "stringpool.h"
#include "onstartup.h"
#include "cregistrationlist.h"

class ResultRecorder;

#define Register_ResultRecorder(NAME, CLASSNAME) \
  static ResultRecorder *__FILEUNIQUENAME__() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(resultRecorders.getInstance()->add(new ResultRecorderDescriptor(NAME,__FILEUNIQUENAME__));)

extern cGlobalRegistrationList resultRecorders;


/**
 * Abstract base class for result recording listeners
 */
class ENVIR_API ResultRecorder : public ResultListener
{
    private:
        static CommonStringPool namesPool;
        cComponent *component;
        const char *statisticName;
        const char *recordingMode;
    protected:
        simtime_t getEndWarmupPeriod() {return simulation.getWarmupPeriod();}
        virtual void extractStatisticAttributes(cComponent *component, opp_string_map& result);
        virtual void tweakTitle(opp_string& title) {}
        cComponent *getComponent() {return component;}
    public:
        virtual void init(cComponent *component, const char *statisticName, const char *recordingMode);
        const char *getStatisticName() {return statisticName;}
        const char *getRecordingMode() {return recordingMode;}
        virtual std::string getResultName()  {return std::string(statisticName)+":"+recordingMode;}
        virtual void listenerAdded(cComponent *component, simsignal_t signalID);
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID);
};

/**
 * Abstract base class for result recording listeners
 */
class ENVIR_API NumericResultRecorder : public ResultRecorder
{
    protected:
        // override one of them, and let the other delegate to it
        virtual void collect(double value) = 0;
        virtual void collect(simtime_t t, double value) = 0;
    public:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

/**
 * Registers a ResultRecorder.
 */
class ENVIR_API ResultRecorderDescriptor : public cNoncopyableOwnedObject
{
  private:
    ResultRecorder *(*creatorfunc)();

  public:
    /**
     * Constructor.
     */
    ResultRecorderDescriptor(const char *name, ResultRecorder *(*f)());

    /**
     * Creates an instance of a particular class by calling the creator
     * function.
     */
    ResultRecorder *create() const  {return creatorfunc();}

    /**
     * Finds the factory object for the given name. The class must have been
     * registered previously with the Register_ResultRecorder() macro.
     */
    static ResultRecorderDescriptor *find(const char *name);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static ResultRecorderDescriptor *get(const char *name);
};

//XXX Misc notes: revise!
//
// When used in SOURCE= key:
//
// Maps ONE SIGNAL to ONE SIGNAL (i.e. vector-to-vector one-to-one mapping).
// Scalars should be recorded by appending a Last (or Mean,Min,Max etc) listener).
// DOES NOT RECORD ANYTHING.
//
// PROCESSORS:
//
// count(s), sum(s), min(s), max(s), mean(s), timeavg(s), last(s), constant(s,VALUE),
// constant0(s), constant1(s), byteLength(s), messageKind(s), queueLength(s),
// filter(s,PREDICATE), removeRepeats(s), timeDiff(s), timeShift(), movingAverage(s),
// eval(s,EXPRESSION); spec versions of filter(s,PREDICATE) (==,!=,<,>,<=,>=);
// spec versions of eval(s,EXPRESSION):add,substract,etc..
//
// simtime(s): the current simulation time
//
// (EXPRESSION and PREDICATE may use common/expression.h stuff)
// common's Expression should be turned into a generic expression parser
// (parser should build the Elem[] via a callback object, and then we can
// use it for signals parsing too)
//
// IN source=, VARIABLES REFER TO SIGNALS.
// IN record=, VARIABLES ARE INTERPRETED AS RECORDER NAMES (foo is short for foo()); AND SIGNALS CANNOT BE REFERENCED
//
// SYNTAX:
//
// source=NAME   <-- NAME is interpreted as signal
// record=NAME   <-- NAME must be a filter (min,max,etc)
// source=NAME(SIGNAL) <-- applies the vector processor
// record=min(SIGNAL)  <-- forbidden, should be written into source=
// record=min()
// record=count/simtime  //simtime() is tricky!!! is should NOT do last(), but the current simulation time!!!
// - if there is no record, the default is vector()
//
//

#endif

