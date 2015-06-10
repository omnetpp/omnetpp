//==========================================================================
//  GRAPHICSPATHARROWITEM.H - part of
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

#ifndef GRAPHICSPATHARROWITEM_H
#define GRAPHICSPATHARROWITEM_H

#include <QGraphicsPathItem>

class GraphicsPathArrowItem : public QGraphicsPathItem
{
public:
    GraphicsPathArrowItem() {}

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

#endif // GRAPHICSPATHARROWITEM_H
