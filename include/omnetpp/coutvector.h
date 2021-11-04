//==========================================================================
//  COUTVECTOR.H - part of
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

#ifndef __OMNETPP_COUTVECTOR_H
#define __OMNETPP_COUTVECTOR_H

#include <cstdio>
#include "cownedobject.h"
#include "simtime_t.h"

namespace omnetpp {


/**
 * @brief Prototype for callback functions that are used to notify graphical user
 * interfaces when values are recorded to an output vector (see cOutVector).
 * @ingroup Statistics
 */
typedef void (*RecordFunc)(void *, simtime_t, double);

class cEnum;


/**
 * @brief Responsible for recording vector simulation results (an output vector).
 *
 * A cOutVector object can write doubles to the output vector file
 * (or any another device determined by the current cIOutputVectorManager).
 *
 * @ingroup Statistics
 */
class SIM_API cOutVector : public cNoncopyableOwnedObject
{
  protected:
    enum {
      FL_ENABLED = 4,      // flag: when false, record() method will do nothing
      FL_RECORDWARMUP = 8, // flag: when set, object records data during warmup period as well
    };

    void *handle = nullptr;   // identifies output vector for the output vector manager
    simtime_t lastTimestamp;  // last timestamp written, needed to ensure increasing timestamp order
    long numReceived = 0;     // total number of values passed to the output vector object
    long numStored = 0;       // number of values actually stored

    // the following members will be used directly by inspectors
    RecordFunc recordInInspector = nullptr; // to notify inspector about file writes
    void *dataForInspector;

  public:
    // internal: called from behind cEnvir
    void setCallback(RecordFunc f, void *d) {recordInInspector=f; dataForInspector=d;}

  public:
    enum Type { TYPE_INT, TYPE_DOUBLE, TYPE_ENUM };
    enum InterpolationMode { NONE, SAMPLE_HOLD, BACKWARD_SAMPLE_HOLD, LINEAR };

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    explicit cOutVector(const char *name=nullptr);

    /**
     * Destructor.
     */
    virtual ~cOutVector();
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Sets the name of the object. It is not possible to call this method after the
     * first call to record().
     */
    virtual void setName(const char *name) override;

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;
    //@}

    /** @name Metadata annotations. */
    //@{
    /**
     * Associate the vector with an enum defined in a msg file.
     * This information gets recorded into the output vector file and
     * may be used by result analysis tools.
     * cOutVector does not verify that recorded values actually comply
     * with this annotation.
     */
    virtual void setEnum(const char *registeredEnumName);

    /**
     * Associate the vector with an enum. The enum name as well as the
     * symbols and their integer values will get recorded into the output
     * vector file, and may be used by result analysis tools.
     * cOutVector does not verify that recorded values actually comply
     * with this annotation.
     */
    virtual void setEnum(cEnum *enumDecl);

    /**
     * Annotate the vector with a physical unit (like "s", "mW" or "bytes").
     * This information gets recorded into the output vector file, and
     * may be used by result analysis tools.
     */
    virtual void setUnit(const char *unit);

    /**
     * Annotate the vector with a data type. This information gets recorded
     * into the output vector file, and may be used by result analysis tools.
     * cOutVector does not verify that recorded values actually comply with
     * this annotation.
     */
    virtual void setType(Type type);

    /**
     * Annotate the vector with an interpolation mode. This information
     * gets recorded into the output vector file, and may be used
     * by result analysis tools as a hint for choosing a plotting style.
     */
    virtual void setInterpolationMode(InterpolationMode mode);

    /**
     * Annotate the vector with a minimum value. This information gets
     * recorded into the output vector file and may be used
     * by result analysis tools. cOutVector does not verify that
     * recorded values actually comply with this annotation.
     */
    virtual void setMin(double minValue);

    /**
     * Annotate the vector with a maximum value. This information gets
     * recorded into the output vector file and may be used
     * by result analysis tools. cOutVector does not verify that
     * recorded values actually comply with this annotation.
     */
    virtual void setMax(double maxValue);
    //@}

    /** @name Writing to output vectors. */
    //@{
    /**
     * Records the value with the current simulation time as timestamp.
     *
     * The return value is true if the data was actually recorded, and false
     * if it was not recorded (because of filtering, etc.)
     */
    virtual bool record(double value);

    /**
     * Convenience method, delegates to record(double).
     */
    virtual bool record(SimTime value)  {return record(value.dbl());}

    /**
     * Records the value with the given time as timestamp. Values must be
     * recorded in increasing timestamp order, that is, it is not possible
     * to record a value with a timestamp that is less than that of the
     * last recorded value.
     *
     * The return value is true if the data was actually recorded, and false
     * if it was not recorded (because of filtering, etc.)
     */
    virtual bool recordWithTimestamp(simtime_t t, double value);

    /**
     * Convenience method, delegates to recordWithTimestamp(simtime_t, double).
     */
    virtual bool recordWithTimestamp(simtime_t t, SimTime value) {return recordWithTimestamp(t, value.dbl());}

    /**
     * Enables recording data via this object. (It is enabled by default.)
     */
    virtual void enable()  {setFlag(FL_ENABLED,true);}

    /**
     * Disables recording data via this object. record() methods will return
     * false without doing anything.
     */
    virtual void disable()  {setFlag(FL_ENABLED,false);}

    /**
     * Enables/disables recording data via this object.
     * @see enable(), disable()
     */
    virtual void setEnabled(bool b)  {setFlag(FL_ENABLED,b);}

    /**
     * Returns true if recording the data is enabled, false otherwise.
     */
    virtual bool isEnabled() const  {return flags&FL_ENABLED;}

    /**
     * Enables/disables recording data during the warm-up period.
     * When set to false, record() calls will be ignored during
     * warm-up period.
     *
     * @see cSimulation::getWarmupPeriod()
     */
    virtual void setRecordDuringWarmupPeriod(bool b)  {setFlag(FL_RECORDWARMUP,b);}

    /**
     * Returns true if the object will record data during the warm-up period.
     * @see cSimulation::getWarmupPeriod()
     */
    virtual bool getRecordDuringWarmupPeriod() const  {return flags&FL_RECORDWARMUP;}

    /**
     * Returns the total number of values passed to the record() method of
     * this output vector object. This includes the values passed while
     * the object was disabled (see disable()).
     */
    long getValuesReceived() const  {return numReceived;}

    /**
     * Returns the number of values actually stored by this output vector object.
     * The values passed while the object was disabled (via disable(),
     * environment configuration, filtering, etc.) do not count.
     */
    long getValuesStored() const  {return numStored;}
    //@}
};

}  // namespace omnetpp


#endif


