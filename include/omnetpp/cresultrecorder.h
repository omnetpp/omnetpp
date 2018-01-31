//==========================================================================
//  CRESULTRECORDER.H - part of
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

#ifndef __OMNETPP_CRESULTRECORDER_H
#define __OMNETPP_CRESULTRECORDER_H

#include "cresultlistener.h"
#include "ccomponent.h"
#include "onstartup.h"
#include "globals.h"

namespace omnetpp {

class cResultRecorder;
class cProperty;

/**
 * @brief Registers a result recorder class.
 *
 * The class must be a subclass of cResultRecorder.
 *
 * Registered result recorders can be used in the <tt>record=</tt> attributes
 * of <tt>@statistic</tt> properties in NED files, and with the
 * <tt>**.result-recording-modes=</tt> configuration option.
 *
 * @ingroup RegMacros
 * @hideinitializer
 */
#define Register_ResultRecorder(NAME, CLASSNAME) \
  static omnetpp::cResultRecorder *__FILEUNIQUENAME__() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(omnetpp::resultRecorders.getInstance()->add(new omnetpp::cResultRecorderType(NAME,__FILEUNIQUENAME__));)


/**
 * @brief Abstract base class for result recorders.
 *
 * @see Register_ResultRecorder()
 *
 * @ingroup Statistics
 */
class SIM_API cResultRecorder : public cResultListener
{
    private:
        cComponent *component;
        const char *statisticName;
        const char *recordingMode;
        cProperty *attrsProperty;  // property to take result attributes from (normally @statistic[statisticName])
        opp_string_map *manualAttrs; // if non-null, overrides attrsProperty
        bool finishCalled; // to prevent double-recording of scalar results based on multiple signals

    protected:
        virtual opp_string_map getStatisticAttributes(); // order: manualAttrs, then attrsProperty
        virtual opp_string_map getStatisticAttributesFrom(cProperty *property);
        virtual void tweakTitle(opp_string& title);
        virtual void callFinish(cResultFilter *prev) override;

    public:
        /**
         * Sets contextual information on the result recorder: it will record a (scalar, vector, etc)
         * result for the given component, with a name composed of statisticName and recordingMode,
         * result attributes taken either from the keys and values of the attrsProperty NED property,
         * or from the manualAttrs string map (exactly one of the two must be non-nullptr). manualAttrs,
         * when specified, will be owned (i.e. deleted) by the result recorder object.
         */
        virtual void init(cComponent *component, const char *statisticName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs=nullptr);
        virtual ~cResultRecorder() {delete manualAttrs;}
        virtual const char *getName() const override {return getStatisticName();}
        virtual std::string getFullPath() const override {return getComponent()->getFullPath() + "." + getResultName();}
        virtual cComponent *getComponent() const {return component;}
        virtual const char *getStatisticName() const {return statisticName;}
        virtual const char *getRecordingMode() const {return recordingMode;}
        virtual std::string getResultName() const {return std::string(getStatisticName())+":"+getRecordingMode();}
};

/**
 * @brief Abstract base class for numeric result recorders.
 *
 * Numeric result recorders convert all numeric data types to double,
 * and throw an error for non-numeric types (const char *, cObject *).
 *
 * @ingroup SimSupport
 */
class SIM_API cNumericResultRecorder : public cResultRecorder
{
    protected:
        // all receiveSignal() methods either throw error or delegate here.
        virtual void collect(simtime_t_cref t, double value, cObject *details) = 0;
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
 * @brief Registers a cResultRecorder.
 *
 * @ingroup Internals
 */
class SIM_API cResultRecorderType : public cNoncopyableOwnedObject
{
  private:
    cResultRecorder *(*creatorfunc)();

  public:
    /**
     * Constructor.
     */
    cResultRecorderType(const char *name, cResultRecorder *(*f)());

    /**
     * Creates an instance of a particular class by calling the creator
     * function.
     */
    cResultRecorder *create() const  {return creatorfunc();}

    /**
     * Finds the factory object for the given name. The class must have been
     * registered previously with the Register_ResultRecorder() macro.
     */
    static cResultRecorderType *find(const char *name);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static cResultRecorderType *get(const char *name);
};

}  // namespace omnetpp

#endif

