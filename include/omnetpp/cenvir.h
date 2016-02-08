//==========================================================================
//  CENVIR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CENVIR_H
#define __OMNETPP_CENVIR_H

#include <sstream>
#include <iostream>
#include "simkerneldefs.h"
#include "simtime_t.h"
#include "opp_string.h"
#include "clistener.h"  // for simsignal_t
#include "clifecyclelistener.h"

namespace omnetpp {

class cObject;
class cOwnedObject;
class cLogEntry;
class cEvent;
class cMessage;
class cPar;
class cGate;
class cComponent;
class cComponentType;
class cModule;
class cSimpleModule;
class cStatistic;
class cProperty;
class cRNG;
class cXMLElement;
class cEnvir;
class cConfiguration;
class cConfigurationEx;

using std::endl;

// internal macro, usage: EVCB.beginSend(...)
#define EVCB  cSimulation::getActiveEnvir()->suppressNotifications ? (void)0 : (*cSimulation::getActiveEnvir())

/**
 * cEnvir represents the "environment" or user interface of the simulation.
 * The active cEnvir object can be accessed via cSimulation::getActiveEnvir()
 * or the getEnvir() global function.
 *
 * Methods useful for simulation models include isGUI() and isExpressMode().
 * Most other cEnvir methods are internal for communication between
 * the simulation kernel and the environment.
 *
 * cEnvir is a common facade for the Cmdenv and Tkenv user interfaces (and any
 * other future user interface).
 *
 * The default implementation of cEnvir can be customized by subclassing
 * the classes declared in the envirext.h header (e.g. cConfiguration,
 * cRNG, cIOutputVectorManager, cIOutputScalarManager), and selecting the
 * new classes from <tt>omnetpp.ini</tt>.
 *
 * @ingroup Envir
 * @ingroup EnvirExtensions
 */
class SIM_API cEnvir
{
    friend class evbuf;
  public:
    // Internal flag for express mode.
    bool loggingEnabled;

    // Indicates whether eventlog recording is currently enabled
    bool recordEventlog;  //FIXME remove!!!! use flag inside eventlogmgr

    // Internal flag. When set to true, the simulation kernel MAY omit calling
    // the following cEnvir methods: messageScheduled(), messageCancelled(),
    // beginSend(), messageSendDirect(), messageSendHop(), messageSendHop(),
    // messageSendHop(), messageDeleted(), moduleReparented(), simulationEvent(),
    // componentMethodBegin(), moduleCreated(), moduleDeleted(), connectionCreated(),
    // connectionDeleted(), displayStringChanged().
    bool suppressNotifications;

    // Internal flags. When set, cRuntimeError constructor executes a debug trap/launches debugger
    bool debugOnErrors;
    bool attachDebuggerOnErrors;

  protected:
    // internal: pop up a dialog with the given message; called from printfmsg()
    virtual void putsmsg(const char *msg) = 0;

    // internal: ask a yes/no question, throws exception if cancelled; askYesNo() delegates here
    virtual bool askyesno(const char *msg) = 0;

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

    /** @name Methods to be called by the simulation kernel to notify the environment about events. */
    //@{

    /**
     * Notifies the environment that the object no longer exists. The
     * user interface should close all inspector windows for the object
     * and remove it from object lists currently displayed. cObject's
     * destructor automatically calls this function.
     */
    // note: this cannot be pure virtual, because it has to work even after ev was disposed of
    virtual void objectDeleted(cObject *object) {}

    /**
     * Notifies the environment that a component's initialize method is about to be called.
     */
    virtual void componentInitBegin(cComponent *component, int stage) {}

    /**
     * Notifies the environment that a message was delivered to its destination
     * module, that is, a message arrival event occurred. Details can be
     * extracted from the message object itself. The user interface
     * implementation may use the notification to animate the message on a
     * network diagram, to write a log entry, etc.
     */
    virtual void simulationEvent(cEvent *event) = 0;

    /**
     * Notifies the environment that a message was sent. Details can be
     * extracted from the message object itself. The user interface
     * implementation may use the notification to animate the message on a
     * network diagram, to write a log entry, etc.
     *
     * The second argument is non-nullptr only when sendDirect() was used, and
     * identifies the target gate that was passed to the sendDirect() call.
     * (This information is necessary for proper animation: the target gate
     * might belong to a compound module and be further connected, and then
     * the message will additionally travel through a series of connections
     * before it arrives in a simple module.)
     */
    virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=nullptr) { }

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
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay) = 0;

    /** Part of the beginSend() sequence. @see beginSend() */
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) = 0;

    /** Part of the beginSend() sequence. @see beginSend() */
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay) = 0;

    /** Closes a beginSend() sequence. @see beginSend() */
    virtual void endSend(cMessage *msg) = 0;

    /**
     * Notifies the environment that a message object is being created.
     * This is called from the cMessage constructor.
     */
    virtual void messageCreated(cMessage *msg) = 0;

    /**
     * Notifies the environment that a message object is being cloned.
     * This is called from the cMessage copy constructor.
     */
    virtual void messageCloned(cMessage *msg, cMessage* clone) = 0;

    /**
     * Notifies the environment that a message object is being deleted.
     * This is called from the cMessage destructor, so any information added
     * to cMessage via subclassing is already lost at the time of the call.
     */
    virtual void messageDeleted(cMessage *msg) = 0;

    /**
     * Notifies the environment that a module changed parent.
     */
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) = 0;

    /**
     * Notifies the environment that one component (module) called a member
     * function of another component. This hook enables a graphical user
     * interface to animate the method call in the network diagram.
     * Pass methodFmt==nullptr for Enter_Method_Silent.
     */
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent) = 0;

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
     * If a compound module (or a module with dynamically created submodules)
     * is deleted, one should not assume anything about the relative order
     * moduleDeleted() is called for the module and its submodules.
     */
    virtual void moduleDeleted(cModule *module) = 0;

    /**
     * Notifies the environment that a gates was created.
     */
    virtual void gateCreated(cGate *newgate) = 0;

    /**
     * Notifies the environment that a gates was (more precisely: is being)
     * deleted.
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
     * that the user did not delete in the module destructor.
     */
    // Note: this may not be pure virtual, as it may get called even after main()
    // exited and StaticEnv was destructed, and we do not want to get a "pure virtual
    // method called" error
    virtual void undisposedObject(cObject *obj) {}
    //@}

    /** @name Methods called by the logging mechanism to query the current state. */
    //@{
    /**
     * Returns the name of the event current being processed by the simulation
     * kernel or nullptr if unknown.
     */
    virtual const char *getCurrentEventName() { return nullptr; }

    /**
     * Returns the class name of the event currently being processed by the
     * simulation kernel or nullptr if unknown.
     */
    virtual const char *getCurrentEventClassName() { return nullptr; }

    /**
     * Returns the module that is processing the current event or nullptr. Calling
     * this function after the module has been already deleted returns nullptr.
     */
    virtual cModule *getCurrentEventModule() { return nullptr; }
    //@}

    /** @name Methods called by the simulation kernel to access configuration settings. */
    //@{

    /**
     * Called when a module or channel object has been created and added
     * to the model, but no parameters or gates have been set up yet.
     * It allows the environment perform extra configuration. One use is
     * to set up RNG mapping.
     */
    virtual void preconfigure(cComponent *component) = 0;

    /**
     * Called when a module or channel has been created and installed in the
     * model, with parameters and gates set up. It lets the environment
     * perform extra configuration. One use is to add signal
     * listeners for result recording.
     */
    virtual void configure(cComponent *component) = 0;

    /**
     * Assigns the module or channel parameter from the configuration, or
     * by asking the user.
     */
    virtual void readParameter(cPar *parameter) = 0;

    /**
     * Used for parallel distributed simulation. Returns true if the
     * named future submodule of parentmod is (or will have any submodule)
     * in the local partition, and false otherwise. index should be -1
     * if the submodule is not in a module vector.
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
     * There is a difference however: paths starting with "." are not
     * accepted, and the first path component must name the root element
     * of the document (with getElementByPath() it would match a child element
     * of the current element). That is, a leading "/" is always assumed
     * at the beginning of the path expression, even if it is not explicitly
     * there.
     *
     * The method throws an exception if the document cannot be found, there
     * was a parse error, or the given path expression is invalid. It returns
     * nullptr if the element denoted by the path expression does not exist in the
     * document.
     *
     * The returned object tree should not be modified because cEnvir may
     * cache the file and return the same pointer to several callers.
     */
    virtual cXMLElement *getXMLDocument(const char *filename, const char *xpath=nullptr) = 0;

    /**
     * Returns the parsed form of an XML content string. The first argument
     * is the XML content string. The optional second argument
     * may contain an XPath-like expression to denote an element within
     * the XML document. If path is not present, the root element is returned.
     *
     * See documentation of cXMLElement::getElementByPath() for path syntax.
     * There is a difference however: paths starting with "." are not
     * accepted, and the first path component must name the root element
     * of the document (with getElementByPath() it would match a child element
     * of the current element). That is, a leading "/" is always assumed
     * at the beginning of the path expression, even if it is not explicitly
     * there.
     *
     * The method throws an exception if there was a parse error or if the given
     * path expression is invalid. It returns nullptr if the element denoted
     * by the path expression does not exist in the document.
     *
     * The returned object tree should not be modified because cEnvir may
     * cache the tree and return the same pointer to several callers.
     */
    virtual cXMLElement *getParsedXMLString(const char *content, const char *xpath=nullptr) = 0;

    /**
     * Removes the given document from the XML document cache (if cached), and
     * deletes the object tree from memory. Further getXMLDocument() calls will
     * reload the file from the disk. After forgetXMLDocument(), cXMLElement
     * objects returned for the same document by getXMLDocument() should
     * no longer be referenced. The call has no effect if the given file
     * does not exist or has not yet been loaded.
     *
     * CAUTION: As of version 4.1, this is not a safe operation! Module
     * parameters (cPar) of type "xml" hold pointers to the element trees
     * returned by getXMLDocument(), and this method makes those pointers
     * invalid.
     */
    virtual void forgetXMLDocument(const char *filename) = 0;

    /**
     * Removes the given document from the XML document cache (if cached), and
     * deletes the object tree from memory. After forgetXMLDocument(),
     * cXMLElement objects returned for the same content string by
     * getParsedXMLString() should no longer be referenced. The call has no
     * effect if the given string has not yet been parsed.
     *
     * CAUTION: As of version 4.1, this is not a safe operation! Module
     * parameters (cPar) of type "xml" hold pointers to the element trees
     * returned by getXMLDocument(), and this method makes those pointers
     * invalid.
     */
    virtual void forgetParsedXMLString(const char *content) = 0;

    /**
     * Clears the cache of loaded XML documents, and deletes the corresponding
     * cXMLElement trees.
     *
     * CAUTION: As of version 4.1, this is not a safe operation, as module
     * parameters (cPar) of type "xml" hold pointers to the element trees
     * returned by getXMLDocument(), and this method makes those pointers
     * invalid.
     */
    virtual void flushXMLDocumentCache() = 0;

    /**
     * Clears the cache of parsed XML content strings, and deletes the
     * corresponding cXMLElement trees.
     *
     * CAUTION: As of version 4.1, this is not a safe operation, as module
     * parameters (cPar) of type "xml" hold pointers to the element trees
     * returned by getXMLDocument(), and this method makes those pointers
     * invalid.
     */
    virtual void flushXMLParsedContentCache() = 0;

    /**
     * Called from cSimpleModule, it returns how much extra stack space
     * the user interface recommends for activity() simple modules.
     */
    virtual unsigned getExtraStackForEnvir() const = 0;

    /**
     * Access to the configuration (by default, omnetpp.ini).
     * This method is provided here for the benefit of schedulers, parallel
     * simulation algorithms and other simulation kernel extensions.
     * Models (simple modules) should NOT directly access the configuration --
     * they should rely on module parameters to get input.
     */
    virtual cConfiguration *getConfig() = 0;

    /**
     * Returns the configuration as used by the Envir library. It will throw
     * an error if the configuration object does not subclass from cConfigurationEx.
     * This method should not be used from the simulation kernel or model code.
     */
    virtual cConfigurationEx *getConfigEx();

    /**
     * Searches a number of folders for a resource given with its file name or
     * relative path, and returns the path for the first match. If the resource
     * is not found, the empty string is returned.
     *
     * The list of the search folders includes the current working directory,
     * the folder of the main ini file, the folder that the NED file of the given
     * "context" type was loaded from, folders in the NED path, and folders
     * in the image path (OMNETPP_IMAGE_PATH).
     */
    virtual std::string resolveResourcePath(const char *fileName, cComponentType *context=nullptr) = 0;
    //@}

    /** @name Input/output methods called from simple modules or the simulation kernel. */
    //@{
    /**
     * Returns true if the current environment is a graphical user interface.
     * (For example, it returns true if the simulation is running over Tkenv or
     * Qtenv, and false if it's running over Cmdenv.) Modules can examine this
     * flag to decide whether or not they need to bother with visualization,
     * e.g. dynamically updating display strings or drawing on canvases.
     */
    virtual bool isGUI() const = 0;

    /**
     * Returns true if the simulation is running under a GUI in Express mode.
     * Visualization code (e.g. inside module refreshDisplay() methods) may
     * check this flag and adapt the visualization accordingly.
     */
    virtual bool isExpressMode() const = 0;

    /**
     * In graphical user interfaces (Tkenv/Qtenv), it pops up a "bubble" over
     * the module icon.
     */
    virtual void bubble(cComponent *component, const char *text) = 0;

    /**
     * Displays the given text in a dialog box. This function should not be
     * used by simple modules. Delegates to putsmsg().
     */
    virtual void printfmsg(const char *fmt,...);

    /**
     * Writes the provided log statement to the standard output.
     */
    virtual void log(cLogEntry *entry) = 0;

    /**
     * Interactively prompts the user to enter a string.
     */
    virtual std::string gets(const char *prompt, const char *defaultreply=nullptr) = 0;

    /**
     * Asks the user a yes/no question. The question text is expected
     * in printf() format (format string + arguments). The return value
     * is true for "yes", and false for "no".
     */
    virtual bool askYesNo(const char *fmt,...);
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
    //@}

    /** @name Methods for recording data from output vectors.
     *
     * These are functions cOutVector internally relies on.
     *
     * The behavior of these functions can be changed by plugging in a different
     * cIOutputVectorManager object into the user interface library. (Or alternatively,
     * by reimplementing the whole cEnvir of course).
     */
    //@{

    /**
     * This method is intended to be called by cOutVector objects to register
     * themselves. The returned value is a handle that identifies the vector
     * in subsequent recordInOutputVector() and deregisterOutputVector()
     * calls. The handle may have any value (it does not have to be a valid
     * pointer), but it should NOT be nullptr.
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
     * cIOutputScalarManager object into the user interface library. (Or alternatively,
     * by reimplementing the whole cEnvir of course).
     */
    //@{

    /**
     * Records a double scalar result, in a default configuration into the scalar result file.
     */
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) = 0;

    /**
     * Records a statistic object (histogram, etc) into the scalar result file.
     * This operation may invoke the transform() method on the histogram object.
     */
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) = 0;
    //@}

    /** @name Statistic recording. */
    //@{

    /**
     * Adds result recording listeners for the given signal on the given component.
     * The result will be recorded to the the given component, with a name equal to
     * or derived from the given statisticName. The statisticTemplateProperty parameter
     * is expected to point to a @statisticTemplate NED property with content similar
     * to @statistic properties. The computation and result type (scalar, vector, etc)
     * will be taken from the 'record' key of statisticTemplateProperty in the same
     * way as with @statistic. Also similar to @statistic, key-value pairs of
     * statisticTemplateProperty will be recorded as result attributes.
     *
     * In statisticTemplateProperty, the 'source' key will be ignored (because the
     * signal given as parameter will be used as source). The actual name and index of
     * statisticTemplateProperty will also be ignored. (With @statistic, the index
     * holds the result name, but here the name is explicitly specified in the
     * statisticName parameter.)
     *
     * The purpose of this function is to allow setting up signal-based result recording
     * dynamically when static configuration via @statistic properties is not possible,
     * e.g. because the set of signals to record is only known at runtime.
     */
    virtual void addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty) = 0;
    //@}

    /** @name Management of streams where snapshots can be written.
     *
     * The behavior of these functions can be changed by plugging in a different
     * cISnapshotManager object into the user interface library. (Or alternatively,
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

    /**
     * Starts an external debugger program and attaches it to this process.
     * The command line to start the debugger can be configured.
     */
    virtual void attachDebugger() = 0;
    //@}

    /** @name Lifecycle listeners */
    //@{
    /**
     * Adds a listener that will be notified about simulation lifecycle events.
     * The listeners will NOT be deleted when the program exits.
     */
    virtual void addLifecycleListener(cISimulationLifecycleListener *listener) = 0;

    /**
     * Removes the given listener.
     */
    virtual void removeLifecycleListener(cISimulationLifecycleListener *listener) = 0;

    // internal. FIXME it swallows exceptions, which is sometimes desirable, sometimes not!
    virtual void notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details=nullptr) = 0;
    //@}
};


/**
 * The interface for cEnvir objects that can be instantiated as a user interface
 * like Cmdenv and Tkenv.
 *
 * @ingroup Envir
 * @ingroup EnvirExtensions
 */
class SIM_API cRunnableEnvir : public cEnvir
{
  public:
    /**
     * Runs the user interface. The return value will become the exit code
     * of the simulation program.
     */
    virtual int run(int argc, char *argv[], cConfiguration *cfg) = 0;
};

}  // namespace omnetpp

#endif


