//==========================================================================
//  TREEITEMMODEL.CC - part of
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
#include "qtutil.h"
#include "inspectorutil.h"
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

    for (int i = 0; i < visitor.getArraySize(); ++i)
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

    switch (role) {
        case Qt::DisplayRole: {
            long id = getObjectId(node);
            return node->getFullName() + QString(" (") + getObjectShortTypeName(node) + ")"
                    + (id >= 0 ? " id=" + QString::number(id) : "");
        }
        case Qt::DecorationRole:
            return getObjectIcon(node);
        case Qt::UserRole:
            // returning the raw cObject* - needed to preserve object tree node expansion state
            return QVariant::fromValue(node);
        case Qt::ToolTipRole: {
            auto info = node->str();
            return QString("(") + getObjectShortTypeName(node) + ") " + node->getFullName()
                    + (info.empty() ? "" : (", " + info).c_str());
        }
        default:
            return QVariant();
    }
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

void TreeItemModel::getExpandedItems(QTreeView *view, QList<QVariant>& list, QModelIndex idx)
{
    if (!idx.isValid()) {
        // the root index must always be added
        list.append(idx.data(Qt::UserRole));
        // this is the root node, so we index the model itself in the iteration
        for (int row = 0; row < rowCount(); ++row)
            getExpandedItems(view, list, index(row, 0));
    }
    else
        if (view->isExpanded(idx)) {
            // adding the pointer of the current node to the list because it is expanded in the view
            list.append(idx.data(Qt::UserRole));

            // this is not the root node, so we iterate on all of its children
            for (int row = 0; row < rowCount(idx); ++row)
                getExpandedItems(view, list, idx.child(row, 0));
        }
}

void TreeItemModel::expandItems(QTreeView *view, const QList<QVariant>& list, QModelIndex idx)
{
    if (list.contains(idx.data(Qt::UserRole))) {
        // expanding the node in the view because it is in the list
        view->expand(idx);

        if (idx.isValid()) {
            // this is not the root node, so we iterate on all of its children
            for (int row = 0; row < rowCount(idx); ++row) {
                expandItems(view, list, idx.child(row, 0));
            }
        }
        else {
            // this is the root node, so we index the model itself in the iteration
            for (int row = 0; row < rowCount(); ++row) {
                expandItems(view, list, index(row, 0));
            }
        }
    }
}

}  // namespace qtenv
}  // namespace omnetpp

