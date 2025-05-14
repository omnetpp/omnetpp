//==========================================================================
//  COMPOUNDMODULEITEM.CC - part of
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

#include "compoundmoduleitem.h"

#include "submoduleitem.h"
#include "graphicsitems.h"

#include <QtGui/QPen>
#include <QtCore/QDebug>
#include <omnetpp/cdisplaystring.h>
#include "qtenv.h"
#include "qtutil.h"
#include "displaystringaccess.h"

namespace omnetpp {
namespace qtenv {

void CompoundModuleItem::updateRectangle()
{
    moduleRectangleItem->setRect(area.adjusted(-outlineWidth / 2, -outlineWidth / 2, outlineWidth / 2, outlineWidth / 2));
    moduleRectangleItem->setBrush(backgroundColor);
    moduleRectangleItem->setPen(QPen(outlineColor, outlineWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
}

void CompoundModuleItem::updateImage()
{
    delete imageItem;
    imageItem = nullptr;
    imageContainer->setBrush(Qt::NoBrush);
    // not including the border outline
    imageContainer->setRect(area);

    if (image) {
        if (imageMode == MODE_TILE) {
            // The tiling mode is a bit of an oddball. In this case there is
            // no pixmap item, but the container item above is drawn with
            // a pixmap brush, using the required image. And that draws tiled.
            imageContainer->setBrush(
                    QPixmap::fromImage(*image)  // Must take zooming into account.
                            .scaled(image->width() * zoomFactor, image->height() * zoomFactor,
                            Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else {
            // This is the image itself, clipped by the container above, and
            // set to size/position as needed. Not used in tiling mode.
            imageItem = new QGraphicsPixmapItem(imageContainer);
            imageItem->setPixmap(QPixmap::fromImage(*image));
            // setting up some defaults
            imageItem->setOffset(-image->width() / 2.0, -image->height() / 2.0);
            imageItem->setPos(area.center());
            imageItem->setScale(zoomFactor);
            imageItem->setTransformationMode(Qt::SmoothTransformation);

            switch (imageMode) {
                case MODE_STRETCH: {
                    imageItem->setScale(1);  // undoing the regular zoom scaling
                    QTransform transf;  // Stretching to correct size.
                    transf.scale(area.width() / image->width(), area.height() / image->height());
                    imageItem->setTransform(transf);
                    break;
                }

                case MODE_TILE:
                    Q_ASSERT(false);
                    // Already handled in the other branch above, this should never execute.
                    break;

                case MODE_CENTER:
                    // the defaults above do exactly this
                    break;

                case MODE_FIX:  // fixed on the top left corner
                    // no positioning at all, please. the zoom scaling can stay.
                    imageItem->setOffset(0, 0);
                    imageItem->setPos(area.topLeft());
                    break;

                default:
                    ASSERT2(false, "unhandled image mode");
            }
        }
    }
}

void CompoundModuleItem::updateGrid()
{
    for (auto l : gridLines)
        delete l;

    gridLines.clear();

    if (gridMajorDistance <= 0)
        return;

    // XXX this code assumes that the top left corner of the
    // border rectangle is always at (0,0) on the layer
    // which is true until the bgp tag is not supported

    auto majorPen = QPen(gridColor, 0, Qt::SolidLine, Qt::FlatCap);
    auto minorPen = QPen(gridColor, 0, Qt::DashLine, Qt::FlatCap);

    int majorIndex = 1;

    while (true) {
        // the int cast and the half pixel offset is to counteract antialiasing
        double majorPos = (int)(majorIndex * gridMajorDistance * zoomFactor) - 0.5;

        if (majorPos < area.height()) {
            auto horizLine = new QGraphicsLineItem(0, majorPos, area.width(), majorPos, this);
            horizLine->setPen(majorPen);
        }
        if (majorPos < area.width()) {
            auto vertLine = new QGraphicsLineItem(majorPos, 0, majorPos, area.height(), this);
            vertLine->setPen(majorPen);
        }

        if (majorPos >= area.height() && majorPos >= area.width()) {
            break;
        }

        ++majorIndex;
    }

    if (gridMinorNum <= 1)
        return;

    int minorIndex = 1;
    while (true) {
        // just avoiding drawing a minor line over a major line
        // (otherwise if the antialiasing smears one of them, it might look ugly)
        if (minorIndex % gridMinorNum != 0) {
            // the int cast and half pixel offset is to counteract antialiasing
            double minorPos = (int)(minorIndex * gridMajorDistance * zoomFactor / gridMinorNum) - 0.5;

            if (minorPos < area.height()) {
                auto horizLine = new QGraphicsLineItem(0, minorPos, area.width(), minorPos, this);
                horizLine->setPen(minorPen);
            }

            if (minorPos < area.width()) {
                auto vertLine = new QGraphicsLineItem(minorPos, 0, minorPos, area.height(), this);
                vertLine->setPen(minorPen);
            }

            if (minorPos >= area.height() && minorPos >= area.width()) {
                break;
            }
        }

        ++minorIndex;
    }
}

CompoundModuleItem::CompoundModuleItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    moduleRectangleItem = new QGraphicsRectItem(this);
    moduleRectangleItem->setZValue(-2);

    imageContainer = new QGraphicsRectItem(this);
    imageContainer->setZValue(-1);
    imageContainer->setPen(Qt::NoPen);
    imageContainer->setBrush(Qt::NoBrush);
    // this makes it clip the child image
    imageContainer->setFlag(QGraphicsItem::ItemClipsChildrenToShape);

    // grid lines will be at Z = 0

    nameLabelItem = new OutlinedTextItem(this);
    // just moving it off the left border outline
    nameLabelItem->setPos(area.topLeft() + QPointF(2, 2));
    nameLabelItem->setZValue(1);

    // text items will be at Z = 2
}

CompoundModuleItem::~CompoundModuleItem()
{
    for (auto l : gridLines)
        delete l;
    delete moduleRectangleItem;
    delete imageItem;
    delete imageContainer;
    delete nameLabelItem;
}

void CompoundModuleItem::setZoomFactor(double zoom)
{
    if (zoomFactor != zoom) {
        zoomFactor = zoom;
        updateRectangle();
        updateImage();
        updateGrid();
    }
}

void CompoundModuleItem::setArea(QRectF area)
{
    if (this->area != area) {
        this->area = area;
        updateRectangle();
        updateImage();
        updateGrid();
        nameLabelItem->setPos(area.topLeft() + QPointF(2, 2));
    }
}

QRectF CompoundModuleItem::getArea()
{
    return area;
}

void CompoundModuleItem::setBackgroundColor(const QColor& color)
{
    if (backgroundColor != color) {
        backgroundColor = color;
        updateRectangle();
    }
}

void CompoundModuleItem::setOutlineColor(const QColor& color)
{
    if (outlineColor != color) {
        outlineColor = color;
        updateRectangle();
    }
}

void CompoundModuleItem::setOutlineWidth(double width)
{
    if (outlineWidth != width) {
        outlineWidth = width;
        updateRectangle();
    }
}

void CompoundModuleItem::setImage(QImage *image)
{
    if (this->image != image) {
        this->image = image;
        updateImage();
    }
}

void CompoundModuleItem::setImageMode(ImageMode mode)
{
    if (this->imageMode != mode) {
        this->imageMode = mode;
        updateImage();
    }
}

void CompoundModuleItem::setGridMajorDistance(double gridDistance)
{
    if (gridMajorDistance != gridDistance) {
        gridMajorDistance = gridDistance;
        updateGrid();
    }
}

void CompoundModuleItem::setGridMinorNum(int minorNum)
{
    if (gridMinorNum != minorNum) {
        gridMinorNum = minorNum;
        updateGrid();
    }
}

void CompoundModuleItem::setGridColor(const QColor& color)
{
    if (gridColor != color) {
        gridColor = color;
        updateGrid();
    }
}

void CompoundModuleItem::setNameLabel(const QString& text)
{
    nameLabelItem->setText(text);
}

void CompoundModuleItem::setNameLabelTooltip(const QString &text)
{
    nameLabelItem->setToolTip(text);
}

int CompoundModuleItem::addText(const CompoundModuleItem::TextData& data)
{
    auto item = new OutlinedTextItem(this);
    item->setPos(data.position);
    item->setText(data.text);
    item->setBrush(data.color);
    item->setZValue(2);

    texts.append(data);
    textItems.append(item);

    return texts.length() - 1;
}

int CompoundModuleItem::addText(const QPointF& pos, const QString& text, const QColor& color)
{
    return addText(TextData { pos, text, color });
}

void CompoundModuleItemUtil::setupFromDisplayString(CompoundModuleItem *cmi, cModule *mod, double zoomFactor, QRectF submodulesRect)
{
    cDisplayString ds = mod->hasDisplayString() && mod->parametersFinalized()
            ? mod->getDisplayString()
            : cDisplayString();

    std::string buffer; // stores getTagArg return values after substitution
    DisplayStringAccess dsa(&ds, mod);

    QRectF border = submodulesRect;

    if (border.top() < 0)
        border.setTop(0);
    if (border.left() < 0)
        border.setLeft(0);

    // making the margin symmetric
    auto top = border.top();
    if (top >= 0) {
        border.setTop(0);
        border.setBottom(border.bottom() + top);
    }

    auto left = border.left();
    if (left >= 0) {
        border.setLeft(0);
        border.setRight(border.right() + left);
    }

    bool widthOk, heightOk;
    double width, height;
    width = dsa.getTagArgAsDouble("bgb", 0, 0.0, &widthOk) * zoomFactor;
    height = dsa.getTagArgAsDouble("bgb", 1, 0.0, &heightOk) * zoomFactor;

    if (widthOk)
        border.setWidth(width);
    if (heightOk)
        border.setHeight(height);

    // round all sides to whole pixels to avoid unnecessary anti-aliasing blur on the border lines
    border.setTop(std::floor(border.top()));
    border.setLeft(std::floor(border.left()));
    border.setBottom(std::ceil(border.bottom()));
    border.setRight(std::ceil(border.right()));

    cmi->setZoomFactor(zoomFactor);
    cmi->setArea(border);

    cmi->setBackgroundColor(parseColor(dsa.getTagArg("bgb", 2, buffer), parseColor("grey82")));
    cmi->setOutlineColor(parseColor(dsa.getTagArg("bgb", 3, buffer), colors::BLACK));
    cmi->setOutlineWidth(dsa.getTagArgAsDouble("bgb", 4, 2.0));

    cmi->setData(ITEMDATA_COBJECT, QVariant::fromValue((cObject *)mod));
    cmi->setData(ITEMDATA_TOOLTIP, QString(dsa.getTagArg("bgtt", 0, buffer)));

    // background image
    const char *imageName = dsa.getTagArg("bgi", 0, buffer);
    cmi->setImage(imageName[0] ? getQtenv()->icons.getImage(imageName) : nullptr);

    const char *imageMode = dsa.getTagArg("bgi", 1, buffer);
    switch (imageMode[0]) {
        case 's': cmi->setImageMode(CompoundModuleItem::MODE_STRETCH); break;
        case 'c': cmi->setImageMode(CompoundModuleItem::MODE_CENTER);  break;
        case 't': cmi->setImageMode(CompoundModuleItem::MODE_TILE);    break;
        default:  cmi->setImageMode(CompoundModuleItem::MODE_FIX);     break;
    }

    // grid
    // if failed to parse, the default 0 will disable the grid
    cmi->setGridMajorDistance(dsa.getTagArgAsDouble("bgg", 0));
    // if failed to parse, the default 0 will disable the minor lines
    cmi->setGridMinorNum(dsa.getTagArgAsLong("bgg", 1));
    cmi->setGridColor(parseColor(dsa.getTagArg("bgg", 2, buffer), colors::GREY));

    // name label in the top left corner
    std::string fullPath = mod->getFullPath().c_str();
    std::string typeName = mod->getModuleType()->getName();
    std::string text = fullPath;
    if (fullPath != typeName)  // practically, is not the toplevel module
        text +=  " (" + typeName + ")";
    cmi->setNameLabel(text.c_str());

    // tooltip for the label in the top left corner
    std::string tooltip;
    bool showNedDoc = getQtenv()->getPref("ned-doc-tooltips", true).toBool();
    if (showNedDoc) {
        std::string nedComment = getComponentDocumentationForTooltip(mod);
        if (!nedComment.empty())
            tooltip = "<i></i>" + typeName + ": " + nedComment;  // note: "<i></i>" forces HTML; we do it to enable word wrapping
    }
    cmi->setNameLabelTooltip(tooltip.c_str());

    // background texts
    int textIndex = 0;
    while (true) {
        // the first indexed is bgt1
        std::string tagName = (QString("bgt") + ((textIndex > 0) ? QString::number(textIndex) : "")).toStdString();
        if (!ds.containsTag(tagName.c_str()))
            break;

        cmi->addText(QPointF(
                         dsa.getTagArgAsDouble(tagName.c_str(), 0),
                         dsa.getTagArgAsDouble(tagName.c_str(), 1)),
                     dsa.getTagArg(tagName.c_str(), 2, buffer),
                     parseColor(dsa.getTagArg(tagName.c_str(), 3, buffer), colors::BLACK));

        ++textIndex;
    }
}

QRectF CompoundModuleItem::boundingRect() const
{
    return area.adjusted(-outlineWidth, -outlineWidth, outlineWidth, outlineWidth);
}

void CompoundModuleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // nothing to do here, the children do the painting
}

}  // namespace qtenv
}  // namespace omnetpp
