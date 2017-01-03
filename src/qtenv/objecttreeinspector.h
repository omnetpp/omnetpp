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
#include <QModelIndex>

class QTreeView;

namespace omnetpp {
namespace qtenv {

class TreeItemModel;

class QTENV_API ObjectTreeInspector : public Inspector
{
    Q_OBJECT
private:
    TreeItemModel *model;
    QTreeView *view;

private slots:
    void onClick(QModelIndex index);
    void onDoubleClick(QModelIndex index);

public slots:
    void createContextMenu(QPoint pos);

public:
    ObjectTreeInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

    virtual void refresh();
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_OBJECTTREEINSPECTOR_H
