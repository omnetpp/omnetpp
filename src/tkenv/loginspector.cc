//==========================================================================
//  LOGINSPECTOR.CC - part of
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

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "common/stringtokenizer.h"
#include "loginspector.h"
#include "tkenv.h"
#include "tklib.h"
#include "tkutil.h"
#include "inspectorfactory.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace tkenv {

void _dummy_for_loginspector() {}

class LogInspectorFactory : public InspectorFactory
{
  public:
    LogInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cComponent*>(obj) != nullptr; }
    int getInspectorType() override { return INSP_MODULEOUTPUT; }
    double getQualityAsDefault(cObject *object) override { return 0.5; }
    Inspector *createInspector() override { return new LogInspector(this); }
};

Register_InspectorFactory(LogInspectorFactory);

LogInspector::LogInspector(InspectorFactory *f) : Inspector(f), mode(MESSAGES)
{
    logBuffer = getTkenv()->getLogBuffer();
    logBuffer->addListener(this);
    componentHistory = getTkenv()->getComponentHistory();
    lastMsgEventNumber = 0;
    lastMsgTime = 0;
}

LogInspector::~LogInspector()
{
    logBuffer->removeListener(this);
}

void LogInspector::createWindow(const char *window, const char *geometry)
{
    Inspector::createWindow(window, geometry);

    strcpy(textWidget, windowName);
    strcat(textWidget, ".main.text");

    CHK(Tcl_VarEval(interp, "createLogInspector ", windowName, " ", TclQuotedString(geometry).get(), TCL_NULL));

    int success = Tcl_GetCommandInfo(interp, textWidget, &textWidgetCmdInfo);
    ASSERT(success);
}

void LogInspector::useWindow(const char *window)
{
    Inspector::useWindow(window);

    strcpy(textWidget, windowName);
    strcat(textWidget, ".main.text");

    int success = Tcl_GetCommandInfo(interp, textWidget, &textWidgetCmdInfo);
    ASSERT(success);
}

void LogInspector::setMode(Mode m)
{
    if (mode != m) {
        mode = m;
        redisplay();
    }
}

void LogInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);

    if (object)
        redisplay();
    else {
        CHK(Tcl_VarEval(interp, "LogInspector:clear ", windowName, TCL_NULL));
        textWidgetGotoBeginning();  // important, do not remove! (moves I from 2.0 to 1.0)
    }
}

cComponent *LogInspector::getInspectedObject()
{
    return static_cast<cComponent*>(object);
}

void LogInspector::logEntryAdded()
{
    printLastLogLine();
}

void LogInspector::logLineAdded()
{
    printLastLogLine();
}

void LogInspector::messageSendAdded()
{
    printLastMessageLine();
}

void LogInspector::textWidgetCommand(const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6)
{
    // Note: args do NOT need to be quoted, because they are directly passed to the text widget's cmd proc as (argc,argv)
    const char *argv[] = {
        textWidget, arg1, arg2, arg3, arg4, arg5, arg6
    };
    int argc = !arg1 ? 1 : !arg2 ? 2 : !arg3 ? 3 : !arg4 ? 4 : !arg5 ? 5 : 6;

    TclCmdProc cmdProc = (TclCmdProc)textWidgetCmdInfo.proc;  // Tcl versions differ in the type of the last arg: char *argv[] vs const char *argv[] -- the purpose of this temp var is to make the code compile with both variants
    int ret = cmdProc(textWidgetCmdInfo.clientData, interp, argc, (char **)argv);
    if (ret == TCL_ERROR)
        getTkenv()->logTclError(__FILE__, __LINE__, Tcl_GetStringResult(interp));
}

void LogInspector::textWidgetInsert(const char *text, const char *tags)
{
    if (text)
        textWidgetCommand("insert", "I", text, tags);
}

void LogInspector::textWidgetGotoBeginning()
{
    textWidgetCommand("mark", "set", "I", "1.0");
}

void LogInspector::textWidgetGotoEnd()
{
    // IMPORTANT: Note that "end-1c" is used instead of "end"! Note that "end"
    // has behavior not suitable for us, see example:
    //   $txt delete 1.0 end
    //   $txt mark set I end  --> I becomes 2.0 !!!
    //   $txt insert I "Hello" --> inserts text in line 1, while I is still 2.0
    //   $txt index "I linestart" --> 2.0, so not the start of the line we've been inserting text into!!!
    //
    textWidgetCommand("mark", "set", "I", "end-1c");  // NOT "end" !
}

void LogInspector::textWidgetSeeEnd()
{
    textWidgetCommand("see", "end");
}

void LogInspector::textWidgetSetBookmark(const char *bookmark, const char *pos)
{
    textWidgetCommand("mark", "set", bookmark, pos);
}

void LogInspector::textWidgetDumpBookmarks(const char *label)
{
    CHK(Tcl_VarEval(interp, "LogInspector:dump ", windowName, " {", label, "}", TCL_NULL));
}

void LogInspector::refresh()
{
    Inspector::refresh();
    CHK(Tcl_VarEval(interp, "LogInspector:trimlines ", windowName, TCL_NULL));
}

void LogInspector::printLastLogLine()
{
    if (mode != LOG)
        return;

    const LogBuffer::Entry *entry = logBuffer->getEntries().back();
    if (entry->lines.empty()) {
        // print banner
        if (entry->componentId == 0) {
            textWidgetInsert(entry->banner, "info");
        }
        else {
            entryMatches = isMatchingComponent(entry->componentId);
            bannerPrinted = false;
            bookmarkAdded = false;
            if (entryMatches)
                printBannerIfNeeded(entry);
        }
    }
    else {
        // print last line
        const LogBuffer::Line& line = entry->lines.back();
        if (entry->componentId == 0)
            printLogLine(entry, line);
        else {
            if (entryMatches || isMatchingComponent(line.contextComponentId)) {
                printBannerIfNeeded(entry);
                printLogLine(entry, line);
                addBookmarkIfNeeded(entry);
            }
        }
    }

    textWidgetSeeEnd();
}

void LogInspector::printBannerIfNeeded(const LogBuffer::Entry *entry)
{
    if (!bannerPrinted && getTkenv()->opt->printEventBanners) {
        textWidgetInsert(entry->banner, "banner");
        bannerPrinted = true;
        addBookmarkIfNeeded(entry);
    }
}

void LogInspector::addBookmarkIfNeeded(const LogBuffer::Entry *entry)
{
    if (!bookmarkAdded) {
        char bookmark[64];
        sprintf(bookmark, "event-%" PRId64, entry->eventNumber);
        textWidgetSetBookmark(bookmark, "I-1li");  // beginning of previous line
        bookmarkAdded = true;
    }
}

void LogInspector::printLogLine(const LogBuffer::Entry *entry, const LogBuffer::Line& line)
{
    textWidgetInsert(line.prefix, "prefix");
    if (entry->componentId != line.contextComponentId && line.contextComponentId != 0)
        textWidgetInsert((componentHistory->getComponentFullPath(line.contextComponentId)+": ").c_str(), "prefix");  // XXX rather merge it into prefix somehow
    textWidgetInsert(line.line, (entry->componentId == 0 || line.contextComponentId == 0) ? "info" : "");
}

void LogInspector::printLastMessageLine()
{
    if (mode != MESSAGES)
        return;

    const LogBuffer::Entry *entry = logBuffer->getEntries().back();
    const LogBuffer::MessageSend& msgsend = entry->msgs.back();
    int hopIndex = findFirstRelevantHop(msgsend, 0);  // TODO check remaining hops too
    int inspectedModuleId = ((cModule *)object)->getId();
    if (hopIndex != -1 || msgsend.hopModuleIds.front() == inspectedModuleId || msgsend.hopModuleIds.back() == inspectedModuleId) {
        printMessage(entry, entry->msgs.size()-1, hopIndex, entry->eventNumber == lastMsgEventNumber, entry->simtime == lastMsgTime);
        textWidgetSeeEnd();
        lastMsgEventNumber = entry->eventNumber;
        lastMsgTime = entry->simtime;
    }
}

void LogInspector::redisplay()
{
    CHK(Tcl_VarEval(interp, "LogInspector:clear ", windowName, TCL_NULL));
    textWidgetGotoBeginning();

    if (!object)
        return;

    const circular_buffer<LogBuffer::Entry *>& entries = logBuffer->getEntries();
    int scrollbackLimit = getTkenv()->opt->scrollbackLimit;

    if (mode == LOG) {
        // display entries in reverse order, so we can stop when scrollback limit is reached
        // (within an entry we print normally, top-down)
        int numLinesPrinted = 0;
        bool printEventBanners = getTkenv()->opt->printEventBanners;
        for (int k = entries.size()-1; k >= 0 && numLinesPrinted <= scrollbackLimit; k--) {
            const LogBuffer::Entry *entry = entries[k];
            bool entryProducedOutput = false;
            if (entry->componentId == 0) {
                textWidgetInsert(entry->banner, "info");
                numLinesPrinted++;
                for (int i = 0; i < (int)entry->lines.size(); i++) {
                    printLogLine(entry, entry->lines[i]);
                    numLinesPrinted++;
                }
                entryProducedOutput = true;
            }
            else {
                // if this module is under the inspected module, and not excluded, display the log
                bool entryMatches = isMatchingComponent(entry->componentId);
                bool bannerPrinted = false;
                if (entryMatches && printEventBanners) {
                    textWidgetInsert(entry->banner, "banner");
                    numLinesPrinted++;
                    bannerPrinted = true;
                    entryProducedOutput = true;
                }
                for (int i = 0; i < (int)entry->lines.size(); i++) {
                    const LogBuffer::Line& line = entry->lines[i];
                    if (entryMatches || isMatchingComponent(line.contextComponentId)) {
                        if (!bannerPrinted && printEventBanners) {
                            textWidgetInsert(entry->banner, "auxbanner");
                            numLinesPrinted++;
                            bannerPrinted = true;
                        }
                        printLogLine(entry, line);
                        numLinesPrinted++;
                        entryProducedOutput = true;
                    }
                }
            }
            if (entryProducedOutput) {
                char bookmark[64];
                sprintf(bookmark, "event-%" PRId64, entry->eventNumber);
                textWidgetSetBookmark(bookmark, "1.0");

                textWidgetGotoBeginning();  // for next entry
            }
        }
    }
    else if (mode == MESSAGES) {
        // for incremental printing (printLastMessageLine()) we'll need to remember last msg's event# and time
        this->lastMsgEventNumber = 0;
        this->lastMsgTime = 0;
        bool firstMsg = true;  // first printed = last on on the screen

        // display in reverse order, so we can stop when scrollback limit is reached
        int numLinesPrinted = 0;
        const LogBuffer::Entry *prevEntry = nullptr;
        int prevMsgsendIndex = 0, prevHopIndex = 0;
        int inspectedModuleId = ((cModule *)object)->getId();
        for (int k = entries.size()-1; k >= 0 && numLinesPrinted <= scrollbackLimit; k--) {
            const LogBuffer::Entry *entry = entries[k];
            int numMsgs = entry->msgs.size();
            for (int i = numMsgs-1; i >= 0; i--) {
                const LogBuffer::MessageSend& msgsend = entry->msgs[i];
                int hopIndex = findFirstRelevantHop(msgsend, 0);  // TODO check remaining hops too
                if (hopIndex != -1 || msgsend.hopModuleIds.front() == inspectedModuleId || msgsend.hopModuleIds.back() == inspectedModuleId) {
                    if (prevEntry) {
                        textWidgetGotoBeginning();
                        printMessage(prevEntry, prevMsgsendIndex, prevHopIndex, entry == prevEntry, entry->simtime == prevEntry->simtime);
                        numLinesPrinted++;
                    }

                    prevEntry = entry;
                    prevMsgsendIndex = i;
                    prevHopIndex = hopIndex;

                    if (firstMsg) {
                        this->lastMsgEventNumber = entry->eventNumber;
                        this->lastMsgTime = entry->simtime;
                        firstMsg = false;
                    }
                }
            }
        }

        if (prevEntry) {
            textWidgetGotoBeginning();
            printMessage(prevEntry, prevMsgsendIndex, prevHopIndex, false, false);
        }
    }

    // TODO refine logged text: mention scrollback limit as well!!!!
    if (logBuffer->getNumEntriesDiscarded() > 0) {
        const LogBuffer::Entry *entry = entries.front();
        std::stringstream os;
        if (entry->eventNumber == 0)
            os << "[Some initialization lines have been discarded from the log]\n";
        else
            os << "[Log starts at event #" << entry->eventNumber << " t=" << SIMTIME_STR(entry->simtime) << ", earlier history has been discarded]\n";
        textWidgetGotoBeginning();
        textWidgetInsert(os.str().c_str(), "warning");
    }

    textWidgetGotoEnd();

    textWidgetSeeEnd();
}

bool LogInspector::isMatchingComponent(int componentId)
{
    if (componentId == 0)
        return false;
    int inspectedComponentId = getInspectedObject()->getId();
    return (componentId == inspectedComponentId || isAncestorModule(componentId, inspectedComponentId))
           && excludedModuleIds.find(componentId) == excludedModuleIds.end();
}

bool LogInspector::isAncestorModule(int componentId, int potentialAncestorModuleId)
{
    cComponent *component = getSimulation()->getComponent(componentId);
    if (component) {
        // more efficient version for live modules
        while (component) {
            if (component->getId() == potentialAncestorModuleId)
                return true;
            component = component->getParentModule();
        }
    }
    else {
        while (componentId != -1) {
            if (componentId == potentialAncestorModuleId)
                return true;
            componentId = componentHistory->getParentModuleId(componentId);
        }
    }
    return false;
}

int LogInspector::findFirstRelevantHop(const LogBuffer::MessageSend& msgsend, int fromHop)
{
    // A hop is relevant (visible in the inspected compound module) when both
    // src and dest are either this module or a direct submodule of it.
    // Normally there is only one relevant hop in a msgsend, although degenerate modules
    // (such as a compound module wired inside like this: in-->out) may result in several hops.

    int inspectedComponentId = getInspectedObject()->getId();
    int srcModuleId = msgsend.hopModuleIds[fromHop];
    bool isSrcOk = srcModuleId == inspectedComponentId || componentHistory->getParentModuleId(srcModuleId) == inspectedComponentId;
    int n = msgsend.hopModuleIds.size();
    for (int i = fromHop; i < n-1; i++) {
        int destModuleId = msgsend.hopModuleIds[i+1];
        bool isDestOk = destModuleId == inspectedComponentId || componentHistory->getParentModuleId(destModuleId) == inspectedComponentId;
        if (isSrcOk && isDestOk)
            return i;
        isSrcOk = isDestOk;
    }
    return -1;
}

void LogInspector::printMessage(const LogBuffer::Entry *entry, int msgIndex, int hopIndex, bool repeatedEvent, bool repeatedSimtime)
{
    char buf[128];

    // add event number
    if (!entry->eventNumber)
        strcpy(buf, "-\t");
    else
        sprintf(buf, "#%" PRId64 "\t", entry->eventNumber);
    textWidgetInsert(buf, repeatedEvent ? "repeatedeventnumcol" : "eventnumcol");

    // Add msghop bookmark at the START of the line. IMPORTANT:
    //  (1) it must have RIGHT gravity, otherwise inserting text at top will move it!
    //  (2) may only be inserted after the line already contains some text (here, eventnum),
    //      otherwise the text will be inserted BEFORE the mark!
    char bookmark[80];
    sprintf(bookmark, "msghop#%" PRId64 ":%d", entry->eventNumber, msgIndex);
    textWidgetSetBookmark(bookmark, "I linestart");

    // add time
    char *time = entry->simtime.str(buf);  // TODO add propdelays of previous hops
    strcat(time, "\t");
    textWidgetInsert(time, repeatedSimtime ? "repeatedtimecol" : "timecol");

    // add src/dest
    const LogBuffer::MessageSend& msgsend = entry->msgs[msgIndex];
    int inspectedComponentId = getInspectedObject()->getId();
    if (hopIndex != -1) {
        int srcModuleId = msgsend.hopModuleIds[hopIndex];
        int destModuleId = msgsend.hopModuleIds[hopIndex+1];
        bool hasSrc = srcModuleId != inspectedComponentId;
        bool hasDest = destModuleId != inspectedComponentId;
        if (hasSrc)
            textWidgetInsert(componentHistory->getComponentFullName(srcModuleId), "srcdestcol hopsrc");
        if (hasSrc && hasDest)
            textWidgetInsert(" --> ", "srcdestcol");
        else if (hasSrc)
            textWidgetInsert(" --->", "srcdestcol hopdest");
        else if (hasDest)
            textWidgetInsert("---> ", "srcdestcol hopsrc");
        else
            textWidgetInsert("---->", "srcdestcol hopsrc hopdest");
        if (hasDest)
            textWidgetInsert(componentHistory->getComponentFullName(destModuleId), "srcdestcol hopdest");
    }
    else if (msgsend.hopModuleIds.front() == inspectedComponentId) {
        textWidgetInsert(componentHistory->getComponentFullName(inspectedComponentId), "srcdestcol hopsrc");
        textWidgetInsert(" --->", "srcdestcol hopdest");
    }
    else if (msgsend.hopModuleIds.back() == inspectedComponentId) {
        textWidgetInsert("---> ", "srcdestcol hopsrc");
        textWidgetInsert(componentHistory->getComponentFullName(inspectedComponentId), "srcdestcol hopdest");
    }
    else {
        ASSERT(false);
    }
    textWidgetInsert("\t", "srcdestcol");

    // add packet name
    cMessage *msg = msgsend.msg;
    opp_strprettytrunc(buf, msg->getName(), sizeof(buf)-2);
    strcat(buf, "\t");
    textWidgetInsert(buf, "namecol");

    // add packet info and newline
    cMessagePrinter *printer = chooseMessagePrinter(msg);
    std::stringstream os2;
    if (printer) {
        cMessagePrinter::Options options;
        options.enabledTags = printer->getDefaultEnabledTags();
        printer->printMessage(os2, msg, &options);
    }
    else
        os2 << "[no message printer for this object]";
    os2 << "\n";
    textWidgetInsert(os2.str().c_str(), "msginfocol");
}

cMessagePrinter *LogInspector::chooseMessagePrinter(cMessage *msg)
{
    cRegistrationList *instance = messagePrinters.getInstance();
    cMessagePrinter *bestPrinter = nullptr;
    int bestScore = 0;
    for (int i = 0; i < (int)instance->size(); i++) {
        cMessagePrinter *printer = (cMessagePrinter *)instance->get(i);
        int score = printer->getScoreFor(msg);
        if (score > bestScore) {
            bestPrinter = printer;
            bestScore = score;
        }
    }
    return bestPrinter;
}

int LogInspector::inspectorCommand(int argc, const char **argv)
{
    if (argc < 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    if (strcmp(argv[0], "redisplay") == 0) {
        if (argc != 1) {
            Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC);
            return TCL_ERROR;
        }
        TRY(redisplay());
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getexcludedmoduleids") == 0) {
        if (argc != 1) {
            Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC);
            return TCL_ERROR;
        }
        Tcl_Obj *listobj = Tcl_NewListObj(0, nullptr);
        for (int excludedModuleId : excludedModuleIds)
            Tcl_ListObjAppendElement(interp, listobj, Tcl_NewIntObj(excludedModuleId));
        Tcl_SetObjResult(interp, listobj);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "setexcludedmoduleids") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC);
            return TCL_ERROR;
        }
        excludedModuleIds.clear();
        StringTokenizer tokenizer(argv[1]);
        while (tokenizer.hasMoreTokens())
            excludedModuleIds.insert(atoi(tokenizer.nextToken()));
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getmode") == 0) {
        if (argc != 1) {
            Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC);
            return TCL_ERROR;
        }
        Tcl_SetResult(interp, TCLCONST(getMode() == LOG ? "log" : getMode() == MESSAGES ? "messages" : "???"), TCL_STATIC);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "setmode") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC);
            return TCL_ERROR;
        }
        const char *arg = argv[1];
        if (strcmp(arg, "log") == 0)
            setMode(LOG);
        else if (strcmp(arg, "messages") == 0)
            setMode(MESSAGES);
        else {
            Tcl_SetResult(interp, TCLCONST("wrong mode"), TCL_STATIC);
            return TCL_ERROR;
        }
        return TCL_OK;
    }
    else if (strcmp(argv[0], "gethopmsg") == 0) {  // <eventNumber> <msgsendIndex> -> <msgptr>
        if (argc != 3) {
            Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC);
            return TCL_ERROR;
        }
        eventnumber_t eventNumber = atol(argv[1]);  // XXX atoi64?
        int msgsendIndex = atoi(argv[2]);
        LogBuffer::Entry *entry = logBuffer->getEntryByEventNumber(eventNumber);
        cMessage *msg = nullptr;
        if (entry != nullptr && msgsendIndex >= 0 && msgsendIndex < (int)entry->msgs.size())
            msg = entry->msgs[msgsendIndex].msg;
        Tcl_SetResult(interp, ptrToStr(msg), TCL_VOLATILE);
        return TCL_OK;
    }

    return Inspector::inspectorCommand(argc, argv);
}

}  // namespace tkenv
}  // namespace omnetpp

