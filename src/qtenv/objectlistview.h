//==========================================================================
//  OBJECTLISTVIEW.H - part of
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

#ifndef __OMNETPP_QTENV_OBJECTLISTVIEW_H
#define __OMNETPP_QTENV_OBJECTLISTVIEW_H

#include <QtWidgets/QTableView>
#include "qtenvdefs.h"

namespace omnetpp {

class cObject;

namespace qtenv {

class ObjectListModel;

class QTENV_API ObjectListView : public QTableView
{
    Q_OBJECT
private:
    bool isSetColumnWidth;
    cObject *object;

protected:
    void contextMenuEvent(QContextMenuEvent *event);

public:
    ObjectListView(QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model) override;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_OBJECTLISTVIEW_H
