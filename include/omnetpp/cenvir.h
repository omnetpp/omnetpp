//==========================================================================
//  CENVIR.H - part of
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

#ifndef __OMNETPP_CENVIR_H
#define __OMNETPP_CENVIR_H

#include <sstream>
#include <iostream>
#include "simkerneldefs.h"
#include "simtime_t.h"
#include "opp_string.h"
#include "clistener.h"  // for simsignal_t
#include "clifecyclelistener.h"
#include "ccanvas.h"

namespace osg { class Node; }

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
struct SendOptions;
struct ChannelResult;

using std::endl;

// internal macro, usage: EVCB.beginSend(...)
#define EVCB  cSimulation::getActiveEnvir()->suppressNotifications ? (void)0 : (*cSimulation::getActiveEnvir())

/**
 * @brief cEnvir represents the "environment" or user interface of the simulation.
 *
 * The active cEnvir object can be accessed via cSimulation::getActiveEnvir()
 * or the getEnvir() global function.
 *
 * Methods useful for simulation models include isGUI() and isExpressMode().
 * Most other cEnvir methods are used in the communication between
 * the simulation kernel and the environment.
 *
 * The default implementation of cEnvir can be extended by subclassing
 * the classes declared in the envirext.h header (e.g. cConfiguration,
 * cRNG, cIOutputVectorManager, cIOutputScalarManager), and selecting the
 * new classes in `omnetpp.ini`.
 *
 * @ingroup EnvirAndExtensions
 */
class SIM_API cEnvir
{
    friend class evbuf;
  public:
    // Internal flag for express mode.
    bool loggingEnabled = true;

    // Internal flag. When set to true, the simulation kernel MAY omit calling
    // the following cEnvir methods: messageScheduled(), messageCancelled(),
    // beginSend(), messageSendDirect(), messageSendHop(), messageSendHop(),
    // messageSendHop(), messageDeleted(), moduleReparented(), simulationEvent(),
    // componentMethodBegin(), moduleCreated(), moduleDeleted(), connectionCreated(),
    // connectionDeleted(), displayStringChanged().
    bool suppressNotifications = false; //FIXME set to true when not needed!

    // Debugging. When set, cRuntimeError constructor executes a debug trap/launches debugger
    bool debugOnErrors = false;

    // Lifecycle listeners
    std::vector<cISimulationLifecycleListener*> listeners;

  public:
    /** @name Constructor, destructor. */
    //@{

    /**
     * Constructor.
     */
    cEnvir() {}

    /**
     * Destructor.
     */
    virtual ~cEnvir() {}
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
     * and finally an endSend(). At least one call to either messageSendDirect()
     * or messageSendHop() must occur between corresponding
     * beginSend()/endSend() pairs.
     * If the result.discard parameter (member) is true in a messageSendHop()
     * call, the sequence ends there without a call to endSend(), and a
     * messageDeleted() call will follow.
     */
    virtual void beginSend(cMessage *msg, const SendOptions& options) = 0;

    /** Part of the beginSend() sequence. @see beginSend() */
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result) = 0;

    /** Part of the beginSend() sequence. @see beginSend() */
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) = 0;

    /** Part of the beginSend() sequence. @see beginSend() */
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, const ChannelResult& result) = 0;

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
     * model, after parameters and gates have been set up, but before submodules
     * and internal connections are added. This method lets the environment
     * perform extra configuration.
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
     * (For example, it returns true if the simulation is running in Qtenv,
     * and false if it's running in Cmdenv.) Modules can examine this
     * flag to decide whether or not they need to bother with visualization,
     * e.g. dynamically updating display strings or drawing on canvases.
     */
    virtual bool isGUI() const = 0;

    /**
     * Returns true if logging is enabled. This is false in Express mode while
     * the simulation is actually running.
     */
    bool isLoggingEnabled() const { return loggingEnabled; }

    /**
     * Returns true if the simulation is running under a GUI in Express mode.
     * Visualization code (e.g. inside module refreshDisplay() methods) may
     * check this flag and adapt the visualization accordingly.
     */
    virtual bool isExpressMode() const = 0;

    /**
     * In graphical user interfaces like Qtenv, it pops up a transient
     * "bubble" over the module icon.
     */
    virtual void bubble(cComponent *component, const char *text) = 0;

    /**
     * Log a line described by the entry.
     */
    virtual void log(cLogEntry *entry) = 0;

    /**
     * Displays the given alert text in a conspicuous way, e.g. in a dialog box.
     * This function should be used sparingly from models, if ever.
     */
    virtual void alert(const char *msg) = 0;

    /**
     * A printf-like frontend for alert().
     */
    _OPP_GNU_ATTRIBUTE(format(printf, 2, 3))
    virtual void printfmsg(const char *fmt,...);

    /**
     * Interactively prompts the user to enter a string. This function should
     * be used sparingly from models, if ever.
     */
    virtual std::string gets(const char *prompt, const char *defaultReply=nullptr) = 0;

    /**
     * Asks the user a yes/no question with the given prompt. The return value
     * is true for "yes", and false for "no". This function should be used
     * sparingly from models, if ever.
     */
    virtual bool askYesNo(const char *prompt) = 0;

    /**
     * Get the generic output stream that where informative messages can be printed
     * for the user (~ standard output).
     */
    virtual std::ostream& getOutputStream() = 0;

    /**
     * Returns the size of the image with the given name (e.g. "block/switch_l")
     * in pixels. If the image does not exist, it returns the size of the
     * "unknown" icon (as that is the image actually displayed). Non-GUI
     * environments like Cmdenv may return made-up (but not completely
     * unrealistic) numbers.
     *
     * This method is used by some methods of cImageFigure, e.g. getBounds().
     */
    virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight) = 0;

    /**
     * Returns the size and ascent of the bounding box of the text when rendered
     * with the given font in the GUI. Non-GUI environments like Cmdenv may return
     * made-up (but not completely unrealistic) numbers.
     *
     * This method is used by some methods of cTextFigure, e.g. getBounds().
     */
    virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent) = 0;

    /**
     * Tells the user interface to load the images from the given directory,
     * as if the directory was part of the the OMNeT++ image path. The loaded
     * images can then be used in display strings and with cImageFigure/cIconFigure.
     *
     * An error is thrown if the given directory does not exist or cannot be
     * accessed. This method is idempotent: the second and further invocations
     * with the same parameter have no effect. The method may have no effect
     * if the simulation is running under Cmdenv or another non-graphical user
     * interface.
     */
    virtual void appendToImagePath(const char *directory) = 0;

    /**
     * Tells the user interface to load the image from the given image file.
     * The loaded image can then be used in display strings and with
     * cImageFigure/cIconFigure, by referencing it with the image name.
     * If the location of the image file is not known or cannot be easily
     * determined, resolveResourcePath() may be used to find it prior to
     * loadImage().
     *
     * The image name may be specified explicitly (in the imageName argument),
     * or if it is missing (is nullptr or empty string), the base file name
     * will be used (fileName with directories and extension stripped). Note
     * that if an image with the given name has already been loaded, this
     * method will do nothing, i.e. already loaded images are NOT replaced.
     * This also means that the method is idempotent: the second and further
     * invocations with the same parameter have no effect.
     *
     * An error is thrown if the given file does not exist or cannot be
     * loaded as a supported image format. The method may have no effect
     * if the simulation is running under Cmdenv or another non-graphical
     * user interface.
     *
     * @see resolveResourcePath()
     */
    virtual void loadImage(const char *fileName, const char *imageName=nullptr) = 0;

    /**
     * Returns the effective bounding rectangle of the given module when it is
     * visualized in the graphical inspector of its containing compound module,
     * or (NAN,NAN,NAN,NAN) if such a rectangle is not available. This method
     * is the only way to access positions of "floating" modules, i.e. those
     * placed by a layouting algorithm that runs as part of the UI. The method
     * may unconditionally return (NAN,NAN,NAN,NAN) when the simulation is
     * running under Cmdenv or another non-graphical user interface.
     *
     * Also note that this method may return different values at different
     * times, e.g. as a result of the user opening and closing inspectors,
     * hitting the "Relayout" button, or even changing the zoom level or icon
     * size of graphical module inspectors displaying the parent module.
     * Visualization code relying on this method is advised to re-read
     * coordinates in every refreshDisplay() call, and check whether they
     * changed since last time.
     */
    virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule) = 0;

    /*
     * Returns the characteristic points (currently the start and end points)
     * of the connection arrow denoted by its source gate when it is visualized
     * in the graphical inspector of its containing compound module,
     * or an empty vector if the connection arrow is not available. This method
     * is the only way to access the coordinates of connection arrows.
     * The method may unconditionally return an empty vector when the simulation
     * is running under Cmdenv or another non-graphical user interface. It is
     * recommended to use the front() and back() methods of std::vector for
     * accessing the start and end points in order to be compatible with potential
     * future OMNeT++ versions that support splines or breakpoints in the arrows.
     *
     * Also note that this method may return different values at different
     * times, e.g. as a result of the user opening and closing inspectors,
     * hitting the "Relayout" button, or even changing the zoom level or icon
     * size of graphical module inspectors displaying the parent module.
     * Visualization code relying on this method is advised to re-read
     * coordinates in every refreshDisplay() call, and check whether they
     * changed since last time.
     */
    virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate) = 0;

    /**
     * Returns the current zoom level of a graphical module inspector displaying
     * the given module (its contents from the inside, not as a submodule icon),
     * or NAN if not available. A value of 1.0 means 100% zoom (actual size),
     * 0.5 means 50% (half size), and so on. The method may unconditionally
     * return NAN if the simulation is running under Cmdenv or another
     * non-graphical user interface.
     *
     * Also note that this method may return different values at different
     * times, e.g. as a result of the user changing the zoom level, or even
     * opening and closing inspectors. Visualization code relying on this method
     * is advised to re-read the value in every refreshDisplay() call, and check
     * whether it changed since last time.
     */
    virtual double getZoomLevel(const cModule *module) = 0;

    /**
     * Returns the current animation time. Animation time starts from zero, and
     * monotonically increases with simulation time and also during zero-simtime
     * animations a.k.a. "holds". The method may return 0 if the simulation is
     * running under Cmdenv or another non-graphical user interface.
     */
    virtual double getAnimationTime() const = 0;

    /**
     * Returns the current animation speed. It is usually computed as the
     * minimum of the animation speeds of visible canvases, unless the user
     * interactively overrides it in the UI, for example imposes a lower limit.
     * The method returns 0 if there is currently no animation speed, or the
     * simulation is running under Cmdenv or another non-graphical user interface.
     *
     * @see cCanvas::setAnimationSpeed()
     */
    virtual double getAnimationSpeed() const = 0;

    /**
     * Returns the remaining animation "hold" time. This is the maximum of the
     * animation times remaining from the holds times requested by visible
     * canvases.
     *
     * @see cCanvas::holdSimulationFor()
     */
    virtual double getRemainingAnimationHoldTime() const = 0;
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
    virtual void *registerOutputVector(const char *modulename, const char *vectorname, opp_string_map *attributes=nullptr) = 0;

    /**
     * cOutVector objects must deregister themselves when they are no longer needed.
     */
    virtual void deregisterOutputVector(void *vechandle) = 0;

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
     * This operation may invoke the setUpBins() method on the histogram object.
     */
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) = 0;

    /**
     * Records a module or channel parameter, in a default configuration into the scalar result file.
     */
    virtual void recordParameter(cPar *par) = 0;

    /**
     * Records the runtime NED type of module or channel, in a default configuration into the scalar result file.
     */
    virtual void recordComponentType(cComponent *component) = 0;
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
     * Used by cOsgCanvas to increase the reference count of an osg::Node.
     * Should delegate to node->ref() when OSG support is available.
     */
    virtual void refOsgNode(osg::Node *scene) = 0;

    /**
     * Used by cOsgCanvas to increase the reference count of an osg::Node.
     * Should delegate to node->unref() when OSG support is available.
     */
    virtual void unrefOsgNode(osg::Node *scene) = 0;

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
     * Potentially blocks the execution of model code when called.
     * When using user interfaces (graphical environments) that support it,
     * gives the user opportunity to examine the state of the simulation even
     * in the middle of performing an event, and resume on demand.
     * This is similar to how breakpoints work in debuggers.
     */
    virtual void pausePoint() = 0;

    /**
     * Starts an external debugger program and attaches it to this process,
     * if no already attached debugger is detected.
     * The command line to start the debugger can be configured.
     *
     * The error parameter points to the exception that is the reason a
     * debugger is needed. If there is no error, just a simple request
     * for debugging, it is nullptr.
     *
     * Returns true if a debugger was already attached, or one could be
     * started and attached successfully.
     * Returns false if a debugger was not already attached and could
     * not be started, or the user chose not to start and attach one.
     */
    virtual bool ensureDebugger(cRuntimeError *error = nullptr) = 0;

    //@}

    /** @name Lifecycle listeners */
    //@{
    /**
     * Adds a listener that will be notified about simulation lifecycle events.
     * It has no effect if the listener is already subscribed.
     * Note: The listeners will NOT be deleted when the program exits.
     */
    virtual void addLifecycleListener(cISimulationLifecycleListener *listener);

    /**
     * Removes the given listener. This method has no effect if the listener
     * is not currently subscribed.
     */
    virtual void removeLifecycleListener(cISimulationLifecycleListener *listener);

    /**
     * Returns the list of installed lifecycle listeners.
     */
    virtual std::vector<cISimulationLifecycleListener*> getLifecycleListeners() const;

    /**
     * Notify lifecycle listeners
     */
    virtual void notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details=nullptr);
    //@}
};


/**
 * @brief A cEnvir that can be instantiated as a user interface, like Cmdenv
 * and Tkenv.
 *
 * @ingroup EnvirAndExtensions
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


