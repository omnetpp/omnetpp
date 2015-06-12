//==========================================================================
//  GRAPHICSSCENE.CC - part of
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

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include "graphicsscene.h"

#include <QDebug>

ModuleInspectorScene::ModuleInspectorScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

void ModuleInspectorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsItem *item = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), QTransform());
    //TODO click to connection
    if(item == nullptr)
    {
        qDebug() << "mouseDoubleClickEvent: item is null";
        return;
    }
//    proc ModuleInspector:dblClick insp {
//       set c $insp.c
//       set item [$c find withtag current]
//       set tags [$c gettags $item]

//       set ptr ""
//       if {[lsearch $tags "ptr*"] != -1} {
//          regexp "ptr.*" $tags ptr
//       }

//       if {$ptr!=""} {
//          inspector:dblClick $insp $ptr
//       }
//    }
}
