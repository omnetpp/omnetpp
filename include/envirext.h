//==========================================================================
//  ENVIREXT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cOutputVectorManager  : actually records for cOutVector objects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVIREXT_H
#define __ENVIREXT_H

#include <iostream>
#include "simkerneldefs.h"
#include "cobject.h"

NAMESPACE_BEGIN


class cModule;
class cStatistic;

/**
 * Abstract base class for output vector managers for cEnvir.
 * cOutputVectorManagers are plugins into the Envir user
 * interface library (src/envir) that handle recording the output for
 * cOutVectors.
 *
 * The default output vector manager is cFileOutputVectorManager, defined in the
 * Envir library.
 *
 * To change the way output vectors are written, subclass cOutputVectorManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * outputvectormanager-class="MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cOutputVectorManager : public cObject
{
  private:
    // copy constructor and assignment unsupported, make them inaccessible and also leave unimplemented
    cOutputVectorManager(const cOutputVectorManager&);
    cOutputVectorManager& operator=(const cOutputVectorManager&);

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cOutputVectorManager() {}

    /**
     * Destructor.
     */
    virtual ~cOutputVectorManager() {}
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Opens collecting. Called at the beginning of a simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    virtual void endRun() = 0;
    //@}

    /** @name Output vectors. */
    //@{

    /**
     * This method is called internally by the Envir library when a cOutVector object registers itself.
     * The return value is a handle of type void*; this handle is passed to
     * record() to identify the vector.
     */
    virtual void *registerVector(const char *modulename, const char *vectorname) = 0;

    /**
     * This method is called internally by the Envir library when a cOutVector object deregisters itself.
     */
    virtual void deregisterVector(void *vechandle) = 0;

    /**
     * This method is called internally by the Envir library when an attribute of the
     * cOutVector object is set.
     */
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value) = 0;

    /**
     * This method is called internally by the Envir library when a cOutVector object
     * writes a value into the output vector. The return value should be true if the
     * data was actually recorded, and false if it was not recorded (because of filtering, etc.)
     */
    virtual bool record(void *vechandle, simtime_t t, double value) = 0;

    /**
     * Returns the output vector file name. Returns NULL if this object is not
     * producing file output.
     */
    virtual const char *getFileName() const = 0;

    /**
     * Force writing out all buffered output.
     */
    virtual void flush() = 0;
    //@}
};


/**
 * Abstract base class for handling recording of output scalar data.
 * cOutputScalarManagers are plugins into the Envir user interface
 * library (src/envir) that ultimately handle calls to the
 * cComponent::recordScalar() family of functions.
 *
 * The default output scalar manager is cFileOutputScalarManager, defined
 * in the Envir library.
 *
 * To change the way scalar results are recorded, subclass cOutputScalarManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * outputscalarmanager-class="MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cOutputScalarManager : public cObject
{
  private:
    // copy constructor and assignment unsupported, make them inaccessible and also leave unimplemented
    cOutputScalarManager(const cOutputScalarManager&);
    cOutputScalarManager& operator=(const cOutputScalarManager&);

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cOutputScalarManager() {}

    /**
     * Destructor.
     */
    virtual ~cOutputScalarManager() {}
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Opens collecting. Called at the beginning of a simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    virtual void endRun() = 0;
    //@}

    /** @name Scalar statistics. */
    //@{

    /**
     * Records a double scalar result, in a default configuration into the scalar result file.
     */
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL) = 0;

    /**
     * Records a histogram or statistic object into the scalar result file.
     */
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL) = 0;

    /**
     * Returns the output scalar file name. Returns NULL if this object is not
     * producing file output.
     */
    virtual const char *getFileName() const = 0;

    /**
     * Force writing out all buffered output.
     */
    virtual void flush() = 0;
    //@}
};


/**
 * Abstract base class for snapshot managers. cSnapshotManagers are plugins
 * into the Envir user interface library (src/envir) that provide an output
 * stream into which snapshots are written (see cSimulation::snapshot()).
 *
 * The default output snapshot manager is cFileSnapshotManager, defined in the
 * Envir library.
 *
 * To create your own snaphot manager class, subclass cSnapshotManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * snapshotmanager-class="MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cSnapshotManager : public cObject
{
  private:
    // copy constructor and assignment unsupported, make them inaccessible and also leave unimplemented
    cSnapshotManager(const cSnapshotManager&);
    cSnapshotManager& operator=(const cSnapshotManager&);

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cSnapshotManager() {}

    /**
     * Destructor.
     */
    virtual ~cSnapshotManager() {}
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() = 0;
    //@}

    /** @name Snapshot management */
    //@{
    /**
     * Returns a stream where a snapshot can be written.
     */
    virtual std::ostream *getStreamForSnapshot() = 0;

    /**
     * Releases a stream after a snapshot was written.
     */
    virtual void releaseStreamForSnapshot(std::ostream *os) = 0;

    /**
     * Returns the snapshot file name. Returns NULL if this object is not
     * producing file output.
     */
    virtual const char *getFileName() const = 0;
    //@}
};

NAMESPACE_END


#endif


