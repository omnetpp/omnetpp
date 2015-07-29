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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cmath>
#include "omnetpp/cwatch.h"
#include "omnetpp/cstlwatch.h"
#include "qtenv.h"
#include "tklib.h"
#include "inspectorfactory.h"
#include "watchinspector.h"

namespace omnetpp {
namespace qtenv {

void _dummy_for_watchinspector() {}

class WatchInspectorFactory : public InspectorFactory
{
  public:
    WatchInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override
    {
        // Return true if it's a watch for a simple type (int, double, string etc).
        // For structures, we prefer the normal GenericObjectInspector.
        // Currently we're prepared for cStdVectorWatcherBase.
        return dynamic_cast<cWatchBase *>(obj) && !dynamic_cast<cStdVectorWatcherBase *>(obj);
    }

    int getInspectorType() override { return INSP_OBJECT; }
    double getQualityAsDefault(cObject *object) override { return 2.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new WatchInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(WatchInspectorFactory);

WatchInspector::WatchInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
}

void WatchInspector::refresh()
{
    Inspector::refresh();

    cWatchBase *watch = static_cast<cWatchBase *>(object);
    //TODO setLabel(".main.name.l", watch ? (std::string(watch->getClassName())+" "+watch->getName()).c_str() : "n/a");
    //TODO setEntry(".main.name.e", watch ? watch->info().c_str() : "n/a");
}

void WatchInspector::commit()
{
    cWatchBase *watch = static_cast<cWatchBase *>(object);
    const char *s = ""; //TODO getEntry(".main.name.e");
    if (watch->supportsAssignment())
        watch->assign(s);
    else
        CHK(Tcl_VarEval(interp, "messagebox {Warning} {This inspector doesn't support changing the value.} warning ok", TCL_NULL));
    Inspector::commit();  // must be there after all changes
}

} // namespace qtenv
} // namespace omnetpp

