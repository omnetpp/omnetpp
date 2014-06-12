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
#include "componenthistory.h"
#include "inspector.h"
#include "cmessageprinter.h"

NAMESPACE_BEGIN


class TKENV_API LogInspector : public Inspector, protected ILogBufferListener
{
   public:
      enum Mode {LOG, MESSAGES};

   protected:
      LogBuffer *logBuffer; // not owned
      ComponentHistory *componentHistory; // not owned
      char textWidget[128];
      Tcl_CmdInfo textWidgetCmdInfo;
      std::set<int> excludedModuleIds;
      Mode mode;

      // state used during incremental printing (printLastLogLine(), printLastMessageLine()):
      // MESSAGES mode:
      eventnumber_t lastMsgEventNumber;
      simtime_t lastMsgTime;
      // LOG mode:
      bool entryMatches;
      bool bannerPrinted;
      bool bookmarkAdded;

   protected:
      void textWidgetCommand(const char *arg1, const char *arg2=NULL, const char *arg3=NULL, const char *arg4=NULL, const char *arg5=NULL, const char *arg6=NULL);
      void textWidgetInsert(const char *text, const char *tags);
      void textWidgetSeeEnd();
      void textWidgetGotoBeginning();
      void textWidgetGotoEnd();
      void textWidgetSetBookmark(const char *bookmark, const char *pos);
      void textWidgetDumpBookmarks(const char *label);

      virtual void logEntryAdded();
      virtual void logLineAdded();
      virtual void messageSendAdded();

      bool isMatchingComponent(int componentId);
      bool isAncestorModule(int moduleId, int potentialAncestorModuleId);

      virtual void printLastLogLine();
      virtual void printLastMessageLine();

      void printBannerIfNeeded(const LogBuffer::Entry *entry);
      void addBookmarkIfNeeded(const LogBuffer::Entry *entry);
      virtual void printLogLine(const LogBuffer::Entry *entry, const LogBuffer::Line& line);

      virtual int findFirstRelevantHop(const LogBuffer::MessageSend& msgsend, int fromHop);
      virtual cMessagePrinter *chooseMessagePrinter(cMessage *msg);
      virtual void printMessage(const LogBuffer::Entry *entry, int msgIndex, int hopIndex, bool repeatedEvent, bool repeatedSimtime);

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

      virtual int inspectorCommand(int argc, const char **argv);
};

NAMESPACE_END


#endif
