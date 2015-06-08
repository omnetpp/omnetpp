//==========================================================================
//  GENERICOBJECTINSPECTOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
#include "omnetpp/carray.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cmessageheap.h"
#include "omnetpp/cdefaultlist.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cregistrationlist.h"
#include "tkenv.h"
#include "tklib.h"
#include "inspectorfactory.h"
#include "genericobjectinspector.h"

NAMESPACE_BEGIN
namespace tkenv {

void _dummy_for_genericobjectinspector() {}

class GenericObjectInspectorFactory : public InspectorFactory
{
  public:
    GenericObjectInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return true; }
    int getInspectorType() override { return INSP_OBJECT; }
    double getQualityAsDefault(cObject *object) override { return 1.0; }
    Inspector *createInspector() override { return new GenericObjectInspector(this); }
};

Register_InspectorFactory(GenericObjectInspectorFactory);

GenericObjectInspector::GenericObjectInspector(InspectorFactory *f) : Inspector(f)
{
}

GenericObjectInspector::~GenericObjectInspector()
{
}

void GenericObjectInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);

    CHK(Tcl_VarEval(interp, "GenericObjectInspector:onSetObject ", windowName, nullptr));
}

void GenericObjectInspector::createWindow(const char *window, const char *geometry)
{
    Inspector::createWindow(window, geometry);

    CHK(Tcl_VarEval(interp, "createGenericObjectInspector ", windowName, " ", TclQuotedString(geometry).get(), nullptr));
}

void GenericObjectInspector::useWindow(const char *window)
{
    Inspector::useWindow(window);
}

void GenericObjectInspector::refresh()
{
    Inspector::refresh();

    CHK(Tcl_VarEval(interp, "GenericObjectInspector:refresh ", windowName, nullptr));
}

void GenericObjectInspector::commit()
{
    Inspector::commit();
}

int GenericObjectInspector::inspectorCommand(int argc, const char **argv)
{
    return Inspector::inspectorCommand(argc, argv);
}

}  // namespace tkenv
NAMESPACE_END

