//==========================================================================
//  OBJECTLISTVIEW.CC - part of
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

#include "objectlistview.h"
#include "objectlistmodel.h"
#include "qtenv.h"
#include "inspectorutil.h"
#include "mainwindow.h"
#include <QtWidgets/QHeaderView>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QMenu>

#include <QtCore/QDebug>

namespace omnetpp {
namespace qtenv {

ObjectListView::ObjectListView(QWidget *parent)
    : QTableView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    horizontalHeader()->setStretchLastSection(true);
    setHorizontalScrollMode(ScrollPerPixel);

    verticalHeader()->setVisible(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    setShowGrid(false);
    setSortingEnabled(true);
    setWordWrap(false);
}

void ObjectListView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);

    // factory defaults
    setColumnWidth(0, 120);
    setColumnWidth(1, 250);
    setColumnWidth(2, 600);
}

void ObjectListView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = selectionModel()->currentIndex();
    if (!index.isValid())
        return;

    QVariant variant = index.data(Qt::UserRole);
    object = variant.isValid() ? variant.value<cObject *>() : nullptr;

    if (!object)
        return;

    // TODO: proper protection against dereferencing dangling pointers!
    variant = getQtenv()->getPref("FindObjectsDialog/outofdate");
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    bool state = (variant.isValid() ? variant.value<bool>() : true) || mainWindow->isRunning();

    QMenu *menu = InspectorUtil::createInspectorContextMenu(object);
    menu->setDisabled(state);

    menu->exec(event->globalPos());

    delete menu;
}

}  // namespace qtenv
}  // namespace omnetpp

