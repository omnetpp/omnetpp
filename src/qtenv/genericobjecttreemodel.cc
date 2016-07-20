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
#include "envir/visitor.h"
#include "qtutil.h"

namespace omnetpp {
namespace qtenv {

//---- helper class declarations ----
// these are only used internally, it would be
// unnecessary to declare them in the header

// this is the base class of the tree model
// each instance of this will be seen as a
// node in the treeview.
// uses transparent lazy loading of children
class TreeNode
{
  protected:
    // these make up the tree structure of the model
    TreeNode *parent = nullptr;
    int indexInParent = 0;
    std::vector<TreeNode *> children;

    // used by cachedData(), which fills it using data() as needed
    std::map<int, QVariant> dataCache;

    bool filled = false;
    // subclasses should fill the children vector in this function
    // it is only called once per instance, in the first getChild or getChildCount call
    virtual void fill() = 0;
    // should override this to avoid filling the children vector if possible
    virtual bool hasChildrenImpl() = 0;

    using Mode = GenericObjectTreeModel::Mode;
    Mode mode;

    // these are nullptrs only in the root node
    void *containingObject = nullptr;  // may or may not be a cObject, so we need the descriptor for it
    cClassDescriptor *containingDesc = nullptr;

    bool objectHasChildren(void *obj, cClassDescriptor *desc, bool excludeInherited = false);
    void addObjectChildren(void *of, cClassDescriptor *desc, bool excludeInherited = false);

    static cClassDescriptor *getDescriptorForField(void *object, cClassDescriptor *desc, int fieldIndex, int arrayIndex = 0);
    static QVariant getDefaultObjectData(cObject *object, int role);

    // Only use this indirectly through cachedData()!
    // The model delegates its data function here, this should behave the same way.
    //  - if role is DisplayNode, it should return a string
    //  - if it is DecorationRole, it should optionally return an icon
    //  - if it is UserRole, it should optionally return a HighlightRange
    //  etc...
    virtual QVariant data(int role) = 0;

  public:
    TreeNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, Mode mode);
    TreeNode *getParent();
    int getIndexInParent();
    bool hasChildren();
    int getChildCount();  // will fill the children vector if needed
    TreeNode *getChild(int index);  // will fill the children vector if needed
    QString getFieldName(int fieldIndex);

    // see data() and dataCache
    QVariant cachedData(int role);

    virtual bool isEditable();
    virtual bool setData(const QVariant& value, int role);

    // only used to save and restore the expansion state in the views
    virtual QString getNodeIdentifier() = 0;

    // returns a nullptr where not applicable
    virtual cObject *getCObjectPointer();

    virtual ~TreeNode();
};

class SuperClassNode : public TreeNode
{
    cClassDescriptor *superDesc;

  protected:
    void fill();

  public:
    // superClassIndex: up in the inheritance chain, 0 is the most specialized class, and cObject is at the highest level
    SuperClassNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int superClassIndex, Mode mode);
    bool hasChildrenImpl() override;
    QVariant data(int role) override;
    QString getNodeIdentifier() override;
};

class ChildObjectNode : public TreeNode
{
    cObject *object;

  protected:
    void fill() override;

  public:
    ChildObjectNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, cObject *object, Mode mode);
    bool hasChildrenImpl() override;
    QVariant data(int role) override;
    QString getNodeIdentifier() override;
    cObject *getCObjectPointer() override;
};

class FieldNode : public TreeNode
{
  protected:
    int fieldIndex = 0;

    void *object = nullptr;
    cClassDescriptor *desc = nullptr;

    void fill() override;

  public:
    FieldNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex, Mode mode);

    bool hasChildrenImpl() override;
    QVariant data(int role) override;

    bool isEditable() override;
    bool setData(const QVariant& value, int role) override;
    cObject *getCObjectPointer() override;
    QString getNodeIdentifier() override;
};

class RootNode : public TreeNode
{
    cObject *object;

  protected:
    void fill() override;

  public:
    RootNode(cObject *object, Mode mode);
    bool hasChildrenImpl() override;
    QVariant data(int role) override;
    QString getNodeIdentifier() override;
    cObject *getCObjectPointer() override;
};

class FieldGroupNode : public TreeNode
{
  protected:
    std::string groupName;
    void fill() override;

  public:
    FieldGroupNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, const std::string& groupName, Mode mode);
    bool hasChildrenImpl() override;
    QVariant data(int role) override;
    QString getNodeIdentifier() override;
};

class ArrayElementNode : public TreeNode
{
  protected:
    int fieldIndex;
    int arrayIndex;
    void fill() override;

  public:
    ArrayElementNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex, int arrayIndex, Mode mode);
    bool hasChildrenImpl() override;

    QVariant data(int role) override;

    bool isEditable() override;
    virtual bool setData(const QVariant& value, int role) override;
    QString getNodeIdentifier() override;
    cObject *getCObjectPointer() override;
};

//---- main model class implementation ----

QSet<QString> GenericObjectTreeModel::getExpandedNodesIn(QTreeView *view, const QModelIndex& index)
{
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

void GenericObjectTreeModel::expandNodesIn(QTreeView *view, const QSet<QString>& ids, const QModelIndex& index)
{
    if (ids.contains(static_cast<TreeNode *>(index.internalPointer())->getNodeIdentifier())) {
        view->expand(index);
        int numChildren = rowCount(index);
        for (int i = 0; i < numChildren; ++i) {
            expandNodesIn(view, ids, index.child(i, 0));
        }
    }
}

GenericObjectTreeModel::GenericObjectTreeModel(cObject *object, Mode mode, QObject *parent)
    : QAbstractItemModel(parent), rootNode(new RootNode(object, mode))
{
}

QModelIndex GenericObjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return createIndex(0, 0, rootNode);
    }
    else {
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        if (parentNode && (parentNode->getChildCount() > row)) {
            return createIndex(row, column, parentNode->getChild(row));
        }
        return QModelIndex();
    }
}

QModelIndex GenericObjectTreeModel::parent(const QModelIndex& child) const
{
    TreeNode *node = static_cast<TreeNode *>(child.internalPointer());
    return node->getParent() ? createIndex(node->getIndexInParent(), 0, node->getParent()) : QModelIndex();
}

bool GenericObjectTreeModel::hasChildren(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        // it is the root, and we have one of it
        return rootNode;
    }
    else {
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        return parentNode ? parentNode->hasChildren() : false;
    }
}

int GenericObjectTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        // it is the root, and we have one of it
        return 1;
    }
    else {
        TreeNode *parentNode = static_cast<TreeNode *>(parent.internalPointer());
        int childCount = parentNode ? parentNode->getChildCount() : 0;
        ASSERT((childCount > 0) == hasChildren(parent));
        return childCount;
    }
}

int GenericObjectTreeModel::columnCount(const QModelIndex& parent) const
{
    return 1;  // is is a tree, not a table
}

QVariant GenericObjectTreeModel::data(const QModelIndex& index, int role) const
{
    // just delegating to the node pointed by the index
    QVariant data = static_cast<TreeNode *>(index.internalPointer())->cachedData(role);
    if (role == Qt::DisplayRole) {
        data = data.toString().replace("\n", "\\n");
    }
    return data;
}

bool GenericObjectTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool success = static_cast<TreeNode *>(index.internalPointer())->setData(value, role);
    dataChanged(index, index);
    return success;
}

Qt::ItemFlags GenericObjectTreeModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags &= ~Qt::ItemIsEditable;
    if (static_cast<TreeNode *>(index.internalPointer())->isEditable()) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

QSet<QString> GenericObjectTreeModel::getExpandedNodesIn(QTreeView *view)
{
    return getExpandedNodesIn(view, index(0, 0, QModelIndex()));
}

void GenericObjectTreeModel::expandNodesIn(QTreeView *view, const QSet<QString>& ids)
{
    expandNodesIn(view, ids, index(0, 0, QModelIndex()));
}

cObject *GenericObjectTreeModel::getCObjectPointer(QModelIndex index)
{
    auto node = static_cast<TreeNode *>(index.internalPointer());
    return node ? node->getCObjectPointer() : nullptr;
}

GenericObjectTreeModel::~GenericObjectTreeModel()
{
    delete rootNode;
}

//---- helper class implementations ----

// the ArrayElementNode contains the children of the element itself
// but the object pointer in those nodes point to the object that
// contains the array itself, so we need this parameter
bool TreeNode::objectHasChildren(void *obj, cClassDescriptor *desc, bool excludeInherited)
{
    if (!obj || !desc) {
        return false;
    }

    switch (mode) {
        case Mode::CHILDREN: {
            envir::cHasChildrenVisitor visitor((cObject *)obj);
            visitor.process((cObject *)obj);
            return visitor.getResult();
        }

        case Mode::INHERITANCE:
            if (!excludeInherited)
                return desc->getInheritanceChainLength() > 0;

        // if the condition fails, falling through, no return here
        default: {
            auto base = desc->getBaseClassDescriptor();
            return (excludeInherited && base ? base->getFieldCount() : 0) < desc->getFieldCount();
        }
    }
}

// the ArrayElementNode contains the children of the element itself
// but the object pointer in those nodes point to the object that
// contains the array itself, so we need this parameter
void TreeNode::addObjectChildren(void *of, cClassDescriptor *desc, bool excludeInherited)
{
    if (!of || !desc) {
        return;
    }

    switch (mode) {
        case Mode::CHILDREN: {
            envir::cCollectChildrenVisitor visitor((cObject *)of);
            visitor.process((cObject *)of);

            cObject **objs = visitor.getArray();
            for (int i = 0; i < visitor.getArraySize(); ++i) {
                children.push_back(new ChildObjectNode(this, i, of, desc, objs[i], mode));
            }
            break;
        }

        case Mode::INHERITANCE:
            if (!excludeInherited) {
                for (int i = 0; i < desc->getInheritanceChainLength(); i++)
                    children.push_back(new SuperClassNode(this, i, of, desc, i, mode));
                break;
            }

        // if the condition fails, falling through, no break here
        default: {  // GROUPED and FLAT have much in common
            int childIndex = 0;
            auto base = desc->getBaseClassDescriptor();

            std::set<std::string> groupNames;
            for (int i = excludeInherited && base ? base->getFieldCount() : 0; i < desc->getFieldCount(); ++i) {
                const char *groupName = desc->getFieldProperty(i, "group");
                if (mode == Mode::GROUPED && groupName)
                    groupNames.insert(groupName);
                else
                    children.push_back(new FieldNode(this, childIndex++, of, desc, i, mode));
            }

            if (mode == Mode::GROUPED)
                for (auto& name : groupNames)
                    children.push_back(new FieldGroupNode(this, childIndex++, of, desc, name, mode));

            else  // FLAT
                std::sort(children.begin(), children.end(), [](TreeNode *a, TreeNode *b) {
                    return a->data(Qt::DisplayRole).toString() < b->data(Qt::DisplayRole).toString();
                });
        }
    }
}

cClassDescriptor *TreeNode::getDescriptorForField(void *object, cClassDescriptor *desc, int fieldIndex, int arrayIndex)
{
    void *fieldPtr = desc->getFieldStructValuePointer(object, fieldIndex, arrayIndex);
    if (!fieldPtr)
        return nullptr;

    if (desc->getFieldIsCObject(fieldIndex)) {
        cObject *object = static_cast<cObject *>(fieldPtr);
        return object->getDescriptor();
    }

    if (desc->getFieldIsCompound(fieldIndex)) {
        const char *dynamicTypeName = desc->getFieldDynamicTypeString(object, fieldIndex, arrayIndex);
        if (dynamicTypeName)
            return cClassDescriptor::getDescriptorFor(dynamicTypeName);
    }

    return cClassDescriptor::getDescriptorFor(desc->getFieldStructName(fieldIndex));
}

QVariant TreeNode::getDefaultObjectData(cObject *object, int role)
{
    switch (role) {
        case Qt::DecorationRole:
            return object ? QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(object))) : QVariant();

        case Qt::ToolTipRole:
        case Qt::DisplayRole:
            return object ? QString(object->getFullName()) + " (" + getObjectShortTypeName(object) + ")" : "no object selected";

        default:
            return QVariant();
    }
}

TreeNode::TreeNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, Mode mode)
    : parent(parent), indexInParent(indexInParent), mode(mode),
    containingObject(contObject), containingDesc(contDesc)
{
}

TreeNode *TreeNode::getParent()
{
    return parent;
}

int TreeNode::getIndexInParent()
{
    return indexInParent;
}

bool TreeNode::hasChildren()
{
    if (filled) {
        return !children.empty();
    }
    else {
        bool result = hasChildrenImpl();
        // If the node was not filled before, but there aren't going to be
        // any children anyway, this node should be considered filled.
        if (!result)
            filled = true;
        return result;
    }
}

int TreeNode::getChildCount()
{
    if (!filled) {
        fill();
        filled = true;
    }
    return children.size();
}

TreeNode *TreeNode::getChild(int index)
{
    if (!filled) {
        fill();
        filled = true;
    }
    return children[index];
}

QString TreeNode::getFieldName(int fieldIndex)
{
    return containingDesc->getFieldName(fieldIndex);
}

QVariant TreeNode::cachedData(int role)
{
    return (dataCache.count(role) == 0)  // if not already in
           ? (dataCache[role] = data(role))  // add it
           : dataCache[role];  // else just return
}

bool TreeNode::isEditable()
{
    return false;
}

bool TreeNode::setData(const QVariant& value, int role)
{
    return false;
}

cObject *TreeNode::getCObjectPointer()
{
    return nullptr;
}

TreeNode::~TreeNode()
{
    for (auto c : children) {
        delete c;
    }
}

SuperClassNode::SuperClassNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int superClassIndex, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode)
{
    superDesc = containingDesc;
    for (int i = 0; i < superClassIndex; ++i)
        superDesc = superDesc->getBaseClassDescriptor();
}

bool SuperClassNode::hasChildrenImpl()
{
    return objectHasChildren(containingObject, superDesc, true);
}

void SuperClassNode::fill()
{
    addObjectChildren(containingObject, superDesc, true);
}

QVariant SuperClassNode::data(int role)
{
    switch (role) {
        case Qt::DisplayRole: return stripNamespace(superDesc->getName());
        case Qt::UserRole: return QVariant::fromValue(HighlightRange {0, (int)strlen(superDesc->getClassName())});
        default: return QVariant();
    };
}

QString SuperClassNode::getNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "")
           + QString("%1{%3}").arg(voidPtrToStr(containingDesc)).arg(superDesc->getClassName());
}

ChildObjectNode::ChildObjectNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, cObject *object, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode), object(object)
{
}

bool ChildObjectNode::hasChildrenImpl()
{
    return objectHasChildren(object, cClassDescriptor::getDescriptorFor(object));
}

QVariant ChildObjectNode::data(int role)
{
    QString defaultText = getDefaultObjectData(object, Qt::DisplayRole).value<QString>();
    QString infoText = object->info().c_str();
    switch (role) {
        case Qt::DisplayRole: return defaultText + (infoText.isEmpty() ? "" : (QString(" ") + infoText));
        case Qt::UserRole: return QVariant::fromValue(HighlightRange{defaultText.length(), infoText.isEmpty() ? 0 : infoText.length() + 1});
        default: return getDefaultObjectData(object, role);
    }
}

QString ChildObjectNode::getNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "")
           + QString("%1{%3}").arg(voidPtrToStr(containingDesc)).arg(object->getClassName());
}

cObject *ChildObjectNode::getCObjectPointer()
{
    return object;
}

void ChildObjectNode::fill()
{
    addObjectChildren(object, cClassDescriptor::getDescriptorFor(object));
}

FieldNode::FieldNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode), fieldIndex(fieldIndex)
{
    if (!contDesc->getFieldIsArray(fieldIndex) && contDesc->getFieldIsCompound(fieldIndex)) {
        object = contDesc->getFieldStructValuePointer(contObject, fieldIndex, 0);
        desc = getDescriptorForField(contObject, contDesc, fieldIndex);
    }
}

bool FieldNode::hasChildrenImpl()
{
    if (containingObject && containingDesc && containingDesc->getFieldIsArray(fieldIndex))
        return containingDesc->getFieldArraySize(containingObject, fieldIndex) > 0;
    else
        return objectHasChildren(object, desc);
}

void FieldNode::fill()
{
    if (containingObject && containingDesc && containingDesc->getFieldIsArray(fieldIndex)) {
        int size = containingDesc->getFieldArraySize(containingObject, fieldIndex);
        for (int i = 0; i < size; ++i) {
            children.push_back(new ArrayElementNode(this, children.size(), containingObject, containingDesc, fieldIndex, i, mode));
        }
    }
    else {
        addObjectChildren(object, desc);
    }
}

QVariant FieldNode::data(int role)
{
    cObject *objectCasted =
              (containingDesc && containingDesc->getFieldIsCObject(fieldIndex))
                ? static_cast<cObject *>(object)
                : nullptr;

    if ((role == Qt::DecorationRole) && objectCasted) {
        return object ? QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(objectCasted))) : QVariant();
    }

    // the rest is for the regular, non-root nodes

    bool isCObject = containingDesc->getFieldIsCObject(fieldIndex);
    bool isCompound = containingDesc->getFieldIsCompound(fieldIndex);
    bool isArray = containingDesc->getFieldIsArray(fieldIndex);

    QString fieldName = containingDesc->getFieldName(fieldIndex);
    QString objectClassName = objectCasted ? (QString(" (") + getObjectShortTypeName(objectCasted) + ")") : "";
    QString objectName = objectCasted ? QString(" ") + objectCasted->getFullName() : "";
    QString arraySize;
    QString fieldValue;
    QString objectInfo;
    QString equals = " = ";
    QString editable = isEditable() ? " [...] " : "";
    QString fieldType = containingDesc->getFieldTypeString(fieldIndex);

    if (isCompound && !isCObject && !isArray) {
        // Even if it's not a CObject, it can have a different dynamic type
        // than the declared static type, which we can get this way.
        const char *dynamicType = containingDesc->getFieldDynamicTypeString(containingObject, fieldIndex, 0);
        if (dynamicType && dynamicType[0])
            objectClassName = QString(" (") + stripNamespace(dynamicType) + ")";
    }

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
        }
        else {
            fieldValue = "nullptr";
        }
    }

    if (isArray)
        arraySize = QString("[") + QVariant::fromValue(containingDesc->getFieldArraySize(containingObject, fieldIndex)).toString() + "]";

    // the apostrophes have to be there when showing the value and when calculating the highlight range
    // but not in the editor or the tooltip
    if ((role == Qt::DisplayRole || role == Qt::UserRole) && fieldType == "string")
        fieldValue = "'" + fieldValue + "'";

    if (isArray || fieldValue.isEmpty())
        equals = "";  // no need to add an " = " in these cases

    switch (role) {
        case Qt::EditRole:
            return fieldValue;

        case Qt::ToolTipRole:
            return ((fieldValue.contains("\n") || fieldValue.length() > 50)) ? fieldValue : QVariant();

        case Qt::DisplayRole:
            return fieldName + arraySize + equals + fieldValue + objectClassName + objectName + objectInfo + editable + " (" + fieldType + ")";

        case Qt::UserRole:
            return QVariant::fromValue(HighlightRange { fieldName.length() + arraySize.length() + equals.length(), fieldValue.length() + objectClassName.length() + objectName.length() + objectInfo.length() });

        default:
            return QVariant();
    }
}

bool FieldNode::isEditable()
{
    return containingDesc  // editing of arrays is handled in the elements themselves
           ? (!containingDesc->getFieldIsArray(fieldIndex) && containingDesc->getFieldIsEditable(fieldIndex))
           : false;
}

bool FieldNode::setData(const QVariant& value, int role)
{
    return containingDesc
           ? containingDesc->setFieldValueAsString(containingObject, fieldIndex, 0,
            value.toString().toStdString().c_str())
           : false;
}

cObject *FieldNode::getCObjectPointer()
{
    return (fieldIndex == -1)  // either it is the root, so it must be cObject
           || (containingDesc  // or it is a node representing a single cObject
               && containingDesc->getFieldIsCObject(fieldIndex)
               && !containingDesc->getFieldIsArray(fieldIndex))
           ? (cObject *)object
           : nullptr;
}

QString FieldNode::getNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "")
           + QString("%1:%2").arg(voidPtrToStr(containingDesc)).arg(fieldIndex);
}

void RootNode::fill()
{
    if (object)
        addObjectChildren(object, cClassDescriptor::getDescriptorFor(object), false);
}

RootNode::RootNode(cObject *object, Mode mode)
    : TreeNode(nullptr, 0, nullptr, nullptr, mode), object(object)
{
}

bool RootNode::hasChildrenImpl()
{
    return object
        ? objectHasChildren(object, cClassDescriptor::getDescriptorFor(object), false)
        : false;
}

QVariant RootNode::data(int role)
{
    return (role == Qt::DisplayRole && object)
        ? QString(object->getFullPath().c_str()) + " (" + getObjectShortTypeName(object) + ")"
        : getDefaultObjectData(object, role);
}

QString RootNode::getNodeIdentifier()
{
    return voidPtrToStr(object ? object->getDescriptor() : nullptr);
}

cObject *RootNode::getCObjectPointer()
{
    return object;
}

FieldGroupNode::FieldGroupNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, const std::string& groupName, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode), groupName(groupName)
{
}

bool FieldGroupNode::hasChildrenImpl()
{
    if (!containingObject || !containingDesc)
        return false;
    for (int i = 0; i < containingDesc->getFieldCount(); ++i) {
        const char *thisFieldGroup = containingDesc->getFieldProperty(i, "group");
        if (thisFieldGroup && (thisFieldGroup == groupName))
            return true;
    }
    return false;
}

void FieldGroupNode::fill()
{
    if (!containingObject || !containingDesc) {
        return;
    }
    for (int i = 0; i < containingDesc->getFieldCount(); ++i) {
        const char *thisFieldGroup = containingDesc->getFieldProperty(i, "group");
        if (thisFieldGroup && (thisFieldGroup == groupName)) {
            children.push_back(new FieldNode(this, children.size(), containingObject, containingDesc, i, mode));
        }
    }
}

QVariant FieldGroupNode::data(int role)
{
    switch (role) {
        case Qt::DisplayRole: return groupName.c_str();
        case Qt::UserRole: return QVariant::fromValue(HighlightRange { 0, (int)groupName.length() });
        default: return QVariant();
    }
}

QString FieldGroupNode::getNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "")
            + QString("%1(%2)").arg(voidPtrToStr(containingDesc)).arg(groupName.c_str());
}

void ArrayElementNode::fill()
{
    if (containingDesc->getFieldIsCompound(fieldIndex)) {
        cClassDescriptor *fieldDesc = getDescriptorForField(containingObject, containingDesc, fieldIndex, arrayIndex);
        addObjectChildren(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex), fieldDesc);
    }
}

ArrayElementNode::ArrayElementNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex, int arrayIndex, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode), fieldIndex(fieldIndex), arrayIndex(arrayIndex)
{
}

bool ArrayElementNode::hasChildrenImpl()
{
    if (containingDesc->getFieldIsCompound(fieldIndex)) {
        cClassDescriptor *fieldDesc = getDescriptorForField(containingObject, containingDesc, fieldIndex, arrayIndex);
        return objectHasChildren(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex), fieldDesc);
    }
    return false;
}

QVariant ArrayElementNode::data(int role)
{
    QString indexEquals = QString("[%1] = ").arg(arrayIndex);
    QString valueInfo = "";
    cObject *fieldObjectPointer = nullptr;

    if (containingDesc->getFieldIsCObject(fieldIndex)) {
        fieldObjectPointer = static_cast<cObject *>(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex));

        valueInfo += (fieldObjectPointer
                        ? QString("(%1) %2")
                          .arg(getObjectShortTypeName(fieldObjectPointer))
                          .arg(fieldObjectPointer->getFullName())
                         : "NULL");

        std::string info = fieldObjectPointer ? fieldObjectPointer->info() : "";
        if (!info.empty()) {
            valueInfo += ": ";
            valueInfo += info.c_str();
        }
    }
    else if (containingDesc->getFieldIsCompound(fieldIndex)) {
        const char *fieldType = containingDesc->getFieldDynamicTypeString(containingObject, fieldIndex, arrayIndex);
        std::string fieldValue = containingDesc->getFieldValueAsString(containingObject, fieldIndex, arrayIndex);

        if (fieldType && fieldType[0])
            valueInfo += QString("(") + stripNamespace(fieldType) + ")";

        if (!fieldValue.empty()) {
            if (!valueInfo.isEmpty())
                valueInfo += " ";
            valueInfo += fieldValue.c_str();
        }
    }
    else {
        valueInfo = containingDesc->getFieldValueAsString(containingObject, fieldIndex, arrayIndex).c_str();
    }

    switch (role) {
        case Qt::DisplayRole:
            return indexEquals + valueInfo;

        case Qt::DecorationRole:
            return fieldObjectPointer ? QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(fieldObjectPointer))) : QVariant();

        case Qt::EditRole:
            return valueInfo;

        case Qt::UserRole:
            return QVariant::fromValue(HighlightRange { indexEquals.length(), valueInfo.length() });

        default:
            return QVariant();
    }
}

bool ArrayElementNode::isEditable()
{
    return containingDesc->getFieldIsEditable(fieldIndex);
}

bool ArrayElementNode::setData(const QVariant& value, int role)
{
    return containingDesc->setFieldValueAsString(containingObject, fieldIndex, arrayIndex, value.toString().toStdString().c_str());
}

QString ArrayElementNode::getNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "")
            + QString("%1:%2[%3]").arg(voidPtrToStr(containingObject)).arg(fieldIndex).arg(arrayIndex);
}

cObject *ArrayElementNode::getCObjectPointer()
{
    return containingDesc->getFieldIsCObject(fieldIndex)
            ? static_cast<cObject *>(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex))
            : nullptr;
}

}  // namespace qtenv
}  // namespace omnetpp

