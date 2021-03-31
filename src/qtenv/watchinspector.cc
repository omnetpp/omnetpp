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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cmath>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QBoxLayout>
#include "omnetpp/cwatch.h"
#include "omnetpp/cstlwatch.h"
#include "qtenv.h"
#include "inspectorfactory.h"
#include "watchinspector.h"

namespace omnetpp {
namespace qtenv {

void _dummy_for_watchinspector() {}

class WatchInspectorFactory : public InspectorFactory
{
  public:
    WatchInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override {
        // Return true if it's a watch for a simple type (int, double, string etc).
        // For structures, we prefer the normal GenericObjectInspector.
        // Currently we're prepared for cStdVectorWatcherBase.
        return dynamic_cast<cWatchBase *>(obj) && !dynamic_cast<cStdVectorWatcherBase *>(obj);
    }

    InspectorType getInspectorType() override { return INSP_OBJECT; }
    double getQualityAsDefault(cObject *object) override { return 2.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new WatchInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(WatchInspectorFactory);

WatchInspector::WatchInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    auto layout = new QHBoxLayout(this);
    layout->setMargin(2);
    layout->setSpacing(2);

    label = new QLabel(this);
    editor = new QLineEdit(this);

    layout->addWidget(label);
    layout->addWidget(editor);

    connect(editor, SIGNAL(returnPressed()), this, SLOT(commit()));
}

void WatchInspector::refresh()
{
    Inspector::refresh();

    cWatchBase *watch = static_cast<cWatchBase *>(object);

    editor->setEnabled(watch->supportsAssignment());
    editor->setText(watch ? watch->str().c_str() : "n/a");
    label->setText(watch ? (std::string(watch->getClassName())+" "+watch->getFullName()).c_str() : "n/a");
}

void WatchInspector::commit()
{
    cWatchBase *watch = static_cast<cWatchBase *>(object);
    if (watch->supportsAssignment()) {
        watch->assign(editor->text().toStdString().c_str());
        getQtenv()->callRefreshInspectors();
    }
    else
        QMessageBox::warning(this, "Warning", "This inspector doesn't support changing the value.", QMessageBox::Ok);
}

}  // namespace qtenv
}  // namespace omnetpp

