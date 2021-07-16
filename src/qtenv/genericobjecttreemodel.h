//==========================================================================
//  GENERICOBJECTTREEMODEL.H - part of
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

#ifndef __OMNETPP_QTENV_GENERICOBJECTTREEMODEL_H
#define __OMNETPP_QTENV_GENERICOBJECTTREEMODEL_H

#include <unordered_map>
#include <QtCore/QAbstractItemModel>
#include <QtWidgets/QMenu>
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QTreeView>
#include "omnetpp/cobject.h"
#include "omnetpp/cclassdescriptor.h"
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

class RootNode;

// this is wrapped in a QVariant to be returned by the model
// when the UserRole data is requested, so the itemdelegate
// of the TreeView can highlight a section in the string
struct QTENV_API HighlightRange {
    int start; // the index of the first highlighted character
    int length; // the number of highlighted characters
};

// This is used as a proxy for GenericObjectTreeModel.
// If the relevant property is set (not empty), it will only show
// elements (objects/fields) which have that property on them.
class QTENV_API PropertyFilteredGenericObjectTreeModel : public QSortFilterProxyModel {
    QString relevantProperty;

public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    void setRelevantProperty(const QString &relevantProperty); // this is "packetData" in PACKET mode, and "" in all others

    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

// encapsulates the tree model, handles QModelIndexes, etc
class QTENV_API GenericObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    // enum classes, so we can typedef them in TreeNode and the Inspector
    enum class Mode {
        GROUPED,
        FLAT,
        INHERITANCE,
        CHILDREN,
        PACKET   // this is never seen by this (source) model or the Nodes, only sets filtering in the proxy model, and FLAT mode in this one
    };

    enum class DataRole : int {
        HIGHLIGHT_RANGE = Qt::UserRole,
        NODE_MODE_OVERRIDE,  // corresponding value is -1 if no override, otherwise maps to Mode
    };

    typedef std::unordered_map<std::string, Mode> NodeModeOverrideMap;

private:
    // this is the "global" mode set on the inspector
    Mode inspectorMode;
    std::vector<RootNode *> rootNodes;
    // maps nodeIdentifier to overridden Mode, for nodes whose mode was overridden by the user
    NodeModeOverrideMap nodeModeOverrides;

    // these can be accessed through setData(), using DataRole::NODE_MODE_OVERRIDE
    void setNodeMode(const QModelIndex &index, Mode mode);
    void unsetNodeMode(const QModelIndex &index);

public:
    GenericObjectTreeModel(cObject *object, Mode mode, const NodeModeOverrideMap& modeOverrides, QObject *parent = nullptr);
    GenericObjectTreeModel(std::vector<cObject *> roots, Mode mode, const NodeModeOverrideMap& modeOverrides, QObject *parent = nullptr);

    std::vector<cObject *> getRootObjects();

    const NodeModeOverrideMap& getNodeModeOverrides() const { return nodeModeOverrides;}

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    void refreshTreeStructure();
    void refreshNodeChildrenRec(const QModelIndex &index);
    void refreshChildList(const QModelIndex &index);

    cObject *getCObjectPointer(const QModelIndex &index);
    // same as above, but translates cWatchObj pointers to their watched cObject pointers
    cObject *getCObjectPointerToInspect(const QModelIndex &index);

    ~GenericObjectTreeModel();

signals:
    void dataEdited(const QModelIndex& index) ;
};

}  // namespace qtenv
}  // namespace omnetpp

// this is needed to wrap the HighlightRange into a QVariant
// according to the Qt docs, this must not be in a namespace
Q_DECLARE_METATYPE(omnetpp::qtenv::HighlightRange)

#endif // __OMNETPP_QTENV_GENERICOBJECTTREEMODEL_H
