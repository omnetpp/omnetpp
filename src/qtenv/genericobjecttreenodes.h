//==========================================================================
//  GENERICOBJECTTREENODES.H - part of
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

#ifndef __OMNETPP_QTENV_GENERICOBJECTTREENODES_H
#define __OMNETPP_QTENV_GENERICOBJECTTREENODES_H

#include <QVariant>
#include "omnetpp/cobject.h"
#include "omnetpp/cclassdescriptor.h"
#include "genericobjecttreemodel.h"

namespace omnetpp {
namespace qtenv {

// This is the base class of all nodes in the tree model.
// Each instance of this will be seen as an item in the TreeView.
// Stores all data, and uses lazy loading of children.
class TreeNode
{
  protected:
    using Mode = GenericObjectTreeModel::Mode;
    Mode mode; // this is stored in every node for convenience, but should be the same in the model and every node

    // these make up the tree structure of the model
    TreeNode *parent = nullptr;
    int indexInParent = 0;
    std::vector<TreeNode *> children; // empty if not filled, holds potentialChildCount elements if filled

    // Whether or not the "children" vector actually holds the (potential) children.
    bool filled = false;

    std::map<int, QVariant> data; // the key type is really Qt::ItemDataRole
    int potentialChildCount = -1; // value is returned by computeChildCount()
    QString nodeIdentifier;

    // these are nullptrs only in the root node
    void *containingObject = nullptr;  // may or may not be a cObject, so we need the descriptor for it
    cClassDescriptor *containingDesc = nullptr;

    // helpers
    static cClassDescriptor *getDescriptorForField(void *obj, cClassDescriptor *desc, int fieldIndex, int arrayIndex = 0);
    static int computeObjectChildCount(void *obj, cClassDescriptor *desc, Mode mode, bool excludeInherited = false);
    static bool fieldMatchesPropertyFilter(cClassDescriptor *containingDesc, int fieldIndex, const char *property);
    // this is not static just to avoid having to pass mode and this (as parent)
    std::vector<TreeNode *> makeObjectChildNodes(void *obj, cClassDescriptor *desc, bool excludeInherited = false);
    // more helpers, not static only to check if "parent in model tree is parent in ownership tree"
    QVariant getDefaultObjectData(cObject *object, int role);
    QString getObjectFullNameOrPath(cObject *object);

    // Creates the data for one of the following roles:
    //  - DisplayRole: a string to be displayed on the item
    //  - DecorationRole: an optional icon
    //  - UserRole: an optional HighlightRange for the blue substring
    //  - EditRole: for editable items, this will be the initial editor text
    //  - TooltipRole: optional and pretty obvious
    virtual QVariant computeData(int role) = 0;

    // regardless of the children vector
    virtual int computeChildCount() = 0;

    // only used to save and restore the expansion state and selection in the views (and for debugging)
    virtual QString computeNodeIdentifier() = 0;

    // these are really Qt::ItemDataRole values
    static const std::vector<int> supportedDataRoles;

  public:
    TreeNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, Mode mode);

    // has to be called before usage. can't be in the constructor because of virtual functions. not recursive.
    void init();

    // Kind of like a virtual == operator, but only checks if the two nodes "represent" the same thing,
    // not for any other state regarding filled state and current data contents.
    virtual bool isSameAs(TreeNode *other) = 0;

    TreeNode *getParent() { return parent; }
    int getIndexInParent() { return indexInParent; }

    int getPotentialChildCount() { ASSERT(potentialChildCount >= 0); return potentialChildCount; }
    int getCurrentChildCount() { return children.size(); } // the actual size of the children vector right now (0 if not yet filled)
    TreeNode *getChild(int index); // should only be called after filling

    // will be empty if not yet filled. does not fill.
    const std::vector<TreeNode *>& getExistingChildren() { return children; }

    void fill(); // populates the children vector
    void unfill(); // deletes the children and clears the data map

    bool isFilled() { return filled; }

    // creates the child nodes and returns a vector of them, but does not touch anything else.
    // does not need to call init on the infants
    virtual std::vector<TreeNode *> makeChildren() = 0;

    bool getHasChildren() { ASSERT(potentialChildCount >= 0); return potentialChildCount > 0; } // even if not filled, will decide if it "would have" children after filling

    bool gatherDataIfMissing(); // returns true if data was actually just gathered
    bool updateData();          // returns true if there was a change in the data
    void updatePotentialChildCount(); // needed for when some children appear in the model when there were none before
    QVariant getData(int role); // safe, does not touch the model, returns an invalid QVariant if no data is present yet

    virtual bool isEditable() { return false; } // subclasses will override as needed
    virtual bool setData(const QVariant& value, int role) { return false; }

    QString getNodeIdentifier() { ASSERT(!nodeIdentifier.isEmpty()); return nodeIdentifier; }

    // returns a nullptr where not applicable
    virtual cObject *getCObjectPointer() { return nullptr; }

    // Will always return a valid cObject (unless nullptr is inspected)
    // and it will be the object that has this node in its node.
    // (This is entirely separate from cObject ownership, consider sender
    // and arrival modules/gates in a message, etc.)
    virtual cObject *getContainingCObjectPointer();

    // used by the PropertyFilteredGenericObjectTreeModel in PACKET mode
    virtual bool matchesPropertyFilter(const QString &property) { return true; }

    virtual ~TreeNode();
};

class SuperClassNode : public TreeNode
{
    cClassDescriptor *superDesc;

  protected:
    std::vector<TreeNode *> makeChildren() override;
    bool isSameAs(TreeNode *other) override;

  public:
    // superClassIndex: up in the inheritance chain, 0 is the most specialized class, and cObject is at the highest level
    SuperClassNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int superClassIndex, Mode mode);
    int computeChildCount() override;
    QVariant computeData(int role) override;
    QString computeNodeIdentifier() override;
    // currently unused, filtering is only done in PACKET mode, which sets FLAT mode on the source model
    bool matchesPropertyFilter(const QString &property) override;
};

class ChildObjectNode : public TreeNode
{
    cObject *object;

  protected:
    std::vector<TreeNode *> makeChildren() override;
    bool isSameAs(TreeNode *other) override;

  public:
    ChildObjectNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, cObject *object, Mode mode);
    int computeChildCount() override;
    QVariant computeData(int role) override;
    QString computeNodeIdentifier() override;
    cObject *getCObjectPointer() override;
};

class FieldNode : public TreeNode
{
  protected:
    int fieldIndex = 0;

    void *object = nullptr;
    cClassDescriptor *desc = nullptr;

    std::vector<TreeNode *> makeChildren() override;
    bool isSameAs(TreeNode *other) override;

  public:
    FieldNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex, Mode mode);

    int computeChildCount() override;
    QVariant computeData(int role) override;

    bool isEditable() override;
    bool setData(const QVariant& value, int role) override;
    cObject *getCObjectPointer() override;
    QString computeNodeIdentifier() override;
    bool matchesPropertyFilter(const QString &property) override;
};

class RootNode : public TreeNode
{
    cObject *object;

  protected:
    std::vector<TreeNode *> makeChildren() override;
    bool isSameAs(TreeNode *other) override;

  public:
    RootNode(cObject *object, Mode mode);
    int computeChildCount() override;
    QVariant computeData(int role) override;
    QString computeNodeIdentifier() override;
    cObject *getCObjectPointer() override;
};

class FieldGroupNode : public TreeNode
{
    std::string groupName;

  protected:
    std::vector<TreeNode *> makeChildren() override;
    bool isSameAs(TreeNode *other) override;

  public:
    FieldGroupNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, const std::string& groupName, Mode mode);
    int computeChildCount() override;
    QVariant computeData(int role) override;
    QString computeNodeIdentifier() override;
    // currently unused, filtering is only done in PACKET mode, which sets FLAT mode on the source model
    bool matchesPropertyFilter(const QString &property) override;
};

class ArrayElementNode : public TreeNode
{
    int fieldIndex;
    int arrayIndex;

  protected:
    std::vector<TreeNode *> makeChildren() override;
    bool isSameAs(TreeNode *other) override;

  public:
    ArrayElementNode(TreeNode *parent, int indexInParent, void *contObject, cClassDescriptor *contDesc, int fieldIndex, int arrayIndex, Mode mode);
    int computeChildCount() override;

    QVariant computeData(int role) override;

    bool isEditable() override;
    virtual bool setData(const QVariant& value, int role) override;
    QString computeNodeIdentifier() override;
    cObject *getCObjectPointer() override;
    bool matchesPropertyFilter(const QString &property) override;
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_GENERICOBJECTTREENODES_H
