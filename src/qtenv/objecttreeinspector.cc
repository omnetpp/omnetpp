//==========================================================================
//  OBJECTTREEINSPECTOR.CC - part of
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

#include "objecttreeinspector.h"
#include "inspectorfactory.h"
#include "treeitemmodel.h"
#include "inspectorutil.h"
#include <QLayout>
#include <QTreeView>
#include <QHeaderView>

#include <QDebug>

#define emit

namespace omnetpp {
namespace qtenv {

void _dummy_for_objecttreeinspector() {}

class ObjectTreeInspectorFactory : public InspectorFactory
{
  public:
    ObjectTreeInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *) override { return false; }
    InspectorType getInspectorType() override { return INSP_OBJECTTREE; }
    double getQualityAsDefault(cObject *) override { return 0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new ObjectTreeInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(ObjectTreeInspectorFactory);

ObjectTreeInspector::ObjectTreeInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    QGridLayout *layout = new QGridLayout(this);
    view = new QTreeView();
    layout->addWidget(view, 0, 0);
    layout->setMargin(0);
    model = new TreeItemModel(parent);
    model->setRootObject(getSimulation());
    view->setModel(model);
    view->setHeaderHidden(true);
    view->setAttribute(Qt::WA_MacShowFocusRect, false);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    view->header()->setStretchLastSection(true);
    view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    parent->setMinimumSize(20, 20);

    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createContextMenu(QPoint)));
    connect(view, SIGNAL(clicked(QModelIndex)), this, SLOT(onClick(QModelIndex)));
    connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClick(QModelIndex)));
}

void ObjectTreeInspector::refresh()
{
    // this will hold the pointers to the expanded nodes in the view
    QList<QVariant> expandedItems;

    // getting the expanded nodes
    model->getExpandedItems(view, expandedItems);
    // updating the view to reflect the changed model
    view->reset();
    // restoring the expansion state
    model->expandItems(view, expandedItems);
    view->resizeColumnToContents(0);
}

void ObjectTreeInspector::createContextMenu(QPoint pos)
{
    QModelIndex index = view->indexAt(pos);
    if (index.isValid()) {
        QVector<cObject *> objects;
        objects.push_back(model->getObjectFromIndex(index));
        QMenu *menu = InspectorUtil::createInspectorContextMenu(objects, this);
        menu->exec(mapToGlobal(pos));
        delete menu;
    }
}

void ObjectTreeInspector::onClick(QModelIndex index)
{
    if (index.isValid()) {
        cObject *object = model->getObjectFromIndex(index);
        emit selectionChanged(object);
    }
}

void ObjectTreeInspector::onDoubleClick(QModelIndex index)
{
    if (index.isValid()) {
        cObject *object = model->getObjectFromIndex(index);
        emit objectDoubleClicked(object);
    }
}

}  // namespace qtenv
}  // namespace omnetpp

