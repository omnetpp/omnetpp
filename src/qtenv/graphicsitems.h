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

#include <cmath> // for NAN
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsEffect>
#include <QtGui/QFont>
#include <QtGui/QPen>
#include <QtCore/QTimer>
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

// these are the custom data "slot" indices used in QGraphicsItems
constexpr int ITEMDATA_COBJECT = 1;
// see modulecanvasviewer.cc for why this is necessary, and setToolTip isn't usable
constexpr int ITEMDATA_TOOLTIP = 2; // not used for figures anymore!

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
    void setSizeForPenWidth(double penWidth, double zoom = 1.0, double addSize = 8.0);

    void setEndPoints(const QPointF &start, const QPointF &end, double addAngle = 0);

    void setArrowWidth(double width);
    void setArrowLength(double length);
    void setArrowSkew(double skew);
    void setFillRatio(double ratio);
    void setColor(const QColor &color);
    void setLineWidth(double width);
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


// Used in the ModuleInspector to make text more readable on a cluttered background,
// as well as on the canvas to implement the halo option for label and text figures.
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

    QBrush bgBrush;
    bool haloEnabled = true;

public:
    OutlinedTextItem(QGraphicsItem *parent = nullptr);
    virtual ~OutlinedTextItem();

    QRectF boundingRect() const;
    QRectF textRect() const; // this would be the bounding box without the outline

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QFont font() const { return fillItem->font(); }
    void setFont(const QFont &font);
    QString text() const { return fillItem->text(); }
    void setText(const QString &text);
    QPen pen() const { return outlineItem->pen(); }
    void setPen(const QPen &pen);
    QBrush brush() const { return fillItem->brush(); }
    void setBrush(const QBrush &brush);
    QBrush backgroundBush() const { return bgBrush; }
    void setBackgroundBrush(const QBrush &brush);
    bool isHaloEnabled() const { return haloEnabled; }
    void setHaloEnabled(bool enabled);
};

class QTENV_API MultiLineOutlinedTextItem : public QGraphicsItem {
protected:

    Qt::Alignment alignment = Qt::AlignLeft; // only Left, Center and Right are accepted.

    // This needs to be a vector only so we can position the lines independently,
    // to left/middle/right align the text for the different positioning modes.
    // QGraphicsSimpleTextItem can't align its lines, while QGraphicsTextItem
    // makes it hard to draw the outline. So we do it manually, no big deal.
    // This MUST always contain at least one element, which is used to hold
    // the desired style info (font, colors, halo, etc.).
    std::vector<OutlinedTextItem *> textItems;

    void realignLines();

public:
    MultiLineOutlinedTextItem(QGraphicsItem *parent = nullptr) : QGraphicsItem(parent) { setText(""); /* just to ensure at least one line item */ }

    QRectF boundingRect() const override { return childrenBoundingRect(); } // just to make positioning calculations easier
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override { /* nothing */ }

    QFont font() const { return textItems.front()->font(); }
    QRectF textRect() const;

    void setText(const QString &text);
    void setAlignment(Qt::Alignment align);
    void setFont(const QFont &font);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setBackgroundBrush(const QBrush &brush);
    void setHaloEnabled(bool enabled);
};

// Label in the bottom right corner that display zoom factor
class QTENV_API ZoomLabel : public QGraphicsSimpleTextItem
{
private:
    double zoomFactor = NAN;

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

}  // namespace qtenv
}  // namespace omnetpp


#endif // __OMNETPP_QTENV_GRAPHICSITEMS_H
