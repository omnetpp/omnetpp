//==========================================================================
//   CMDENV.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    TCmdenvApp:         command line simulation application
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMDENV_H
#define __CMDENV_H

#include "ctypes.h"
#include "csimul.h"
#include "omnetapp.h"
#include <signal.h>


#define CMDENV_EXTRASTACK  8192

//==========================================================================
// TCmdenvApp: command line user interface.

class TCmdenvApp : public TOmnetApp
{
   protected:
     // new simulation options:
     int opt_helponly;
     opp_string opt_runstoexec;
     unsigned opt_extrastack;

     bool opt_expressmode;
     bool opt_autoflush; // all modes
     bool opt_modulemsgs;  // if normal mode
     bool opt_eventbanners; // if normal mode
     bool opt_messagetrace; // if normal mode
     int opt_status_frequency_ev; // if express mode
     // double opt_status_frequency_sec; -- not yet implemented
     bool opt_perfdisplay; // if express mode

     // set to true on SIGINT/SIGTERM signals
     static bool sigint_received;

   public:
     TCmdenvApp(ArgList *args, cIniFile *inifile);
     ~TCmdenvApp();

     // redefined virtual funcs:
     virtual void setup();
     virtual int run();
     //virtual void shutdown();

     virtual void messageSent(cMessage *msg);
     virtual void messageDelivered(cMessage *msg);

     virtual void readOptions();
     virtual void readPerRunOptions( int run_nr );
     virtual void makeOptionsEffective();

     // new functions:
     void help();
     void simulate();

     // redefined I/O functions:
     //virtual void putmsg(const char *s);
     virtual void puts(const char *s);
     //virtual bool gets(const char *promptstr, char *buf, int len=255);
     //virtual bool askYesNo(const char *question);

     virtual unsigned extraStackForEnvir();

     // cmdenv has an own memory manager (heap.cc)
     virtual bool memoryIsLow();

     // SIG_USR1 handling
     void setupSignals();
     static void signalHandler(int signum);
};

#endif

