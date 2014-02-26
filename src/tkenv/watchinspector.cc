//==========================================================================
//  WATCHINSPECTOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Implementation of
//    inspectors
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <math.h>
#include "tkenv.h"
#include "tklib.h"
#include "inspectorfactory.h"
#include "watchinspector.h"
#include "cwatch.h"
#include "cstlwatch.h"

NAMESPACE_BEGIN


void _dummy_for_watchinspector() {}


class WatchInspectorFactory : public InspectorFactory
{
  public:
    WatchInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) {
        // Return true if it's a watch for a simple type (int, double, string etc).
        // For structures, we prefer the normal TGenericObjectInspector.
        // Currently we're prepared for cStdVectorWatcherBase.
        return dynamic_cast<cWatchBase *>(obj) && !dynamic_cast<cStdVectorWatcherBase *>(obj);
    }
    int getInspectorType() {return INSP_OBJECT;}
    double getQualityAsDefault(cObject *object) {return 2.0;}
    Inspector *createInspector() {
        return prepare(new WatchInspector());
    }
};

Register_InspectorFactory(WatchInspectorFactory);


WatchInspector::WatchInspector() : Inspector()
{
}

void WatchInspector::createWindow(const char *window, const char *geometry)
{
   Inspector::createWindow(window, geometry);

   CHK(Tcl_VarEval(interp, "createWatchInspector ", windowName, " \"", geometry, "\"", NULL ));
}

void WatchInspector::refresh()
{
   Inspector::refresh();

   cWatchBase *watch = static_cast<cWatchBase *>(object);
   setLabel(".main.name.l", (std::string(watch->getClassName())+" "+watch->getName()).c_str());
   setEntry(".main.name.e", watch->info().c_str());
}

void WatchInspector::commit()
{
   cWatchBase *watch = static_cast<cWatchBase *>(object);
   const char *s = getEntry(".main.name.e");
   if (watch->supportsAssignment())
       watch->assign(s);
   else
      CHK(Tcl_VarEval(interp,"messagebox {Warning} {This inspector doesn't support changing the value.} warning ok", NULL));
   Inspector::commit();    // must be there after all changes
}

NAMESPACE_END

