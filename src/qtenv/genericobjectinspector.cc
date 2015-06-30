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
#include "omnetpp/cregistrationlist.h"
#include "qtenv.h"
#include "tklib.h"
#include "inspectorfactory.h"
#include "genericobjectinspector.h"
#include "genericobjecttreemodel.h"
#include "envir/objectprinter.h"
#include <QTreeView>
#include <QDebug>

namespace omnetpp {
namespace qtenv {

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

    if (!window) {
        return;
    }

    QTreeView *view = static_cast<QTreeView *>(window);

    GenericObjectTreeModel *oldModel = dynamic_cast<GenericObjectTreeModel *>(view->model());
    GenericObjectTreeModel *newModel = new GenericObjectTreeModel(obj, view);
    view->setModel(newModel);
    delete oldModel;
    view->reset();
    // expanding the top level item
    view->expand(newModel->index(0, 0, QModelIndex()));
}

void GenericObjectInspector::createWindow(const char *window, const char *geometry)
{
    Inspector::createWindow(window, geometry);
}

void GenericObjectInspector::useWindow(QWidget *parent)
{
    Inspector::useWindow(parent);
}

void GenericObjectInspector::refresh()
{
    Inspector::refresh();

    CHK(Tcl_VarEval(interp, "GenericObjectInspector:refresh ", windowName, TCL_NULL));
}

} // namespace qtenv
} // namespace omnetpp

