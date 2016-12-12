//==========================================================================
//  INSPECTORLISTBOXVIEW.CC - part of
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

#include "inspectorlistboxview.h"
#include "inspectorlistbox.h"
#include "qtenv.h"
#include "inspectorutil.h"
#include "mainwindow.h"
#include <QHeaderView>
#include <QKeyEvent>
#include <QMenu>

#include <QDebug>

#define emit

namespace omnetpp {
namespace qtenv {

InspectorListBoxView::InspectorListBoxView(QWidget *parent)
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

void InspectorListBoxView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);

    // factory defaults
    setColumnWidth(0, 120);
    setColumnWidth(1, 250);
    setColumnWidth(2, 600);
}

void InspectorListBoxView::contextMenuEvent(QContextMenuEvent *event)
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

