//==========================================================================
//  COUTVECT.H - part of
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

#ifndef __COUTVECT_H
#define __COUTVECT_H

#include <stdio.h>
#include "cobject.h"


/**
 * Prototype for callback functions that are used to notify graphical user
 * interfaces when values are recorded to an output vector (see cOutVector).
 * @ingroup EnumsTypes
 */
typedef void (*RecordFunc)(void *, simtime_t, double, double);

class TOutVectorInspector;
class TOutVectorWindow;


/**
 * Responsible for recording vector simuotation results (an output vector).
 * A cOutVector object can write doubles to the output vector file
 * (or any another device determined by the current cOutputVectorManager).
 *
 * @ingroup Statistics
 */
class SIM_API cOutVector : public cObject
{
  protected:
    bool enabled;        // if false, record() method will do nothing
    int tupl;            // values: 1 or 2
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
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. Accepts the object name, and tuple which can be 1 or 2
     * (2 is discouraged).
     */
    explicit cOutVector(const char *name=NULL, int tuple=1);

    /**
     * Copy constructor.
     */
    cOutVector(const cOutVector& r) : cObject() {setName(r.name());operator=(r);}

    /**
     * Destructor.
     */
    virtual ~cOutVector();

    /**
     * Assignment is not supported by this class: this method throws a cRuntimeError when called.
     */
    cOutVector& operator=(const cOutVector&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Sets the name of the object. It is not possible to call this method after the
     * first call to record().
     */
    virtual void setName(const char *name);

    /**
     * Dupping is not implemented for cOutVector. This function
     * gives an error (throws cRuntimeError) when called.
     */
    virtual cPolymorphic *dup() const    {return new cOutVector(*this);}

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

    /**
     * Records the value with the current simulation time as timestamp.
     * This method can be used with cOutVectors created with tuple=1.
     *
     * The return value is true if the data was actually recorded, and false
     * if it was not recorded (because of filtering, etc.)
     */
    virtual bool record(double value);

    /**
     * Records two values with the current simulation time as timestamp.
     * This method can be used with cOutVectors created with tuple=2.
     *
     * The return value is true if the data was actually recorded, and false
     * if it was not recorded (because of filtering, etc.)
     */
    virtual bool record(double value1, double value2);

    /**
     * Records the value with the given time as timestamp. Values must be
     * recorded in increasing timestamp order, that is, it is not possible
     * to record a value with a timestamp that is less than that of the
     * last recorded value.
     *
     * This method can be used with cOutVectors created with tuple=1.
     *
     * The return value is true if the data was actually recorded, and false
     * if it was not recorded (because of filtering, etc.)
     */
    virtual bool recordWithTimestamp(simtime_t t, double value);

    /**
     * Records two values with the given time as timestamp. Values must be
     * recorded in increasing timestamp order, that is, it is not possible
     * to record a value with a timestamp that is less than that of the
     * last recorded value.
     *
     * This method can be used with cOutVectors created with tuple=2.
     *
     * The return value is true if the data was actually recorded, and false
     * if it was not recorded (because of filtering, etc.)
     */
    virtual bool recordWithTimestamp(simtime_t t, double value1, double value2);

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

    /**
     * Returns the tuple parameter passed to the constructor.
     */
    int tuple() {return tupl;}
    //@}
};

#endif


