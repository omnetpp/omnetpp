//==========================================================================
//  OBJECTLISTMODEL.H - part of
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

#ifndef __OMNETPP_QTENV_OBJECTLISTMODEL_H
#define __OMNETPP_QTENV_OBJECTLISTMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QVector>
#include "qtenvdefs.h"

namespace omnetpp {

class cObject;

namespace qtenv {

class QTENV_API ObjectListModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QVector<cObject*> objects;

    // I could not find a way to query the current sorting indicator of the View, nor the last sort params of the model, so...
    int lastSortColumn = 0;
    Qt::SortOrder lastSortOrder = Qt::AscendingOrder;

public slots:
    void sort(int i, Qt::SortOrder order) override;
    void removeObject(cObject *object);

public:
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override { return objects.size(); }
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 3; }
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void setObjects(QVector<cObject *> objects);

    int getLastSortColumn() { return lastSortColumn; }
    Qt::SortOrder getLastSortOrder() { return lastSortOrder; }
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_OBJECTLISTMODEL_H
