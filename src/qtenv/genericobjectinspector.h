//==========================================================================
//  GENERICOBJECTINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_GENERICOBJECTINSPECTOR_H
#define __OMNETPP_QTENV_GENERICOBJECTINSPECTOR_H

#include "inspector.h"
#include "genericobjecttreemodel.h"

class QLabel;
class QTreeView;
class QTabWidget;
class QToolBar;

namespace omnetpp {
namespace qtenv {

class InspectorListBox;

class QTENV_API GenericObjectInspector : public Inspector
{
    Q_OBJECT

protected:
      QLabel *icon, *name; // the top row
      QTabWidget *tabs;
      QTreeView *treeView;
      GenericObjectTreeModel *model;
      InspectorListBox *listModel;

      void mousePressEvent(QMouseEvent *) override;
      QToolBar *createToolbar();

protected slots:
      void onTreeViewActivated(QModelIndex index);
      void onListActivated(QModelIndex index);
      void onDataChanged();
      void createContextMenu(QPoint pos);

public:
      GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
      ~GenericObjectInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void refresh() override;
};

} // namespace qtenv
} // namespace omnetpp

#endif

