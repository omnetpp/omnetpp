//==========================================================================
//   CENVIR.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cEnvir    : user interface class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CENVIR_H
#define __CENVIR_H

#include "defs.h"

#include <stdlib.h> // atol, atof
#include "cpar.h"


class cObject;
class cMessage;
class cGate;
class cSimpleModule;

class TOmnetApp;

//=== class declared:
class cEnvir;

/**
 * The environment object global instance.
 *
 * @ingroup Envir
 */
ENVIR_API extern cEnvir ev;


// return codes for runningMode()
enum {
    SLAVE_MODE = 0,  // must be 0
    MASTER_MODE = 1,
    NONPARALLEL_MODE = 2,
    STARTUPERROR_MODE = 3
};

ENVIR_API bool memoryIsLow();
ENVIR_API bool opp_loadlibrary(const char *libname);

//==========================================================================

/**
 * Interface to the environment (user interface) of the simulation. cEnvir
 * is a common facade for the Cmdenv and Tkenv user interfaces (and any
 * other future user interface).
 *
 * cEnvir has only one instance, the ev global variable.
 *
 * cEnvir member functions can be rougly divided into two groups:
 * <UL>
 *   <LI> I/O for module activities; actual implementation is different for each
 *        user interface (e.g. stdin/stdout for Cmdenv, windowing in Tkenv)
 *   <LI> functions for exchanging information between the simulation and the
 *        environment.
 * </UL>

 * The implementation of cEnvir is <b>not</b> part of the simulation kernel,
 * it's in a separate library (the Envir library; see src/envir). This means that
 * customizers are free to replace the environment of the simulation as
 * they like, by simply linking the executable with a different library.
 *
 * The default (Envir library) implementation of cEnvir delegates its task to an
 * internal "simulation application" object. Cmdenv and Tkenv inherit from the
 * default simulation application class.
 *
 * The default (src/envir) implementation of cEnvir can be customized without
 * changing the via classes
 * declared in the envirext.h header (e.g. cOutputVectorManager, cOutputScalarManager).
 *
 * @see cOutputVectorManager class
 * @see cOutputScalarManager class
 * @ingroup Envir
 */
class ENVIR_API cEnvir
{
  public:
    TOmnetApp *app;  // the "simulation application" instance
  public:
    int disable_tracing;
  private:
    int running_mode; // MASTER_MODE / SLAVE_MODE / NONPARALLEL_MODE / STARTUPERROR_MODE
    char prmpt[81];    // prompt used by prompt() and operator >>

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
    void messageSent(cMessage *msg, cGate *directToGate=NULL);

    /**
     * Notifies the environment that a message was delivered to its destination
     * module, that is, a message arrival event occurred. Details can be
     * extracted from the message object itself. The user interface
     * implementation may use the notification to animate the message on a
     * network diagram, to write a log entry, etc.
     */
    void messageDelivered(cMessage *msg);

    /**
     * Notifies the environment that a simple module executed a
     * breakpoint() call.
     */
    void breakpointHit(const char *lbl, cSimpleModule *mod);

    /**
     * Notifies the environment that one module called a member function 
     * of another module object. This hook enables a graphical user 
     * interface animate the method call in the network diagram.    
     */
    void moduleMethodCalled(cModule *from, cModule *to, const char *method);
    //@}

    /** @name Methods called by the simulation kernel to access configuration settings. */
    //@{

    /**
     * Called by the simulation kernel (cModulePar) to obtain value for an input
     * module parameter.
     */
    const char *getParameter(int run_no, const char *parname);

    /**
     * User by distributed execution. Returns physical name for a logical
     * machine name.
     */
    const char *getPhysicalMachineFor(const char *logical_mach);

    /**
     * Returns display string for an object given with its full name.
     */
    const char *getDisplayString(int run_no, const char *name);
    //@}

    /** @name Input/output methods called from simple modules or the simulation kernel. */
    //@{

    /**
     * Displays a message in dialog box. This function should not be
     * used too much by simple modules, if ever.
     */
    void printfmsg(const char *fmt,...);

    /**
     * Simple modules can output text into their own window through this
     * function. The text  is expected in printf() format (format
     * string + arguments).
     */
    void printf(const char *fmt="\n",...);

    /**
     * Similar to ev.printf(), but just writes out its argument string with no formatting.
     */
    void puts(const char *s);

    /**
     * Flushes the output buffer of ev.printf() and ev<< operations.
     * Only some user interfaces need it: it can be useful with Cmdenv which
     * writes to the standard output, but no need for it with Tkenv which
     * displays all output immediately anyway.
     */
    void flush();

    /**
     * Similar to cEnvir::askf(), but just writes out the prompt
     * message string with no formatting.
     */
    bool gets(const char *prompt, char *buf, int len=255);

    /**
     * Pops up a dialog, displays the message given in 'promptfmt' and following
     * arguments in printf() format and reads a line (maximum
     * len characters) from the user into the buffer 'buf'.
     * Returns true if the user pressed the Cancel button.
     */
    bool askf(char *buf, int len, const char *promptfmt,...);

    /**
     * Puts a yes/no question to the user. The question itself  is expected
     * in the printf() format (format string + arguments). The
     * true return value means yes, false means no.
     */
    bool askYesNo(const char *msgfmt,...);

    /**
     * Sets the prompt for >> operators.
     */
    cEnvir& setPrompt(const char *s);

    /**
     * Returns the prompt string.
     */
    const char *prompt() const  {return prmpt;}

    /**
     * Used with parallel execution. It is called internally by the simulation
     * kernel when something was written to ev by another segment.
     */
    void foreignPuts(const char *hostname, const char *mod, const char *str);
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
    void *registerOutputVector(const char *modulename, const char *vectorname, int tuple);

    /**
     * cOutVector objects must deregister themselves when they are no longer needed.
     */
    void deregisterOutputVector(void *vechandle);

    /**
     * This method is intended to be called by cOutVector objects to write
     * a value into the output vector. The return value is true if the data was
     * actually recorded, and false if it was not recorded (because of filtering, etc.)
     */
    bool recordInOutputVector(void *vechandle, simtime_t t, double value);

    /**
     * This method is intended to be called by cOutVector objects to write
     * a value pair into the output vector. The return value is true if the data was
     * actually recorded, and false if it was not recorded (because of filtering, etc.)
     */
    bool recordInOutputVector(void *vechandle, simtime_t t, double value1, double value2);
    //@}

    /** @name Scalar statistics.
     *
     * These are the functions the cSimpleModule::recordScalar() functions internally
     * rely on.
     *
     * The behavior of these functions can be changed by plugging in a different
     * cOutputScalarManager object into the user interface library. (Or alternatively,
     * by reimplementing the whole cEnvir of course).
     */
    //@{

    /**
     * Records a double scalar result, in a default configuration into the scalar result file.
     */
    void recordScalar(cModule *module, const char *name, double value);

    /**
     * Records a string result, in a default configuration into the scalar result file.
     */
    void recordScalar(cModule *module, const char *name, const char *text);

    /**
     * Records a statistics object, in a default configuration into the scalar result file.
     */
    void recordScalar(cModule *module, const char *name, cStatistic *stats);
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
    ostream *getStreamForSnapshot();

    /**
     * Releases a stream after a snapshot was written.
     */
    void releaseStreamForSnapshot(ostream *os);
    //@}

    /** @name Miscellaneous functions. */
    //@{

    /**
     * Called from cSimpleModule; it returns how much extra stack space
     * the user interface recommends for the simple modules.
     */
    unsigned extraStackForEnvir();

    /**
     * Returns whether the simulation is distributed, and if yes,
     * if this process is master or slave.
     */
    int runningMode() const     {return running_mode;}

    /**
     * Access to original command-line arguments.
     */
    int argCount();

    /**
     * Access to original command-line arguments.
     */
    char **argVector();
    //@}
};

//==========================================================================
//  Overloaded operators to provide iostream-like I/O for cEnvir

cEnvir& operator<< (cEnvir& ev, cPar& p);
inline cEnvir& operator<< (cEnvir& ev, const char *s)
  {ev.puts(s); return ev;}
inline cEnvir& operator<< (cEnvir& ev, const signed char *s)
  {ev.puts((const char *)s); return ev;}
inline cEnvir& operator<< (cEnvir& ev, const unsigned char *s)
  {ev.puts((const char *)s); return ev;}
inline cEnvir& operator<< (cEnvir& ev, char c)
  {ev.printf("%c",c); return ev;}
inline cEnvir& operator<< (cEnvir& ev, unsigned char c)
  {ev.printf("%c",c); return ev;}
inline cEnvir& operator<< (cEnvir& ev, signed char c)
  {ev.printf("%c",c); return ev;}
inline cEnvir& operator<< (cEnvir& ev, short i)
  {ev.printf("%d", (int)i); return ev;}
inline cEnvir& operator<< (cEnvir& ev, unsigned short i)
  {ev.printf("%u", (int)i); return ev;}
inline cEnvir& operator<< (cEnvir& ev, int i)
  {ev.printf("%d", i); return ev;}
inline cEnvir& operator<< (cEnvir& ev, unsigned int i)
  {ev.printf("%u", i); return ev;}
inline cEnvir& operator<< (cEnvir& ev, long l)
  {ev.printf("%ld", l); return ev;}
inline cEnvir& operator<< (cEnvir& ev, unsigned long l)
  {ev.printf("%lu", l); return ev;}
inline cEnvir& operator<< (cEnvir& ev, double d)
  {ev.printf("%g", d); return ev;}
inline cEnvir& operator<< (cEnvir& ev, long double d)
  {ev.printf("%lg", d); return ev;}

// endl
inline cEnvir& endl(cEnvir& ev) {ev.puts("\n"); return ev;}
inline cEnvir& operator<<(cEnvir& ev, cEnvir& (*f)(cEnvir&)) {return (*f)(ev);}

// '*' operator is a synonym to ev.setPrompt()
// e.g.: ev*"How many?" >> n;
cEnvir& operator* (cEnvir& ev, char *s);
cEnvir& operator* (cEnvir& ev, const signed char *s);
cEnvir& operator* (cEnvir& ev, const unsigned char *s);

// NOTE: each >> operator reads a whole line!
cEnvir& operator>> (cEnvir& ev, cPar& p);
cEnvir& operator>> (cEnvir& ev, char *s);
cEnvir& operator>> (cEnvir& ev, signed char *s);
cEnvir& operator>> (cEnvir& ev, unsigned char *s);
cEnvir& operator>> (cEnvir& ev, char& c);
cEnvir& operator>> (cEnvir& ev, signed char& c);
cEnvir& operator>> (cEnvir& ev, unsigned char& c);
cEnvir& operator>> (cEnvir& ev, short& i);
cEnvir& operator>> (cEnvir& ev, int& i);
cEnvir& operator>> (cEnvir& ev, long& l);
cEnvir& operator>> (cEnvir& ev, unsigned short& i);
cEnvir& operator>> (cEnvir& ev, unsigned int& i);
cEnvir& operator>> (cEnvir& ev, unsigned long& l);
cEnvir& operator>> (cEnvir& ev, double& d);
cEnvir& operator>> (cEnvir& ev, long double& d);


#endif
