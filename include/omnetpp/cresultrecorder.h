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
#include "opp_pooledstring.h"
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
 * @see @ref ResultFiltersRecorders
 * @ingroup RegMacros
 * @hideinitializer
 */
#define Register_ResultRecorder(NAME, CLASSNAME) \
  __REGISTER_CLASS_X(CLASSNAME, omnetpp::cResultRecorder, "result recorder", omnetpp::internal::resultRecorders.getInstance()->add(new omnetpp::cResultRecorderType(NAME,omnetpp::opp_typename(typeid(CLASSNAME)))) )

/**
 * @brief Registers a result recorder class with a description string.
 *
 * The class must be a subclass of cResultRecorder.
 *
 * Registered result recorders can be used in the <tt>record=</tt> attributes
 * of <tt>@statistic</tt> properties in NED files, and with the
 * <tt>**.result-recording-modes=</tt> configuration option.
 *
 * @see @ref ResultFiltersRecorders
 * @ingroup RegMacros
 * @hideinitializer
 */
#define Register_ResultRecorder2(NAME, CLASSNAME, DESCRIPTION) \
  __REGISTER_CLASS_X(CLASSNAME, omnetpp::cResultRecorder, "result recorder", omnetpp::internal::resultRecorders.getInstance()->add(new omnetpp::cResultRecorderType(NAME,omnetpp::opp_typename(typeid(CLASSNAME)),DESCRIPTION)) )


/**
 * @brief Abstract base class for result recorders.
 *
 * @see Register_ResultRecorder()
 *
 * @ingroup ResultFiltersRecorders
 */
class SIM_API cResultRecorder : public cResultListener
{
    public:
        /**
         * Context information for initialization. The result recorded should record a
         * result (scalar, vector, etc) for the given component, with a name composed
         * of (at least) statisticName and recordingMode. Result attributes should be
         * taken either from the keys and values of the attrsProperty NED property,
         * or from the manualAttrs string map (exactly one of the two must be non-nullptr).
         * manualAttrs, when specified, will be owned (i.e. deleted) by the result recorder
         * object.
         */
        struct Context {
            cComponent *component;     ///< The statistic should be recorded for this module/channel; usually the module on which the @statistic property is defined.
            const char *statisticName; ///< The base name for the statistic; usually the index of the @statistic[] property (the name in brackets).
            const char *recordingMode; ///< The recording mode; usually an element the the record=... list in the @statistic property.
            cProperty *attrsProperty;  ///< An optional property from which extra attributes can be taken; usually the @statistic property.
            opp_string_map *manualAttrs = nullptr;  // If attrsProperty==nullptr, specifies an alternative source of attributes; the ownership is passed into the  result recorder object.
        };
    private:
        cComponent *component = nullptr;
        opp_pooledstring statisticName = nullptr;
        opp_pooledstring recordingMode = nullptr;
        opp_pooledstring demuxLabel = nullptr;
        cProperty *attrsProperty = nullptr;  // property to take result attributes from (normally @statistic[statisticName])
        opp_string_map *manualAttrs = nullptr; // if non-null, overrides attrsProperty
        bool finishCalled = false; // to prevent double-recording of scalar results based on multiple signals

    protected:
        virtual opp_string_map getStatisticAttributes(); // order: manualAttrs, then attrsProperty
        virtual opp_string_map getStatisticAttributesFrom(cProperty *property);
        virtual void tweakTitle(opp_string& title);
        virtual void callFinish(cResultFilter *prev) override;

    public:
        cResultRecorder() {}
        virtual ~cResultRecorder() {delete manualAttrs;}
        virtual void init(Context *ctx);
        [[deprecated]] virtual void init(cComponent *component, const char *statisticName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs=nullptr) {} // deprecated, left for backward compatibility
        virtual cResultRecorder *clone() const override;
        virtual const char *getName() const override {return getStatisticName();}
        virtual std::string getFullPath() const override {return getComponent()->getFullPath() + "." + getResultName();}
        virtual cComponent *getComponent() const {return component;}
        virtual const char *getStatisticName() const {return statisticName.c_str();}
        virtual const char *getRecordingMode() const {return recordingMode.c_str();}
        virtual const char *getDemuxLabel() const {return demuxLabel.c_str();}
        virtual void setDemuxLabel(const char *s) {demuxLabel = s;}
        virtual std::string getResultName() const;
};

/**
 * @brief Abstract base class for numeric result recorders.
 *
 * Numeric result recorders convert all numeric data types to double,
 * and throw an error for non-numeric types (const char *, cObject *).
 *
 * @ingroup ResultFiltersRecorders
 */
class SIM_API cNumericResultRecorder : public cResultRecorder
{
    protected:
        // all receiveSignal() methods either throw error or delegate here.
        virtual void collect(simtime_t_cref t, double value, cObject *details) = 0;
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details) override;
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
    std::string description;
    std::string className;

  public:
    /**
     * Constructor.
     */
    cResultRecorderType(const char *name, const char *className, const char *description=nullptr);

    /**
     * Creates an instance of this result recorder type.
     */
    cResultRecorder *create() const;

    /**
     * Returns the documentation of this result recorder.
     */
    const char *getDescription() const {return description.c_str();}

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

