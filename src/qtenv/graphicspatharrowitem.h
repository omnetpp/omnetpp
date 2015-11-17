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
public:
    enum ArrowStyle {BARBED, TRIANGLE, SIMPLE};

private:
    static const int PTS_IN_ARROW = 4;

    typedef struct ArrowDescr
    {
        double arrowLength;         /* Length of arrowhead. */
        double arrowWidth;          /* width of arrowhead. */
        double arrowFillRatio;      /* filled part of arrow head, relative to arrowLengthRel.
                                     * 0: special case, arrowhead only 2 line, without fill */
        QVector<QPointF> arrowPointsPtr;  /* Points to array of PTS_IN_ARROW points
                                     * describing polygon for arrowhead in line.
                                     * NULL means no arrowhead at current point. */
    } ArrowDescr;

    //-------------------------------------------------------------------------------------------

    ArrowDescr arrowDescr;
    QPointF first, last;
    int arrowStyle;

    void arrowDescrInit();
    QPolygonF configureArrow(const QPointF &pf, const QPointF &pl) const;
    QPolygonF getTranslatePoints() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

public:
    GraphicsPathArrowItem(QGraphicsItem *parent = nullptr);

    void setEndPoints(const QPointF &pf, const QPointF &pl);
    void setArrowStyle(const int style) { arrowStyle = style; }

    QRectF boundingRect() const;

};

#endif // __OMNETPP_QTENV_GRAPHICSPATHARROWITEM_H
