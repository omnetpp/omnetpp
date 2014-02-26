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

    Inspector *createInspector() {
        return prepare(new LogInspector());
    }
};

Register_InspectorFactory(LogInspectorFactory);


LogInspector::LogInspector() : Inspector()
{
}

void LogInspector::createWindow(const char *window, const char *geometry)
{
    Inspector::createWindow(window, geometry);

    strcpy(textWidget,windowName);
    strcat(textWidget, ".main.text");

    CHK(Tcl_VarEval(interp, "createModuleWindow ", windowName, " \"", geometry, "\"", NULL ));
}

void LogInspector::useWindow(const char *window)
{
   Inspector::useWindow(window);

    strcpy(textWidget,windowName);
    strcat(textWidget, ".main.text");
}

void LogInspector::setObject(cObject *obj)
{
    Inspector::setObject(obj);

    if (object)
        redisplay(getTkenv()->getLogBuffer());
}

void LogInspector::refresh()
{
    Inspector::refresh();

    if (!object)
    {
        textWidget_clear(interp, textWidget);
        return;
    }

    CHK(Tcl_VarEval(interp, "moduleWindow:trimlines ", windowName, NULL));
}

void LogInspector::printLastLineOf(const LogBuffer& logBuffer)
{
    printLastLineOf(getTkenv()->getInterp(), textWidget, logBuffer, excludedModuleIds);
}

void LogInspector::redisplay(const LogBuffer& logBuffer)
{
    redisplay(getTkenv()->getInterp(), textWidget, logBuffer, static_cast<cModule *>(object), excludedModuleIds);
}

void LogInspector::printLastLineOf(Tcl_Interp *interp, const char *textWidget, const LogBuffer& logBuffer, const std::set<int>& excludedModuleIds)
{
    const LogBuffer::Entry& entry = logBuffer.getEntries().back();
    if (!entry.moduleIds)
    {
        if (entry.lines.empty())
            textWidget_insert(interp, textWidget, entry.banner, "log");
        else
            textWidget_insert(interp, textWidget, entry.lines.back());
    }
    else if (excludedModuleIds.find(entry.moduleIds[0])==excludedModuleIds.end())
    {
        if (entry.lines.empty())
            textWidget_insert(interp, textWidget, entry.banner, "event");
        else
            textWidget_insert(interp, textWidget, entry.lines.back());
    }
    textWidget_gotoEnd(interp, textWidget);
}

void LogInspector::redisplay(Tcl_Interp *interp, const char *textWidget, const LogBuffer& logBuffer, cModule *mod, const std::set<int>& excludedModuleIds)
{
    textWidget_clear(interp, textWidget);

    if (!mod)
        return;

    int inspModuleId = mod->getId();
    const std::list<LogBuffer::Entry>& entries = logBuffer.getEntries();
    for (std::list<LogBuffer::Entry>::const_iterator it=entries.begin(); it!=entries.end(); it++)
    {
        const LogBuffer::Entry& entry = *it;
        if (!entry.moduleIds)
        {
            textWidget_insert(interp, textWidget, entry.banner, "log");
            for (int i=0; i<(int)entry.lines.size(); i++)
                textWidget_insert(interp, textWidget, entry.lines[i]); //?
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
                textWidget_insert(interp, textWidget, entry.banner, "event");
                for (int i=0; i<(int)entry.lines.size(); i++)
                    textWidget_insert(interp, textWidget, entry.lines[i]);
            }
        }
    }
    textWidget_gotoEnd(interp, textWidget);
}

int LogInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc<1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

    if (strcmp(argv[0],"redisplay")==0)
    {
       if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       TRY(redisplay(getTkenv()->getLogBuffer()));
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
    return TCL_ERROR;
}

NAMESPACE_END

