//==========================================================================
//   SLAVEAPP.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    TSlaveApp:    simulation application working in "slave" mode
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SLAVEAPP_H
#define __SLAVEAPP_H

#include "omnetapp.h"

//==========================================================================
// TSlaveApp: "slave" mode simulation application

class TSlaveApp : public TOmnetApp
{
   protected:
     bool opt_write_slavelog;
     opp_string opt_slavelogfile_name;

     bool opt_module_msgs;

     bool opt_errmsgs2cons;
     bool opt_infomsgs2cons;
     bool opt_modmsgs2cons;

   public:
     TSlaveApp(int argc, char *argv[]);
     ~TSlaveApp();

     // redefined functions
     virtual void run();
     virtual void shutdown();

     virtual void readOptions();

     virtual void putmsg(char *s);
     virtual void puts(char *s);
     virtual bool gets(char *promptstr, char *buf, int len=255);
     virtual int  askYesNo(char *question );

     // new functions:
     void simulate();
};

#endif
