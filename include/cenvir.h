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
  Copyright (C) 1992-2005 Andras Varga

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
class TOmnetApp;
class cEnvir;

using std::endl;

// internal macro, usage: EVCB.messageSent_OBSOLETE(...)
#define EVCB  ev.suppress_notifications ? (void)0 : ev


//
// std::streambuf used by cEnvir's ostream base. It redirects writes to
// cEnvir::sputn(s,n). Flush is done at the end of each line, meanwhile
// writes are buffered in a stringbuf.
//
template <class E, class T = std::char_traits<E> >
class basic_evbuf : public std::basic_stringbuf<E,T> {
  public:
    basic_evbuf(cEnvir *ev) : _ev(ev) {}
    // gcc>=3.4 needs either this-> or std::basic_stringbuf<E,T>:: in front of pptr()/pbase()
    bool isempty() {return this->pptr()==this->pbase();}
  protected:
    virtual int sync();
    virtual std::streamsize xsputn(const E *s, std::streamsize n) {
        std::streamsize r = std::basic_stringbuf<E,T>::xsputn(s,n);
        for(;n>0;n--,s++)
            if (*s=='\n')
               {sync();break;}
        return r;
    }
    cEnvir *_ev;
};

typedef basic_evbuf<char> evbuf;


/**
 * Represents the "environment" or user interface of the simulation.
 *
 * Most common usage from model code (simple modules) is to write log
 * messages, e.g.:
 *
 * <pre>
 * ev << "Received packet " << msg->name() << ", length " << msg->length()/8 << " bytes\n";
 * ev << "Sending up to higher layer\n";
 * </pre>
 *
 * Other useful methods are cEnvir::isGUI() and cEnvir::disabled().
 *
 * The rest of cEnvir methods are used internally for communication between
 * the simulation kernel and the environment.
 *
 * @ingroup Envir
 */
ENVIR_API extern cEnvir ev;


/**
 * Interface to the environment (user interface) of the simulation. cEnvir
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
 * The implementation of cEnvir is <b>not</b> part of the simulation kernel,
 * it is in a separate library (the Envir library; see src/envir). This means that
 * customizers are free to replace the environment of the simulation as
 * they like, by simply linking the executable with a different library.
 *
 * The default (Envir library) implementation of cEnvir delegates its task to an
 * internal "simulation application" object. Cmdenv and Tkenv inherit from the
 * default simulation application class.
 *
 * The default (src/envir) implementation of cEnvir can be customized
 * by subclassing the classes declared in the envirext.h header (e.g.
 * cConfiguration, cRNG, cOutputVectorManager, cOutputScalarManager),
 * and selecting the new classes from <tt>omnetpp.ini</tt>.
 *
 * @ingroup Envir
 * @ingroup EnvirExtensions
 */
class ENVIR_API cEnvir : public std::ostream
{
  public:
    // internal variables
    TOmnetApp *app;  // the application instance

    // Internal flag for express mode. XXX define exactly
    bool disable_tracing;

    // Internal flag. When set to true, the simulation kernel MAY omit calling
    // the following cEnvir methods: messageScheduled(), messageCancelled(),
    // beginSend(), messageSendDirect(), messageSendHop(), messageSendHop(),
    // messageSendHop(), messageDeleted(), moduleReparented(), simulationEvent(),
    // componentMethodBegin(), moduleCreated(), moduleDeleted(), connectionCreated(),
    // connectionRemoved(), displayStringChanged().
    bool suppress_notifications; //XXX make use of this flag in Envir impl.

    // Internal flag. When set, cRuntimeError constructor to raises an exception.
    bool debug_on_errors;

  private:
    // further internal vars
    evbuf ev_buf;
    bool isgui;

  public:
    // internal: writes the first n characters of string s.
    // evbuf (the streambuf underlying cEnvir's ostream base class)
    // writes via this function.
    void sputn(const char *s, int n);

    // internal: flushes the internal stream buffer by terminating last line if needed
    // note: exploits the fact that evbuf does sync() on "\n"'s
    void flushlastline() {if (!ev_buf.isempty()) ev_buf.sputn("\n",1);}

  public:
    /** @name Constructor, destructor.
     *
     * Note that only one instance of cEnvir exists, the ev object.
     */
    //@{

    /**
     * Constructor.
     */
    cEnvir();

    /**
     * Destructor.
     */
    ~cEnvir();
    //@}

    /** @name Methods called from main(). */
    //@{

    /**
     * This function is called by main() at the beginning of the program.
     * It receives the command-line arguments as parameters.
     */
    void setup(int ac, char *av[]);

    /**
     * Called from main(). This function should encapsulate the whole functionality
     * of running the application. The return value may be used as exit code
     * for the simulation program.
     */
    int run();

    /**
     * Called from main() before exiting.
     */
    void shutdown();
    //@}

    /** @name Methods to be called by the simulation kernel to notify the environment about events. */
    //@{

    /**
     * Notifies the environment that the object no longer exists. The
     * user interface should close all inspector windows for the object
     * and remove it from object lists currently displayed. cObject's
     * destructor automatically calls this function.
     */
    void objectDeleted(cObject *object);

    /**
     * Notifies the environment that a message was delivered to its destination
     * module, that is, a message arrival event occurred. Details can be
     * extracted from the message object itself. The user interface
     * implementation may use the notification to animate the message on a
     * network diagram, to write a log entry, etc.
     */
    void simulationEvent(cMessage *msg);

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
    //FIXME obsolete -- see beginSend(), etc.
    void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL);

    void messageScheduled(cMessage *msg);
    void messageCancelled(cMessage *msg);

    // beginSend() will be followed by a messageSendDirect (optional), and several messageSendHop() calls
    void beginSend(cMessage *msg);
    void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay=0, simtime_t transmissionDelay=0);
    void messageSendHop(cMessage *msg, cGate *srcGate);
    void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    void endSend(cMessage *msg);
    void messageDeleted(cMessage *msg);  //XXX document this and all above funcs

    /**
     * Notifies the environment that a module changed parent.
     */
    void moduleReparented(cModule *module, cModule *oldparent);

    /**
     * Notifies the environment that one component (module) called a member
     * function of another component. This hook enables a graphical user
     * interface to animate the method call in the network diagram.
     */
    void componentMethodBegin(cComponent *from, cComponent *to, const char *method);

    /**
     * Notifies the environment that the method entered in the last
     * componentMethodBegin() call has exited.
     */
    void componentMethodEnd();

    /**
     * Notifies the environment that a module was created. This method is called
     * from cModuleType::create(), when the module has already been created
     * but buildInside() has not been invoked yet.
     */
    void moduleCreated(cModule *newmodule);

    /**
     * Notifies the environment that a module was (more precisely: is being)
     * deleted. This method is called from cModule destructor, so the
     * "real" type (className() and everything from the actual subclass)
     * is already lost at this point, however name(), fullName(), fullPath(),
     * gates, parameters (everything that comes from cModule) are still valid.
     *
     * If a compound module (or a module with dynamically created submodules)
     * is deleted, one should not assume anything about the relative order
     * moduleDeleted() is called for the module and its submodules.
     */
    void moduleDeleted(cModule *module);

    /**
     * Notifies the environment that a connection has been created using
     * srcgate->connectTo().
     */
    void connectionCreated(cGate *srcgate);

    /**
     * Notifies the environment that a connection has been removed using
     * srcgate->disconnect().
     */
    void connectionRemoved(cGate *srcgate);

    /**
     * Notifies the environment that a connection display string (stored in
     * the source gate) has been changed.
     */
    void displayStringChanged(cGate *gate);

    /**
     * Notifies the environment that a module display string has been changed.
     */
    // TBD: how does module name change propagate to gui?
    void displayStringChanged(cModule *submodule);

    /**
     * Called from module destructors, to notify the environment about objects
     * that the user didn't delete in the module destructor.
     */
    void undisposedObject(cObject *obj);
    //@}

    /** @name Methods called by the simulation kernel to access configuration settings. */
    //@{

    /**
     * Assigns the module or channel parameter from the configuration, or
     * by asking the user.
     */
    void readParameter(cPar *parameter);

    /**
     * Used for parallel distributed simulation. Returns true if a
     * to-be-created module which is (or has any submodule which is)
     * in the local partition, and false otherwise.
     *
     * Note that for compound modules that contain simple modules in
     * several partitions, this function will return true on all those
     * partitions.
     */
    bool isModuleLocal(cModule *parentmod, const char *modname, int index);

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
    cXMLElement *getXMLDocument(const char *filename, const char *path=NULL);

    /**
     * Called from cSimpleModule, it returns how much extra stack space
     * the user interface recommends for activity() simple modules.
     */
    unsigned extraStackForEnvir();

    /**
     * Access to the configuration data (by default, omnetpp.ini).
     * This is provided here for the benefit of schedulers, parallel
     * simulation algorithms and other simulation kernel extensions.
     * Models (simple modules) should NOT directly access the configuration --
     * they should rely on module parameters to get input.
     */
    cConfiguration *config();
    //@}

    /** @name Input/output methods called from simple modules or the simulation kernel. */
    //@{

    /**
     * Tells if the current environment is graphical or not. (For Tkenv it returns true,
     * and with Cmdenv it returns false.) Simple modules can examine this flag
     * to decide whether or not they need to bother updating display strings.
     */
    bool isGUI()  {return isgui;}

    /**
     * Returns true if the simulation is running in an Express or Express-like mode
     * where output from <tt>ev&lt;&lt;</tt> and <tt>ev.printf()</tt> statement is
     * not printed or logged anywhere but discarded. Model code may make <tt>ev&lt;&lt;</tt>
     * statements conditional on this flag to save CPU cycles. For example:
     * <pre>
     *     if (!ev.disabled())  ev << "Packet " << msg->name() << " received";
     * </pre>
     *
     * The following version may also be useful (it makes use of the fact that <<
     * binds stronger than ?:)
     * <pre>
     *     #define EV  ev.disabled()?ev:ev
     *     EV << "Packet " << msg->name() << " received";
     * </pre>
     */
    bool disabled() {return disable_tracing;}

    /**
     * In Tkenv it pops up a "bubble" over the module icon.
     */
    void bubble(cModule *mod, const char *text);

    /**
     * Displays a message in dialog box. This function should not be
     * used too much by simple modules, if ever.
     */
    void printfmsg(const char *fmt,...);

    /**
     * Simple modules can output text into their own window through this
     * function. The text is expected in printf() format (format
     * string + arguments).
     *
     * It is recommended to use C++-style I/O instead of this function.
     */
    void printf(const char *fmt="\n",...);

    /**
     * DEPRECATED. Similar to ev.printf(), but just writes out its argument
     * string with no formatting. Use ev<< instead.
     */
    _OPPDEPRECATED void puts(const char *s);

    /**
     * Flushes the output buffer of ev.printf() and ev<< operations.
     * Only some user interfaces need it: it can be useful with Cmdenv which
     * writes to the standard output, but no need for it with Tkenv which
     * displays all output immediately anyway.
     */
    cEnvir& flush();

    /**
     * Interactively prompts the user to enter a string. std::stringstream
     * can be used to further process the input.
     */
    std::string gets(const char *prompt, const char *defaultreply=NULL);

    /**
     * DEPRECATED. Retained for compatibility only.
     */
    _OPPDEPRECATED bool gets(const char *prompt, char *buf, int len=255);

    /**
     * Puts a yes/no question to the user. The question itself  is expected
     * in the printf() format (format string + arguments). The
     * true return value means yes, false means no.
     */
    bool askYesNo(const char *msgfmt,...);
    //@}

    /** @name Access to RNGs. */
    //@{

    /**
     * Returns the number of RNGs available for the simulation
     * ("num-rngs=" omnetpp.ini setting).
     */
    int numRNGs();

    /**
     * Returns pointer to "physical" RNG k (0 <= k < numRNGs()).
     */
    cRNG *rng(int k);

    /**
     * Sets up RNG mapping (which maps module-local RNG numbers to "physical"
     * RNGs) for the given module.
     */
    void getRNGMappingFor(cComponent *component);
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
    void *registerOutputVector(const char *modulename, const char *vectorname);

    /**
     * cOutVector objects must deregister themselves when they are no longer needed.
     */
    void deregisterOutputVector(void *vechandle);

    /**
     * This method is called when an attribute of the output vector is set.
     */
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value);

    /**
     * This method is intended to be called by cOutVector objects to write
     * a value into the output vector. The return value is true if the data was
     * actually recorded, and false if it was not recorded (because of filtering, etc.)
     */
    bool recordInOutputVector(void *vechandle, simtime_t t, double value);
    //@}

    /** @name Scalar statistics.
     *
     * The method cSimpleModule::recordScalar() function internally relies on.
     *
     * The behavior of this function can be changed by plugging in a different
     * cOutputScalarManager object into the user interface library. (Or alternatively,
     * by reimplementing the whole cEnvir of course).
     */
    //@{

    /**
     * Records a double scalar result, in a default configuration into the scalar result file.
     */
    void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL);

    /**
     * Records histogram and statistics objects into the scalar result file.
     */
    void recordScalar(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL);
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
    std::ostream *getStreamForSnapshot();

    /**
     * Releases a stream after a snapshot was written.
     */
    void releaseStreamForSnapshot(std::ostream *os);
    //@}

    /** @name Miscellaneous functions. */
    //@{
    /**
     * Access to original command-line arguments.
     */
    int argCount();

    /**
     * Access to original command-line arguments.
     */
    char **argVector();

    /**
     * Returns the partitionID when parallel simulation is active.
     */
    int getParsimProcId();

    /**
     * Returns the number of partitions when parallel simulation is active;
     * otherwise it returns 0.
     */
    int getParsimNumPartitions();

    /**
     * The function underlying cSimulation::getUniqueNumber().
     */
    unsigned long getUniqueNumber();

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
    bool idle();
    //@}
};

template <class E, class T>
int basic_evbuf<E,T>::sync() {
    _ev->sputn(this->pbase(), this->pptr()-this->pbase());
    setp(this->pbase(),this->epptr());
    return 0;
}

NAMESPACE_END


#endif
