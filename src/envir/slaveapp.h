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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SLAVEAPP_H
#define __SLAVEAPP_H

#include "envdefs.h"
#include "omnetapp.h"

//==========================================================================
// TSlaveApp: "slave" mode simulation application

class ENVIR_API TSlaveApp : public TOmnetApp
{
   protected:
     bool opt_write_slavelog;
     opp_string opt_slavelogfile_name;

     bool opt_module_msgs;

     bool opt_errmsgs2cons;
     bool opt_infomsgs2cons;
     bool opt_modmsgs2cons;

   public:
     TSlaveApp(ArgList *args, cIniFile *inifile);
     ~TSlaveApp();

     // redefined functions
     virtual int run();
     virtual void shutdown();

     virtual void readOptions();

     virtual void putmsg(const char *s);
     virtual void puts(const char *s);
     virtual bool gets(const char *promptstr, char *buf, int len=255);
     virtual int  askYesNo(const char *question);

     // new functions:
     void simulate();
};

#endif
