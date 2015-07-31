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

InspectorListBoxView::InspectorListBoxView()
{
    horizontalHeader()->setStretchLastSection(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

InspectorListBoxView::~InspectorListBoxView()
{
    QString widths = "";
    for(int i = 0; i < model()->columnCount(); ++i)
        widths += QString::number(columnWidth(i)) + "#";
    getQtenv()->setPref("objdialog:columnwidths", widths);
}

void InspectorListBoxView::setModel(InspectorListBox *model)
{
    connect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), model, SLOT(sort(int,Qt::SortOrder)));
    connect(this, SIGNAL(clicked(QModelIndex)), model, SLOT(selectionChanged(QModelIndex)));
    QTableView::setModel(model);

    QVariant variant = getQtenv()->getPref("objdialog:columnwidths");
    if(variant.isValid())
    {
        QString widths = variant.value<QString>();
        QStringList columnWidths = widths.split("#");
        for(int i = 0; i < columnWidths.size(); ++i)
            setColumnWidth(i, columnWidths[i].toInt());
    }
}

void InspectorListBoxView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();
        if(selectedIndexes.size() > 0)
            emit pressEnter(selectedIndexes[0]);
    }

    QTableView::keyPressEvent(event);
}

void InspectorListBoxView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = selectionModel()->currentIndex();
    if(!index.isValid())
        return;

    QVariant variant = index.data(Qt::UserRole);
    object = variant.isValid() ? variant.value<cObject*>() : nullptr;

    if(!object)
        return;

    QVector<int> inspTypes = InspectorUtil::supportedInspTypes(object);

    variant = getQtenv()->getPref("outofdate");
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    bool state = (variant.isValid() ? variant.value<bool>() : true) || mainWindow->isRunning();

    QMenu menu;
    for(int type : inspTypes)
    {
        QString label = InspectorUtil::getInspectMenuLabel(type);
        QAction *action = menu.addAction(label, this, SLOT(contextMenuItemTriggerd()));
        action->setDisabled(state);
        action->setData(QVariant::fromValue(type));
    }

    menu.exec(event->globalPos());
}

void InspectorListBoxView::contextMenuItemTriggerd()
{
    QAction *action = static_cast<QAction*>(sender());
    QVariant variant = action->data();
    if(!variant.isValid())
        return;


    getQtenv()->inspect(object, variant.value<int>(), true, "");
}

} // namespace qtenv
} // namespace omnetpp
