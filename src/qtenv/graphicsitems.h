//==========================================================================
//  graphicsitems.h - part of
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

#ifndef __OMNETPP_QTENV_GRAPHICSITEMS_H
#define __OMNETPP_QTENV_GRAPHICSITEMS_H

#include <QGraphicsObject>
#include <QGraphicsEffect>
#include <QFont>
#include <QTimer>
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

// these are the custom data "slot" indices used in QGraphicsItems
constexpr int ITEMDATA_COBJECT = 1;
// see modulecanvasviewer.cc for why this is necessary, and setToolTip isn't usable
constexpr int ITEMDATA_TOOLTIP = 2;

class QTENV_API ArrowheadItem : public QGraphicsPolygonItem
{
    double arrowWidth = 6;
    // This will offset the tail sideways. -1 is fully left, 1 is fully right.
    double arrowSkew = 0;
    double arrowLength = 4;
    double fillRatio = 0.75;

    void updatePolygon();

public:
    ArrowheadItem(QGraphicsItem *parent);

    QPainterPath shape() const override;

    // Sets the size of the arrow so it fits
    // a line of penWidth width well.
    void setSizeForPenWidth(double penWidth, double scale = 1.0, double addSize = 10);

    void setEndPoints(const QPointF &start, const QPointF &end, double addAngle = 0);

    void setArrowWidth(double width);
    void setArrowLength(double length);
    void setArrowSkew(double skew);
    void setFillRatio(double ratio);
    void setColor(const QColor &color);
    void setLineWidth(double width);
};

// does something similar to QGraphicsColorizeEffect,
// but in a way that matches the Tkenv colorization,
// and also preserves smooth scaling of pixmaps
class QTENV_API ColorizeEffect : public QGraphicsEffect {
    QColor color = QColor(255, 0, 0); // alpha is ignored
    double weight = 0; // [0..1]

    // if true, chachedImage has to be recomputed before the next draw
    bool changed = true;
    // we store the colorized pixels here, so we won't have to colorize every frame
    QImage cachedImage;
    QPoint offset;
    bool smooth = false;

public:
    QColor getColor() { return color; }
    void setColor(const QColor &color);

    double getWeight() { return weight; }
    void setWeight(double weight);

    bool getSmooth() { return smooth; }
    void setSmooth(double smooth);

protected:
    void draw(QPainter *painter) override;
    void sourceChanged(ChangeFlags flags) override;
};


// used in the ModuleInspector and some related classes
class QTENV_API GraphicsLayer : public QGraphicsObject {
    Q_OBJECT

public:
    QRectF boundingRect() const override { return QRectF(); /* doesn't matter */ }
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override { /* nothing here... */ }
    void addItem(QGraphicsItem *item) { item->setParentItem(this); }
    void removeItem(QGraphicsItem *item) { item->setParentItem(nullptr); }
    void clear();
};


// Used in the ModuleInspector to make text more readable on a cluttered background.
// If the functionality is not enough, just implement more of QGraphicsSimpleTextItems
// functions, and forward them to one or both members accordingly.
// The occasional small "offset" of the outline relative to the text
// itself (mostly with small fonts) is likely caused by font hinting.
class QTENV_API OutlinedTextItem : public QGraphicsItem {
protected:
    // these are NOT PART of the scene, not even children of this object
    // we just misuse them in the paint method
    QGraphicsSimpleTextItem *outlineItem; // never has a Brush
    QGraphicsSimpleTextItem *fillItem; // never has a Pen

    QBrush backgroundBrush;
    bool haloEnabled = true;

    QPointF offset;

public:
    OutlinedTextItem(QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
    virtual ~OutlinedTextItem();

    QRectF boundingRect() const;
    QRectF textRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QFont font() const { return fillItem->font(); }
    void setFont(const QFont &font);
    void setText(const QString &text);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setBackgroundBrush(const QBrush &brush);
    void setOffset(const QPointF& offset);
    void setHaloEnabled(bool enabled);
};

// Label in the bottom right corner that display zoom factor
class QTENV_API ZoomLabel : public QGraphicsSimpleTextItem
{
private:
    double zoomFactor;

protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

public:
    ZoomLabel() { setText("ZoomLabel"); }

    void setZoomFactor(double zoomFactor);
};

// XXX: Why not QGraphicsPathItem ?
class QTENV_API BubbleItem : public QGraphicsObject {
    Q_OBJECT

protected:

    // the distance between the tip of the handle, and the bottom of the text bounding rectangle
    // includes the bottom margin (the yellow thingy will begin a margin size lower than this)
    static constexpr double vertOffset = 15;
    static constexpr double margin = 3; // also acts as corner rounding radius

    QString text;

    QPainterPath path;
    bool pathBuilt = false;

    void timerEvent(QTimerEvent *event) override { delete this;  /* BOOM! */ }

public:
    BubbleItem(QPointF position, const QString &text, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

// This item has the ability to apply a transformation to itself
// without affecting the coordinate system of its children.
// It is needed to make image figures scale with different ratios
// in both dimensions without it affecting their children.
// This is in combined with the regular, inherited transformation.
class QTENV_API SelfTransformingPixmapItem : public QGraphicsPixmapItem {
    QTransform selfTransform;

public:
    using QGraphicsPixmapItem::QGraphicsPixmapItem;

    // needed to avoid the internal Qt fast path for painting pixmap
    // type items that made paint() useless (it was not called at all)
    int type() const override { return UserType; }

    void setSelfTransform(const QTransform& transform);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

}  // namespace qtenv
}  // namespace omnetpp


#endif // __OMNETPP_QTENV_GRAPHICSITEMS_H
