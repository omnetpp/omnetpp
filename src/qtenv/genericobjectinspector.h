//==========================================================================
//  GENERICOBJECTINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_GENERICOBJECTINSPECTOR_H
#define __OMNETPP_QTENV_GENERICOBJECTINSPECTOR_H

#include "qtenvdefs.h"
#include "inspector.h"

#include <QtCore/QModelIndex>

#include "genericobjecttreemodel.h"

class QLabel;
class QTreeView;
class QToolBar;
class QToolButton;

namespace omnetpp {
namespace qtenv {

class QTENV_API GenericObjectInspector : public Inspector
{
    Q_OBJECT

    static const QString PREF_SORT_BY_NAME;

public:
    using Mode = GenericObjectTreeModel::Mode;
    using DetailsMode = GenericObjectTreeModel::DetailsMode;

    GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

    virtual void doSetObject(cObject *obj) override;
    virtual void refresh() override;

    void setSortByName(bool sorted);

protected:
    QTreeView *treeView;
    GenericObjectTreeModel *sourceModel = nullptr;

    QAction *copyLineAction;
    QAction *copyLineHighlightedAction;

    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override; // DELETES THIS INSPECTOR
    bool eventFilter(QObject *watched, QEvent *event) override;

    void recreateModel(bool keepNodeModeOverrides = true);

    // Single toggle button shown on hover, right-aligned in each row
    QWidget *subtreeModeOverlay = nullptr;
    QToolButton *detailsToggleButton = nullptr;

    void updateSubtreeModeOverlay(const QPoint &viewportPos);

    bool sortByName = true;
    QAction *sortByNameAction;

    // Toggle the given node between Children and Details mode
    void toggleNodeDetails(const QModelIndex &proxyIndex);
    // Put the given node into the given mode, and expand it
    void switchNodeMode(const QModelIndex &sourceIndex, Mode mode, DetailsMode detailsMode = DetailsMode::GROUPED);

protected Q_SLOTS:
    void onTreeViewActivated(const QModelIndex& index);
    void onDataEdited();
    void gatherVisibleDataIfSafe();
    void createContextMenu(QPoint pos);
    void copySelectedLineToClipboard(bool onlyHighlightedPart);

    void toggleSelectedNodeDetails();
    void cycleSelectedNodeDetailsMode();

private:

    // treeView manipulation/querying functions:

    bool gatherMissingDataIfSafe();
    bool gatherMissingData();
    bool updateData();

    QModelIndexList getVisibleNodes();

    QString getSelectedNode();
    void selectNode(const QString &identifier);

    QSet<QString> getExpandedNodes();
    QSet<QString> getExpandedNodes(const QModelIndex& index);

    void expandNodes(const QSet<QString>& ids);
    void expandNodes(const QSet<QString>& ids, const QModelIndex& index);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

