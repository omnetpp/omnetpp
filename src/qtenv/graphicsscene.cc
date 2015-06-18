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
#include "qtenv.h"
#include "moduleinspector.h"

#include <QDebug>

ModuleInspectorScene::ModuleInspectorScene(qtenv::ModuleInspector *inspector, QObject *parent) :
    QGraphicsScene(parent),
    insp(inspector)
{
}

void ModuleInspectorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsItem *item = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), QTransform());
    // TODO click to connection
    if (item == nullptr) {
        qDebug() << "mouseDoubleClickEvent: item is null";
        return;
    }

    // Modules
    QVariant variant = item->data(1);
    cObject *object = nullptr;
    if (variant.isValid())
        object = variant.value<cObject *>();

    if (object == nullptr)
        return;  // TODO error

    if (insp->supportsObject(object))  // TODO && $config(reuse-inspectors)
        insp->setObject(object);
    else {}  // TODO opp_inspect $ptr
}

