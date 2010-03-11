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
 * Abstract base class for result recorders.
 */
class ENVIR_API ResultRecorder : public ResultListener
{
    private:
        static CommonStringPool namesPool;
        cComponent *component;
        const char *statisticName;
        const char *recordingMode;
    protected:
        virtual opp_string_map getStatisticAttributes();
        virtual void tweakTitle(opp_string& title);
    public:
        virtual void init(cComponent *component, const char *statisticName, const char *recordingMode);
        virtual cComponent *getComponent() const {return component;}
        virtual const char *getStatisticName() const {return statisticName;}
        virtual const char *getRecordingMode() const {return recordingMode;}
        virtual std::string getResultName() const {return std::string(getStatisticName())+":"+getRecordingMode();}
};

/**
 * Abstract base class for numeric result recorders. Numeric result recorders
 * convert all numeric data types to double, and throw an error for non-numeric
 * types (const char *, cObject *).
 */
class ENVIR_API NumericResultRecorder : public ResultRecorder
{
    protected:
        // all receiveSignal() methods either throw error or delegate here.
        // advice: override one of them, and let the other delegate to it
        virtual void collect(double value) = 0;
        virtual void collect(simtime_t t, double value) = 0;
    public:
        virtual void receiveSignal(ResultFilter *prev, long l);
        virtual void receiveSignal(ResultFilter *prev, double d);
        virtual void receiveSignal(ResultFilter *prev, simtime_t t, double d);
        virtual void receiveSignal(ResultFilter *prev, simtime_t t);
        virtual void receiveSignal(ResultFilter *prev, const char *s);
        virtual void receiveSignal(ResultFilter *prev, cObject *obj);
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

#endif

