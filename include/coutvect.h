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
 * @ingroup SimSupport
 */
class SIM_API cOutVector : public cObject
{
  protected:
    bool enabled;        // if false, record() method will do nothing
    int tuple;           // values: 1 or 2
    void *handle;        // identifies output vector for the output vector manager
    long num_recorded;   // total number of values passed to output vector
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
     * Assignment is not supported for this class. This function raises an error when called.
     */
    cOutVector& operator=(const cOutVector&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Sets the name of the object.
     */
    virtual void setName(const char *name);

    /**
     * Returns pointer to a string containing the class name, "cOutVector".
     */
    virtual const char *className() const {return "cOutVector";}

    /**
     * Dupping is not implemented for cOutVector. This function
     * gives an error when called.
     */
    virtual cObject *dup() const    {return new cOutVector(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cOutVectorIFC";}
    //@}

    /** @name Configuring and writing to output vectors. */
    //@{

    /**
     * Records the value with the current simulation time as timestamp.
     * It can be used only in the case if the instance of cOutVector was created with tuple=1,
     * otherwise it gives an error message.
     *
     * The return value is true if the data was actually recorded, and false if it
     * was not recorded (because of filtering, etc.)
     */
    virtual bool record(double value);

    /**
     * Records two values with the current simulation time as timestamp.
     * It can be used only in the case if the instance of cOutVector was created with tuple=2,
     * otherwise it gives an error message.
     *
     * The return value is true if the data was actually recorded, and false if it
     * was not recorded (because of filtering, etc.)
     */
    virtual bool record(double value1, double value2);

    /**
     * Enables recording data via this object. (It is enabled by default.)
     */
    virtual void enable() {enabled=true;}

    /**
     * Disables recording data via this object. record() methods will return
     * false without doing anything.
     */
    virtual void disable()  {enabled=false;}

    /**
     * Returns true if recording the data is enabled, false otherwise.
     */
    virtual bool isEnabled()  {return enabled;}
    //@}
};

#endif


