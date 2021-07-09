//==========================================================================
//  OBJECTTREEINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_OBJECTTREEINSPECTOR_H
#define __OMNETPP_QTENV_OBJECTTREEINSPECTOR_H

#include "inspector.h"
#include <QtCore/QModelIndex>

class QTreeView;

namespace omnetpp {
namespace qtenv {

class GenericObjectTreeModel;

class QTENV_API ObjectTreeInspector : public Inspector
{
    Q_OBJECT
private:
    GenericObjectTreeModel *model = nullptr;
    QTreeView *view = nullptr;

    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onClick(QModelIndex index);
    void onDoubleClick(QModelIndex index);

    bool gatherVisibleData();
    bool gatherVisibleDataIfSafe();

public slots:
    void createContextMenu(QPoint pos);

public:
    ObjectTreeInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

    virtual void doSetObject(cObject *obj) override;
    virtual void refresh() override;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_OBJECTTREEINSPECTOR_H
