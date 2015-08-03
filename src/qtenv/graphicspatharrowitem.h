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

#ifndef __OMNETPP_QTENV_GRAPHICSPATHARROWITEM_H
#define __OMNETPP_QTENV_GRAPHICSPATHARROWITEM_H

#include <QGraphicsPolygonItem>

class GraphicsPathArrowItem : public QGraphicsPolygonItem
{
    // any smaller width will be clamped to this, so the arrowhead
    // will not be too small to notice even on thin lines
    const double minimumWidth = 3.0;
public:
    GraphicsPathArrowItem(QGraphicsItem *parent = nullptr);

    // The width of the line on the end of which the arrow is placed.
    // You should make the line (with a flat cap) this much shorter
    // on the arrowed end, otherwise it will poke through the arrowhead.
    void setLineWidth(double width);
};

#endif // __OMNETPP_QTENV_GRAPHICSPATHARROWITEM_H
