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
public:
    TreeNode(TreeNode *parent, int indexInParent);
    TreeNode *getParent();
    int getIndexInParent();
    int getChildCount(); // will fill the children vector if needed
    TreeNode *getChild(int index); // will fill the children vector if needed

    virtual QString getFieldName(int fieldIndex);

    // the model delegates its data function here, this should behave the same way
    // if role is DisplayNode, it should return a string
    // if it is DecorationRole, it should optionally return an icon
    // if it is UserRole, it should optionally return a HighlightRange
    virtual QVariant data(int role) = 0;

    virtual ~TreeNode();
};

class FieldNode : public TreeNode {
protected:
    cObject *object;
    int fieldIndex;
    void fill() override;
public:
    FieldNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex);
    QVariant data(int role) override;
};

class FieldGroupNode : public TreeNode {
protected:
    cObject *object;
    std::string groupName;
    void fill() override;
public:
    FieldGroupNode(TreeNode *parent, int indexInParent, cObject *object, const std::string &groupName);
    QString getFieldName(int fieldIndex);
    QVariant data(int role) override;
};

class ArrayElementNode : public TreeNode {
protected:
    cObject *object;
    int fieldIndex;
    int arrayIndex;
    void fill() override;
public:
    ArrayElementNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex, int arrayIndex);
    QVariant data(int role) override;
};

class cObjectNode : public TreeNode {
protected:
    cObject *object;
    // if this is a child object, this index is the field index in the parent
    // otherwise it is -1
    int fieldIndex;
    cClassDescriptor *desc;
    void fill() override;
public:
    cObjectNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex);
    QString getFieldName(int fieldIndex);
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
    return static_cast<TreeNode*>(index.internalPointer())->data(role);
}

GenericObjectTreeModel::~GenericObjectTreeModel() {
    delete rootNode;
}

// ---- helper class implementations ----

TreeNode::TreeNode(TreeNode *parent, int indexInParent): parent(parent), indexInParent(indexInParent) {

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
    return "";
}

TreeNode::~TreeNode() {
    for (auto c : children) {
        delete c;
    }
}


FieldNode::FieldNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex)
    : TreeNode(parent, indexInParent), object(object), fieldIndex(fieldIndex) {
}

void FieldNode::fill() {
    cClassDescriptor *desc = object->getDescriptor();
    if (desc->getFieldIsArray(fieldIndex)) {
        int size = desc->getFieldArraySize(object, fieldIndex);
        for (int i = 0; i < size; ++i) {
            if (desc->getFieldIsArray(fieldIndex)) {
                children.push_back(new ArrayElementNode(this, i, object, fieldIndex, i));
            } else if (desc->getFieldIsCObject(fieldIndex)) {
                children.push_back(new cObjectNode(this, i, (cObject*)desc->getFieldStructValuePointer(object, fieldIndex, i), i));
            } else {
                children.push_back(new FieldNode(this, i, object, i));
            }
        }
    }
}

QVariant FieldNode::data(int role) {

    cClassDescriptor *desc = object->getDescriptor();

    const char *name = desc->getFieldName(fieldIndex);
    const char *type = desc->getFieldTypeString(fieldIndex);

    std::string value;

    int size = desc->getFieldArraySize(object, fieldIndex);
    if (!desc->getFieldIsCompound(fieldIndex)) {
        value = desc->getFieldValueAsString(object, fieldIndex, 0);
    }

    if (!strcmp(type, "string")) {
        value = "'" + value + "'";
    }
    QString arraySize;
    QString equals(" = ");
    if (desc->getFieldIsArray(fieldIndex)) {
        arraySize = QString("[") + QVariant(desc->getFieldArraySize(object, fieldIndex)).toString() + "]";
        equals = "";
    }

    if (value.empty()) {
        equals = "";
    }

    switch (role) {
    case Qt::DisplayRole:
        return name + arraySize + equals + value.c_str() + QString(" (") + type + ")";
    case Qt::UserRole:
        HighlightRange valueRange;
        valueRange.start = strlen(name) + arraySize.length() + equals.length();
        valueRange.length = value.length();
        return QVariant::fromValue(valueRange);
    default:
        return QVariant();
    }
}



FieldGroupNode::FieldGroupNode(TreeNode *parent, int indexInParent, cObject *object, const std::string &groupName)
    : TreeNode(parent, indexInParent), object(object), groupName(groupName) {

}

QString FieldGroupNode::getFieldName(int fieldIndex) {
    return object->getDescriptor()->getFieldName(fieldIndex);
}

void FieldGroupNode::fill() {
    if (!object) {
        return;
    }
    int childIndex = 0;
    cClassDescriptor *desc = object->getDescriptor();
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
        return QVariant::fromValue(HighlightRange {0, groupName.length()});
    default:
        return QVariant();
    };
}



void ArrayElementNode::fill() {
    cClassDescriptor *desc = object ? object->getDescriptor() : nullptr;
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
            if (desc->getFieldIsCObject(i)) {
                children.push_back(new cObjectNode(this, childIndex++, (cObject*)desc->getFieldStructValuePointer(object, i, arrayIndex), i));
            } else {
                children.push_back(new FieldNode(this, childIndex++, object, i));
            }
        }
    }

    for (auto &name : groupNames) {
        children.push_back(new FieldGroupNode(this, childIndex++, object, name));
    }




    /*

    cClassDescriptor *desc = object->getDescriptor();
    int n = desc->getFieldArraySize(object, fieldIndex);
    if (desc->getFieldIsCompound(fieldIndex)) {
        void *fieldElement = desc->getFieldStructValuePointer(object, fieldIndex, arrayIndex);
        qDebug() << "field element type:" << desc->getFieldTypeString(fieldIndex);
        cClassDescriptor *elementDesc = cClassDescriptor::getDescriptorFor((std::string("omnetpp::") + desc->getFieldTypeString(fieldIndex)).c_str());
        for (int i = 0; i < elementDesc->getFieldCount(); ++i) {
            children.push_back(new FieldNode(this, i, (cObject *)fieldElement, i));
        }
    }*/
}

ArrayElementNode::ArrayElementNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex, int arrayIndex)
    : TreeNode(parent, indexInParent), object(object), fieldIndex(fieldIndex), arrayIndex(arrayIndex) {

}

QVariant ArrayElementNode::data(int role)
{
    switch (role) {
    case Qt::DisplayRole:
        return QString("[%1] %2").arg(arrayIndex).arg(object->info().c_str());
    default:
        return QVariant();
    }
}


cObjectNode::cObjectNode(TreeNode *parent, int indexInParent, cObject *object, int fieldIndex)
    : TreeNode(parent, indexInParent), object(object), fieldIndex(fieldIndex),
      desc(object ? object->getDescriptor() : nullptr) {
}

QString cObjectNode::getFieldName(int fieldIndex) {
    return desc->getFieldName(fieldIndex);
}

void cObjectNode::fill() {
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
                children.push_back(new cObjectNode(this, childIndex++, (cObject*)desc->getFieldStructValuePointer(object, i, 0), i));
            } else {
                children.push_back(new FieldNode(this, childIndex++, object, i));
            }
        }
    }

    for (auto &name : groupNames) {
        children.push_back(new FieldGroupNode(this, childIndex++, object, name));
    }
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

    switch (role) {
    case Qt::DisplayRole:
        return fieldName + fieldValue + " (" + className + ")";
    case Qt::UserRole:
        return parent ? QVariant::fromValue(HighlightRange {fieldName.length(), fieldValue.length()}) : QVariant();
    case Qt::DecorationRole:
        return QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(object)));
    default:
        return QVariant();
    }
}

} // namespace qtenv
} // namespace omnetpp

