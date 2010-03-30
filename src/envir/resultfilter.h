//==========================================================================
//  RESULTFILTER.H - part of
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

#ifndef __ENVIR_RESULTFILTER_H
#define __ENVIR_RESULTFILTER_H

#include "envirdefs.h"
#include "resultlistener.h"
#include "stringpool.h"
#include "onstartup.h"
#include "cregistrationlist.h"
#include "cownedobject.h"

class ResultFilter;

#define Register_ResultFilter(NAME, CLASSNAME) \
  static ResultFilter *__FILEUNIQUENAME__() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(resultFilters.getInstance()->add(new ResultFilterDescriptor(NAME,__FILEUNIQUENAME__));)

extern cGlobalRegistrationList resultFilters;

/**
 * Base class for result filters. Result filters map ONE SIGNAL to ONE SIGNAL
 * (i.e. vector-to-vector one-to-one mapping), and accept several listeners
 * (delegates). Result filters do not record anything -- that is left to result
 * recorders.
 */
class ENVIR_API ResultFilter : public ResultListener
{
    private:
        ResultListener **delegates; // NULL-terminated array
    protected:
        // methods for invoking the delegates
        void fire(ResultFilter *prev, simtime_t_cref t, long l);
        void fire(ResultFilter *prev, simtime_t_cref t, unsigned long l);
        void fire(ResultFilter *prev, simtime_t_cref t, double d);
        void fire(ResultFilter *prev, simtime_t_cref t, const SimTime& v);
        void fire(ResultFilter *prev, simtime_t_cref t, const char *s);
        void fire(ResultFilter *prev, simtime_t_cref t, cObject *obj);
    public:
        ResultFilter();
        ~ResultFilter();
        virtual void addDelegate(ResultListener *delegate);
        virtual int getNumDelegates() const;
        std::vector<ResultListener*> getDelegates() const;
        virtual void finish(ResultFilter *prev);
};

/**
 * Base class for filters that expect to receive an numeric value.
 * This class overrides all other receiveSignal() methods to throw an exception,
 * and delegates numeric types to a common (actually, two common) methods.
 */
class ENVIR_API NumericResultFilter : public ResultFilter
{
    protected:
        // all receiveSignal() methods either throw error or delegate here;
        // return value: whether to invoke chained listeners (true) or to swallow the value (false)
        virtual bool process(simtime_t& t, double& value) = 0;
    public:
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, long l);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, double d);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const char *s);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *obj);
};

/**
 * Base class for filters that expect to receive an object.
 * This class overrides all other receiveSignal() methods
 * to throw an exception.
 */
class ENVIR_API ObjectResultFilter : public ResultFilter
{
    public:
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, long l);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, double d);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const char *s);
};

/**
 * Registers a ResultFilter.
 */
class ENVIR_API ResultFilterDescriptor : public cNoncopyableOwnedObject
{
  private:
    ResultFilter *(*creatorfunc)();

  public:
    /**
     * Constructor.
     */
    ResultFilterDescriptor(const char *name, ResultFilter *(*f)());

    /**
     * Creates an instance of a particular class by calling the creator
     * function.
     */
    ResultFilter *create() const  {return creatorfunc();}

    /**
     * Finds the factory object for the given name. The class must have been
     * registered previously with the Register_ResultFilter() macro.
     */
    static ResultFilterDescriptor *find(const char *name);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static ResultFilterDescriptor *get(const char *name);
};

#endif


