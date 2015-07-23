//==========================================================================
//  TIMELINEINSPECTOR.CC - part of
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

#include "timelineinspector.h"
#include <QHBoxLayout>
#include <QMenu>
#include "timelinegraphicsview.h"
#include "inspectorutil.h"
#include "inspectorfactory.h"

#define emit

namespace omnetpp {
namespace qtenv {

class TimeLineInspectorFactory : public InspectorFactory
{
  public:
    TimeLineInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *) override { return false; }
    int getInspectorType() override { return INSP_MODULEOUTPUT; }
    double getQualityAsDefault(cObject *) override { return 0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new TimeLineInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(TimeLineInspectorFactory);

TimeLineInspector::TimeLineInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    QGridLayout *layout = new QGridLayout(this);
    timeLine = new TimeLineGraphicsView();
    timeLine->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    timeLine->setMaximumHeight(100); // TODO FIXME proper layouting
    timeLine->setScene(new QGraphicsScene());
    layout->addWidget(timeLine, 0, 0);
    layout->setMargin(0);

    connect(timeLine, SIGNAL(contextMenuRequested(QVector<cObject*>,QPoint)), this, SLOT(createContextMenu(QVector<cObject*>,QPoint)));
    connect(timeLine, SIGNAL(click(cObject*)), this, SLOT(setObjectToObjectInspector(cObject*)));
    connect(timeLine, SIGNAL(doubleClick(cObject*)), this, SLOT(openInspector(cObject*)));
}

void TimeLineInspector::createContextMenu(QVector<cObject*> objects, QPoint globalPos)
{
    if(objects.size())
    {
        QMenu *menu = InspectorUtil::createInspectorContextMenu(objects, this);
        menu->exec(globalPos);
        delete menu;
    }
    else
    {
        QMenu *menu = new QMenu();
        menu->addAction("Timeline Settings...", this, SLOT(runPreferencesDialog()));
        menu->exec(globalPos);
        delete menu;
    }
}

void TimeLineInspector::setObjectToObjectInspector(cObject* object)
{
    emit selectionChanged(object);
}

void TimeLineInspector::openInspector(cObject *object)
{
    Inspector::openInspector(object, INSP_OBJECT);
}

void TimeLineInspector::refresh()
{
    timeLine->rebuildScene();
}

void TimeLineInspector::runPreferencesDialog()
{
    InspectorUtil::preferencesDialog(InspectorUtil::TAB_FILTERING);
}

} // namespace qtenv
} // namespace omnetpp
