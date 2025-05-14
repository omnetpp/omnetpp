//==========================================================================
//  COMPOUNDMODULEITEM.H - part of
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

#ifndef __OMNETPP_QTENV_COMPOUNDMODULEITEM_H
#define __OMNETPP_QTENV_COMPOUNDMODULEITEM_H

#include <QtCore/QString>
#include <QtCore/QPointF>
#include <QtGui/QColor>
#include <QtWidgets/QGraphicsObject>
#include "qtenvdefs.h"
#include "qtutil.h"

namespace omnetpp {

class cModule;

namespace qtenv {

class OutlinedTextItem;

class CompoundModuleItem;
class QTENV_API CompoundModuleItemUtil {
public:
    static void setupFromDisplayString(CompoundModuleItem *cmi, cModule *mod, double zoomFactor, QRectF submodulesRect);
};

class QTENV_API CompoundModuleItem : public QGraphicsObject
{
    Q_OBJECT

public:
    enum ImageMode {
        MODE_CENTER,
        MODE_STRETCH,
        MODE_TILE,
        MODE_FIX
    };

    struct TextData {
        QPointF position;
        QString text;
        QColor color;
    };

protected:
    double zoomFactor = 1;
    QRectF area; // the whole inner region of the module, not including the border outline
    QColor backgroundColor = colors::GREY82;
    QColor outlineColor = colors::BLACK;
    double outlineWidth = 2;
    QImage *image = nullptr;
    ImageMode imageMode = MODE_FIX;
    double gridMajorDistance = -1;
    int gridMinorNum = 0;
    QColor gridColor = colors::GREY;

    QGraphicsRectItem *moduleRectangleItem; // That big grey thing in the back with black sides.
    QGraphicsPixmapItem *imageItem = nullptr; // not used in tiling mode
    // This rectangle is an intermediate item, it's only visible
    // in tiling mode, otherwise it's used to clip the pixmap item
    // to fit into the inside of the border rectangle.
    QGraphicsRectItem *imageContainer;
    QList<QGraphicsLineItem *> gridLines;

    OutlinedTextItem *nameLabelItem;

    QList<TextData> texts;
    QList<OutlinedTextItem *> textItems;

    void updateRectangle();
    void updateImage();
    void updateGrid();

public:
    explicit CompoundModuleItem(QGraphicsItem *parent = nullptr);
    ~CompoundModuleItem();

    void setZoomFactor(double zoom);
    void setArea(QRectF area);
    QRectF getArea(); // see the area field

    void setBackgroundColor(const QColor &color);
    void setOutlineColor(const QColor &color);
    void setOutlineWidth(double width);

    void setImage(QImage *image);
    void setImageMode(ImageMode mode);

    // anything <= 0 means no grid will be drawn
    void setGridMajorDistance(double gridDistance);
    // how many parts the minor lines divide the major sections into
    // so if this is set to 5, there will be 4 minor lines between each major line
    // anything <= 1 means no minor grid lines will be drawn
    void setGridMinorNum(int minorNum);
    void setGridColor(const QColor &color);

    void setNameLabel(const QString &text);
    void setNameLabelTooltip(const QString &text);

    // these return the newly added item index for possible future reference
    int addText(const TextData &data);
    int addText(const QPointF &pos, const QString &text, const QColor &color = colors::BLACK);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_COMPOUNDMODULEITEM_H
