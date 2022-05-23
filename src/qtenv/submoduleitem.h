//==========================================================================
//  SUBMODULEITEM.H - part of
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

#ifndef __OMNETPP_QTENV_SUBMODULEITEM_H
#define __OMNETPP_QTENV_SUBMODULEITEM_H

#include <QtWidgets/QGraphicsColorizeEffect>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QAbstractGraphicsShapeItem>
#include "qtenvdefs.h"

namespace omnetpp {

class cModule;
class cDisplayString;

namespace qtenv {

class OutlinedTextItem;
class MultiLineOutlinedTextItem;
class GraphicsLayer;

class SubmoduleItem;
class QTENV_API SubmoduleItemUtil {
public:
    static void setNameLabel(SubmoduleItem *si, cModule *mod);
    static void setupFromDisplayString(SubmoduleItem *si, cModule *mod);
    static void updateQueueSizeLabel(SubmoduleItem *si, cModule *mod);
};


class QTENV_API SubmoduleItem : public QGraphicsObject
{
    Q_OBJECT

    // if the bounding rectangle of the shape and the image would be
    // smaller than this in any dimension, this is used instead
    static constexpr double minimumRectSize = 10;

protected:
    cModule *module; // the submodule

    // Sets the text of the name item, appending the
    // vector index to it if any, and center-aligns it.
    void updateNameItem();

    // Realigns the queue length label, the info text, and the
    // decoration icon. Call this every time after the size
    // of the image or shape changes. Also updates the name.
    void realignAnchoredItems();
    void updateShapeItem();
    void adjustRangeItem(int i);
    // whichever is bigger in each direction, has a minimum size, see above.
    // assumed to be always centered around the origin.
    QRectF shapeImageBoundingRect() const;

protected Q_SLOTS:
    void onPositionChanged(); // keeping the range items underneath ourselves

public:
    static const char *const DEFAULT_ICON;

    enum Shape {
        SHAPE_NONE,
        SHAPE_OVAL,
        SHAPE_RECT
    };

    enum NameFormat {
        FMT_FULLNAME,
        FMT_QDISPLAYNAME, // quoted displayname
        FMT_FULLNAME_AND_QDISPLAYNAME,
    };

    enum TextPos {
        TEXTPOS_LEFT,
        TEXTPOS_RIGHT,
        TEXTPOS_TOP
    };

    struct RangeData {
        double radius;
        double outlineWidth;
        QColor fillColor; // this will get appied with halved alpha, no need to correct it here
        QColor outlineColor;
    };

protected:
    double zoomFactor = 1;
    double imageSizeFactor = 1;
    Shape shape = SHAPE_NONE;
    double shapeWidth = 0; // zero if unspec
    double shapeHeight = 0; // zero if unspec
    QColor shapeFillColor;
    QColor shapeOutlineColor;
    double shapeOutlineWidth = 2;
    NameFormat nameFormat = FMT_FULLNAME_AND_QDISPLAYNAME;
    QString text;
    TextPos textPos = TEXTPOS_TOP;
    QColor textColor;
    QString queueText;

    QList<RangeData> ranges; // these two lists must be in sync at all times
    QList<QGraphicsEllipseItem *> rangeItems;

    QAbstractGraphicsShapeItem *shapeItem = nullptr;
    QGraphicsPixmapItem *imageItem = nullptr;
    QGraphicsPixmapItem *decoratorImageItem = nullptr;

    OutlinedTextItem *nameItem; // includes the vector index
    OutlinedTextItem *queueItem;
    MultiLineOutlinedTextItem *textItem;

    GraphicsLayer *rangeLayer = nullptr;

public:
    SubmoduleItem(cModule *mod, GraphicsLayer *rangeLayer);
    virtual ~SubmoduleItem();

    void setZoomFactor(double zoom);
    void setImageSizeFactor(double imageSize);

    void setShape(Shape shape);
    void setWidth(double width);
    void setHeight(double height);
    void setFillColor(const QColor &color);
    void setOutlineColor(const QColor &color);
    void setOutlineWidth(double width);

    void setIcon(QPixmap icon);
    void setDecoratorIcon(QPixmap icon);

    void setNameFormat(NameFormat format);
    void setNameVisible(bool visible);
    void setQueueText(const QString &queueText);
    void setInfoText(const QString &text, TextPos pos, const QColor &color);

    void setRangeLayer(GraphicsLayer *layer);

    // these return the index of the newly created range, for future reference
    int addRangeItem(double radius, double outlineWidth, const QColor &fillColor, const QColor &outlineColor);
    int addRangeItem(const RangeData &data);

    void clearRangeItems();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_SUBMODULEITEM_H
