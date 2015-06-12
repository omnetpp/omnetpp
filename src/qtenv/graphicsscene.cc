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
#include "graphicsscene.h"

#include <QDebug>

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mousePressEvent(mouseEvent);
    switch(mouseEvent->button())
    {
        case Qt::MouseButton::LeftButton:
            qDebug() << "Left";
            break;
        case Qt::MouseButton::RightButton:
            qDebug() << "Right";
            break;
    }
}
