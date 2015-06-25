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
    TreeNode *parent;
    int indexInParent;
    bool filled = false;

    // subclasses should fill the children vector in this function
    // it is only called once per instance, in the first getChild or getChildCount call
    virtual void fill() = 0;

    std::vector<TreeNode *> children;

    cObject *object = nullptr;
    cClassDescriptor *desc;

    void addObjectChildren(); // can't use default
    void addObjectChildren(cObject *of);

public:
    TreeNode(TreeNode *parent, int indexInParent, cObject *object);
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

    virtual ~TreeNode();
};

class FieldNode : public TreeNode {
protected:
    int fieldIndex;
    void fill() override;
public:
    FieldNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex);

    QVariant data(int role) override;

    virtual bool isEditable() override;
    virtual bool setData(const QVariant &value, int role) override;
};

class FieldGroupNode : public TreeNode {
protected:
    std::string groupName;
    void fill() override;
public:
    FieldGroupNode(TreeNode *parent, int indexInParent, cObject *object, const std::string &groupName);
    QVariant data(int role) override;
};

class ArrayElementNode : public TreeNode {
protected:
    int fieldIndex;
    int arrayIndex;
    void fill() override;
public:
    ArrayElementNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex, int arrayIndex);
    QVariant data(int role) override;
};

class cObjectNode : public TreeNode {
protected:
    // if this is a child object, this index is the field index in the parent
    // otherwise it is -1
    int fieldIndex;
    void fill() override;
public:
    cObjectNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex);
    QVariant data(int role) override;
};

// ---- main model class implementation ----

GenericObjectTreeModel::GenericObjectTreeModel(cObject *object, QObject *parent) :
    QAbstractItemModel(parent),
    rootNode(new cObjectNode(nullptr, 0, object, -1)) {
}

QModelIndex GenericObjectTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return createIndex(0, 0, rootNode);
    } else {
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        if (parentNode) {
            if (parentNode->getChildCount() > row) {
                return createIndex(row, column, parentNode->getChild(row));
            }
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
        return 1;
    } else {
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        if (parentNode) {
            return parentNode->getChildCount();
        } else {
            return 0;
        }
    }
}

int GenericObjectTreeModel::columnCount(const QModelIndex &parent) const {
    return 1; // is is a tree, not a table
}

QVariant GenericObjectTreeModel::data(const QModelIndex &index, int role) const {
    // just delegating to the node pointed by the index
    return static_cast<TreeNode *>(index.internalPointer())->data(role);
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

GenericObjectTreeModel::~GenericObjectTreeModel() {
    delete rootNode;
}

// ---- helper class implementations ----

void TreeNode::addObjectChildren() {
    addObjectChildren(object);
}

void TreeNode::addObjectChildren(cObject *of) {
    cClassDescriptor *desc = of ? of->getDescriptor() : nullptr;
    if (!desc) {
        return;
    }

    std::set<std::string> groupNames;

    int childIndex = 0;
    for (int i = 0; i < desc->getFieldCount(); ++i) {
        const char *groupName = desc->getFieldProperty(i, "group");
        if (groupName) {
            groupNames.insert(groupName);
        } else {
            if (!desc->getFieldIsArray(i) && desc->getFieldIsCObject(i)) {
                children.push_back(new cObjectNode(this, childIndex++, (cObject*)desc->getFieldStructValuePointer(of, i, 0), i));
            } else {
                children.push_back(new FieldNode(this, childIndex++, of, i));
            }
        }
    }

    for (auto &name : groupNames) {
        children.push_back(new FieldGroupNode(this, childIndex++, of, name));
    }
}

TreeNode::TreeNode(TreeNode *parent, int indexInParent, cObject *object)
    : parent(parent), indexInParent(indexInParent), object(object), desc(object ? object->getDescriptor() : nullptr) {

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
    return desc->getFieldName(fieldIndex);
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


FieldNode::FieldNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex)
    : TreeNode(parent, indexInParent, object), fieldIndex(fieldIndex) {
}

void FieldNode::fill() {
    if (desc->getFieldIsArray(fieldIndex)) {
        int size = desc->getFieldArraySize(object, fieldIndex);
        for (int i = 0; i < size; ++i) {
            if (desc->getFieldIsArray(fieldIndex)) {
                children.push_back(new ArrayElementNode(this, i, object, fieldIndex, i));
            }
        }
    }
}

QVariant FieldNode::data(int role) {
    const char *name = desc->getFieldName(fieldIndex);
    const char *type = desc->getFieldTypeString(fieldIndex);

    std::string value;

    int size = desc->getFieldArraySize(object, fieldIndex);
    if (!desc->getFieldIsCompound(fieldIndex)) {
        value = desc->getFieldValueAsString(object, fieldIndex, 0);
    }

    if (type && !strcmp(type, "string")) {
        value = "'" + value + "'";
    }

    QString arraySize;
    QString equals(" = ");
    if (desc->getFieldIsArray(fieldIndex)) {
        arraySize = QString("[") + QVariant(size).toString() + "]";
        equals = "";
    }

    if (value.empty()) {
        equals = "";
    }

    switch (role) {
    case Qt::DisplayRole:
        return name + arraySize + equals + value.c_str() + (isEditable() ? " [...] " : "") + QString(" (") + type + ")";
    case Qt::UserRole:
        HighlightRange valueRange;
        valueRange.start = strlen(name) + arraySize.length() + equals.length();
        valueRange.length = value.length();
        return QVariant::fromValue(valueRange);
    default:
        return QVariant();
    }
}

bool FieldNode::isEditable() {
    return desc->getFieldIsEditable(fieldIndex);
}

bool FieldNode::setData(const QVariant &value, int role) {
    return desc->setFieldValueAsString(object, fieldIndex, 0, value.toString().toStdString().c_str());
}



FieldGroupNode::FieldGroupNode(TreeNode *parent, int indexInParent, cObject *object, const std::string &groupName)
    : TreeNode(parent, indexInParent, object), groupName(groupName) {

}

void FieldGroupNode::fill() {
    if (!desc) {
        return;
    }
    int childIndex = 0;
    for (int i = 0; i < desc->getFieldCount(); ++i) {
        const char *thisFieldGroup = desc->getFieldProperty(i, "group");
        if (thisFieldGroup && (thisFieldGroup == groupName)) {
            if (!desc->getFieldIsArray(i) && (desc->getFieldIsCObject(i))) {
                children.push_back(new cObjectNode(this, childIndex++, (cObject*)desc->getFieldStructValuePointer(object, i, 0), i));
            } else {
                children.push_back(new FieldNode(this, childIndex++, object, i));
            }
        }
    }
    filled = true;
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



void ArrayElementNode::fill() {
    if (!desc) {
        return;
    }

    if (desc->getFieldIsCompound(fieldIndex)) {
        addObjectChildren((cObject *)desc->getFieldStructValuePointer(object, fieldIndex, arrayIndex));
    }
}

ArrayElementNode::ArrayElementNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex, int arrayIndex)
    : TreeNode(parent, indexInParent, object), fieldIndex(fieldIndex), arrayIndex(arrayIndex) {

}

QVariant ArrayElementNode::data(int role) {
    switch (role) {
    case Qt::DisplayRole:
        return QString("[%1] %2").arg(arrayIndex).arg(object->info().c_str());
    case Qt::DecorationRole:
        return QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon((cObject*)desc->getFieldStructValuePointer(object, fieldIndex, arrayIndex))));
    default:
        return QVariant();
    }
}


cObjectNode::cObjectNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex)
    : TreeNode(parent, indexInParent, object), fieldIndex(fieldIndex) {
}

void cObjectNode::fill() {
    addObjectChildren();
}

QVariant cObjectNode::data(int role) {
    if (!desc) {
        if (role == Qt::DisplayRole) {
            return parent ? "NULL" : "no object selected";
        } else {
            return QVariant();
        }
    }

    QString fieldName;
    if (parent) {
        fieldName = parent->getFieldName(fieldIndex);
        if (fieldName.length() > 0)
            fieldName += " = ";
    }

    int nameField = desc->findField("name");
    const char *className = getObjectShortTypeName(object);

    QString fieldValue(desc->getFieldValueAsString(object, nameField, 0).c_str());

    QString infoString;
    std::string info = object->info();
    if (!info.empty()) {
        infoString += ": ";
        infoString += info.c_str();
    }

    switch (role) {
    case Qt::DisplayRole:
        return fieldName + fieldValue + infoString + " (" + className + ")";
    case Qt::UserRole:
        return parent
                ? QVariant::fromValue(HighlightRange{fieldName.length(), fieldValue.length() + infoString.length()})
                : QVariant();
    case Qt::DecorationRole:
        return QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(object)));
    default:
        return QVariant();
    }
}

} // namespace qtenv
} // namespace omnetpp

