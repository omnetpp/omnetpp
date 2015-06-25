//==========================================================================
//  GENERICOBJECTTREEMODEL.H - part of
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

#ifndef __OMNETPP_QTENV_GENERICOBJECTTREEMODEL_H
#define __OMNETPP_QTENV_GENERICOBJECTTREEMODEL_H

#include <QAbstractItemModel>
#include <QMenu>
#include <QTreeView>
#include "qtenvdefs.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cclassdescriptor.h"

namespace omnetpp {
namespace qtenv {

class TreeNode;

// this is wrapped in a QVariant to be returned by the model
// when the UserRole data is requested, so the itemdelegate
// of the TreeView can highlight a section in the string
struct HighlightRange {
    int start; // the index of the first highlighted character
    int length; // the number of highlighted characters
};

// encapsulates the tree model, handles QModelIndexes, etc
class GenericObjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    TreeNode *rootNode;

public:
    explicit GenericObjectTreeModel(cObject *object, QObject *parent = nullptr);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    ~GenericObjectTreeModel();
};

} // namespace qtenv
} // namespace omnetpp

// this is needed to wrap the HighlightRange into a QVariant
// according to the Qt docs, this must not be in a namespace
Q_DECLARE_METATYPE(omnetpp::qtenv::HighlightRange);

#endif // __OMNETPP_QTENV_GENERICOBJECTTREEMODEL_H
