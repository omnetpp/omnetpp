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
#include "genericobjecttreemodel.h"
#include "genericobjecttreenodes.h"
#include "highlighteritemdelegate.h"
#include "inspectorutil.h"
#include "qtenv.h"
#include <omnetpp/cfutureeventset.h>
#include <omnetpp/cmodule.h>
#include <QtWidgets/QLayout>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollBar>

#include <QtCore/QDebug>

#define emit

namespace omnetpp {
namespace qtenv {

void _dummy_for_objecttreeinspector() {}

class ObjectTreeInspectorFactory : public InspectorFactory
{
  public:
    ObjectTreeInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *object) override { return dynamic_cast<cSimulation *>(object) != nullptr; }
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
    model = new GenericObjectTreeModel(nullptr, GenericObjectTreeModel::Mode::CHILDREN, {}, this);

    view->setModel(model);
    view->setUniformRowHeights(true);
    view->setHeaderHidden(true);
    view->setAttribute(Qt::WA_MacShowFocusRect, false);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    auto delegate = new HighlighterItemDelegate(view);
    view->setItemDelegate(delegate);

    view->header()->setStretchLastSection(true);
    view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    parent->setMinimumSize(20, 20);

    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createContextMenu(QPoint)));
    connect(view, SIGNAL(clicked(QModelIndex)), this, SLOT(onClick(QModelIndex)));
    connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClick(QModelIndex)));


    // getting the data into any items newly brought into view
    connect(view, SIGNAL(expanded(QModelIndex)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(view, SIGNAL(collapsed(QModelIndex)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(view->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(view->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(gatherVisibleDataIfSafe()));
}

void ObjectTreeInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    refresh();
}

void ObjectTreeInspector::refresh()
{
    Inspector::refresh();

    cSimulation *simulation = dynamic_cast<cSimulation *>(object);
    std::vector<cObject*> roots;
    if (simulation != nullptr) {
        roots.push_back(simulation->getSystemModule());
        roots.push_back(simulation->getFES());
    }

    if (roots != model->getRootObjects()) {
        // the FES and Network are recreated on run restart or config change
        delete model;
        model = new GenericObjectTreeModel(roots, GenericObjectTreeModel::Mode::CHILDREN, {}, this);
        view->setModel(model);
    }

    model->refreshTreeStructure();

    gatherVisibleData();

    // because properly doing it is super slow
    view->dataChanged(QModelIndex(), QModelIndex());
    view->resizeColumnToContents(0); // and this is needed because of it
}

void ObjectTreeInspector::createContextMenu(QPoint pos)
{
    QModelIndex index = view->indexAt(pos);
    if (index.isValid()) {
        QVector<cObject *> objects;
        cObject *obj = model->getCObjectPointer(index);
        objects.push_back(obj);

        cObject *objToInspect = model->getCObjectPointerToInspect(index);
        if (objToInspect != nullptr && objToInspect != obj)
            objects.push_back(objToInspect);

        QMenu *menu = InspectorUtil::createInspectorContextMenu(objects, this);
        menu->exec(mapToGlobal(pos));
        delete menu;
    }
}

bool ObjectTreeInspector::gatherVisibleData()
{
    bool changed = false;

    QModelIndexList indices;

    QModelIndex topIndex = view->indexAt(view->rect().topLeft());
    QModelIndex bottomIndex = view->indexAt(view->rect().bottomLeft());

    for (QModelIndex i = topIndex; i != bottomIndex; i = view->indexBelow(i))
        indices.append(i);

    if (bottomIndex.isValid())
        indices.append(bottomIndex);

    for (auto i : indices) {
        TreeNode *node = static_cast<TreeNode *>(i.internalPointer());
        if (node->updateData()) { // gatherDataIfMissing()?
            // not doing it, super slow, see caller
            //emit dataChanged(i, i);
            changed = true;
        }
    }

    return changed;
}

bool ObjectTreeInspector::gatherVisibleDataIfSafe()
{
    bool changed = false;
    if (getQtenv()->inspectorsAreFresh()) {
        changed = gatherVisibleData();

        if (changed) {
            // because properly doing it is super slow
            view->dataChanged(QModelIndex(), QModelIndex());
            view->resizeColumnToContents(0); // and this is needed because of it
        }
    }

    return changed;
}

void ObjectTreeInspector::resizeEvent(QResizeEvent *event)
{
    Inspector::resizeEvent(event);
    gatherVisibleDataIfSafe();
}

void ObjectTreeInspector::onClick(QModelIndex index)
{
    if (index.isValid())
        emit selectionChanged(model->getCObjectPointer(index));
}

void ObjectTreeInspector::onDoubleClick(QModelIndex index)
{
    if (index.isValid())
        emit objectDoubleClicked(model->getCObjectPointerToInspect(index));
}

}  // namespace qtenv
}  // namespace omnetpp

