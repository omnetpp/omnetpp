//==========================================================================
//  GENERICOBJECTTREEMODEL.CC - part of
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

#include "genericobjecttreemodel.h"

#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <set>
#include "qtutil.h"
#include "envir/visitor.h"
#include "qtenv.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "omnetpp/cwatch.h"
#include "genericobjecttreenodes.h"

#define emit

namespace omnetpp {

using namespace common;

namespace qtenv {


void PropertyFilteredGenericObjectTreeModel::setRelevantProperty(const QString &relevantProperty)
{
    if (this->relevantProperty != relevantProperty) {
        this->relevantProperty = relevantProperty;
        invalidateFilter();
    }
}

bool PropertyFilteredGenericObjectTreeModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (relevantProperty.isEmpty() || !sourceParent.isValid())
        return true; // empty filter is pass-through, and always including the tree roots

    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    TreeNode *treeNode = static_cast<TreeNode *>(sourceIndex.internalPointer());

    return treeNode ? treeNode->matchesPropertyFilter(relevantProperty) : true;
}


GenericObjectTreeModel::GenericObjectTreeModel(cObject *object, Mode mode, const NodeModeOverrideMap& modeOverrides, QObject *parent)
    : GenericObjectTreeModel(std::vector<cObject*>{object}, mode, modeOverrides, parent)
{
    // nothing, delegating to other ctor
}

GenericObjectTreeModel::GenericObjectTreeModel(std::vector<cObject *> roots, Mode mode, const NodeModeOverrideMap& modeOverrides, QObject *parent)
    : QAbstractItemModel(parent), inspectorMode(mode), nodeModeOverrides(modeOverrides)
{
    for (int i = 0; i < (int)roots.size(); ++i) {
        cObject *root = roots[i];
        RootNode *rootNode = new RootNode(root, i, mode, nodeModeOverrides);
        rootNode->init();
        // Since the root node is always going to be expanded right at the beginning,
        // let's just go ahead and fill it with children and data to reduce flickering.
        rootNode->fill();
        rootNode->gatherDataIfMissing();

        rootNodes.push_back(rootNode);
    }
}

std::vector<cObject *> GenericObjectTreeModel::getRootObjects()
{
    std::vector<cObject *> result;
    for (RootNode *rootNode : rootNodes)
        result.push_back(rootNode->getCObjectPointer());
    return result;
}

QModelIndex GenericObjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        ASSERT(row < rootNodes.size());
        return createIndex(row, column, rootNodes[row]);
    }
    else {
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        if (parentNode && (parentNode->getCurrentChildCount() > row)) {
            return createIndex(row, column, parentNode->getChild(row));
        }
        return QModelIndex();
    }
}

QModelIndex GenericObjectTreeModel::parent(const QModelIndex& child) const
{
    ASSERT(child.model() == this);
    TreeNode *node = static_cast<TreeNode *>(child.internalPointer());
    // the "row" of the parent ModelIndex is its own index in its parent,
    // and not the index of this child in the parent ModelIndex
    return node->getParent()
            ? createIndex(node->getParent()->getIndexInParent(), 0, node->getParent())
            : QModelIndex();
}

bool GenericObjectTreeModel::hasChildren(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        // it is the root
        return !rootNodes.empty();
    }
    else {
        ASSERT(parent.model() == this);
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        return parentNode ? parentNode->getHasChildren() : false;
    }
}

int GenericObjectTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        // it is the root
        return rootNodes.size();
    }
    else {
        ASSERT(parent.model() == this);
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        int childCount = parentNode ? parentNode->getCurrentChildCount() : 0;
        return childCount;
    }
}

int GenericObjectTreeModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant GenericObjectTreeModel::data(const QModelIndex& index, int role) const
{
    ASSERT(index.model() == this);
    auto node = static_cast<TreeNode *>(index.internalPointer());

    if (!index.parent().isValid() && role == (int)DataRole::NODE_MODE_OVERRIDE)
        return inspectorMode == node->getMode() ? -1 : (int)node->getMode();

    // just delegating to the node pointed by the index
    QVariant data = node->getData(role);
    if (role == Qt::DisplayRole) {
        data = data.toString().replace("\n", "\\n");
    }
    return data;
}

bool GenericObjectTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    ASSERT(index.model() == this);
    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());
    ASSERT(node != nullptr);

    bool success;

    if (role == (int)DataRole::NODE_MODE_OVERRIDE) {
        if (value.toInt() == -1)
            unsetNodeMode(index);
        else
            setNodeMode(index, (Mode)value.toInt());
        success = true;
    }
    else
        success = node->setData(value, role);

    emit dataChanged(index, index); // it is acceptable here, as this doesn't happen often
    emit dataEdited(index);
    return success;
}

Qt::ItemFlags GenericObjectTreeModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags &= ~Qt::ItemIsEditable;

    if (!index.isValid())
        return flags;

    ASSERT(index.model() == this);
    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());

    ASSERT(node != nullptr);
    if (node->isEditable()) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool GenericObjectTreeModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return false;
    TreeNode *node = static_cast<TreeNode *>(parent.internalPointer());
    return !node->isFilled();
}

void GenericObjectTreeModel::fetchMore(const QModelIndex &parent)
{
    ASSERT(parent.model() == this);
    if (parent.isValid()) {
        TreeNode *node = static_cast<TreeNode *>(parent.internalPointer());
        int n = node->getPotentialChildCount();
        beginInsertRows(parent, 0, n - 1);
        node->fill();
        endInsertRows();
    }
}

void GenericObjectTreeModel::refreshTreeStructure()
{
    emit layoutAboutToBeChanged();

    for (int i = 0; i < rootNodes.size(); ++i)
        refreshNodeChildrenRec(index(i, 0, QModelIndex()));

    // It's important to restore the data into the root nodes so they can give an
    // accurate sizeHint. Since we have uniform item heights enabled, the height
    // of the first item (the first root) determines the height of all items.
    for (auto rootNode : rootNodes)
        rootNode->gatherDataIfMissing();

    // also, this will make Qt realize that some nodes have gained or lost children,
    // and the triangle to expand the item will be shown/hidden accordingly
    emit layoutChanged();
}

void GenericObjectTreeModel::refreshNodeChildrenRec(const QModelIndex &index)
{
    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());

    if (node->isFilled()) {
        refreshChildList(index);

        int n = node->getCurrentChildCount();
        for (int i = 0; i < n; ++i)
            refreshNodeChildrenRec(this->index(i, 0, index));
    }
}

// XXX this is not the most efficient... Maybe could add TreeNode::fillWith(vector)
// to just pass in the child list we have to create here?
void GenericObjectTreeModel::refreshChildList(const QModelIndex &index)
{
    ASSERT(index.model() == this);
    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());

    node->updatePotentialChildCount();

    auto oldChildren = node->getExistingChildren(); // we just borrow the list of pointers
    auto newChildren = node->makeChildren(); // we get ownership of some newly created nodes
    for (auto nc : newChildren)
        nc->restoreModeFromOverrides();
    // no need to call init on newChildren, because we don't use anything in them, just isSameAs

    bool same = oldChildren.size() == newChildren.size();

    for (size_t i = 0; same && (i < oldChildren.size()); ++i)
        if (!newChildren[i]->isSameAs(oldChildren[i]))
            same = false;

    // not else, 'same' might have changed
    if (!same) {
        if (node->isFilled()) {
            beginRemoveRows(index, 0, oldChildren.size()-1);
            node->unfill();
            endRemoveRows();
        }

        beginInsertRows(index, 0, newChildren.size()-1);
        ASSERT(!node->isFilled());
        node->fill();
        endInsertRows();
    }

    ASSERT((int)newChildren.size() == node->getPotentialChildCount());

    // the old children have already been deleted in unfill(), but the "new ones" still exist
    for (auto c : newChildren)
        delete c;
}

void GenericObjectTreeModel::setNodeMode(const QModelIndex &index, Mode mode)
{
    ASSERT(index.model() == this);
    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());

    emit layoutAboutToBeChanged();

    ASSERT(mode != Mode::PACKET);
    node->doSetMode(mode);

    refreshChildList(index);

    refreshNodeChildrenRec(index);

    node->gatherDataIfMissing();

    nodeModeOverrides[node->getNodeIdentifier().toStdString()] = mode;
    // also, this will make Qt realize that some nodes have gained or lost children,
    // and the triangle to expand the item will be shown/hidden accordingly
    emit layoutChanged();
}

void GenericObjectTreeModel::unsetNodeMode(const QModelIndex &index)
{
    ASSERT(index.model() == this);
    TreeNode *node = static_cast<TreeNode *>(index.internalPointer());

    emit layoutAboutToBeChanged();

    Mode parentMode = node->getParent() != nullptr ? node->getParent()->getMode() : inspectorMode;
    ASSERT(parentMode != Mode::PACKET);
    node->doSetMode(parentMode);

    refreshChildList(index);

    refreshNodeChildrenRec(index);

    node->gatherDataIfMissing();

    nodeModeOverrides.erase(node->getNodeIdentifier().toStdString());

    // also, this will make Qt realize that some nodes have gained or lost children,
    // and the triangle to expand the item will be shown/hidden accordingly
    emit layoutChanged();
}


cObject *GenericObjectTreeModel::getCObjectPointer(const QModelIndex &index)
{
    ASSERT(index.model() == this);
    auto node = static_cast<TreeNode *>(index.internalPointer());
    return node ? node->getCObjectPointer() : nullptr;
}

cObject *GenericObjectTreeModel::getCObjectPointerToInspect(const QModelIndex &index)
{
    ASSERT(index.model() == this);
    cObject *object = getCObjectPointer(index);
    if (auto w = dynamic_cast<cWatch_cObject *>(object))
        return w->getObjectPtr();
    if (auto w = dynamic_cast<cWatch_cObjectPtr *>(object))
        return w->getObjectPtr();
    return object;
}

GenericObjectTreeModel::~GenericObjectTreeModel()
{
    for (auto rootNode : rootNodes)
        delete rootNode;
}

}  // namespace qtenv
}  // namespace omnetpp
