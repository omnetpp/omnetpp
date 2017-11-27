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

#include <QModelIndex>

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

protected:
    QTreeView *treeView;
    GenericObjectTreeModel *model = nullptr;

    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override; // DELETES THIS INSPECTOR

    void recreateModel();

    Mode mode = Mode::GROUPED;
    QAction *toGroupedModeAction;
    QAction *toFlatModeAction;
    QAction *toChildrenModeAction;
    QAction *toInheritanceModeAction;

    void doSetMode(Mode mode);

protected slots:
    void onTreeViewActivated(const QModelIndex& index);
    void onDataEdited();
    void gatherVisibleDataIfSafe();
    void createContextMenu(QPoint pos);

    void setMode(Mode mode);

    void toGroupedMode()     { setMode(Mode::GROUPED);     }
    void toFlatMode()        { setMode(Mode::FLAT);        }
    void toChildrenMode()    { setMode(Mode::CHILDREN);    }
    void toInheritanceMode() { setMode(Mode::INHERITANCE); }

public:
    GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
    ~GenericObjectInspector();
    virtual void doSetObject(cObject *obj) override;
    virtual void refresh() override;
};

} // namespace qtenv
} // namespace omnetpp

#endif

