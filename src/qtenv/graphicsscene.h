//==========================================================================
//  GRAPHICSSCENE.H - part of
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

#ifndef __OMNETPP_QTENV_GRAPHICSSCENE_H
#define __OMNETPP_QTENV_GRAPHICSSCENE_H

#include <QGraphicsScene>
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

class ModuleInspector;

class ModuleInspectorScene : public QGraphicsScene
{
    Q_OBJECT

private:
    ModuleInspector *insp;

public:
    ModuleInspectorScene(ModuleInspector *inspector, QObject *parent = 0);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
};

} // namespace qtenv
} // namespace omnetpp

#endif // GRAPHICSSCENE_H
