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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CENVIR_H
#define __CENVIR_H

#include <stdlib.h> // atol, atof
#include "defs.h"
#include "cpar.h"


class cObject;
class cMessage;
class cSimpleModule;

class TOmnetApp;

//=== class declared:
class cEnvir;

// the environment object global instance
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
// cEnvir


/**
 * FIXME: //=== class declared:
 * the environment object global instance
 * return codes for runningMode()
 * 
 * cEnvir
 */
class ENVIR_API cEnvir
{
  public:
    TOmnetApp *app;  // user interface application
    int disable_tracing;
  private:
    int running_mode; // MASTER_MODE / SLAVE_MODE / NONPARALLEL_MODE / STARTUPERROR_MODE
    char prmpt[81];    // prompt used by prompt() and operator >>

  public:

    /**
     * The constructor and the destructor in most cases do nothing.
     */
    cEnvir();
    ~cEnvir();

    // called from main()

    /**
     * FIXME: called from main()
     */
    void setup(int ac, char *av[]);

    /**
     * MISSINGDOC: cEnvir:void run()
     */
    void run();

    /**
     * MISSINGDOC: cEnvir:void shutdown()
     */
    void shutdown();

    // called by the sim.kernel to notify environment about events

    /**
     * Notifies the environment that the object no longer exists. The
     * user interface should close all inspector windows for the object
     * and remove it from object lists currently displayed. cObject's
     * destructor automatically calls this function.
     */
    void objectDeleted(cObject *object);

    /**
     * MISSINGDOC: cEnvir:void messageSent(cMessage*)
     */
    void messageSent(cMessage *msg);

    /**
     * MISSINGDOC: cEnvir:void messageDelivered(cMessage*)
     */
    void messageDelivered(cMessage *msg);

    /**
     * MISSINGDOC: cEnvir:void breakpointHit(char*,cSimpleModule*)
     */
    void breakpointHit(const char *lbl, cSimpleModule *mod);

    // called by the sim.kernel to get info

    /**
     * FIXME: called by the sim.kernel to get info
     */
    const char *getParameter(int run_no, const char *parname);

    /**
     * MISSINGDOC: cEnvir:char*getPhysicalMachineFor(char*)
     */
    const char *getPhysicalMachineFor(const char *logical_mach);
    void getOutVectorConfig(const char *modname,const char *vecname, /*input*/
                            bool& enabled, /*output*/

    /**
     * MISSINGDOC: cEnvir:void getOutVectorConfig(char*,char*,//input// bool&,//output// double&,double&)
     */
                            double& starttime, double& stoptime);

    /**
     * MISSINGDOC: cEnvir:char*getDisplayString(int,char*)
     */
    const char *getDisplayString(int run_no,const char *name);

    // called from simple modules or the sim.kernel for I/O

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
     * Similar to cEnvir::printf(), but just writes out its
     * argument string with no formatting.
     */
    void puts(const char *s);

    /**
     * Similar to cEnvir::askf(), but just writes out the prompt
     * message string with no formatting.
     */
    bool gets(const char *prompt, char *buf, int len=255);

    /**
     * Pops up a dialog, displays the message given in 'profmt' and following
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
     * MISSINGDOC: cEnvir:void foreignPuts(char*,char*,char*)
     */
    void foreignPuts(const char *hostname, const char *mod, const char *str);


    /**
     * MISSINGDOC: cEnvir:cEnvir&setPrompt(char*)
     */
    cEnvir& setPrompt(const char *s);      // set prompt for >> operators

    /**
     * Returns the prompt string.
     */
    const char *prompt()  {return prmpt;}  // return prompt string

    // runningMode() returns whether the simulation is distributed,
    // and if yes, this process is master or slave

    /**
     * FIXME: runningMode() returns whether the simulation is distributed,
     * and if yes, this process is master or slave
     */
    int runningMode()     {return running_mode;}

    // extraStackForEnvir() is called from cSimpleModule; returns how much extra
    // stack space the user interface recommends for the simple modules

    /**
     * FIXME: extraStackForEnvir() is called from cSimpleModule; returns how much extra
     * stack space the user interface recommends for the simple modules
     */
    unsigned extraStackForEnvir();

    // access to original command-line arguments

    /**
     * FIXME: access to original command-line arguments
     */
    int argCount();

    /**
     * MISSINGDOC: cEnvir:char**argVector()
     */
    char **argVector();
};

//==========================================================================
//  Overloaded operators to provide iostream-like I/O for cEnvir

inline cEnvir& operator<< (cEnvir& ev, cPar& p)
  {char buf[81]; p.getAsText(buf,80); ev.puts(buf); return ev;}
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
  {ev.printf("%Lg", d); return ev;}

// use the '*' operator instead/besides ev.setPrompt()
// e.g.: ev*"How many?" >> n;
inline cEnvir& operator* (cEnvir& ev, char *s)
 {return ev.setPrompt((char *)s);}
inline cEnvir& operator* (cEnvir& ev, const signed char *s)
 {return ev.setPrompt((const char *)s);}
inline cEnvir& operator* (cEnvir& ev, const unsigned char *s)
 {return ev.setPrompt((const char *)s);}

// note: each >> operator reads a whole line!
inline cEnvir& operator>> (cEnvir& ev, cPar& p)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); p.setFromText(buf,'?'); return ev;}
inline cEnvir& operator>> (cEnvir& ev, char *s)
 {ev.gets(ev.prompt(),(char *)s, 80); return ev;}
inline cEnvir& operator>> (cEnvir& ev, signed char *s)
 {ev.gets(ev.prompt(),(char *)s, 80); return ev;}
inline cEnvir& operator>> (cEnvir& ev, unsigned char *s)
 {ev.gets(ev.prompt(),(char *)s, 80); return ev;}
inline cEnvir& operator>> (cEnvir& ev, char& c)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); c=buf[0]; return ev;}
inline cEnvir& operator>> (cEnvir& ev, signed char& c)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); c=buf[0]; return ev;}
inline cEnvir& operator>> (cEnvir& ev, unsigned char& c)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); c=buf[0]; return ev;}
inline cEnvir& operator>> (cEnvir& ev, short& i)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); i=(short)atol(buf); return ev;}
inline cEnvir& operator>> (cEnvir& ev, int& i)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); i=(int)atol(buf); return ev;}
inline cEnvir& operator>> (cEnvir& ev, long& l)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); l=atol(buf); return ev;}
inline cEnvir& operator>> (cEnvir& ev, unsigned short& i)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); i=(unsigned short)atol(buf); return ev;}
inline cEnvir& operator>> (cEnvir& ev, unsigned int& i)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); i=(unsigned int)atol(buf); return ev;}
inline cEnvir& operator>> (cEnvir& ev, unsigned long& l)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); l=(unsigned long)atol(buf); return ev;}
inline cEnvir& operator>> (cEnvir& ev, double& d)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); d=atof(buf); return ev;}
inline cEnvir& operator>> (cEnvir& ev, long double& d)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); d=atof(buf); return ev;}

inline cEnvir& endl(cEnvir& ev) {ev.puts("\n"); return ev;}
inline cEnvir& operator<<(cEnvir& ev, cEnvir& (*f)(cEnvir&)) {return (*f)(ev);}

#endif
