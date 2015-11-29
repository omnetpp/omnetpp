//==========================================================================
//  INSPECTORLISTBOX.H - part of
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

#ifndef __OMNETPP_QTENV_INSPECTORLISTBOX_H
#define __OMNETPP_QTENV_INSPECTORLISTBOX_H

#include <QAbstractTableModel>
#include <QVector>

namespace omnetpp {

class cObject;

namespace qtenv {

class InspectorListBox : public QAbstractTableModel
{
    Q_OBJECT
private:
    QVector<cObject*> objects;

public slots:
    void sort(int i, Qt::SortOrder order);

public:
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void setObjects(QVector<cObject *> objects);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_INSPECTORLISTBOX_H
