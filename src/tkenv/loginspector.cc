//==========================================================================
//  LOGINSPECTOR.CC - part of
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

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "loginspector.h"
#include "tkenv.h"
#include "tklib.h"
#include "tkutil.h"
#include "inspectorfactory.h"
#include "stringtokenizer.h"

NAMESPACE_BEGIN

void _dummy_for_loginspector() {}


class LogInspectorFactory : public InspectorFactory
{
  public:
    LogInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cModule *>(obj)!=NULL;}
    int getInspectorType() {return INSP_MODULEOUTPUT;}
    double getQualityAsDefault(cObject *object) {return 0.5;}
    Inspector *createInspector() {return new LogInspector(this);}
};

Register_InspectorFactory(LogInspectorFactory);


LogInspector::LogInspector(InspectorFactory *f) : Inspector(f), mode(MESSAGES)
{
    logBuffer = getTkenv()->getLogBuffer();
    logBuffer->addListener(this);
}

LogInspector::~LogInspector()
{
    logBuffer->removeListener(this);
}

void LogInspector::createWindow(const char *window, const char *geometry)
{
    Inspector::createWindow(window, geometry);

    strcpy(textWidget,windowName);
    strcat(textWidget, ".main.text");

    CHK(Tcl_VarEval(interp, "createLogInspector ", windowName, " ", TclQuotedString(geometry).get(), NULL ));
}

void LogInspector::useWindow(const char *window)
{
   Inspector::useWindow(window);

    strcpy(textWidget,windowName);
    strcat(textWidget, ".main.text");
}

void LogInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);

    if (object)
        redisplay();
    else
        textWidget_clear(interp, textWidget);
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

cMessagePrinter *LogInspector::chooseMessagePrinter(cMessage *msg)
{
    cRegistrationList *instance = messagePrinters.getInstance();
    cMessagePrinter *bestPrinter = NULL;
    int bestScore = 0;
    for (int i=0; i<(int)instance->size(); i++) {
        cMessagePrinter *printer = (cMessagePrinter *)instance->get(i);
        int score = printer->getScoreFor(msg);
        if (score > bestScore) {
            bestPrinter = printer;
            bestScore = score;
        }
    }
    return bestPrinter;
}

void LogInspector::refresh()
{
    Inspector::refresh();
    CHK(Tcl_VarEval(interp, "LogInspector:trimlines ", windowName, NULL));
}

void LogInspector::printLastLogLine()
{
    if (mode != LOG)
        return;

    const LogBuffer::Entry& entry = logBuffer->getEntries().back();
    if (!entry.moduleIds)
    {
        if (entry.lines.empty())
            textWidget_insert(interp, textWidget, entry.banner, "log");
        else {
            textWidget_insert(interp, textWidget, entry.lines.back().prefix, "prefix");
            textWidget_insert(interp, textWidget, entry.lines.back().line);
        }
    }
    else if (excludedModuleIds.find(entry.moduleIds[0])==excludedModuleIds.end())
    {
        if (entry.lines.empty())
            textWidget_insert(interp, textWidget, entry.banner, "event");
        else {
            textWidget_insert(interp, textWidget, entry.lines.back().prefix, "prefix");
            textWidget_insert(interp, textWidget, entry.lines.back().line);
        }
    }
    textWidget_gotoEnd(interp, textWidget);
}

void LogInspector::printLastMessageLine()
{
    if (mode != MESSAGES)
        return;

    const LogBuffer::Entry& entry = logBuffer->getEntries().back();
    for (int i=0; i<(int)entry.msgs.size(); i++)
    {
        const LogBuffer::MessageSend& msgsend = entry.msgs[i];
        int hopIndex = findFirstRelevantHop(msgsend); //TODO in degenerate case , there may be more than one relevant hops of the same message (e.g. if one node is an empty compound module wired inside)
        if (hopIndex != -1) {
            printMessage(entry.eventNumber, entry.simtime, msgsend.hopModuleIds[hopIndex], msgsend.hopModuleIds[hopIndex+1], msgsend.msg);  //FIXME add up propdelays to simtime
        }
    }
    textWidget_gotoEnd(interp, textWidget);
}

void LogInspector::redisplay()
{
    textWidget_clear(interp, textWidget);

    if (!object)
        return;

    bool printEventBanners = getTkenv()->opt->printEventBanners;
    cModule *mod = static_cast<cModule *>(object);
    int inspModuleId = mod->getId();
    const std::list<LogBuffer::Entry>& entries = logBuffer->getEntries();
    //simtime_t prevTime = -1;
    for (std::list<LogBuffer::Entry>::const_iterator it=entries.begin(); it!=entries.end(); it++)
    {
        const LogBuffer::Entry& entry = *it;
        if (mode == LOG)
        {
            if (!entry.moduleIds)
            {
                textWidget_insert(interp, textWidget, entry.banner, "log");
                for (int i=0; i<(int)entry.lines.size(); i++) {
                    textWidget_insert(interp, textWidget, entry.lines[i].prefix, "prefix");
                    textWidget_insert(interp, textWidget, entry.lines[i].line);
                }
            }
            else
            {
                // check that this module is covered in entry.moduleIds[] (module path up to the root)
                bool found = false;
                for (int *p = entry.moduleIds; !found && *p; p++)
                    if (*p == inspModuleId)
                        found = true;

                // if so, and is not excluded, display log
                if (found && excludedModuleIds.find(entry.moduleIds[0])==excludedModuleIds.end())
                {
                    if (printEventBanners)
                        textWidget_insert(interp, textWidget, entry.banner, "event");
                    for (int i=0; i<(int)entry.lines.size(); i++) {
                        textWidget_insert(interp, textWidget, entry.lines[i].prefix, "prefix");
                        textWidget_insert(interp, textWidget, entry.lines[i].line);
                    }
                }
            }
        }

        if (mode == MESSAGES)
        {
            //bool bannerPrinted = false;
            for (int i=0; i<(int)entry.msgs.size(); i++)
            {
                const LogBuffer::MessageSend& msgsend = entry.msgs[i];
                int hopIndex = findFirstRelevantHop(msgsend); //TODO in degenerate case , there may be more than one relevant hops of the same message (e.g. if one node is an empty compound module wired inside)
                if (hopIndex != -1) {
                    //if (printEventBanners && !bannerPrinted) {
                    //    textWidget_insert(interp, textWidget, entry.banner, "event");
                    //    bannerPrinted = true;
                    //}
                    //simtime_t delta = prevTime<0 ? 0 : entry.simtime - prevTime;
                    printMessage(entry.eventNumber, entry.simtime, msgsend.hopModuleIds[hopIndex], msgsend.hopModuleIds[hopIndex+1], msgsend.msg);  //FIXME add up propdelays to simtime
                    //prevTime = entry.simtime;
                }
            }
        }
    }
    textWidget_gotoEnd(interp, textWidget);
}

//TODO deleted modules!!! (use ComponentHistory class)
inline int getParentModuleId(int moduleId)
{
    return simulation.getModule(moduleId)->getParentModule()->getId(); // assumes that the module has a parent, i.e. it is not the system module!
}

int LogInspector::findFirstRelevantHop(const LogBuffer::MessageSend& msgsend)
{
    // msg is relevant when it has a send hop where both src and dest are
    // either this module or its direct submodule.
    cModule *mod = static_cast<cModule *>(object);
    int inspectedModuleId = mod->getId();
    int srcModuleId = msgsend.hopModuleIds[0];
    bool isSrcOk = srcModuleId == inspectedModuleId || getParentModuleId(srcModuleId) == inspectedModuleId;
    int n = msgsend.hopModuleIds.size();
    for (int i=0; i<n-1; i++)
    {
        int destModuleId = msgsend.hopModuleIds[i+1];
        bool isDestOk = destModuleId == inspectedModuleId || getParentModuleId(destModuleId) == inspectedModuleId;
        if (isSrcOk && isDestOk)
            return i;
        isSrcOk = isDestOk;
    }
    return -1;
}

void LogInspector::printMessage(eventnumber_t eventNum, simtime_t time, int srcModuleId, int destModuleId, cMessage *msg)
{
    cMessagePrinter *printer = chooseMessagePrinter(msg);
    std::stringstream os;
    os << eventNum << "\t";
    textWidget_insert(interp, textWidget, TclQuotedString(os.str().c_str()).get(), "eventnumcol");

    std::stringstream os0;
    os0 << time << "\t";
    textWidget_insert(interp, textWidget, TclQuotedString(os0.str().c_str()).get(), "timecol");

    std::stringstream os1;
    int inspectedModuleId = static_cast<cModule *>(object)->getId();
    if (srcModuleId != inspectedModuleId)
        os1 << simulation.getModule(srcModuleId)->getFullName() << " ";
    os1 << "-->";
    if (destModuleId != inspectedModuleId)
        os1 << " " << simulation.getModule(destModuleId)->getFullName();
    os1 << "\t";
    textWidget_insert(interp, textWidget, TclQuotedString(os1.str().c_str()).get(), "srcdestcol");

    std::stringstream os2;
    if (printer)
        printer->printMessage(os2, msg);
    else
        os2 << "(" << msg->getClassName() << ") " << msg->getFullName() << " [no message printer for this object]";
    os2 << "\n";
    textWidget_insert(interp, textWidget, TclQuotedString(os2.str().c_str()).get(), "msginfocol");
}

void LogInspector::setMode(Mode m)
{
    if (mode != m)
    {
        mode = m;
        redisplay();
    }
}

int LogInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc<1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

    if (strcmp(argv[0],"redisplay")==0)
    {
       if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       TRY(redisplay());
       return TCL_OK;
    }
    else if (strcmp(argv[0],"getexcludedmoduleids")==0)
    {
       if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       Tcl_Obj *listobj = Tcl_NewListObj(0, NULL);
       for (std::set<int>::iterator it=excludedModuleIds.begin(); it!=excludedModuleIds.end(); it++)
           Tcl_ListObjAppendElement(interp, listobj, Tcl_NewIntObj(*it));
       Tcl_SetObjResult(interp, listobj);
       return TCL_OK;
    }
    else if (strcmp(argv[0],"setexcludedmoduleids")==0)
    {
       if (argc!=2) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       excludedModuleIds.clear();
       StringTokenizer tokenizer(argv[1]);
       while (tokenizer.hasMoreTokens())
           excludedModuleIds.insert(atoi(tokenizer.nextToken()));
       return TCL_OK;
    }
    else if (strcmp(argv[0],"getmode")==0)
    {
       if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       Tcl_SetResult(interp, TCLCONST(getMode()==LOG?"log":getMode()==MESSAGES?"messages":"???"), TCL_STATIC);
       return TCL_OK;
    }
    else if (strcmp(argv[0],"setmode")==0)
    {
       if (argc!=2) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       const char *arg = argv[1];
       if (strcmp(arg, "log")==0)
           setMode(LOG);
       else if (strcmp(arg, "messages")==0)
           setMode(MESSAGES);
       else
           {Tcl_SetResult(interp, TCLCONST("wrong mode"), TCL_STATIC); return TCL_ERROR;}
       return TCL_OK;
    }

    return Inspector::inspectorCommand(interp, argc, argv);
}

NAMESPACE_END

