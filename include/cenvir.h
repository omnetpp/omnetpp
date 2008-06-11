//==========================================================================
//  CENVIR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cEnvir    : user interface class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CENVIR_H
#define __CENVIR_H

#include <sstream>
#include <iostream>
#include "simkerneldefs.h"
#include "opp_string.h"

NAMESPACE_BEGIN

class cObject;
class cOwnedObject;
class cMessage;
class cPar;
class cGate;
class cComponent;
class cModule;
class cSimpleModule;
class cStatistic;
class cRNG;
class cConfiguration;
class cXMLElement;
class cEnvir;

using std::endl;

// internal macro, usage: EVCB.messageSent_OBSOLETE(...)
#define EVCB  ev.suppress_notifications ? (void)0 : ev

/**
 * Represents the "environment" or user interface of the simulation.
 *
 * Most common usage from model code (simple modules) is to write log
 * messages, e.g.:
 *
 * <pre>
 * ev << "Received packet " << msg->getName() << ", length " << msg->length()/8 << " bytes\n";
 * ev << "Sending up to higher layer\n";
 * </pre>
 *
 * Other useful methods are cEnvir::isGUI() and cEnvir::isDisabled().
 *
 * The rest of cEnvir methods are used internally for communication between
 * the simulation kernel and the environment.
 *
 * @ingroup Envir
 */
#define ev  (*evPtr)

extern SIM_API cEnvir *evPtr;


/**
 * cEnvir represents the "environment" of the simulation. cEnvir
 * is a common facade for the Cmdenv and Tkenv user interfaces (and any
 * other future user interface).
 *
 * cEnvir has only one instance, the ev global variable.
 *
 * cEnvir member functions can be rougly divided into two groups:
 *  - I/O for module activities; actual implementation is different for each
 *    user interface (e.g. stdin/stdout for Cmdenv, windowing in Tkenv)
 *  - functions for exchanging information between the simulation and the
 *    environment.
 *
 * The default implementation of cEnvir can be customized by subclassing
 * the classes declared in the envirext.h header (e.g. cConfiguration,
 * cRNG, cOutputVectorManager, cOutputScalarManager),
 * and selecting the new classes from <tt>omnetpp.ini</tt>.
 *
 * @ingroup Envir
 * @ingroup EnvirExtensions
 */
class SIM_API cEnvir
{
    friend class evbuf;
  public:
    // Internal flag for express mode.
    bool disable_tracing;

    // Internal flag. When set to true, the simulation kernel MAY omit calling
    // the following cEnvir methods: messageScheduled(), messageCancelled(),
    // beginSend(), messageSendDirect(), messageSendHop(), messageSendHop(),
    // messageSendHop(), messageDeleted(), moduleReparented(), simulationEvent(),
    // componentMethodBegin(), moduleCreated(), moduleDeleted(), connectionCreated(),
    // connectionDeleted(), displayStringChanged().
    bool suppress_notifications;

    // Internal flag. When set, cRuntimeError constructor to raises an exception.
    bool debug_on_errors;

  protected:
    // further internal vars
    std::ostream out;

  protected:
    // internal: ev.printf() and ev<< eventually ends up here; write the first n characters of string s
    virtual void sputn(const char *s, int n) = 0;

    // internal: pop up a dialog with the given message; called from printfmsg()
    virtual void putsmsg(const char *msg) = 0;

    // internal: ask a yes/no question, throws exception if cancelled; askYesNo() delegates here
    virtual bool askyesno(const char *msg) = 0;

    // internal: flushes the internal stream buffer by terminating last line if needed
    void flushLastLine();

  public:
    /** @name Constructor, destructor. */
    //@{

    /**
     * Constructor.
     */
    cEnvir();

    /**
     * Destructor.
     */
    virtual ~cEnvir();
    //@}

    /** @name Methods called from main(). */
    //@{
    /**
     * Called from main(). This function should encapsulate the whole functionality
     * of running the application. The return value will become the exit code
     * of the simulation program.
     */
    virtual int run(int argc, char *argv[], cConfiguration *cfg) = 0;
    //@}

    /** @name Methods to be called by the simulation kernel to notify the environment about events. */
    //@{

    /**
     * Notifies the environment that the object no longer exists. The
     * user interface should close all inspector windows for the object
     * and remove it from object lists currently displayed. cObject's
     * destructor automatically calls this function.
     */
    // note: this cannot be pure virtual, because it has to work even after ev was disposed of
    virtual void objectDeleted(cObject *object) {};

    /**
     * Notifies the environment that a message was delivered to its destination
     * module, that is, a message arrival event occurred. Details can be
     * extracted from the message object itself. The user interface
     * implementation may use the notification to animate the message on a
     * network diagram, to write a log entry, etc.
     */
    virtual void simulationEvent(cMessage *msg) = 0;

    /**
     * Notifies the environment that a message was sent. Details can be
     * extracted from the message object itself. The user interface
     * implementation may use the notification to animate the message on a
     * network diagram, to write a log entry, etc.
     *
     * The second argument is non-NULL only when sendDirect() was used, and
     * identifies the target gate that was passed to the sendDirect() call.
     * (This information is necessary for proper animation: the target gate
     * might belong to a compound module and be further connected, and then
     * the message will additionally travel through a series of connections
     * before it arrives in a simple module.)
     */
    virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL) = 0;

    /**
     * Notifies the environment that a message was scheduled.
     * @see cSimpleModule::scheduleAt()
     */
    virtual void messageScheduled(cMessage *msg) = 0;

    /**
     * Notifies the environment that a scheduled message was cancelled.
     * @see cSimpleModule::cancelEvent()
     */
    virtual void messageCancelled(cMessage *msg) = 0;

    /**
     * Notifies the environment that a message is being sent from a
     * simple module. beginSend() will be followed by a messageSendDirect()
     * (optional, only present when cSimpleModule::sendDirect() was called),
     * several messageSendHop() calls (one for each connection in the path),
     * and finally an endSend().
     */
    virtual void beginSend(cMessage *msg) = 0;

    /** Part of the beginSend() sequence. @see beginSend() */
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay=0, simtime_t transmissionDelay=0) = 0;

    /** Part of the beginSend() sequence. @see beginSend() */
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) = 0;

    /** Part of the beginSend() sequence. @see beginSend() */
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay) = 0;

    /** Closes a beginSend() sequence. @see beginSend() */
    virtual void endSend(cMessage *msg) = 0;

    /**
     * Notifies the environment that a message object is being deleted.
     * This is called from the cMessage destructor, so any information added
     * to cMessage via subclassing is already lost at the time of the call.
     */
    virtual void messageDeleted(cMessage *msg) = 0;

    /**
     * Notifies the environment that a module changed parent.
     */
    virtual void moduleReparented(cModule *module, cModule *oldparent) = 0;

    /**
     * Notifies the environment that one component (module) called a member
     * function of another component. This hook enables a graphical user
     * interface to animate the method call in the network diagram.
     */
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *method) = 0;

    /**
     * Notifies the environment that the method entered in the last
     * componentMethodBegin() call has exited.
     */
    virtual void componentMethodEnd() = 0;

    /**
     * Notifies the environment that a module was created. This method is called
     * from cModuleType::create(), when the module has already been created
     * but buildInside() has not been invoked yet.
     */
    virtual void moduleCreated(cModule *newmodule) = 0;

    /**
     * Notifies the environment that a module was (more precisely: is being)
     * deleted. This method is called from cModule destructor, so the
     * "real" type (getClassName() and everything from the actual subclass)
     * is already lost at this point, however getName(), getFullName(), getFullPath(),
     * gates, parameters (everything that comes from cModule) are still valid.
     *
     * If a compound getModule(or a module with dynamically created submodules)
     * is deleted, one should not assume anything about the relative order
     * moduleDeleted() is called for the module and its submodules.
     */
    virtual void moduleDeleted(cModule *module) = 0;

    /**
     * TODO:
     */
    virtual void gateCreated(cGate *newgate) = 0;

    /**
     * TODO:
     */
    virtual void gateDeleted(cGate *gate) = 0;

    /**
     * Notifies the environment that a connection has been created using
     * srcgate->connectTo().
     */
    virtual void connectionCreated(cGate *srcgate) = 0;

    /**
     * Notifies the environment that a connection has been deleted using
     * srcgate->disconnect().
     */
    virtual void connectionDeleted(cGate *srcgate) = 0;

    /**
     * Notifies the environment that a module or channel display string has
     * changed.
     */
    virtual void displayStringChanged(cComponent *component) = 0;

    /**
     * Called from module destructors, to notify the environment about objects
     * that the user didn't delete in the module destructor.
     */
    virtual void undisposedObject(cObject *obj) = 0;
    //@}

    /** @name Methods called by the simulation kernel to access configuration settings. */
    //@{

    /**
     * Assigns the module or channel parameter from the configuration, or
     * by asking the user.
     */
    virtual void readParameter(cPar *parameter) = 0;

    /**
     * Used for parallel distributed simulation. Returns true if a
     * to-be-created module which is (or has any submodule which is)
     * in the local partition, and false otherwise.
     *
     * Note that for compound modules that contain simple modules in
     * several partitions, this function will return true on all those
     * partitions.
     */
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index) = 0;

    /**
     * Resolves reference to an XML model configuration file. First argument
     * is the file name of the XML document. The optional second argument
     * may contain an XPath-like expression to denote an element within
     * the XML document. If path is not present, the root element is returned.
     *
     * See documentation of cXMLElement::getElementByPath() for path syntax.
     * There's a difference however: paths starting with "." are not
     * accepted, and the first path component must name the root element
     * of the document (with getElementByPath() it would match a child element
     * of the current element). That is, a leading "/" is always assumed
     * at the beginning of the path expression, even if it is not explicitly
     * there.
     *
     * The method throws an exception if the document cannot be found or the
     * given path expression is invalid. Returns NULL if the element denoted
     * by the path expression doesn't exist in the document.
     *
     * The returned object tree should not be modified because cEnvir may
     * cache the file and return the same pointer to several callers.
     */
    virtual cXMLElement *getXMLDocument(const char *filename, const char *path=NULL) = 0;

    /**
     * Called from cSimpleModule, it returns how much extra stack space
     * the user interface recommends for activity() simple modules.
     */
    virtual unsigned getExtraStackForEnvir() const = 0;

    /**
     * Access to the configuration data (by default, omnetpp.ini).
     * This is provided here for the benefit of schedulers, parallel
     * simulation algorithms and other simulation kernel extensions.
     * Models (simple modules) should NOT directly access the configuration --
     * they should rely on module parameters to get input.
     */
    virtual cConfiguration *getConfig() = 0;
    //@}

    /** @name Input/output methods called from simple modules or the simulation kernel. */
    //@{
    /**
     * Tells if the current environment is graphical or not. (For Tkenv it returns true,
     * and with Cmdenv it returns false.) Simple modules can examine this flag
     * to decide whether or not they need to bother updating display strings.
     */
    virtual bool isGUI() const = 0;

    /**
     * Returns true if the simulation is running in an Express or Express-like mode
     * where output from <tt>ev&lt;&lt;</tt> and <tt>ev.printf()</tt> statement is
     * not printed or logged anywhere but discarded. Model code may make <tt>ev&lt;&lt;</tt>
     * statements conditional on this flag to save CPU cycles. For example:
     * <pre>
     *     if (!ev.isDisabled())  ev << "Packet " << msg->getName() << " received";
     * </pre>
     *
     * The following version may also be useful (it makes use of the fact that <<
     * binds stronger than ?:)
     * <pre>
     *     \#define EV  ev.isDisabled()?ev:ev
     *     EV << "Packet " << msg->getName() << " received";
     * </pre>
     */
    bool isDisabled() const {return disable_tracing;}

    /**
     * Overloaded << operator to make cEnvir behave like an ostream.
     * @see getOStream()
     */
    // implementation note: needed because otherwise the templated version
    // would cause ambiguity errors in some cases
    cEnvir& operator<<(const std::string& t) {out << t; return *this;}

    /**
     * Overloaded << operator to make cEnvir behave like an ostream.
     *
     * This method can be used by modules and channels to display debugging output.
     * It is up to the user interface implementation to display the text in
     * the way it wants. The text is usually associated with the module or channel
     * in context (see cSimulation::getContext()), and may get displayed in the
     * module's debug window, or enabled/disabled per module.
     *
     * @see getOStream()
     */
    template<typename T> cEnvir& operator<<(const T& t) {out << t; return *this;}

    /**
     * Overloaded << operator to handle stream manipulators such as <tt>endl</tt>.
     */
    cEnvir& operator<<(std::ostream& (t)(std::ostream&)) {out << t; return *this;}

    /**
     * Returns the std::ostream instance where '<<' operators delegate.
     * Writes will be eventually delegated to cEnvir::sputn(), after buffering.
     */
    std::ostream& getOStream() {return out;}

    /**
     * In graphical user interfaces (Tkenv), it pops up a "bubble" over the
     * module icon.
     */
    virtual void bubble(cComponent *component, const char *text) = 0;

    /**
     * Displays the given text in a dialog box. This function should not be
     * used by simple modules. Delegates to putsmsg().
     */
    void printfmsg(const char *fmt,...);

    /**
     * This method can be used by modules and channels to display debugging output.
     * It is up to the user interface implementation to display the text in
     * the way it wants. The text is usually associated with the module or channel
     * in context (see cSimulation::getContext()), and may get displayed in the
     * module's debug window, or enabled/disabled per module.
     *
     * The function's arguments are identical to the standard \<stdio.h\> printf().
     * It is recommended to use C++-style I/O (operator<<) instead of this function.
     */
    // note: non-virtual, delegates to sputn()
    int printf(const char *fmt,...);

    /**
     * Flushes the output buffer of ev.printf() and ev<< operations.
     * Only some user interfaces need it: it can be useful with Cmdenv which
     * writes to the standard output, but no need for it with Tkenv which
     * displays all output immediately anyway.
     */
    virtual cEnvir& flush() = 0;

    /**
     * Interactively prompts the user to enter a string.
     */
    virtual std::string gets(const char *prompt, const char *defaultreply=NULL) = 0;

    /**
     * Asks the user a yes/no question. The question text is expected
     * in printf() format (format string + arguments). The return value
     * is true for "yes", and false for "no".
     */
    // note: non-virtual, delegates to askyesno()
    bool askYesNo(const char *fmt,...);
    //@}

    /** @name Access to RNGs. */
    //@{

    /**
     * Returns the number of RNGs available for the simulation
     * ("num-rngs=" omnetpp.ini setting).
     */
    virtual int getNumRNGs() const = 0;

    /**
     * Returns pointer to "physical" RNG k (0 <= k < getNumRNGs()).
     */
    virtual cRNG *getRNG(int k) = 0;

    /**
     * Sets up RNG mapping (which maps module-local RNG numbers to "physical"
     * RNGs) for the given module.
     */
    virtual void getRNGMappingFor(cComponent *component) = 0;
    //@}

    /** @name Methods for recording data from output vectors.
     *
     * These are functions cOutVector internally relies on.
     *
     * The behavior of these functions can be changed by plugging in a different
     * cOutputVectorManager object into the user interface library. (Or alternatively,
     * by reimplementing the whole cEnvir of course).
     */
    //@{

    /**
     * This method is intended to be called by cOutVector objects to register
     * themselves. The return value is a handle of type void*;
     * this handle has to be passed to record() to identify the vector
     * each time a value is written.
     */
    virtual void *registerOutputVector(const char *modulename, const char *vectorname) = 0;

    /**
     * cOutVector objects must deregister themselves when they are no longer needed.
     */
    virtual void deregisterOutputVector(void *vechandle) = 0;

    /**
     * This method is called when an attribute of the output vector is set.
     */
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value) = 0;

    /**
     * This method is intended to be called by cOutVector objects to write
     * a value into the output vector. The return value is true if the data was
     * actually recorded, and false if it was not recorded (because of filtering, etc.)
     */
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value) = 0;
    //@}

    /** @name Scalar statistics.
     *
     * The method cComponent::recordScalar() function internally relies on.
     *
     * The behavior of this function can be changed by plugging in a different
     * cOutputScalarManager object into the user interface library. (Or alternatively,
     * by reimplementing the whole cEnvir of course).
     */
    //@{

    /**
     * Records a double scalar result, in a default configuration into the scalar result file.
     */
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL) = 0;

    /**
     * Records a statistic object (histogram, etc) into the scalar result file.
     */
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL) = 0;
    //@}

    /** @name Management of streams where snapshots can be written.
     *
     * The behavior of these functions can be changed by plugging in a different
     * cSnapshotManager object into the user interface library. (Or alternatively,
     * by reimplementing the whole cEnvir of course).
     */
    //@{

    /**
     * Returns a stream where a snapshot can be written. Called from cSimulation::snapshot().
     */
    virtual std::ostream *getStreamForSnapshot() = 0;

    /**
     * Releases a stream after a snapshot was written.
     */
    virtual void releaseStreamForSnapshot(std::ostream *os) = 0;
    //@}

    /** @name Miscellaneous functions. */
    //@{
    /**
     * Access to original command-line arguments.
     */
    virtual int getArgCount() const = 0;

    /**
     * Access to original command-line arguments.
     */
    virtual char **getArgVector() const = 0;

    /**
     * Returns the partitionID when parallel simulation is active.
     */
    virtual int getParsimProcId() const = 0;

    /**
     * Returns the number of partitions when parallel simulation is active;
     * otherwise it returns 0.
     */
    virtual int getParsimNumPartitions() const = 0;

    /**
     * Returns the Run Id of the current simulation run, or NULL if
     * no simulation run is active.
     */
    virtual const char *getRunId() const = 0;

    /**
     * The function underlying cSimulation::getUniqueNumber().
     */
    virtual unsigned long getUniqueNumber() = 0;

    /**
     * May be called from the simulation while actively waiting
     * for some external condition to occur -- for example from
     * blocking receive in parallel simulation, or during wait
     * in real-time simulation.
     *
     * In a graphical user interface, this method may take care
     * of display redraw and handling user interaction (including
     * handling of the Stop button).
     *
     * Normally returns false. A true value means the user wants to
     * abort waiting (e.g. pushed the Stop button).
     */
    virtual bool idle() = 0;
    //@}
};

NAMESPACE_END

#endif


