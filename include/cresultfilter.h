//==========================================================================
//  CRESULTFILTER.H - part of
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

#ifndef __CRESULTFILTER_H
#define __CRESULTFILTER_H

#include "simkerneldefs.h"
#include "cresultlistener.h"
#include "onstartup.h"
#include "globals.h"
#include "cownedobject.h"

NAMESPACE_BEGIN

class cResultFilter;

/**
 * Registers a result filter. The class must be a subclass of cResultFilter.
 * Registered result filters can be used in the <tt>source=</tt> and
 * <tt>record=</tt> attributes of <tt>\@statistic</tt> properties in NED files,
 * and with the <tt>**.result-recording-modes=</tt> configuration option.
 *
 * @ingroup MacrosReg
 * @hideinitializer
 */
#define Register_ResultFilter(NAME, CLASSNAME) \
  static cResultFilter *__FILEUNIQUENAME__() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(resultFilters.getInstance()->add(new cResultFilterDescriptor(NAME,__FILEUNIQUENAME__));)


/**
 * Base class for result filters. Result filters map ONE SIGNAL to ONE SIGNAL
 * (i.e. vector-to-vector one-to-one mapping), and accept several listeners
 * (delegates). Result filters do not record anything -- that is left to result
 * recorders.
 *
 * @see Register_ResultFilter()
 * @ingroup EnvirExtensions
 */
class SIM_API cResultFilter : public cResultListener
{
    private:
        cResultListener **delegates; // NULL-terminated array
    protected:
        // methods for invoking the delegates
        void fire(cResultFilter *prev, simtime_t_cref t, bool b);
        void fire(cResultFilter *prev, simtime_t_cref t, long l);
        void fire(cResultFilter *prev, simtime_t_cref t, unsigned long l);
        void fire(cResultFilter *prev, simtime_t_cref t, double d);
        void fire(cResultFilter *prev, simtime_t_cref t, const SimTime& v);
        void fire(cResultFilter *prev, simtime_t_cref t, const char *s);
        void fire(cResultFilter *prev, simtime_t_cref t, cObject *obj);
    public:
        cResultFilter();
        ~cResultFilter();
        virtual void addDelegate(cResultListener *delegate);
        virtual int getNumDelegates() const;
        std::vector<cResultListener*> getDelegates() const;
        virtual void finish(cResultFilter *prev);
};

/**
 * Base class for filters that expect to receive an numeric value.
 * This class overrides all other receiveSignal() methods to throw an exception,
 * and delegates numeric types to a common (actually, two common) methods.
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cNumericResultFilter : public cResultFilter
{
    protected:
        // all receiveSignal() methods either throw error or delegate here;
        // return value: whether to invoke chained listeners (true) or to swallow the value (false)
        virtual bool process(simtime_t& t, double& value) = 0;
    public:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj);
};

/**
 * Base class for filters that expect to receive an object.
 * This class overrides all other receiveSignal() methods
 * to throw an exception.
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cObjectResultFilter : public cResultFilter
{
    public:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v);
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s);
};

/**
 * Registers a cResultFilter.
 *
 * @ingroup Internals
 */
class SIM_API cResultFilterDescriptor : public cNoncopyableOwnedObject
{
  private:
    cResultFilter *(*creatorfunc)();

  public:
    /**
     * Constructor.
     */
    cResultFilterDescriptor(const char *name, cResultFilter *(*f)());

    /**
     * Creates an instance of a particular class by calling the creator
     * function.
     */
    cResultFilter *create() const  {return creatorfunc();}

    /**
     * Finds the factory object for the given name. The class must have been
     * registered previously with the Register_ResultFilter() macro.
     */
    static cResultFilterDescriptor *find(const char *name);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static cResultFilterDescriptor *get(const char *name);
};

NAMESPACE_END

#endif


