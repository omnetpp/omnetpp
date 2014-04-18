//==========================================================================
//  LOGINSPECTOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __LOGINSPECTOR_H
#define __LOGINSPECTOR_H

#include <map>
#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "logbuffer.h"
#include "inspector.h"
#include "cmessageprinter.h"

NAMESPACE_BEGIN


class TKENV_API LogInspector : public Inspector, protected ILogBufferListener
{
   public:
      enum Mode {LOG, MESSAGES};

   protected:
      LogBuffer *logBuffer;
      char textWidget[128];
      std::set<int> excludedModuleIds;
      Mode mode;

   protected:
      virtual void logEntryAdded();
      virtual void logLineAdded();
      virtual void messageSendAdded();

      virtual void printLastLogLine();
      virtual void printLastMessageLine();

      virtual int findFirstRelevantHop(const LogBuffer::MessageSend& msgsend);
      virtual cMessagePrinter *chooseMessagePrinter(cMessage *msg);
      virtual void printMessage(eventnumber_t eventNum, simtime_t time, int srcModuleId, int destModuleId, cMessage *msg);

   public:
      LogInspector(InspectorFactory *f);
      virtual ~LogInspector();
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void doSetObject(cObject *obj);
      virtual void refresh();

      virtual void redisplay();

      virtual Mode getMode() const {return mode;}
      virtual void setMode(Mode mode);

      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);
};

NAMESPACE_END


#endif
