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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMDENV_H
#define __CMDENV_H

#include "ctypes.h"
#include "csimul.h"
#include "omnetapp.h"

//==========================================================================
// TCmdenvApp: command line user interface.

class TCmdenvApp : public TOmnetApp
{
   protected:
     // new simulation options:
     int opt_helponly;
     opp_string opt_runstoexec;

     int opt_modulemsgs;
     int opt_verbose;
     simtime_t opt_displayinterval;

   public:
     TCmdenvApp(int argc, char *argv[]);
     ~TCmdenvApp();

     // redefined virtual funcs:
     virtual void setup(int argc, char *argv[]);
     virtual void run();
     //virtual void shutdown();

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

     virtual unsigned extraStackForEnvir() {return 8192;}

     // cmdenv has an own memory manager (heap.cc)
     virtual bool memoryIsLow();
};

#endif

