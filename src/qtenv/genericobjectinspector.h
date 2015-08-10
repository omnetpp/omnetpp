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

class QTreeView;

namespace omnetpp {
namespace qtenv {

class QTENV_API GenericObjectInspector : public Inspector
{
    Q_OBJECT

protected:
      QTreeView *treeView;
      GenericObjectTreeModel *model;

protected slots:
      void onTreeViewActivated(QModelIndex index);

public:
      GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
      ~GenericObjectInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void refresh() override;
};

} // namespace qtenv
} // namespace omnetpp

#endif

