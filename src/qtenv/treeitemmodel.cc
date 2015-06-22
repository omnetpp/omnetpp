//==========================================================================
//  TREEITEMMODEL.CC - part of
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

#include "treeitemmodel.h"
#include "envir/visitor.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cfutureeventset.h"
#include "inspectorfactory.h"
#include "qtenv.h"
#include "tkutil.h"
#include "mainwindow.h"
#include <QIcon>
#include <QMainWindow>

#include <QDebug>

namespace omnetpp {
namespace qtenv {

TreeItemModel::TreeItemModel(QObject *parent) :
    QAbstractItemModel(parent),
    rootNode(nullptr)
{
}

void TreeItemModel::setRootObject(cObject *root)
{
    beginResetModel();
    if (root == nullptr) {
    }  // qDebug() << "TreeItemModel::setRootObject: root is null";}//TODO log: TreeItemModel::setRootObject: root is null
    rootNode = root;
    endResetModel();
}

QModelIndex TreeItemModel::index(int row, int, const QModelIndex& parent) const
{
    // qDebug() << "index" << row << 1;
    if (!rootNode)
        return QModelIndex();

    cObject *node = getObjectFromIndex(parent);

    return createIndex(row, 0, node);
}

cObject *TreeItemModel::getObjectFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return rootNode;

    cObject *parent = static_cast<cObject *>(index.internalPointer());
    cCollectChildrenVisitor visitor(parent);
    visitor.process(parent);
    cObject **objs = visitor.getArray();
    return visitor.getArraySize() > index.row() ? objs[index.row()] : nullptr;
}

QModelIndex TreeItemModel::parent(const QModelIndex& index) const
{
    // qDebug() << "parent";

    cObject *parent = index.isValid() ? static_cast<cObject *>(index.internalPointer()) :
        rootNode;
    if (!parent)
        return QModelIndex();
    cObject *grandParent = parent->getOwner();
    if (!grandParent)
        return QModelIndex();

    int row = findObjectInParent(parent, grandParent);
    if (row != -1)
        return createIndex(row, 0, grandParent);

    return QModelIndex();
}

int TreeItemModel::findObjectInParent(cObject *obj, cObject *parent) const
{
    cCollectChildrenVisitor visitor(parent);
    visitor.process(parent);
    cObject **objs = visitor.getArray();

    for (size_t i = 0; i < visitor.getArraySize(); ++i)
        if (objs[i] == obj)
            return i;


    return -1;
}

int TreeItemModel::rowCount(const QModelIndex& parent) const
{
    // qDebug() << "RowCount";
    cObject *node = getObjectFromIndex(parent);
    if (!node)
        return 0;
    cCountChildrenVisitor visitor(node);
    visitor.process(node);
    // qDebug() << visitor.getCount();
    return visitor.getCount();
}

QVariant TreeItemModel::data(const QModelIndex& index, int role) const
{
    // qDebug() << "data" << index;
    cObject *node = getObjectFromIndex(index);
    if (!node)
        return QVariant();

    if (role == Qt::DisplayRole) {
        long id = -1;
        if (dynamic_cast<cModule *>(node))
            id = dynamic_cast<cModule *>(node)->getId();
        else if (dynamic_cast<cMessage *>(node))
            id = dynamic_cast<cMessage *>(node)->getId();

        QString text = node->getFullName() + QString(" (") + getObjectShortTypeName(node) + ")"
            +(id >= 0 ? "(id=" + QString::number(id) + QString(")") : "");
        return QVariant(text);
    } else if (role == Qt::DecorationRole) {
        return QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(node)));
    } else if (role == Qt::UserRole) {
        // returning the raw cObject* - needed to preserve object tree node expansion state
        return QVariant::fromValue(node);
    }

    return QVariant();
}

bool TreeItemModel::hasChildren(const QModelIndex& parent) const
{
    cObject *parentNode = getObjectFromIndex(parent);
    if (!parentNode)
        return false;

    cHasChildrenVisitor visitor(parentNode);
    visitor.process(parentNode);
    return visitor.getResult();
}

QString TreeItemModel::getObjectIcon(cObject *object) const
{
    QString iconName;
    if (object == nullptr)
        iconName = "none_vs.png";
    else if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        iconName = "placeholder_vs.png";
    else if (dynamic_cast<cSimpleModule *>(object))
        iconName = "simple_vs.png";
    else if (dynamic_cast<cModule *>(object))
        iconName = "compound_vs.png";
    else if (dynamic_cast<cWatchBase *>(object))
        iconName = "cogwheel_vs.png";
    else if (dynamic_cast<cMessage *>(object))
        iconName = "message_vs.png";
    else if (dynamic_cast<cArray *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cQueue *>(object))
        iconName = "queue_vs.png";
    else if (dynamic_cast<cGate *>(object))
        iconName = "gate_vs.png";
    else if (dynamic_cast<cPar *>(object))
        iconName = "param_vs.png";
    else if (dynamic_cast<cChannel *>(object))
        iconName = "chan_vs.png";
    else if (dynamic_cast<cOutVector *>(object))
        iconName = "outvect_vs.png";
    else if (dynamic_cast<cStatistic *>(object))
        iconName = "stat_vs.png";
    else if (dynamic_cast<cFigure *>(object))
        iconName = "figure_vs.png";
    else if (dynamic_cast<cCanvas *>(object))
        iconName = "canvas_vs.png";
    else if (dynamic_cast<cSimulation *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cFutureEventSet *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cRegistrationList *>(object))
        iconName = "container_vs.png";
    else
        iconName = "cogwheel_vs.png";

    return iconName;
}

void TreeItemModel::getExpandedItems(QTreeView *view, QList<QVariant> &list, QModelIndex idx)
{
    if (view->isExpanded(idx)) {
        // adding the pointer of the current node to the list because it is expanded in the view
        list.append(idx.data(Qt::UserRole));
    }

    if (idx.isValid()) {
        // this is not the root node, so we iterate on all of its children
        for (int row = 0; row < rowCount(idx); ++row) {
            getExpandedItems(view, list, idx.child(row, 0));
        }
    } else {
        // this is the root node, so we index the model itself in the iteration
        for (int row = 0; row < rowCount(); ++row) {
            getExpandedItems(view, list, index(row, 0));
        }
    }
}

void TreeItemModel::expandItems(QTreeView *view, const QList<QVariant> &list, QModelIndex idx)
{
    if (list.contains(idx.data(Qt::UserRole))) {
        // expanding the node in the view because it is in the list
        view->expand(idx);
    }

    if (idx.isValid()) {
        // this is not the root node, so we iterate on all of its children
        for (int row = 0; row < rowCount(idx); ++row) {
            expandItems(view, list, idx.child(row, 0));
        }
    } else {
        // this is the root node, so we index the model itself in the iteration
        for (int row = 0; row < rowCount(); ++row) {
            expandItems(view, list, index(row, 0));
        }
    }
}

// fillInspectorContextMenu without insp
QMenu *TreeItemModel::getContextMenu(QModelIndex& index, QMainWindow *mainWindow)
{
    // TODO global contextmenurules
    QMenu *menu = new QMenu();

    cObject *object = getObjectFromIndex(index);
    const char *name = object->getFullName();

    // add inspector types supported by the object
    for (int type : supportedInspTypes(object)) {
        QString label;
        switch (type) {
            case INSP_DEFAULT:
                label = "Open Best View";
                break;

            case INSP_OBJECT:
                label = "Open Details";
                break;

            case INSP_GRAPHICAL:
                label = "Open Graphical View";
                break;

            case INSP_MODULEOUTPUT:
                label = "Open Component Log";
                break;
        }
        label += QString(" for '") + name + "'";
        QAction *action = menu->addAction(label, mainWindow, SLOT(onClickOpenInspector()));
        action->setData(QVariant::fromValue(ActionDataPair(object, type)));
    }

    // add "run until" menu items
    if (dynamic_cast<cSimpleModule *>(object) || dynamic_cast<cModule *>(object)) {
        menu->addSeparator();
        QAction *action = menu->addAction(QString("Run Until Next Event in Module '") + name + "'", mainWindow, SLOT(onClickRun()));
        action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_NORMAL)));
        action = menu->addAction(QString("Fast Run Until Next Event in Module '") + name + "'", mainWindow, SLOT(onClickRun()));
        action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_FAST)));
    }

    if (dynamic_cast<cMessage *>(object)) {
        menu->addSeparator();
        QAction *action = menu->addAction(QString("Run Until Delivery of Message '") + name + "'", mainWindow, SLOT(onClickRunMessage()));
        action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_NORMAL)));
        action = menu->addAction(QString("Fast Run Until Delivery of Message '") + name + "'", mainWindow, SLOT(onClickRunMessage()));
        action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_FAST)));
        action = menu->addAction(QString("Express Run Until Delivery of Message '") + name + "'", mainWindow, SLOT(onClickRunMessage()));
        menu->addSeparator();
        action = menu->addAction(QString("Exclude Messages Like '") + name + "' From Animation", mainWindow, SLOT(onClickExcludeMessage()));
        action->setData(QVariant::fromValue(object));
    }

    // add utilities menu
    menu->addSeparator();
    QMenu *subMenu = menu->addMenu(QString("Utilities for '") + name + "'");
    QAction *action = subMenu->addAction("Copy Pointer With Cast (for Debugger)", mainWindow, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, MainWindow::COPY_PTRWITHCAST)));
    action = subMenu->addAction("Copy Pointer Value (for Debugger)", mainWindow, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, MainWindow::COPY_PTR)));
    subMenu->addSeparator();
    action = subMenu->addAction("Copy Full Path", mainWindow, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, MainWindow::COPY_FULLPATH)));
    action = subMenu->addAction("Copy Name", mainWindow, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, MainWindow::COPY_FULLNAME)));
    action = subMenu->addAction("Copy Class Name", mainWindow, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, MainWindow::COPY_CLASSNAME)));

    return menu;
}

QVector<int> TreeItemModel::supportedInspTypes(cObject *object)
{
    using namespace qtenv;
    QVector<int> insp_types;
    cRegistrationList *a = inspectorfactories.getInstance();
    for (int i = 0; i < a->size(); i++) {
        InspectorFactory *ifc = static_cast<InspectorFactory *>(a->get(i));
        if (ifc->supportsObject(object)) {
            int k;
            for (k = 0; k < insp_types.size(); k++)
                if (insp_types[k] == ifc->getInspectorType())
                    break;

            if (k == insp_types.size())  // not yet in vector, insert
                insp_types.push_front(ifc->getInspectorType());
        }
    }

    return insp_types;
}

} // namespace qtenv
} // namespace omnetpp

