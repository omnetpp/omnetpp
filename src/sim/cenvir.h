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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CENVIR_H
#define __CENVIR_H

#include <stdlib.h> // atol, atof
#include "defs.h"


class cObject;
class cMessage;
class cSimpleModule;

class TOmnetApp;

//=== class declared:
class cEnvir;

extern cEnvir ev;               // the environment object

// return codes for runningMode()
enum { SLAVE_MODE = 0,  // must be 0
       MASTER_MODE = 1,
       NONPARALLEL_MODE = 2,
       STARTUPERROR_MODE = 3
     };

//==========================================================================
// cEnvir

class cEnvir
{
  public:
    int argc;
    char **argv;
    TOmnetApp *app;  // user interface application
    int disable_tracing;
  private:
    int running_mode; // MASTER_MODE / SLAVE_MODE / NONPARALLEL_MODE / STARTUPERROR_MODE
    char prmpt[81];    // prompt used by prompt() and operator >>

  public:
    cEnvir();
    ~cEnvir();

    // called from main()
    void setup(int ac, char *av[]);
    void run();
    void shutdown();

    // called by the sim.kernel to notify environment about events
    void objectDeleted(cObject *object);
    void messageSent(cMessage *msg);
    void messageDelivered(cMessage *msg);
    void breakpointHit(const char *lbl, cSimpleModule *mod);

    // called by the sim.kernel to get info
    const char *getParameter(int run_no, const char *parname);
    const char *getPhysicalMachineFor(const char *logical_mach);
    void getOutVectorConfig(const char *modname,const char *vecname, /*input*/
                            bool& enabled, /*output*/
                            double& starttime, double& stoptime);
    const char *getDisplayString(int run_no,const char *name);

    // called from simple modules or the sim.kernel for I/O
    void printfmsg(const char *fmt,...);
    void printf(const char *fmt="\n",...);
    void puts(const char *s);
    bool gets(const char *prompt, char *buf, int len=255);
    bool askf(char *buf, int len, const char *promptfmt,...);
    bool askYesNo(const char *msgfmt,...);

    void foreignPuts(const char *hostname, const char *mod, const char *str);

    cEnvir& setPrompt(const char *s);      // set prompt for >> operators
    const char *prompt()  {return prmpt;}  // return prompt string

    // runningMode() returns whether the simulation is distributed,
    // and if yes, this process is master or slave
    int runningMode()     {return running_mode;}

    // extraStackForEnvir() is called from cSimpleModule; returns how much extra
    // stack space the user interface recommends for the simple modules
    unsigned extraStackForEnvir();
};

//==========================================================================
//  Overloaded operators to provide iostream-like I/O for cEnvir

#if !defined(__BORLANDC__) || __BCPLUSPLUS__>0x0310
/* Compilers don't agree whether char==unsigned/signed char */
inline cEnvir& operator<< (cEnvir& ev, const char *s)
  {ev.puts(s); return ev;}
inline cEnvir& operator<< (cEnvir& ev, char c)
  {ev.printf("%c",c); return ev;}
#endif
inline cEnvir& operator<< (cEnvir& ev, const signed char *s)
  {ev.puts((const char *)s); return ev;}
inline cEnvir& operator<< (cEnvir& ev, const unsigned char *s)
  {ev.puts((const char *)s); return ev;}
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
  {ev.printf("%lg", d); return ev;}
inline cEnvir& operator<< (cEnvir& ev, long double d)
  {ev.printf("%Lg", d); return ev;}

// use the '*' operator instead/besides ev.setPrompt()
// e.g.: ev*"How many?" >> n;
#if !defined(__BORLANDC__) || __BCPLUSPLUS__>0x0310
/* Compilers don't agree whether char==unsigned/signed char */
inline cEnvir& operator* (cEnvir& ev, char *s)
 {return ev.setPrompt((char *)s);}
#endif
inline cEnvir& operator* (cEnvir& ev, const signed char *s)
 {return ev.setPrompt((const char *)s);}
inline cEnvir& operator* (cEnvir& ev, const unsigned char *s)
 {return ev.setPrompt((const char *)s);}

// note: each >> operator reads a whole line!
#if !defined(__BORLANDC__) || __BCPLUSPLUS__>0x0310
inline cEnvir& operator>> (cEnvir& ev, char *s)
 {ev.gets(ev.prompt(),(char *)s, 80); return ev;}
inline cEnvir& operator>> (cEnvir& ev, char& c)
 {char buf[80];buf[0]=0; ev.gets(ev.prompt(), buf, 80); c=buf[0]; return ev;}
#endif
inline cEnvir& operator>> (cEnvir& ev, signed char *s)
 {ev.gets(ev.prompt(),(char *)s, 80); return ev;}
inline cEnvir& operator>> (cEnvir& ev, unsigned char *s)
 {ev.gets(ev.prompt(),(char *)s, 80); return ev;}
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

