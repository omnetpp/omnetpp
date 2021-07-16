//==========================================================================
//  GENERICOBJECTTREENODES.CC - part of
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

#include "genericobjecttreenodes.h"

#include "common/stlutil.h"
#include "common/stringutil.h"
#include "qtutil.h"
#include "qtenv.h"  // for getQtenv
#include <QtCore/QElapsedTimer>
#include <set>
#include "envir/visitor.h"

namespace omnetpp {
namespace qtenv {

using common::opp_nulltoempty;
using common::contains;
using common::containsKey;

const std::vector<int> TreeNode::supportedDataRoles
    = { Qt::DisplayRole, Qt::DecorationRole, Qt::EditRole, Qt::ToolTipRole, Qt::UserRole };

void TreeNode::fill()
{
    ASSERT(!filled);
    ASSERT(children.empty());

    children = makeChildren();
    potentialChildCount = computeChildCount();

    ASSERT((int)children.size() == potentialChildCount);

    for (auto c : children) {
        c->restoreModeFromOverrides();
        c->init();
    }

    filled = true;
}

void TreeNode::unfill()
{
    ASSERT(filled);

    for (auto c : children)
        delete c;
    children.clear();

    data.clear();

    filled = false;
}

bool TreeNode::isSameAs(TreeNode *other)
{
    return mode == other->mode
            && containingObject == other->containingObject
            && containingDesc == other->containingDesc;
}

// the ArrayElementNode contains the children of the element itself
// but the object pointer in those nodes point to the object that
// contains the array itself, so we need this parameter
int TreeNode::computeObjectChildCount(any_ptr obj, cClassDescriptor *desc, Mode mode, bool excludeInherited)
{
    if (obj == nullptr || desc == nullptr)
        return 0;

    switch (mode) {
        case Mode::GROUPED: {
            std::set<std::string> groupNames;
            int count = 0;
            for (int i = 0; i < desc->getFieldCount(); ++i) {
                const char *thisFieldGroup = desc->getFieldProperty(i, "group");
                if (!thisFieldGroup)
                    ++count; // this is a field that is not in a group
                else
                    if (groupNames.find(thisFieldGroup) == groupNames.end()) {
                        ++count; // this is a new field group
                        groupNames.insert(thisFieldGroup);
                    }
            }
            return count;
        }

        case Mode::CHILDREN: {
            if (!obj.contains<cObject>())
                return 0;

            envir::cCountChildrenVisitor visitor(fromAnyPtr<cObject>(obj));

            try {
                visitor.process(fromAnyPtr<cObject>(obj));
            }
            catch (std::exception &e) {
                return 1; // the error marker
            }

            return visitor.getCount();
        }

        case Mode::INHERITANCE:
            if (!excludeInherited)
                return desc->getInheritanceChainLength();
            // no break (fall through)

        case Mode::FLAT: {
            auto base = desc->getBaseClassDescriptor();
            return desc->getFieldCount() - (excludeInherited && base ? base->getFieldCount() : 0);
        }

        default:
            return 0;
    }
}

// the ArrayElementNode contains the children of the element itself
// but the object pointer in those nodes point to the object that
// contains the array itself, so we need this parameter
std::vector<TreeNode *> TreeNode::makeObjectChildNodes(any_ptr obj, cClassDescriptor *desc, bool excludeInherited)
{
    if (obj == nullptr || desc == nullptr)
        return {};

    std::vector<TreeNode *> result;

    switch (mode) {
        case Mode::CHILDREN: {
            if (!obj.contains<cObject>())
                break;

            envir::cCollectChildrenVisitor visitor(fromAnyPtr<cObject>(obj));
            try {
                visitor.process(fromAnyPtr<cObject>(obj));

                cObject **objs = visitor.getArray();
                for (int i = 0; i < visitor.getArraySize(); ++i)
                    result.push_back(new ChildObjectNode(this, result.size(), obj, desc, objs[i], mode));
            }
            catch (std::exception &e) {
                result.push_back(new TextNode(this, result.size(), QString("<!> Error: ") + e.what(), mode));
            }

            break;
        }

        case Mode::INHERITANCE:
            if (!excludeInherited) {
                for (int i = 0; i < desc->getInheritanceChainLength(); i++)
                    result.push_back(new SuperClassNode(this, result.size(), obj, desc, i, mode));
                break;
            }

        // if the condition fails, falling through, no break here
        default: {  // GROUPED and FLAT have much in common
            auto base = desc->getBaseClassDescriptor();

            std::set<std::string> groupNames;
            for (int i = excludeInherited && base ? base->getFieldCount() : 0; i < desc->getFieldCount(); ++i) {
                const char *groupName = desc->getFieldProperty(i, "group");
                if (mode == Mode::GROUPED && groupName)
                    groupNames.insert(groupName);
                else
                    result.push_back(new FieldNode(this, result.size(), obj, desc, i, mode));
            }

            if (mode == Mode::GROUPED)
                for (auto& name : groupNames)
                    result.push_back(new FieldGroupNode(this, result.size(), obj, desc, name, mode));

            else { // FLAT
                // sorting the fields alphabetically
                std::sort(result.begin(), result.end(), [](TreeNode *a, TreeNode *b) {
                    return a->computeData(Qt::DisplayRole).toString() < b->computeData(Qt::DisplayRole).toString();
                });
                // then adjusting the indexInParent field accordingly
                for (size_t i = 0; i < result.size(); ++i)
                    result[i]->indexInParent = i;
            }
        }
    }

    return result;
}

cClassDescriptor *TreeNode::getDescriptorForField(any_ptr obj, cClassDescriptor *desc, int fieldIndex, int arrayIndex)
{
    any_ptr fieldPtr = desc->getFieldStructValuePointer(obj, fieldIndex, arrayIndex);
    if (fieldPtr == nullptr)
        return nullptr;

    if (desc->getFieldIsCObject(fieldIndex)) {
        cObject *object = fromAnyPtr<cObject>(fieldPtr);
        return object->getDescriptor();
    }

    if (desc->getFieldIsCompound(fieldIndex)) {
        const char *dynamicTypeName = desc->getFieldDynamicTypeString(obj, fieldIndex, arrayIndex);
        if (dynamicTypeName) {
            auto dynamicDesc = cClassDescriptor::getDescriptorFor(dynamicTypeName);
            if (dynamicDesc)
                return dynamicDesc;
        }
    }

    return cClassDescriptor::getDescriptorFor(desc->getFieldStructName(fieldIndex));
}

QVariant TreeNode::getDefaultObjectData(cObject *object, int role)
{
    switch (role) {
        case Qt::DecorationRole:
            return object ? getObjectIcon(object) : QVariant();

        case Qt::ToolTipRole:
            return opp_nulltoempty(object ? object->getDescriptor()->getProperty("hint") : "");

        case Qt::DisplayRole:
            return object ? (getObjectFullNameOrPath(object) + " (" + getObjectShortTypeName(object) + ")") : "no object selected";

        default:
            return QVariant();
    }
}

QString TreeNode::getObjectFullNameOrPath(cObject *object)
{
    return !object
            ? "no object selected"
            : object->getOwner() == getContainingCObjectPointer()
              ? object->getFullName()
              : object->getFullPath().c_str();
}


const TreeNode::NodeModeOverrideMap& TreeNode::getNodeModeOverrides()
{
    return getRootNode()->getNodeModeOverrides();
}

RootNode *TreeNode::getRootNode()
{
    TreeNode *iter = this;
    while (true) {
        TreeNode *parent = iter->getParent();
        if (parent == nullptr)
            break;
        iter = parent;
    }
    RootNode *result = dynamic_cast<RootNode *>(iter);
    ASSERT(result != nullptr);
    return result;
}

TreeNode::TreeNode(TreeNode *parent, int indexInParent, any_ptr contObject, cClassDescriptor *contDesc, Mode mode)
    : mode(mode), parent(parent), indexInParent(indexInParent),
    containingObject(contObject), containingDesc(contDesc)
{
}

void TreeNode::init()
{
    potentialChildCount = computeChildCount();
    nodeIdentifier = computeNodeIdentifier();
}

TreeNode *TreeNode::getChild(int index)
{
    ASSERT(filled);
    ASSERT(index < (int)children.size());
    return children[index];
}

QVariant TreeNode::getData(int role)
{
    if (role == (int)DataRole::NODE_MODE_OVERRIDE)
        // note: for root items, this is handled on the model level
        return (parent == nullptr || mode == parent->mode) ? -1 : (int)mode;

    bool roleSupported = contains(supportedDataRoles, role);
    if (!roleSupported || data.empty())
        return QVariant();

    // data is either empty, or contains a value for every supported role
    ASSERT(containsKey(data, role));

    return data[role];
}

bool TreeNode::gatherDataIfMissing()
{
    if (!data.empty())
        return false;

    for (auto role : supportedDataRoles)
        data[role] = computeData(role);

    return true;
}

bool TreeNode::updateData()
{
    bool changed = false;
    for (auto role : supportedDataRoles) {
        QVariant newData = computeData(role);
        if (!containsKey(data, role) || data[role] != newData)
            changed = true;

        data[role] = newData;
    }

    potentialChildCount = computeChildCount();

    return changed;
}

void TreeNode::updatePotentialChildCount()
{
    potentialChildCount = computeChildCount();
}

void TreeNode::restoreModeFromOverrides()
{
    nodeIdentifier = computeNodeIdentifier();
    const NodeModeOverrideMap& overrides = getNodeModeOverrides();

    auto i = overrides.find(nodeIdentifier.toStdString());

    if (i != overrides.end())
        mode = i->second;
}

cObject *TreeNode::getContainingCObjectPointer()
{
    TreeNode *anc = parent;
    while (anc) {
        cObject *obj = anc->getCObjectPointer();
        if (obj)
            return obj;
        anc = anc->parent;
    }
    return nullptr;
}

// simple internal helper. -1 is "explicitly disabled", 0 is "no preference", 1 is "enabled"
static int propertyValueToTriState(const char *value)
{
    if (!value)
        return 0;
    else if (!strcmp("false", value))
        return -1;
    else
        return 1;
}

bool TreeNode::fieldMatchesPropertyFilter(cClassDescriptor *desc, int fieldIndex, const char *property)
{
    // XXX once field property overriding is implemented in msgc, using getFieldDeclaredOn will probably not cut it
    cClassDescriptor *declDesc = cClassDescriptor::getDescriptorFor(desc->getFieldDeclaredOn(fieldIndex));
    int declProp = propertyValueToTriState(declDesc->getProperty(property));
    int fieldProp = propertyValueToTriState(desc->getFieldProperty(fieldIndex, property));

    switch (declProp) {
        case -1:
            return false;
        case 0:
            return fieldProp == 1;
        case 1:
            return fieldProp != -1;
    }
    return true;
}

TreeNode::~TreeNode()
{
    for (auto c : children)
        delete c;
}

SuperClassNode::SuperClassNode(TreeNode *parent, int indexInParent, any_ptr contObject, cClassDescriptor *contDesc, int superClassIndex, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode)
{
    superDesc = containingDesc;
    for (int i = 0; i < superClassIndex; ++i)
        superDesc = superDesc->getBaseClassDescriptor();
}

int SuperClassNode::computeChildCount()
{
    return computeObjectChildCount(containingObject, superDesc, mode, true);
}

std::vector<TreeNode *> SuperClassNode::makeChildren()
{
    return makeObjectChildNodes(containingObject, superDesc, true);
}

bool SuperClassNode::isSameAs(TreeNode *other)
{
    if (typeid(*this) != typeid(*other) || !TreeNode::isSameAs(other))
        return false;

    SuperClassNode *o = static_cast<SuperClassNode *>(other);
    return superDesc == o->superDesc;
}

QVariant SuperClassNode::computeData(int role)
{
    switch (role) {
        case Qt::DisplayRole: return stripNamespace(superDesc->getName());
        case (int)DataRole::HIGHLIGHT_RANGE: return QVariant::fromValue(HighlightRange {0, stripNamespace(superDesc->getName()).length()});
        default: return QVariant();
    };
}

QString SuperClassNode::computeNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "") + superDesc->getName();
}

bool SuperClassNode::matchesPropertyFilter(const QString &property)
{
    if (!isFilled())
        fill();
    for (int i = 0; i < getCurrentChildCount(); ++i)
        if (getChild(i)->matchesPropertyFilter(property))
            return true;
    return false;
}

ChildObjectNode::ChildObjectNode(TreeNode *parent, int indexInParent, any_ptr contObject, cClassDescriptor *contDesc, cObject *object, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode), object(object)
{
}

int ChildObjectNode::computeChildCount()
{
    return computeObjectChildCount(toAnyPtr(object), object ? object->getDescriptor() : nullptr, mode);
}

QVariant ChildObjectNode::computeData(int role)
{
    DisableDebugOnErrors dummy;

    QString defaultText = getDefaultObjectData(object, Qt::DisplayRole).value<QString>();
    QString infoText = object->str().c_str();

    switch (role) {
        case Qt::DisplayRole: return defaultText + (infoText.isEmpty() ? "" : (QString(" ") + infoText));
        case (int)DataRole::HIGHLIGHT_RANGE: return QVariant::fromValue(HighlightRange{defaultText.length(), infoText.isEmpty() ? 0 : infoText.length() + 1});
        default: return getDefaultObjectData(object, role);
    }
}

QString ChildObjectNode::computeNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "") + voidPtrToStr(object);
}

cObject *ChildObjectNode::getCObjectPointer()
{
    return object;
}

std::vector<TreeNode *> ChildObjectNode::makeChildren()
{
    return makeObjectChildNodes(toAnyPtr(object), object ? object->getDescriptor() : nullptr);
}

bool ChildObjectNode::isSameAs(TreeNode *other)
{
    if (typeid(*this) != typeid(*other) || !TreeNode::isSameAs(other))
        return false;

    ChildObjectNode *o = static_cast<ChildObjectNode *>(other);
    return object == o->object;
}


TextNode::TextNode(TreeNode *parent, int indexInParent, const QString &message, Mode mode)
    : TreeNode(parent, indexInParent, any_ptr(nullptr), nullptr, mode), message(message)
{
}

QVariant TextNode::computeData(int role)
{
    switch (role) {
        case Qt::DisplayRole: return message;
        case (int)DataRole::HIGHLIGHT_RANGE: return QVariant::fromValue(HighlightRange{0, message.length()});
        default: return getDefaultObjectData(nullptr, role);
    }
}

QString TextNode::computeNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "") + "<" + message + ">";
}

bool TextNode::isSameAs(TreeNode *other)
{
    if (typeid(*this) != typeid(*other) || !TreeNode::isSameAs(other))
        return false;

    TextNode *o = static_cast<TextNode *>(other);
    return message == o->message;
}


FieldNode::FieldNode(TreeNode *parent, int indexInParent, any_ptr contObject, cClassDescriptor *contDesc, int fieldIndex, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode), fieldIndex(fieldIndex)
{
    if (!contDesc->getFieldIsArray(fieldIndex) && contDesc->getFieldIsCompound(fieldIndex)) {
        object = contDesc->getFieldStructValuePointer(contObject, fieldIndex, 0);
        desc = getDescriptorForField(contObject, contDesc, fieldIndex);
    }
}

int FieldNode::computeChildCount()
{
    if (containingObject != nullptr && containingDesc != nullptr && containingDesc->getFieldIsArray(fieldIndex))
        return containingDesc->getFieldArraySize(containingObject, fieldIndex);
    else
        return computeObjectChildCount(object, desc, mode);
}

std::vector<TreeNode *> FieldNode::makeChildren()
{
    if (containingObject != nullptr && containingDesc != nullptr && containingDesc->getFieldIsArray(fieldIndex)) {
        std::vector<TreeNode *> result;
        int size = containingDesc->getFieldArraySize(containingObject, fieldIndex);
        for (int i = 0; i < size; ++i)
            result.push_back(new ArrayElementNode(this, result.size(), containingObject, containingDesc, fieldIndex, i, mode));
        return result;
    }
    else {
        return makeObjectChildNodes(object, desc);
    }
}

bool FieldNode::isSameAs(TreeNode *other)
{
    if (typeid(*this) != typeid(*other) || !TreeNode::isSameAs(other))
        return false;

    FieldNode *o = static_cast<FieldNode *>(other);
    return fieldIndex == o->fieldIndex && object == o->object && desc == o->desc;
}

QVariant FieldNode::computeData(int role)
{
    DisableDebugOnErrors dummy;

    cObject *objectCasted =
              (containingDesc && containingDesc->getFieldIsCObject(fieldIndex))
                ? fromAnyPtr<cObject>(object)
                : nullptr;

    if ((role == Qt::DecorationRole) && objectCasted)
        return object != nullptr ? getObjectIcon(objectCasted) : QVariant();

    // the rest is for the regular, non-root nodes

    bool isPointer = containingDesc->getFieldIsPointer(fieldIndex);
    bool isCObject = containingDesc->getFieldIsCObject(fieldIndex);
    bool isCompound = containingDesc->getFieldIsCompound(fieldIndex);
    bool isArray = containingDesc->getFieldIsArray(fieldIndex);

    QString fieldName = containingDesc->getFieldName(fieldIndex);
    QString objectClassName = objectCasted ? (QString(" (") + getObjectShortTypeName(objectCasted) + ")") : "";
    QString objectName = objectCasted ? QString(" ") + getObjectFullNameOrPath(objectCasted): "";
    QString arraySize;
    QString prefix, fieldValue, postfix; // pre- and postfix mostly (only) for 'string values'
    QString objectInfo;
    QString equals = " = ";
    QString editable = isEditable() ? " [...] " : "";
    QString fieldType = containingDesc->getFieldTypeString(fieldIndex);
    any_ptr valuePointer = isCompound && !isCObject && !isArray ? containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, 0) : any_ptr(nullptr);

    if (isCompound && !isCObject && !isArray && valuePointer != nullptr) {
        // Even if it's not a CObject, it can have a different dynamic type
        // than the declared static type, which we can get this way.
        const char *dynamicType = containingDesc->getFieldDynamicTypeString(containingObject, fieldIndex, 0);
        if (dynamicType && dynamicType[0])
            objectClassName = QString(" (") + stripNamespace(dynamicType) + ")";
    }

    // the name can be overridden by a label property
    if (const char *label = containingDesc->getFieldProperty(fieldIndex, "label"))
        fieldName = label;

    // it is a simple value (not an array, but may be compound - like color or transform)
    if (!isArray && !isCObject) {
        try {
            fieldValue = containingDesc->getFieldValueAsString(containingObject, fieldIndex, 0).c_str();
        }
        catch (cRuntimeError& e) {
            fieldValue = QString("<!> Error: ") + e.what();
        }
    }

    if (!isArray && isPointer) {
        if (objectCasted) {
            objectInfo = objectCasted->str().c_str();
            if (objectInfo.length() > 0)
                objectInfo = QString(": ") + objectInfo;
        }
        else if (valuePointer == nullptr)
            fieldValue = "nullptr";
    }

    if (isArray)
        arraySize = QString("[") + QVariant::fromValue(containingDesc->getFieldArraySize(containingObject, fieldIndex)).toString() + "]";

    if (fieldType == "string" && !isArray)
        prefix = postfix = "'";

    if (isArray || (prefix + fieldValue + postfix).isEmpty())
        equals = "";  // no need to add an " = " in these cases

    QString tooltip = opp_nulltoempty(containingDesc->getFieldProperty(fieldIndex, "hint"));

    if (fieldValue.contains("\n") || fieldValue.length() > 50)
        tooltip += "\n\n" + fieldValue;

    switch (role) {
        case Qt::EditRole:
            return fieldValue;

        case Qt::ToolTipRole:
            return tooltip;

        case Qt::DisplayRole:
            return fieldName + arraySize + equals + prefix + fieldValue + postfix + objectClassName + objectName + objectInfo + editable + " (" + fieldType + ")";

        case Qt::UserRole:
            return QVariant::fromValue(HighlightRange { fieldName.length() + arraySize.length() + equals.length() + prefix.length(), fieldValue.length() + objectClassName.length() + objectName.length() + objectInfo.length() });

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
    DisableDebugOnErrors dummy;
    if (!containingDesc)
        return false;
    containingDesc->setFieldValueAsString(containingObject, fieldIndex, 0, value.toString().toStdString().c_str());
    return true;
}

cObject *FieldNode::getCObjectPointer()
{
    return (fieldIndex == -1)  // either it is the root, so it must be cObject
           || (containingDesc  // or it is a node representing a single cObject
               && containingDesc->getFieldIsCObject(fieldIndex)
               && !containingDesc->getFieldIsArray(fieldIndex))
           ? fromAnyPtr<cObject>(object)
           : nullptr;
}

QString FieldNode::computeNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "") + containingDesc->getFieldName(fieldIndex);
}

bool FieldNode::matchesPropertyFilter(const QString &property)
{
    return fieldMatchesPropertyFilter(containingDesc, fieldIndex, property.toUtf8());
}

std::vector<TreeNode *> RootNode::makeChildren()
{
    return makeObjectChildNodes(toAnyPtr(object), object ? object->getDescriptor() : nullptr, false);
}

bool RootNode::isSameAs(TreeNode *other)
{
    if (typeid(*this) != typeid(*other) || !TreeNode::isSameAs(other))
        return false;

    RootNode *o = static_cast<RootNode *>(other);
    return object == o->object;
}

RootNode::RootNode(cObject *object, int indexInParent, Mode mode, const NodeModeOverrideMap& nodeModeOverrides)
    : TreeNode(nullptr, indexInParent, any_ptr(nullptr), nullptr, mode), object(object), nodeModeOverrides(nodeModeOverrides)
{
}

int RootNode::computeChildCount()
{
    return computeObjectChildCount(toAnyPtr(object), object ? object->getDescriptor() : nullptr, mode);
}

QVariant RootNode::computeData(int role)
{
    DisableDebugOnErrors dummy;

    if (!object)
        return getDefaultObjectData(object, role);

    QString pathAndType = QString(object->getFullPath().c_str()) + " (" + getObjectShortTypeName(object) + ")";
    QString infoText = object->str().c_str();
    if (!infoText.isEmpty())
        infoText = " " + infoText;

    switch (role) {
        case Qt::DisplayRole: return pathAndType + infoText;
        case (int)DataRole::HIGHLIGHT_RANGE: return QVariant::fromValue(HighlightRange { pathAndType.length(), infoText.length() });
        default: return getDefaultObjectData(object, role);
    }
}

QString RootNode::computeNodeIdentifier()
{
    return "<root-" + QString::number(getIndexInParent()) + ">";
}

cObject *RootNode::getCObjectPointer()
{
    return object;
}

FieldGroupNode::FieldGroupNode(TreeNode *parent, int indexInParent, any_ptr contObject, cClassDescriptor *contDesc, const std::string& groupName, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode), groupName(groupName)
{
}

int FieldGroupNode::computeChildCount()
{
    if (containingObject == nullptr || containingDesc == nullptr)
        return 0;

    int count = 0;
    for (int i = 0; i < containingDesc->getFieldCount(); ++i) {
        const char *thisFieldGroup = containingDesc->getFieldProperty(i, "group");
        if (thisFieldGroup && (thisFieldGroup == groupName))
            ++count;
    }
    return count;
}

std::vector<TreeNode *> FieldGroupNode::makeChildren()
{
    std::vector<TreeNode *> result;

    if (containingObject == nullptr || containingDesc == nullptr)
        return result;

    for (int i = 0; i < containingDesc->getFieldCount(); ++i) {
        const char *thisFieldGroup = containingDesc->getFieldProperty(i, "group");
        if (thisFieldGroup && (thisFieldGroup == groupName)) {
            result.push_back(new FieldNode(this, result.size(), containingObject, containingDesc, i, mode));
        }
    }

    return result;
}

bool FieldGroupNode::isSameAs(TreeNode *other)
{
    if (typeid(*this) != typeid(*other) || !TreeNode::isSameAs(other))
        return false;

    FieldGroupNode *o = static_cast<FieldGroupNode *>(other);
    return groupName == o->groupName;
}

QVariant FieldGroupNode::computeData(int role)
{
    switch (role) {
        case Qt::DisplayRole: return groupName.c_str();
        case Qt::UserRole: return QVariant::fromValue(HighlightRange { 0, (int)groupName.length() });
        default: return QVariant();
    }
}

QString FieldGroupNode::computeNodeIdentifier()
{
    return (parent ? parent->getNodeIdentifier() + "|" : "") + groupName.c_str();
}

bool FieldGroupNode::matchesPropertyFilter(const QString &property)
{
    if (!isFilled())
        fill();
    for (int i = 0; i < getCurrentChildCount(); ++i)
        if (getChild(i)->matchesPropertyFilter(property))
            return true;
    return false;
}

std::vector<TreeNode *> ArrayElementNode::makeChildren()
{
    if (containingDesc->getFieldIsCompound(fieldIndex)) {
        cClassDescriptor *fieldDesc = getDescriptorForField(containingObject, containingDesc, fieldIndex, arrayIndex);
        return makeObjectChildNodes(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex), fieldDesc);
    }
    return {};
}

bool ArrayElementNode::isSameAs(TreeNode *other)
{
    if (typeid(*this) != typeid(*other) || !TreeNode::isSameAs(other))
        return false;

    ArrayElementNode *o = static_cast<ArrayElementNode *>(other);
    return fieldIndex == o->fieldIndex && arrayIndex == o->arrayIndex;
}

ArrayElementNode::ArrayElementNode(TreeNode *parent, int indexInParent, any_ptr contObject, cClassDescriptor *contDesc, int fieldIndex, int arrayIndex, Mode mode)
    : TreeNode(parent, indexInParent, contObject, contDesc, mode), fieldIndex(fieldIndex), arrayIndex(arrayIndex)
{
}

int ArrayElementNode::computeChildCount()
{
    if (containingDesc->getFieldIsCompound(fieldIndex)) {
        cClassDescriptor *fieldDesc = getDescriptorForField(containingObject, containingDesc, fieldIndex, arrayIndex);
        return computeObjectChildCount(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex), fieldDesc, mode);
    }
    return 0;
}

QVariant ArrayElementNode::computeData(int role)
{
    DisableDebugOnErrors dummy;

    QString indexEquals = QString("[%1] ").arg(arrayIndex);
    QString value;
    QString info;
    cObject *fieldObjectPointer = nullptr;

    if (containingDesc->getFieldIsCObject(fieldIndex)) {
        fieldObjectPointer = fromAnyPtr<cObject>(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex));

        info += (fieldObjectPointer
                        ? QString("(%1) %2")
                          .arg(getObjectShortTypeName(fieldObjectPointer))
                          .arg(getObjectFullNameOrPath(fieldObjectPointer))
                         : "nullptr");

        std::string i = fieldObjectPointer ? fieldObjectPointer->str() : "";
        if (!i.empty()) {
            info += ": ";
            value += i.c_str();
        }
    }
    else if (containingDesc->getFieldIsCompound(fieldIndex)) {
        const char *fieldType = containingDesc->getFieldDynamicTypeString(containingObject, fieldIndex, arrayIndex);
        std::string fieldValue;
        try {
            fieldValue = containingDesc->getFieldValueAsString(containingObject, fieldIndex, arrayIndex);
        }
        catch (cRuntimeError& e) {
            fieldValue = std::string("<!> Error: ") + e.what();
        }

        if (fieldType && fieldType[0])
            info += QString("(") + stripNamespace(fieldType) + ")";

        if (!fieldValue.empty()) {
            if (!info.isEmpty())
                info += " ";
            value += fieldValue.c_str();
        }
    }
    else {
        try {
            value = containingDesc->getFieldValueAsString(containingObject, fieldIndex, arrayIndex).c_str();
        }
        catch (cRuntimeError& e) {
            value = QString("<!> Error: ") + e.what();
        }
    }

    QString editable = containingDesc->getFieldIsEditable(fieldIndex) ? " [...]" : "";

    switch (role) {
        case Qt::DisplayRole:
            return indexEquals + info + value + editable;

        case Qt::DecorationRole:
            return fieldObjectPointer ? getObjectIcon(fieldObjectPointer) : QVariant();

        case Qt::EditRole:
            return value;

        case Qt::UserRole:
            return QVariant::fromValue(HighlightRange { indexEquals.length() + info.length(), value.length() });

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
    containingDesc->setFieldValueAsString(containingObject, fieldIndex, arrayIndex, value.toString().toStdString().c_str());
    return true;
}

QString ArrayElementNode::computeNodeIdentifier()
{
    cObject *obj = getCObjectPointer();
    return (parent ? parent->getNodeIdentifier() + "|" : "")
            + (obj ? voidPtrToStr(obj) : QString("[%1]").arg(arrayIndex));
}

cObject *ArrayElementNode::getCObjectPointer()
{
    return containingDesc->getFieldIsCObject(fieldIndex)
            ? fromAnyPtr<cObject>(containingDesc->getFieldStructValuePointer(containingObject, fieldIndex, arrayIndex))
            : nullptr;
}

bool ArrayElementNode::matchesPropertyFilter(const QString &property)
{
    return fieldMatchesPropertyFilter(containingDesc, fieldIndex, property.toUtf8());
}

}  // namespace qtenv
}  // namespace omnetpp
