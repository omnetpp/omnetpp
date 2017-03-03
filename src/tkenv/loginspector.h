//==========================================================================
//  LOGINSPECTOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_TKENV_LOGINSPECTOR_H
#define __OMNETPP_TKENV_LOGINSPECTOR_H

#include <map>
#include "omnetpp/cmessageprinter.h"
#include "logbuffer.h"
#include "componenthistory.h"
#include "inspector.h"

namespace omnetpp {
namespace tkenv {


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
      cComponent *getInspectedObject();
      void textWidgetCommand(const char *arg1, const char *arg2=nullptr, const char *arg3=nullptr, const char *arg4=nullptr, const char *arg5=nullptr, const char *arg6=nullptr);
      void textWidgetInsert(const char *text, const char *tags);
      void textWidgetSeeEnd();
      void textWidgetGotoBeginning();
      void textWidgetGotoEnd();
      void textWidgetSetBookmark(const char *bookmark, const char *pos);
      void textWidgetDumpBookmarks(const char *label);

      virtual void logEntryAdded() override;
      virtual void logLineAdded() override;
      virtual void messageSendAdded() override;

      bool isMatchingComponent(int componentId);
      bool isAncestorModule(int componentId, int potentialAncestorModuleId);

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
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(const char *window) override;
      virtual void doSetObject(cObject *obj) override;
      virtual void refresh() override;

      virtual void redisplay();

      virtual Mode getMode() const {return mode;}
      virtual void setMode(Mode mode);

      virtual int inspectorCommand(int argc, const char **argv) override;
};

} // namespace tkenv
}  // namespace omnetpp


#endif
