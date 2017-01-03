//==========================================================================
//  TREEITEMMODEL.H - part of
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

#ifndef __OMNETPP_QTENV_TREEITEMMODEL_H
#define __OMNETPP_QTENV_TREEITEMMODEL_H

#include <QAbstractItemModel>
#include <QMenu>
#include <QTreeView>
#include "qtenvdefs.h"

class QMainWindow;

namespace omnetpp {

class cObject;

namespace qtenv {

class QTENV_API TreeItemModel : public QAbstractItemModel
{
    Q_OBJECT

    cObject *rootNode;

    int findObjectInParent(cObject *obj, cObject *parent) const;

public:
    explicit TreeItemModel(QObject *parent = 0);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex&) const{ return 1; }

    // if the role is Qt::UserRole, the internal cObject pointer is returned in the QVariant
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    // collects the cObject pointers of the nodes that are expanded in the view into the list
    // the idx is used for recursion, and should be omitted to traverse the whole model
    void getExpandedItems(QTreeView *view, QList<QVariant> &list, QModelIndex idx = QModelIndex());

    // expands the nodes int the view whose cObject pointers are found in the list
    // the idx is used for recursion, and should be omitted to traverse the whole model
    void expandItems(QTreeView *view, const QList<QVariant> &list, QModelIndex idx = QModelIndex());

    void setRootObject(cObject *root);
    cObject *getObjectFromIndex(const QModelIndex &index) const;
};

} // namespace qtenv
} // namespace omnetpp

#endif
