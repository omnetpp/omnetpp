//==========================================================================
//   COUTVECT.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cOutVector         : represents an output vector
//    cOutputVectorManager  : actually records for cOutVector objects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

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
typedef void (*RecordFunc)(void *, double, double);

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
    int tuple;           // values: 1 or 2
    void *handle;        // identifies output vector for the output vector manager
    long num_received;   // total number of values passed to the output vector object
    long num_stored;     // number of values actually stored

    // the following members will be used directly by inspectors
    friend class TOutVectorInspector;
    friend class TOutVectorWindow;
    RecordFunc record_in_inspector; // to notify inspector about file writes
    void *data_for_inspector;       // FIXME: why not via a setCallback() function??

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor. Accepts the object name. The second argument can be 1 or 2.
     */
    explicit cOutVector(const char *name=NULL, int tuple=1);

    /**
     * Copy constructor.
     */
    cOutVector(const cOutVector& r) : cObject(r) {setName(r.name());operator=(r);}

    /**
     * Destructor.
     */
    virtual ~cOutVector();

    /**
     * Assignment is not supported by this class: this method throws a cException when called.
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
     * gives an error (throws cException) when called.
     */
    virtual cObject *dup() const    {return new cOutVector(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);
    //@}

    /** @name Configuring and writing to output vectors. */
    //@{

    /**
     * Records the value with the current simulation time as timestamp.
     * It can be used only in the case if the instance of cOutVector was created with tuple=1,
     * otherwise it throws cException.
     *
     * The return value is true if the data was actually recorded, and false if it
     * was not recorded (because of filtering, etc.)
     */
    virtual bool record(double value);

    /**
     * Records two values with the current simulation time as timestamp.
     * It can be used only in the case if the instance of cOutVector was created with tuple=2,
     * otherwise it throws cException.
     *
     * The return value is true if the data was actually recorded, and false if it
     * was not recorded (because of filtering, etc.)
     */
    virtual bool record(double value1, double value2);

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
     * Returns true if recording the data is enabled, false otherwise.
     */
    virtual bool isEnabled()  {return enabled;}

    /**
     * Returns the total number of values passed to the record() method of
     * this output vector object. This includes the values passed while
     * the object was disabled (see disable()).
     */
    long valuesReceived()  {return num_received;}

    /**
     * Returns the number of values actually stored by this output vector object.
     * The values passed while the object was disabled (via disable(),
     * environment configuration, filtering, etc.) do not count.
     */
    long valuesStored()  {return num_stored;}
    //@}
};

#endif


