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

#include <QAbstractItemModel>
#include <QMenu>
#include <QTreeView>
#include "omnetpp/cobject.h"
#include "omnetpp/cclassdescriptor.h"
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

class TreeNode;

// this is wrapped in a QVariant to be returned by the model
// when the UserRole data is requested, so the itemdelegate
// of the TreeView can highlight a section in the string
struct QTENV_API HighlightRange {
    int start; // the index of the first highlighted character
    int length; // the number of highlighted characters
};

// encapsulates the tree model, handles QModelIndexes, etc
class QTENV_API GenericObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    TreeNode *rootNode;

    QSet<QString> getExpandedNodesIn(QTreeView *view, const QModelIndex &index);
    void expandNodesIn(QTreeView *view, const QSet<QString> &ids, const QModelIndex &index);

    QModelIndexList getVisibleNodesIn(QTreeView *view);

    bool gatherMissingDataIn(QTreeView *view);

public:
    // enum class so we can typedef it in TreeNode and the Inspector
    enum class Mode {
        GROUPED,
        FLAT,
        CHILDREN,
        INHERITANCE,
        //PACKET
    };

    GenericObjectTreeModel(cObject *object, Mode mode, QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QString getSelectedNodeIn(QTreeView *view);
    void selectNodeIn(QTreeView *view, const QString& identifier);

    QSet<QString> getExpandedNodesIn(QTreeView *view);
    void expandNodesIn(QTreeView *view, const QSet<QString> &ids);

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    bool gatherMissingDataIfSafeIn(QTreeView *view);
    bool updateDataIn(QTreeView *view);

    void refreshTreeStructure();
    void refreshNodeChildrenRec(const QModelIndex &index);
    void refreshChildList(const QModelIndex &index);

    cObject *getCObjectPointer(const QModelIndex &index);
    ~GenericObjectTreeModel();

signals:
    void dataEdited(const QModelIndex& index) ;
};

} // namespace qtenv
} // namespace omnetpp

// this is needed to wrap the HighlightRange into a QVariant
// according to the Qt docs, this must not be in a namespace
Q_DECLARE_METATYPE(omnetpp::qtenv::HighlightRange)

#endif // __OMNETPP_QTENV_GENERICOBJECTTREEMODEL_H
