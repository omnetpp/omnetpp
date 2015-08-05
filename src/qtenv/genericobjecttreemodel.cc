//==========================================================================
//  GENERICOBJECTTREEMODEL.CC - part of
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

#include "genericobjecttreemodel.h"

#include <QDebug>
#include <set>
#include "qtutil.h"

namespace omnetpp {
namespace qtenv {

// ---- helper class declarations ----
// these are only used internally, it would be
// unnecessary to declare them in the header

// this is the base class of the tree model
// each instance of this will be seen as a
// node in the treeview.
// uses transparent lazy loading of children
class TreeNode {
protected:
    // these make up the tree structure of the model
    TreeNode *parent = nullptr;
    int indexInParent = 0;
    std::vector<TreeNode *> children;

    bool filled = false;
    // subclasses should fill the children vector in this function
    // it is only called once per instance, in the first getChild or getChildCount call
    virtual void fill() = 0;

    // these are nullptrs only in the root node
    void *containingObject = nullptr; // may or may not be a cObject, so we need the descriptor for it
    cClassDescriptor *containingDesc = nullptr;

    void addObjectChildren(void *of, cClassDescriptor *desc);

    static cClassDescriptor *getDescriptorForField(void *object, cClassDescriptor *desc, int fieldIndex, int arrayIndex = 0);

public:
    TreeNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc);
    TreeNode *getParent();
    int getIndexInParent();
    int getChildCount(); // will fill the children vector if needed
    TreeNode *getChild(int index); // will fill the children vector if needed
    QString getFieldName(int fieldIndex);

    // the model delegates its data function here, this should behave the same way
    // if role is DisplayNode, it should return a string
    // if it is DecorationRole, it should optionally return an icon
    // if it is UserRole, it should optionally return a HighlightRange
    virtual QVariant data(int role) = 0;

    virtual bool isEditable();
    virtual bool setData(const QVariant &value, int role);

    // only used to save and restore the expansion state in the views
    virtual QString getNodeIdentifier() = 0;

    virtual ~TreeNode();
};

class FieldNode : public TreeNode {
protected:
    // which field are we in the container. for the root it is -1
    int fieldIndex = -1;

    void *object = nullptr;
    cClassDescriptor *desc = nullptr;

    void fill() override;
public:
    // only use this to create the root node for the model!
    explicit FieldNode(cObject *rootObject);
    FieldNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex);

    QVariant data(int role) override;

    virtual bool isEditable() override;
    virtual bool setData(const QVariant &value, int role) override;

    QString getNodeIdentifier() override;
};

class FieldGroupNode : public TreeNode {
protected:
    std::string groupName;
    void fill() override;
public:
    FieldGroupNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, const std::string &groupName);
    QVariant data(int role) override;
    QString getNodeIdentifier() override;
};

class ArrayElementNode : public TreeNode {
protected:
    int fieldIndex;
    int arrayIndex;
    void fill() override;
public:
    ArrayElementNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex, int arrayIndex);
    QVariant data(int role) override;

    bool isEditable() override;
    virtual bool setData(const QVariant &value, int role) override;
    QString getNodeIdentifier() override;
};

// ---- main model class implementation ----

QSet<QString> GenericObjectTreeModel::getExpandedNodesIn(QTreeView *view, const QModelIndex &index) {
    QSet<QString> result;
    if (view->isExpanded(index)) {
        result.insert(static_cast<TreeNode *>(index.internalPointer())->getNodeIdentifier());
        int numChildren = rowCount(index);
        for (int i = 0; i < numChildren; ++i) {
            result.unite(getExpandedNodesIn(view, index.child(i, 0)));
        }
    }
    return result;
}

void GenericObjectTreeModel::expandNodesIn(QTreeView *view, const QSet<QString> &ids, const QModelIndex &index) {
    if (ids.contains(static_cast<TreeNode *>(index.internalPointer())->getNodeIdentifier())) {
        view->expand(index);
        int numChildren = rowCount(index);
        for (int i = 0; i < numChildren; ++i) {
            expandNodesIn(view, ids, index.child(i, 0));
        }
    }
}

GenericObjectTreeModel::GenericObjectTreeModel(cObject *object, QObject *parent) :
    QAbstractItemModel(parent),
    rootNode(new FieldNode(object)) {
}

QModelIndex GenericObjectTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return createIndex(0, 0, rootNode);
    } else {
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        if (parentNode && (parentNode->getChildCount() > row)) {
            return createIndex(row, column, parentNode->getChild(row));
        }
        return QModelIndex();
    }
}

QModelIndex GenericObjectTreeModel::parent(const QModelIndex &child) const {
    TreeNode *node = static_cast<TreeNode *>(child.internalPointer());
    return node->getParent() ? createIndex(node->getIndexInParent(), 0, node->getParent()) : QModelIndex();
}

int GenericObjectTreeModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        // it is the root, and we have one of it
        return 1;
    } else {
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        return parentNode ? parentNode->getChildCount() : 0;
    }
}

int GenericObjectTreeModel::columnCount(const QModelIndex &parent) const {
    return 1; // is is a tree, not a table
}

QVariant GenericObjectTreeModel::data(const QModelIndex &index, int role) const {
    // just delegating to the node pointed by the index
    QVariant data = static_cast<TreeNode *>(index.internalPointer())->data(role);
    if (role == Qt::DisplayRole) {
        data = data.toString().replace("\n", "\\n");
    }
    return data;
}

bool GenericObjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    bool success = static_cast<TreeNode *>(index.internalPointer())->setData(value, role);
    dataChanged(index, index);
    return success;
}

Qt::ItemFlags GenericObjectTreeModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags &= ~Qt::ItemIsEditable;
    if (static_cast<TreeNode *>(index.internalPointer())->isEditable()) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

QSet<QString> GenericObjectTreeModel::getExpandedNodesIn(QTreeView *view) {
    return getExpandedNodesIn(view, index(0, 0, QModelIndex()));
}

void GenericObjectTreeModel::expandNodesIn(QTreeView *view, const QSet<QString> &ids) {
    expandNodesIn(view, ids, index(0, 0, QModelIndex()));
}

cObject *GenericObjectTreeModel::getCObjectPointer(QModelIndex index)
{
    auto node = static_cast<TreeNode *>(index.internalPointer());
    // FIXME TODO - extract the object pointer from the node to inspect
    // return node->containingObject;
    return nullptr;
}

GenericObjectTreeModel::~GenericObjectTreeModel() {
    delete rootNode;
}

// ---- helper class implementations ----

// the ArrayElementNode contains the children of the element itself
// but the object pointer in those nodes point to the object that
// contains the array itself, so we need this parameter
void TreeNode::addObjectChildren(void *of, cClassDescriptor *desc) {
    if (!of || !desc) {
        return;
    }

    std::set<std::string> groupNames;

    int childIndex = 0;
    for (int i = 0; i < desc->getFieldCount(); ++i) {
        const char *groupName = desc->getFieldProperty(i, "group");
        if (groupName) {
            groupNames.insert(groupName);
        } else {
            children.push_back(new FieldNode(this, childIndex++, of, desc, i));
        }
    }

    for (auto &name : groupNames) {
        children.push_back(new FieldGroupNode(this, childIndex++, of, desc, name));
    }
}

cClassDescriptor *TreeNode::getDescriptorForField(void *object, cClassDescriptor *desc, int fieldIndex, int arrayIndex) {
    cClassDescriptor *fieldDesc = cClassDescriptor::getDescriptorFor(desc->getFieldStructName(fieldIndex));

    void *fieldPtr = desc->getFieldStructValuePointer(object, fieldIndex, arrayIndex);

    // we can ask the field object itself for a better (more specialized) descriptor
    if (fieldPtr && desc->getFieldIsCObject(fieldIndex)) {
        fieldDesc = (static_cast<cObject *>(fieldPtr))->getDescriptor();
    }

    return fieldDesc;
}


TreeNode::TreeNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc)
    : parent(parent), indexInParent(indexInParent), containingObject(contObject), containingDesc(contDesc) {

}

TreeNode *TreeNode::getParent() {
    return parent;
}

int TreeNode::getIndexInParent() {
    return indexInParent;
}

int TreeNode::getChildCount() {
    if (!filled) {
        fill();
        filled = true;
    }
    return children.size();
}

TreeNode *TreeNode::getChild(int index) {
    if (!filled) {
        fill();
        filled = true;
    }
    return children[index];
}

QString TreeNode::getFieldName(int fieldIndex) {
    return containingDesc->getFieldName(fieldIndex);
}

bool TreeNode::isEditable() {
    return false;
}

bool TreeNode::setData(const QVariant &value, int role) {
    return false;
}

TreeNode::~TreeNode() {
    for (auto c : children) {
        delete c;
    }
}


FieldNode::FieldNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex)
    : TreeNode(parent, indexInParent, contObject, contDesc), fieldIndex(fieldIndex) {
    if (!contDesc->getFieldIsArray(fieldIndex) && contDesc->getFieldIsCompound(fieldIndex)) {
        object = contDesc->getFieldStructValuePointer(contObject, fieldIndex, 0);
        desc = getDescriptorForField(contObject, contDesc, fieldIndex);
    }
}

FieldNode::FieldNode(cObject *rootObject):
    TreeNode(nullptr, 0, nullptr, nullptr) {
    object = rootObject;
    desc = rootObject ? rootObject->getDescriptor() : nullptr;
}

void FieldNode::fill() {
    if (containingObject && containingDesc && containingDesc->getFieldIsArray(fieldIndex)) {
        int size = containingDesc->getFieldArraySize(containingObject, fieldIndex);
        for (int i = 0; i < size; ++i) {
            children.push_back(new ArrayElementNode(this, children.size(), containingObject, containingDesc, fieldIndex, i));
        }
    } else {
        addObjectChildren(object, desc);
    }
}

QVariant FieldNode::data(int role) {
    cObject *objectCasted = nullptr;

    // if no containingDesc, then this the root, which must be cObject
    if (!containingDesc || containingDesc->getFieldIsCObject(fieldIndex)) {
        objectCasted = static_cast<cObject *>(object);
    }

    if ((role == Qt::DecorationRole) && objectCasted) {
        return object ? QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(objectCasted))) : QVariant();
    }

    if (!containingDesc) {
        // this is the root object, so it can't have "field name", and it must be a cObject
        switch (role) {
        case Qt::ToolTipRole:
        case Qt::DisplayRole: {
            QString value = object
                             ? QString(objectCasted->getFullName())
                                + " (" + getObjectShortTypeName(objectCasted) + ")"
                             : "no object selected";
            return value;
        }
        default:
            return QVariant();
        }
    }

    // the rest is for the regular, non-root nodes

    bool isCObject = containingDesc->getFieldIsCObject(fieldIndex);
    bool isArray = containingDesc->getFieldIsArray(fieldIndex);

    QString fieldName = containingDesc->getFieldName(fieldIndex);
    QString objectClassName = objectCasted ? (QString(" (") + objectCasted->getClassName() + ")") : "";
    QString objectName = objectCasted ? QString(" ") + objectCasted->getFullName() : "";
    QString arraySize;
    QString fieldValue;
    QString objectInfo;
    QString equals = " = ";
    QString editable = isEditable() ? " [...] " : "";
    QString fieldType = containingDesc->getFieldTypeString(fieldIndex);

    // the name can be overridden by a label property
    const char *label = containingDesc->getFieldProperty(fieldIndex, "label");
    if (label) {
        fieldName = label;
    }

    // it is a simple value (not an array, but may be compound - like color or transform)
    if (!isArray && !isCObject) {
        fieldValue = containingDesc->getFieldValueAsString(containingObject, fieldIndex, 0).c_str();
    }

    if (!isArray && isCObject) {
        if (objectCasted) {
            objectInfo = objectCasted->info().c_str();
            if (objectInfo.length() > 0) {
                objectInfo = QString(": ") + objectInfo;
            }
        } else {
            fieldValue = "nullptr";
        }
    }

    if (isArray) {
        arraySize = QString("[") + QVariant::fromValue(containingDesc->getFieldArraySize(containingObject, fieldIndex)).toString() + "]";
        equals = "";
    }

    if ((role == Qt::DisplayRole || role == Qt::UserRole) && fieldType == "string") {
        // the apostrophes have to be there when showing the value and when calculating the highlight range
        // but not in the editor or the tooltip
        fieldValue = "'" + fieldValue + "'";
    }

    switch (role) {
    case Qt::EditRole:
        return fieldValue;
    case Qt::ToolTipRole:
        return ((fieldValue.contains("\n") || fieldValue.length() > 50)) ? fieldValue : QVariant();
    case Qt::DisplayRole:
        return fieldName + arraySize + equals + fieldValue + objectClassName + objectName + objectInfo + editable + " (" + fieldType + ")";
    case Qt::UserRole:
        return QVariant::fromValue(HighlightRange{fieldName.length() + equals.length(), objectClassName.length() + objectName.length() + fieldValue.length() + objectInfo.length()});
    default:
        return QVariant();
    }
}

bool FieldNode::isEditable() {
    return containingDesc // editing of arrays is handled in the elements themselves
            ? (!containingDesc->getFieldIsArray(fieldIndex) && containingDesc->getFieldIsEditable(fieldIndex))
            : false;
}

bool FieldNode::setData(const QVariant &value, int role) {
    return containingDesc
            ? containingDesc->setFieldValueAsString(containingObject, fieldIndex, 0,
                                                    value.toString().toStdString().c_str())
            : false;
}

QString FieldNode::getNodeIdentifier() {
    return (parent ? parent->getNodeIdentifier() + "|" : "")
            + QString("%1:%2").arg(voidPtrToStr(containingObject)).arg(fieldIndex);
}


FieldGroupNode::FieldGroupNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, const std::string &groupName)
    : TreeNode(parent, indexInParent, contObject, contDesc), groupName(groupName) {

}

void FieldGroupNode::fill() {
    if (!containingObject || !containingDesc) {
        return;
    }
    for (int i = 0; i < containingDesc->getFieldCount(); ++i) {
        const char *thisFieldGroup = containingDesc->getFieldProperty(i, "group");
        if (thisFieldGroup && (thisFieldGroup == groupName)) {
            children.push_back(new FieldNode(this, children.size(), containingObject, containingDesc, i));
        }
    }
}

QVariant FieldGroupNode::data(int role) {
    switch (role) {
    case Qt::DisplayRole:
        return groupName.c_str();
    case Qt::UserRole:
        return QVariant::fromValue(HighlightRange {0, (int)groupName.length()});
    default:
        return QVariant();
    };
}

QString FieldGroupNode::getNodeIdentifier() {
    return (parent ? parent->getNodeIdentifier() + "|" : "")
            + QString("%1(%2)").arg(voidPtrToStr(containingObject)).arg(groupName.c_str());
}



void ArrayElementNode::fill() {
    if (containingDesc->getFieldIsCompound(fieldIndex)) {
        cClassDescriptor *fieldDesc = getDescriptorForField(containingObject, containingDesc, fieldIndex, arrayIndex);
        addObjectChildren(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex), fieldDesc);
    }
}

ArrayElementNode::ArrayElementNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex, int arrayIndex)
    : TreeNode(parent, indexInParent, contObject, contDesc), fieldIndex(fieldIndex), arrayIndex(arrayIndex) {
}

QVariant ArrayElementNode::data(int role) {
    QString indexEquals = QString("[%1] = ").arg(arrayIndex);
    QString valueInfo = "";
    cObject *fieldPointer = nullptr;

    if (containingDesc->getFieldIsCObject(fieldIndex)) {
        fieldPointer = static_cast<cObject *>(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex));

        valueInfo += (fieldPointer
                        ? QString("(%1) %2")
                          .arg(fieldPointer->getClassName())
                          .arg(fieldPointer->getFullName())
                         : "nullptr");

        std::string info = fieldPointer ? fieldPointer->info() : "";
        if (!info.empty()) {
            valueInfo += ": ";
            valueInfo += info.c_str();
        }
    } else {
        valueInfo = containingDesc->getFieldValueAsString(containingObject, fieldIndex, arrayIndex).c_str();
    }

    switch (role) {
    case Qt::DisplayRole:
        return indexEquals + valueInfo;
    case Qt::DecorationRole:
        return fieldPointer ? QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(fieldPointer))) : QVariant();
    case Qt::EditRole:
        return valueInfo;
    case Qt::UserRole:
        return QVariant::fromValue(HighlightRange{indexEquals.length(), valueInfo.length()});
    default:
        return QVariant();
    }
}

bool ArrayElementNode::isEditable() {
    return containingDesc->getFieldIsEditable(fieldIndex);
}

bool ArrayElementNode::setData(const QVariant &value, int role) {
    return containingDesc->setFieldValueAsString(containingObject, fieldIndex, arrayIndex, value.toString().toStdString().c_str());
}

QString ArrayElementNode::getNodeIdentifier() {
    return (parent ? parent->getNodeIdentifier() + "|" : "")
            + QString("%1:%2[%3]").arg(voidPtrToStr(containingObject)).arg(fieldIndex).arg(arrayIndex);
}

} // namespace qtenv
} // namespace omnetpp

