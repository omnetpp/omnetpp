//==========================================================================
//  COUTVECTOR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cOutVector         : represents an output vector
//    cOutputVectorManager  : actually records for cOutVector objects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COUTVECTOR_H
#define __COUTVECTOR_H

#include <stdio.h>
#include "cownedobject.h"

NAMESPACE_BEGIN


/**
 * Prototype for callback functions that are used to notify graphical user
 * interfaces when values are recorded to an output vector (see cOutVector).
 * @ingroup EnumsTypes
 */
typedef void (*RecordFunc)(void *, simtime_t, double, double);

class cEnum;


/**
 * Responsible for recording vector simuotation results (an output vector).
 * A cOutVector object can write doubles to the output vector file
 * (or any another device determined by the current cOutputVectorManager).
 *
 * @ingroup Statistics
 */
class SIM_API cOutVector : public cNoncopyableOwnedObject
{
  protected:
    bool enabled;        // if false, record() method will do nothing
    void *handle;        // identifies output vector for the output vector manager
    long num_received;   // total number of values passed to the output vector object
    long num_stored;     // number of values actually stored
    simtime_t last_t;    // last timestamp written, needed to ensure increasing timestamp order

    // the following members will be used directly by inspectors
    RecordFunc record_in_inspector; // to notify inspector about file writes
    void *data_for_inspector;

  public:
    // internal: called from behind cEnvir
    void setCallback(RecordFunc f, void *d) {record_in_inspector=f; data_for_inspector=d;}

  public:
    enum Type { TYPE_INT, TYPE_DOUBLE, TYPE_ENUM };
    enum InterpolationMode { NONE, SAMPLE_HOLD, BACKWARD_SAMPLE_HOLD, LINEAR };

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    explicit cOutVector(const char *name=NULL);

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
    virtual void setName(const char *name);

    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Packing and unpacking cannot be supported with this class.
     * This methods raises an error.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Packing and unpacking cannot be supported with this class.
     * This methods raises an error.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Configuring and writing to output vectors. */
    //@{

    //FIXME document!!!!
    virtual void setEnum(const char *registeredEnumName);
    virtual void setEnum(cEnum *enumDecl);
    virtual void setUnit(const char *unit);  // "s", "%", etc
    virtual void setType(Type type);
    virtual void setInterpolationMode(InterpolationMode mode);
    virtual void setMin(double minValue);
    virtual void setMax(double maxValue);

    /**
     * Records the value with the current simulation time as timestamp.
     *
     * The return value is true if the data was actually recorded, and false
     * if it was not recorded (because of filtering, etc.)
     */
    virtual bool record(double value);

#ifndef USE_DOUBLE_SIMTIME
    /**
     * Convenience method, delegates to record(double).
     */
    virtual bool record(simtime_t value)  {return record(value.dbl());}
#endif

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

#ifndef USE_DOUBLE_SIMTIME
    /**
     * Convenience method, delegates to recordWithTimestamp(simtime_t, double).
     */
    virtual bool recordWithTimestamp(simtime_t t, simtime_t value) {return recordWithTimestamp(t, value.dbl());}
#endif

    /**
     * Enables recording data via this object. (It is enabled by default.)
     */
    virtual void enable()  {enabled=true;}

    /**
     * Disables recording data via this object. record() methods will return
     * false without doing anything.
     */
    virtual void disable()  {enabled=false;}

    /**
     * Enables/disables recording data via this object.
     * @see enable(), disable()
     */
    virtual void setEnabled(bool b)  {enabled=b;}

    /**
     * Returns true if recording the data is enabled, false otherwise.
     */
    virtual bool isEnabled() const  {return enabled;}

    /**
     * Returns the total number of values passed to the record() method of
     * this output vector object. This includes the values passed while
     * the object was disabled (see disable()).
     */
    long valuesReceived() const  {return num_received;}

    /**
     * Returns the number of values actually stored by this output vector object.
     * The values passed while the object was disabled (via disable(),
     * environment configuration, filtering, etc.) do not count.
     */
    long valuesStored() const  {return num_stored;}
    //@}
};

NAMESPACE_END


#endif


