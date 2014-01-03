//==========================================================================
//  CRESULTRECORDER.H - part of
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

#ifndef __CRESULTRECORDER_H
#define __CRESULTRECORDER_H

#include "cresultlistener.h"
#include "ccomponent.h"
#include "onstartup.h"
#include "globals.h"

NAMESPACE_BEGIN

class cResultRecorder;
class cProperty;

/**
 * Registers a result recorder. The class must be a subclass of cResultRecorder.
 * Registered result recorders can be used in the <tt>record=</tt> attributes
 * of <tt>\@statistic</tt> properties in NED files, and with the
 * <tt>**.result-recording-modes=</tt> configuration option.
 *
 * @ingroup MacrosReg
 * @hideinitializer
 */
#define Register_ResultRecorder(NAME, CLASSNAME) \
  static cResultRecorder *__FILEUNIQUENAME__() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(resultRecorders.getInstance()->add(new cResultRecorderDescriptor(NAME,__FILEUNIQUENAME__));)


/**
 * Abstract base class for result recorders.
 *
 * @see Register_ResultRecorder()
 * @ingroup EnvirExtensions
 */
class SIM_API cResultRecorder : public cResultListener
{
    private:
        cComponent *component;
        const char *statisticName;
        const char *recordingMode;
        cProperty *attrsProperty;  // property to take result attributes from (normally @statistic[statisticName])
        opp_string_map *manualAttrs; // if non-null, overrides attrsProperty

    protected:
        virtual opp_string_map getStatisticAttributes(); // order: manualAttrs, then attrsProperty
        virtual opp_string_map getStatisticAttributesFrom(cProperty *property);
        virtual void tweakTitle(opp_string& title);

    public:
        /**
         * Sets contextual information on the result recorder: it will record a (scalar, vector, etc)
         * result for the given component, with a name composed of statisticName and recordingMode,
         * result attributes taken either from the keys and values of the attrsProperty NED property,
         * or from the manualAttrs string map (exactly one of the two must be non-NULL). manualAttrs,
         * when specified, will be owned (i.e. deleted) by the result recorder object.
         */
        virtual void init(cComponent *component, const char *statisticName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs=NULL);
        virtual ~cResultRecorder() {delete manualAttrs;}
        virtual cComponent *getComponent() const {return component;}
        virtual const char *getStatisticName() const {return statisticName;}
        virtual const char *getRecordingMode() const {return recordingMode;}
        virtual std::string getResultName() const {return std::string(getStatisticName())+":"+getRecordingMode();}
};

/**
 * Abstract base class for numeric result recorders. Numeric result recorders
 * convert all numeric data types to double, and throw an error for non-numeric
 * types (const char *, cObject *).
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cNumericResultRecorder : public cResultRecorder
{
    protected:
        // all receiveSignal() methods either throw error or delegate here.
        virtual void collect(simtime_t_cref t, double value) = 0;
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
 * Registers a cResultRecorder.
 *
 * @ingroup Internals
 */
class SIM_API cResultRecorderDescriptor : public cNoncopyableOwnedObject
{
  private:
    cResultRecorder *(*creatorfunc)();

  public:
    /**
     * Constructor.
     */
    cResultRecorderDescriptor(const char *name, cResultRecorder *(*f)());

    /**
     * Creates an instance of a particular class by calling the creator
     * function.
     */
    cResultRecorder *create() const  {return creatorfunc();}

    /**
     * Finds the factory object for the given name. The class must have been
     * registered previously with the Register_ResultRecorder() macro.
     */
    static cResultRecorderDescriptor *find(const char *name);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static cResultRecorderDescriptor *get(const char *name);
};

NAMESPACE_END

#endif

