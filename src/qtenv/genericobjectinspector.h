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

#include "inspector.h"

#include <QtCore/QModelIndex>

#include "genericobjecttreemodel.h"

class QLabel;
class QTreeView;
class QToolBar;

namespace omnetpp {
namespace qtenv {

class QTENV_API GenericObjectInspector : public Inspector
{
    Q_OBJECT

    // The default mode for these types should be CHILDREN
    static const std::vector<std::string> containerTypes;
    static const QString PREF_MODE;

public:
    using Mode = GenericObjectTreeModel::Mode;

    GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

    virtual void doSetObject(cObject *obj) override;
    virtual void refresh() override;

protected:
    QTreeView *treeView;
    // This is the "source" model, producing the tree in all modes.
    // The view never uses this directly, only through the proxyModel.
    GenericObjectTreeModel *sourceModel = nullptr;
    // This is a proxy, which is a transparent pass-through in all modes except in PACKET.
    // In PACKET mode, it filters the source model (which should be set to FLAT mode in this case)
    // to match a certain (object or field) property - "packetData" at the moment.
    // The view is always connected to this model, so accessing TreeNodes
    // (internalPointer, then static_cast) requires index mapping (mapToSource) first.
    PropertyFilteredGenericObjectTreeModel *proxyModel = nullptr;

    QAction *copyLineAction;
    QAction *copyLineHighlightedAction;

    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override; // DELETES THIS INSPECTOR

    void recreateModel(bool keepNodeModeOverrides = true);

    Mode mode = Mode::GROUPED;
    QAction *toGroupedModeAction;
    QAction *toFlatModeAction;
    QAction *toInheritanceModeAction;
    QAction *toChildrenModeAction;
    QAction *toPacketModeAction;

    void doSetMode(Mode mode);

protected slots:
    void onTreeViewActivated(const QModelIndex& index);
    void onDataEdited();
    void gatherVisibleDataIfSafe();
    void createContextMenu(QPoint pos);
    void copySelectedLineToClipboard(bool onlyHighlightedPart);

    void cycleSelectedSubtreeMode();

    void setMode(Mode mode);

    void toGroupedMode()     { setMode(Mode::GROUPED);     }
    void toFlatMode()        { setMode(Mode::FLAT);        }
    void toInheritanceMode() { setMode(Mode::INHERITANCE); }
    void toChildrenMode()    { setMode(Mode::CHILDREN);    }
    void toPacketMode()      { setMode(Mode::PACKET);      }

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

