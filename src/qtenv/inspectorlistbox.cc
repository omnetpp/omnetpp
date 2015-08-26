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

int InspectorListBox::rowCount(const QModelIndex &parent) const
{
    return objects.size();
}

int InspectorListBox::columnCount(const QModelIndex &parent) const
{
    return 3;
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
    beginResetModel();
    // qDebug() << "Sort" << order;
    qSort(objects.begin(), objects.end(),
          [i, order](cObject *arg1, cObject *arg2) -> bool
          {
              QString first, second;
              switch(i)
              {
                  case 0:
                      first = getObjectShortTypeName(arg1);
                      second = getObjectShortTypeName(arg2);
                      return order == Qt::SortOrder::AscendingOrder ?
                                 first.toLower() < second.toLower() :
                                 first.toLower() > second.toLower();
                  case 1:
                      first = arg1->getFullPath().c_str();
                      second = arg2->getFullPath().c_str();
                      return order == Qt::SortOrder::AscendingOrder ?
                                 first.toLower() < second.toLower() :
                                 first.toLower() > second.toLower();
                  case 2:
                      first = arg1->info().c_str();
                      second = arg2->info().c_str();
                      return order == Qt::SortOrder::AscendingOrder ?
                                 first.toLower() < second.toLower() :
                                 first.toLower() > second.toLower();
                  case 3:
                      char buffer1[50];
                      char buffer2[50];
                      sprintf(buffer1, "%p", (void *)arg1);
                      sprintf(buffer2, "%p", (void *)arg2);
                      first = buffer1;
                      second = buffer2;
                      return order == Qt::SortOrder::AscendingOrder ?
                                 first.toLower() < second.toLower() :
                                 first.toLower() > second.toLower();
              };
              return false;
          }
    );
    endResetModel();
}

QVariant InspectorListBox::data(const QModelIndex &index, int role) const
{
    if(objects.size() < index.row())
    {
        // qDebug() << "object is nullptr";
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QString label;
        switch(index.column())
        {
            case 0:
                label = getObjectShortTypeName(objects[index.row()]);
                break;
            case 1:
                label = objects[index.row()]->getFullPath().c_str();
                break;
            case 2:
                label = objects[index.row()]->info().c_str();
                break;
            case 3:
                char buffer[50];
                sprintf(buffer, "%p", (void *)objects[index.row()]);
                label = buffer;
                break;
        }
        return QVariant::fromValue(label);
    } else if (role == Qt::DecorationRole && index.column() == 0) {
        return QVariant::fromValue(QIcon(":/objects/icons/objects/" + getObjectIcon(objects[index.row()])));
    } else if (role == Qt::UserRole)
        return QVariant::fromValue(objects[index.row()]);

    return QVariant();
}

void InspectorListBox::setObjects(QVector<cObject*> objects)
{
    beginResetModel();
    this->objects = objects;
    endResetModel();
}

} // namespace qtenv
} // namespace omnetpp
