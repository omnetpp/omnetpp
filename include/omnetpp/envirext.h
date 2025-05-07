//==========================================================================
//  ENVIREXT.H - part of
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

#ifndef __OMNETPP_ENVIREXT_H
#define __OMNETPP_ENVIREXT_H

#include <iostream>
#include "simkerneldefs.h"
#include "opp_string.h"
#include "simtime_t.h"
#include "cobject.h"
#include "simtime_t.h"
#include "opp_string.h"
#include "clifecyclelistener.h"

namespace omnetpp {

class cModule;
class cStatistic;
class cEvent;
class cMessage;
class cGate;
struct SendOptions;
struct ChannelResult;

/**
 * @brief Abstract base class for output vector managers for cEnvir.
 *
 * cIOutputVectorManagers are plugins into the Envir user
 * interface library (src/envir) that handle recording the output for
 * cOutVectors.
 *
 * The default output vector manager is cFileOutputVectorManager, defined in the
 * Envir library.
 *
 * To change the way output vectors are written, subclass cIOutputVectorManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to `omnetpp.ini`:
 *
 * ```
 * [General]
 * outputvectormanager-class="MyClass"
 * ```
 *
 * @ingroup EnvirAndExtensions
 */
class SIM_API cIOutputVectorManager : public cObject, public cISimulationLifecycleListener, noncopyable
{
  protected:
    /**
     * A cISimulationLifecycleListener method. Delegates to startRun(), endRun() and flush(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cIOutputVectorManager() {}

    /**
     * Destructor.
     */
    virtual ~cIOutputVectorManager() {}
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
     * Registers an output vector, and returns a handle that identifies the
     * vector in subsequent calls e.g. record().
     *
     * Note: registerVector() calls are also allowed before startRun().
     */
    virtual void *registerVector(const char *modulename, const char *vectorname, opp_string_map *attributes=nullptr) = 0;

    /**
     * Deregisters an output vector. Deregistration makes the vector handle invalid.
     *
     * Note: deregisterVector() calls are also allowed after endRun().
     */
    virtual void deregisterVector(void *vechandle) = 0;

    /**
     * Write data into the output vector. The return value is true if the
     * data was actually recorded, and false if it was not recorded (because of
     * filtering, etc.)
     *
     * Calling record() is only allowed between startRun() and endRun().
     */
    virtual bool record(void *vechandle, simtime_t t, double value) = 0;

    /**
     * Returns the output vector file name. Returns nullptr if this object is not
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
 * @brief Abstract base class for handling recording of output scalar data.
 *
 * cIOutputScalarManagers are plugins into the Envir user interface
 * library (src/envir) that ultimately handle calls to the
 * cComponent::recordScalar() family of functions.
 *
 * The default output scalar manager is cFileOutputScalarManager, defined
 * in the Envir library.
 *
 * To change the way scalar results are recorded, subclass cIOutputScalarManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to `omnetpp.ini`:
 *
 * ```
 * [General]
 * outputscalarmanager-class="MyClass"
 * ```
 *
 * @ingroup EnvirAndExtensions
 */
class SIM_API cIOutputScalarManager : public cObject, public cISimulationLifecycleListener, noncopyable
{
  protected:
    /**
     * A cISimulationLifecycleListener method. Delegates to startRun(), endRun() and flush(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cIOutputScalarManager() {}

    /**
     * Destructor.
     */
    virtual ~cIOutputScalarManager() {}
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
     * The return value indicates whether the recording has actually taken place (true=yes).
     */
    virtual bool recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) = 0;

    /**
     * Records a histogram or statistic object into the scalar result file.
     * This operation may invoke the setUpBins() method on the histogram object.
     * The return value indicates whether the recording has actually taken place (true=yes).
     */
    virtual bool recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) = 0;

    /**
     * Records a module or channel parameter, in a default configuration into the scalar result file.
     * The return value indicates whether the recording has actually taken place (true=yes).
     */
    virtual bool recordParameter(cPar *par) = 0;

    /**
     * Records the runtime NED type of module or channel, in a default configuration into the scalar result file.
     * The return value indicates whether the recording has actually taken place (true=yes).
     */
    virtual bool recordComponentType(cComponent *component) = 0;

    /**
     * Returns the output scalar file name. Returns nullptr if this object is not
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
 * @brief Abstract base class for snapshot managers.
 *
 * cISnapshotManagers are plugins into the Envir user interface library
 * (src/envir) that provide an output stream into which snapshots are written
 * (see cSimulation::snapshot()).
 *
 * The default output snapshot manager is cFileSnapshotManager, defined in the
 * Envir library.
 *
 * To create your own snaphot manager class, subclass cISnapshotManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to `omnetpp.ini`:
 *
 * ```
 * [General]
 * snapshotmanager-class="MyClass"
 * ```
 *
 * @ingroup EnvirAndExtensions
 */
class SIM_API cISnapshotManager : public cObject, public cISimulationLifecycleListener, noncopyable
{
  protected:
    /**
     * A cISimulationLifecycleListener method. Delegates to startRun() and endRun(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cISnapshotManager() {}

    /**
     * Destructor.
     */
    virtual ~cISnapshotManager() {}
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
     * Returns the snapshot file name. Returns nullptr if this object is not
     * producing file output.
     */
    virtual const char *getFileName() const = 0;
    //@}
};


/**
 * @brief Abstract base class for eventlog managers for cEnvir.
 *
 * cIEventlogManagers are plugins into the Envir user interface library
 * (src/envir) that handle the recording of simulation history into an
 * event log.
 *
 * The default eventlog manager is EventlogFileManager, defined in the
 * Envir library.
 *
 * To change the way event logs are written, subclass cIEventlogManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to `omnetpp.ini`:
 *
 * ```
 * [General]
 * eventlogmanager-class="MyClass"
 * ```
 *
 * @ingroup EnvirAndExtensions
 */
class SIM_API cIEventlogManager : public cObject, noncopyable
{
  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cIEventlogManager() {}

    /**
     * Destructor.
     */
    virtual ~cIEventlogManager() {}
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * (Re)starts eventlog recording. Whether eventlog recording is enabled by
     * default depends on the eventlog manager (e.g. omnetpp.ini configuration
     * options). This function may be called several times (from the user
     * interface) during the simulation, but only between events.
     * The eventlog manager is expected to produce output starting from the
     * next event.
     */
    virtual void resume() = 0;

    /**
     * Temporarily stops eventlog recording. See resume() for more details.
     */
    virtual void suspend() = 0;

    /**
     * Forces writing out all buffered output.
     */
    virtual void flush() = 0;

    /**
     * Returns the output scalar file name. Returns nullptr if this object is not
     * producing file output.
     */
    virtual const char *getFileName() const = 0;

    //TODO this one could be removed from the interface by invoking it via the LF_ON_RUN_END lifecycle event, but it needs to send more details
    virtual void endRun(bool isError, int resultCode, const char *message) = 0;
    //@}

    /** @name Functions called from cEnvir's similar functions */
    //@{
    virtual void simulationEvent(cEvent *event) = 0;
    virtual void bubble(cComponent *component, const char *text) = 0;
    virtual void messageScheduled(cMessage *msg) = 0;
    virtual void messageCancelled(cMessage *msg) = 0;
    virtual void beginSend(cMessage *msg, const SendOptions& options) = 0;
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result) = 0;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) = 0;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, const ChannelResult& result) = 0;
    virtual void endSend(cMessage *msg) = 0;
    virtual void messageCreated(cMessage *msg) = 0;
    virtual void messageCloned(cMessage *msg, cMessage *clone) = 0;
    virtual void messageDeleted(cMessage *msg) = 0;
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) = 0;
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va) = 0;
    virtual void componentMethodEnd() = 0;
    virtual void moduleCreated(cModule *newmodule) = 0;
    virtual void moduleDeleted(cModule *module) = 0;
    virtual void gateCreated(cGate *newgate) = 0;
    virtual void gateDeleted(cGate *gate) = 0;
    virtual void connectionCreated(cGate *srcgate) = 0;
    virtual void connectionDeleted(cGate *srcgate) = 0;
    virtual void displayStringChanged(cComponent *component) = 0;
    virtual void logLine(const char *prefix, const char *line, int lineLength) = 0;
    virtual void stoppedWithException(bool isError, int resultCode, const char *message) = 0;
    //@}
};

}  // namespace omnetpp


#endif


