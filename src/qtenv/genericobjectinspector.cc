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
#include <QGridLayout>

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
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new GenericObjectInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(GenericObjectInspectorFactory);

GenericObjectInspector::GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    auto layout = new QGridLayout(this);
    treeView = new QTreeView(this);
    treeView->setHeaderHidden(true);
    layout->addWidget(treeView, 0, 0, 1, 1);
    layout->setMargin(0);
}

GenericObjectInspector::~GenericObjectInspector()
{
}

void GenericObjectInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);

    GenericObjectTreeModel *oldModel = dynamic_cast<GenericObjectTreeModel *>(treeView->model());
    GenericObjectTreeModel *newModel = new GenericObjectTreeModel(obj, treeView);
    treeView->setModel(newModel);
    delete oldModel;
    treeView->reset();
    // expanding the top level item
    treeView->expand(newModel->index(0, 0, QModelIndex()));
}

void GenericObjectInspector::refresh()
{
    Inspector::refresh();

    CHK(Tcl_VarEval(interp, "GenericObjectInspector:refresh ", windowName, TCL_NULL));
}

} // namespace qtenv
} // namespace omnetpp

