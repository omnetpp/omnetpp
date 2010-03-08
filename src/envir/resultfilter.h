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


class ENVIR_API ResultFilter : public ResultListener
{
    private:
        ResultListener **delegates; // NULL-terminated array
    protected:
        void fire(cComponent *source, simsignal_t signalID, long l);
        void fire(cComponent *source, simsignal_t signalID, double d);
        void fire(cComponent *source, simsignal_t signalID, simtime_t t);
        void fire(cComponent *source, simsignal_t signalID, const char *s);
        void fire(cComponent *source, simsignal_t signalID, cObject *obj);
    public:
        ResultFilter();
        ~ResultFilter();
        virtual void addDelegate(ResultListener *delegate);
        virtual int getNumDelegates() const;
        std::vector<ResultListener*> getDelegates() const;
        virtual void listenerAdded(cComponent *component, simsignal_t signalID);
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID);
        virtual void finish(cComponent *component, simsignal_t signalID);
};

class ENVIR_API NumericResultFilter : public ResultFilter
{
    protected:
        // return value: whether to delegate (true) or to swallow the value (false)
        virtual bool process(double& value) = 0;
        virtual bool process(simtime_t& t, double& value) = 0;
    public:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
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


