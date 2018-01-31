//==========================================================================
//  CRESULTFILTER.H - part of
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

#ifndef __OMNETPP_CRESULTFILTER_H
#define __OMNETPP_CRESULTFILTER_H

#include "simkerneldefs.h"
#include "cresultlistener.h"
#include "onstartup.h"
#include "globals.h"
#include "cownedobject.h"

namespace omnetpp {

class cResultFilter;
class cProperty;

/**
 * @brief Registers a result filter.
 *
 * The class must be a subclass of cResultFilter. Registered result filters
 * can be used in the <tt>source=</tt> and <tt>record=</tt> attributes of
 * <tt>@statistic</tt> properties in NED files, and with the
 * <tt>**.result-recording-modes=</tt> configuration option.
 *
 * @ingroup RegMacros
 * @hideinitializer
 */
#define Register_ResultFilter(NAME, CLASSNAME) \
  static omnetpp::cResultFilter *__FILEUNIQUENAME__() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(omnetpp::resultFilters.getInstance()->add(new omnetpp::cResultFilterType(NAME,__FILEUNIQUENAME__));)


/**
 * @brief Base class for result filters.
 *
 * Result filters map ONE SIGNAL to ONE SIGNAL (i.e. vector-to-vector
 * one-to-one mapping), and accept several listeners (delegates).
 * Result filters do not record anything -- that is left to result recorders.
 *
 * @see Register_ResultFilter()
 *
 * @ingroup Statistics
 */
class SIM_API cResultFilter : public cResultListener
{
    private:
        cResultListener **delegates; // nullptr-terminated array
    protected:
        // methods for invoking the delegates
        void fire(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details);
        void fire(cResultFilter *prev, simtime_t_cref t, long l, cObject *details);
        void fire(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details);
        void fire(cResultFilter *prev, simtime_t_cref t, double d, cObject *details);
        void fire(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details);
        void fire(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details);
        void fire(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details);
        virtual void callFinish(cResultFilter *prev) override;
        virtual void forEachChild(cVisitor *v) override;
    public:
        cResultFilter();
        ~cResultFilter();
        virtual void init(cComponent *component, cProperty *attrsProperty) {}
        virtual void addDelegate(cResultListener *delegate);
        virtual int getNumDelegates() const;
        cResultListener *getDelegate(int k) const {return delegates[k];}  // unsafe
        std::vector<cResultListener*> getDelegates() const;
};

/**
 * @brief Base class for filters that expect to receive an numeric value.
 *
 * This class overrides all other receiveSignal() methods to throw an exception,
 * and delegates numeric types to a common (actually, two common) methods.
 *
 * @ingroup SimSupport
 */
class SIM_API cNumericResultFilter : public cResultFilter
{
    protected:
        // all receiveSignal() methods either throw error or delegate here;
        // return value: whether to invoke chained listeners (true) or to swallow the value (false)
        virtual bool process(simtime_t& t, double& value, cObject *details) = 0;
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override;
};

/**
 * @brief Base class for filters that expect to receive an object.
 *
 * This class overrides all other receiveSignal() methods
 * to throw an exception.
 *
 * @ingroup SimSupport
 */
class SIM_API cObjectResultFilter : public cResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override;
};

/**
 * @brief Registers a cResultFilter.
 *
 * @ingroup Internals
 */
class SIM_API cResultFilterType : public cNoncopyableOwnedObject
{
  private:
    cResultFilter *(*creatorfunc)();

  public:
    /**
     * Constructor.
     */
    cResultFilterType(const char *name, cResultFilter *(*f)());

    /**
     * Creates an instance of a particular class by calling the creator
     * function.
     */
    cResultFilter *create() const  {return creatorfunc();}

    /**
     * Finds the factory object for the given name. The class must have been
     * registered previously with the Register_ResultFilter() macro.
     */
    static cResultFilterType *find(const char *name);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static cResultFilterType *get(const char *name);
};

}  // namespace omnetpp

#endif


