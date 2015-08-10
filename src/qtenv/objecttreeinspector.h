//==========================================================================
//  OBJECTTREEINSPECTOR.H - part of
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

#ifndef OBJECTTREEINSPECTOR_H
#define OBJECTTREEINSPECTOR_H

#include "inspector.h"
#include <QModelIndex>

class QTreeView;

namespace omnetpp {
namespace qtenv {

class TreeItemModel;

class ObjectTreeInspector : public Inspector
{
    Q_OBJECT
private:
    TreeItemModel *model;
    QTreeView *view;

private slots:
    void onClick(QModelIndex index);
    void onDoubleClick(QModelIndex index);
    void openInspector(QModelIndex index);

public slots:
    void createContextMenu(QPoint pos);

public:
    ObjectTreeInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

    virtual void refresh();
};

} // namespace qtenv
} // namespace omnetpp

#endif // OBJECTTREEINSPECTOR_H
