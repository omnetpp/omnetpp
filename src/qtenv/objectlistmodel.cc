//==========================================================================
//  OBJECTLISTMODEL.CC - part of
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

#include "objectlistmodel.h"

#include <algorithm>

#include <QtCore/QDebug>

#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

QModelIndex ObjectListModel::index(int row, int column, const QModelIndex&) const
{
    return objects.empty()
            ? QModelIndex()
            : createIndex(row, column, objects[row]);
}

QVariant ObjectListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QString titles[] = {"Class / NED Type", "Full Path", "Info"};
    return (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            ? titles[section]
            : QVariant();
}

void ObjectListModel::sort(int i, Qt::SortOrder order)
{
    lastSortColumn = i;
    lastSortOrder = order;

    beginResetModel();
    std::sort(objects.begin(), objects.end(),
          [i, order](cObject *arg1, cObject *arg2) -> bool {
              QString first, second;
              switch(i) {
                  case 0:
                      first = getObjectShortTypeName(arg1);
                      second = getObjectShortTypeName(arg2);
                      break;
                  case 1:
                      first = arg1->getFullPath().c_str();
                      second = arg2->getFullPath().c_str();
                      break;
                  case 2:
                      first = arg1->str().c_str();
                      second = arg2->str().c_str();
                      break;
              };
              return order == Qt::SortOrder::AscendingOrder ?
                         first.toLower() < second.toLower() :
                         first.toLower() > second.toLower();
          }
    );
    endResetModel();
}

void ObjectListModel::removeObject(cObject *object)
{
    for (int i = 0; i < objects.size(); ++i) {
        if (objects[i] == object) {
            beginRemoveRows(QModelIndex(), i, i);
            objects.remove(i, 1);
            endRemoveRows();
            --i;
        }
    }
}

QVariant ObjectListModel::data(const QModelIndex& index, int role) const
{
    if (objects.size() < index.row())
        return QVariant();

    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case 0:
                    return getObjectFullTypeName(objects[index.row()]);
                case 1:
                    return objects[index.row()]->getFullPath().c_str();
                case 2:
                    return objects[index.row()]->str().c_str();
            }
            break;
        case Qt:: DecorationRole:
            return index.column() == 0
                    ? getObjectIcon(objects[index.row()])
                    : QVariant();
        case Qt::UserRole:
            return QVariant::fromValue(objects[index.row()]);
    }

    return QVariant();
}

void ObjectListModel::setObjects(QVector<cObject *> objects)
{
    beginResetModel();
    this->objects = objects;
    endResetModel();
    sort(lastSortColumn, lastSortOrder);
}

}  // namespace qtenv
}  // namespace omnetpp

