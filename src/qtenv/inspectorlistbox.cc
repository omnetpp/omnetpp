//==========================================================================
//  INSPECTORLISTBOX.CC - part of
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

#include "inspectorlistbox.h"
#include "qtenv.h"

#include <QDebug>

namespace omnetpp {
namespace qtenv {

QModelIndex InspectorListBox::index(int row, int column, const QModelIndex&) const
{
    return objects.empty()
            ? QModelIndex()
            : createIndex(row, column, objects[row]);
}

QVariant InspectorListBox::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QString titles[] = {"Class", "Name", "Info"};
    return (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            ? titles[section]
            : QVariant();
}

void InspectorListBox::sort(int i, Qt::SortOrder order)
{
    lastSortColumn = i;
    lastSortOrder = order;

    beginResetModel();
    qSort(objects.begin(), objects.end(),
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

QVariant InspectorListBox::data(const QModelIndex& index, int role) const
{
    if (objects.size() < index.row())
        return QVariant();

    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case 0:
                    return getObjectShortTypeName(objects[index.row()]);
                case 1:
                    return objects[index.row()]->getFullPath().c_str();
                case 2:
                    return objects[index.row()]->str().c_str();
            }
            break;
        case Qt:: DecorationRole:
            return index.column() == 0
                    ? QVariant::fromValue(QIcon(":/objects/icons/objects/" + getObjectIcon(objects[index.row()])))
                    : QVariant();
        case Qt::UserRole:
            return QVariant::fromValue(objects[index.row()]);
    }

    return QVariant();
}

void InspectorListBox::setObjects(QVector<cObject *> objects)
{
    beginResetModel();
    this->objects = objects;
    endResetModel();
    sort(lastSortColumn, lastSortOrder);
}

}  // namespace qtenv
}  // namespace omnetpp

