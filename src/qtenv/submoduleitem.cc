//==========================================================================
//  SUBMODULEITEM.CC - part of
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

#include "submoduleitem.h"
#include "qtenv.h"
#include <QDebug>
#include <QGraphicsColorizeEffect>
#include <omnetpp/cdisplaystring.h>
#include <omnetpp/cqueue.h>

namespace omnetpp {
namespace qtenv {

void SubmoduleItemUtil::setupFromDisplayString(SubmoduleItem *si, cModule *mod)
{
    si->setName(mod->getName());
    si->setVectorIndex(mod->isVector() ? mod->getIndex() : -1);

    // making a copy, so we can modify freely
    cDisplayString ds = mod->getDisplayString();

    // replacing $param args with the actual parameter values
    std::string buffer;
    ds = substituteDisplayStringParamRefs(ds, buffer, mod, true);

    bool widthOk, heightOk;
    double shapeWidth = QString(ds.getTagArg("b", 0)).toDouble(&widthOk);
    double shapeHeight = QString(ds.getTagArg("b", 1)).toDouble(&heightOk);

    if (!widthOk) shapeWidth = shapeHeight;
    if (!heightOk) shapeHeight = shapeWidth;
    if (!widthOk && !heightOk) {
        shapeWidth = 40;
        shapeHeight = 24;
    }

    si->setWidth(shapeWidth);
    si->setHeight(shapeHeight);

    QString shapeName = QString(ds.getTagArg("b", 2)).toLower();

    auto shape = (ds.getNumArgs("b") <= 0)
                   ? SubmoduleItem::SHAPE_NONE
                   : shapeName == "oval"
                       ? SubmoduleItem::SHAPE_OVAL
                       : SubmoduleItem::SHAPE_RECT; // if unknown, this is the default
    si->setShape(shape);

    si->setFillColor(parseColor(ds.getTagArg("b", 3)));
    si->setOutlineColor(parseColor(ds.getTagArg("b", 4)));
    bool ok;
    int outlineWidth = QString(ds.getTagArg("b", 5)).toInt(&ok);
    si->setOutlineWidth(ok ? outlineWidth : 2);


    const char *imageName = ds.getTagArg("i", 0);

    if (!imageName[0] && shape == SubmoduleItem::SHAPE_NONE)
        imageName = "block/process";

    if (imageName[0]) {
        auto image = getQtenv()->icons.getImage(imageName, ds.getTagArg("is", 0));
        si->setImage(image);

        const char *imageColor = ds.getTagArg("i", 1);
        si->setImageColor(parseColor(imageColor));
        si->setImageColorPercentage((ds.getNumArgs("i") == 2) // color given, but no percentage
                                      ? 30
                                      : QString(ds.getTagArg("i", 2)).toDouble());
    } else {
        si->setImage(nullptr);
    }


    const char *decoratorImageName = ds.getTagArg("i2", 0);
    if (decoratorImageName[0]) {
        auto decoratorImage = getQtenv()->icons.getImage(decoratorImageName);
        si->setDecoratorImage(decoratorImage);

        const char *decoratorImageColor = ds.getTagArg("i2", 1);
        si->setDecoratorImageColor(parseColor(decoratorImageColor));
        si->setDecoratorImageColorPercentage((ds.getNumArgs("i2") == 2) // color given, but no percentage
                                               ? 30
                                               : QString(ds.getTagArg("i2", 2)).toDouble());
    } else {
        si->setDecoratorImage(nullptr);
    }


    const char *toolTip = ds.getTagArg("tt", 0);
    si->setToolTip(toolTip);


    const char *text = ds.getTagArg("t", 0);
    const char *textPosition = ds.getTagArg("t", 1);
    const char *textColor = ds.getTagArg("t", 2);

    SubmoduleItem::TextPos textPos;
    switch (textPosition[0]) {
    case 'l': textPos = SubmoduleItem::TEXTPOS_LEFT;  break;
    case 'r': textPos = SubmoduleItem::TEXTPOS_RIGHT; break;
    default:  textPos = SubmoduleItem::TEXTPOS_TOP;   break;
    }
    auto color = parseColor(textColor);
    if (!color.isValid()) {
        color = QColor("blue");
    }
    si->setInfoText(text, textPos, color);

    int rangeIndex = 0;
    while (true) {
        // the first indexed is r1
        std::string tagName = (QString("r") + ((rangeIndex > 0) ? QString::number(rangeIndex) : "")).toStdString();
        if (!ds.containsTag(tagName.c_str()))
            break;

        double r = QString(ds.getTagArg(tagName.c_str(), 0)).toDouble();
        QColor rangeFillColor = parseColor(ds.getTagArg(tagName.c_str(), 1));
        QColor rangeOutlineColor = parseColor(ds.getTagArg(tagName.c_str(), 2));
        bool ok;
        int rangeOutlineWidth = QString(ds.getTagArg(tagName.c_str(), 3)).toInt(&ok);
        if (!ok) {
            rangeOutlineWidth = 1;
        }
        if (!rangeOutlineColor.isValid() && rangeOutlineWidth > 0) {
            rangeOutlineColor = QColor("black");
        }
        si->addRangeItem(r, rangeOutlineWidth, rangeFillColor, rangeOutlineColor);

        ++rangeIndex;
    }

    QString queueText;
    if (ds.containsTag("q")) {
        auto queueName = ds.getTagArg("q", 0);
        cQueue *q = dynamic_cast<cQueue *>(mod->findObject(queueName));
        if (q)
            queueText = QString("q: %1").arg(q->getLength());
    }
    si->setQueueText(queueText);

//    proc ModuleInspector:drawSubmodule {c submodptr x y name dispstr isplaceholder} {
//       #puts "DEBUG: ModuleInspector:drawSubmodule $c $submodptr $x $y $name $dispstr $isplaceholder"
//       global icons inspectordata tkpFont canvasTextOptions
/*
    QString prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":zoomfactor";
    QVariant variant = getQtenv()->getPref(prefName);
    double zoom = 0;
    if(variant.isValid())
        zoom = variant.value<double>();

    prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":imagesizefactor";
    variant = getQtenv()->getPref(prefName);
    double imagesizefactor = 0;
    if(variant.isValid())
        imagesizefactor = variant.value<double>();

    double alphamult = 1;
    if(submod->isPlaceholder()) {
        alphamult = 0.3;
    }
*/
    //opp_displaystring $dispstr parse tags $submodptr 1

//    const char *array = "tags";
//    cComponent *component = dynamic_cast<cComponent *>(submod);

//    cDisplayString dp(dispstr);
//    for (int k = 0; k < dp.getNumTags(); k++) {
//        Tcl_Obj *arglist = Tcl_NewListObj(0, nullptr);
//        for (int i = 0; i < dp.getNumArgs(k); i++) {
//            const char *s = dp.getTagArg(k, i);
//            TRY(s = substituteDisplayStringParamRefs(s, buffer, component, true))
//            Tcl_ListObjAppendElement(interp, arglist, Tcl_NewStringObj(TCLCONST(s), -1));
//        }
//        Tcl_SetVar2Ex(interp, TCLCONST(array), TCLCONST(dp.getTagName(k)), arglist, 0);
//    }

//           # scale (x,y)
//           if {$zoom != ""} {
//               set x [expr $zoom*$x]
//               set y [expr $zoom*$y]
//           }

//           # set sx and sy (and look up image)
//           set isx 0
//           set isy 0
//           set bsx 0
//           set bsy 0
//           if ![info exists tags(is)] {
//               set tags(is) {}
//           }
//           if [info exists tags(i)] {
//               setvars {img isx isy} [dispstrGetImage $tags(i) $tags(is) $imagesizefactor $alphamult $icons(defaulticon)]
//           }

//           if [info exists tags(b)] {
//               set bsx [lindex $tags(b) 0]
//               set bsy [lindex $tags(b) 1]
//               if {$bsx=="" && $bsy==""} {
//                   set bsx 40
//                   set bsy 24
//               }
//               if {$bsx==""} {set bsx $bsy}
//               if {$bsy==""} {set bsy $bsx}
//               if {$zoom != ""} {
//                   set bsx [expr $zoom*$bsx]
//                   set bsy [expr $zoom*$bsy]
//               }
//           } elseif ![info exists tags(i)] {
//               setvars {img isx isy} [dispstrGetImage "" "" $imagesizefactor $alphamult $icons(defaulticon)]
//           }

//           set sx [expr {$isx<$bsx ? $bsx : $isx}]
//           set sy [expr {$isy<$bsy ? $bsy : $isy}]

//           if [info exists tags(b)] {

//               set width [lindex $tags(b) 5]
//               if {$width == ""} {set width 2}

//               set rx [expr $bsx/2 - $width/2]
//               set ry [expr $bsy/2 - $width/2]
//               set x1 [expr $x - $rx]
//               set y1 [expr $y - $ry]
//               set x2 [expr $x + $rx]
//               set y2 [expr $y + $ry]

//               set fill [lindex $tags(b) 3]
//               if {$fill == ""} {set fill #8080ff}
//               if {$fill == "-"} {set fill ""}
//               if {[string index $fill 0]== "@"} {set fill [opp_hsb_to_rgb $fill]}
//               set outline [lindex $tags(b) 4]
//               if {$outline == ""} {set outline black}
//               if {$outline == "-"} {set outline ""}
//               if {[string index $outline 0]== "@"} {set outline [opp_hsb_to_rgb $outline]}
//               if {$isplaceholder} {set dash "1 1"} else {set dash ""}

//               switch -regexp [lindex $tags(b) 2] {
//                  "o.*"   {set what [list ellipse $x $y -rx $rx -ry $ry]}
//                  "l.*"   {set what [list pline $x1 $y1 $x2 $y2]}
//                  default {set what [list prect $x1 $y1 $x2 $y2 -strokelinejoin miter]}
//               }

//               $c create {*}$what \
//                   -fill $fill -strokewidth $width -stroke $outline -strokedasharray $dash \
//                   -tags "dx tooltip submod submodext $submodptr"

//               if [info exists tags(i)] {
//                   $c create pimage $x $y {*}$img -anchor c -tags "dx tooltip submod submodext $submodptr"
//               }
//               if {$inspectordata($c:showlabels)} {
//                   $c create ptext $x [expr $y2+$width/2+3] -text $name -textanchor n {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx submodext"
//               }

//           } else {
//               # draw an icon when no shape is present (only i tag, or neither i nor b tag)
//               $c create pimage $x $y {*}$img -anchor c -tags "dx tooltip submod submodext $submodptr"
//               if {$inspectordata($c:showlabels)} {
//                   $c create ptext $x [expr $y+$sy/2+3] -text $name -textanchor n {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx submodext"
//               }
//           }

//           # queue length
//           if {[info exists tags(q)]} {
//               set r [ModuleInspector:getSubmodCoords $c $submodptr]
//               set qx [expr [lindex $r 2]+1]
//               set qy [lindex $r 1]
//               $c create ptext $qx $qy -text "q:?" -textanchor nw {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx tooltip qlen qlen-$submodptr submodext"
//           }

//           # modifier icon (i2 tag)
//           if {[info exists tags(i2)]} {
//               if ![info exists tags(is2)] {
//                   set tags(is2) {}
//               }
//               set r [ModuleInspector:getSubmodCoords $c $submodptr]
//               set mx [expr [lindex $r 2]+2]
//               set my [expr [lindex $r 1]-2]
//               setvars {img2 dummy dummy} [dispstrGetImage $tags(i2) $tags(is2) $imagesizefactor $alphamult]
//               $c create pimage $mx $my {*}$img2 -anchor ne -tags "dx tooltip submod submodext $submodptr"
//           }

//           # text (t=<text>,<position>,<color>); multiple t tags supported (t1,t2,etc)
//           foreach {ttag} [array names tags -regexp {^t\d*$} ] {
//               set txt [lindex $tags($ttag) 0]
//               set pos [lindex $tags($ttag) 1]
//               if {$pos == ""} {set pos "t"}
//               set color [lindex $tags($ttag) 2]
//               if {$color == ""} {set color "#0000ff"}
//               if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}

//               set r [ModuleInspector:getSubmodCoords $c $submodptr]
//               if {$pos=="l"} {
//                   set tx [lindex $r 0]
//                   set ty [lindex $r 1]
//                   set anch "ne"
//                   set just "right"
//               } elseif {$pos=="r"} {
//                   set tx [lindex $r 2]
//                   set ty [lindex $r 1]
//                   set anch "nw"
//                   set just "left"
//               } elseif {$pos=="t"} {
//                   set tx [expr ([lindex $r 0]+[lindex $r 2])/2]
//                   set ty [expr [lindex $r 1]+2]
//                   set anch "s"
//                   set just "center"
//               } else {
//                   error "wrong position in t= tag, should be `l', `r' or `t'"
//               }
//               $c create ptext $tx $ty -text $txt -fill $color -textanchor $anch {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx submodext"
//           }

//           # r=<radius>,<fillcolor>,<color>,<width>; multiple r tags supported (r1,r2,etc)
//           foreach {rtag} [array names tags -regexp {^r\d*$} ] {
//               set radius [lindex $tags($rtag) 0]
//               if {$radius == ""} {set radius 0}
//               set rfill [lindex $tags($rtag) 1]
//               if {$rfill == "-"} {set rfill ""}
//               if {[string index $rfill 0]== "@"} {set rfill [opp_hsb_to_rgb $rfill]}
//               # if rfill=="" --> not filled
//               set routline [lindex $tags($rtag) 2]
//               if {$routline == ""} {set routline black}
//               if {$routline == "-"} {set routline ""}
//               if {[string index $routline 0]== "@"} {set routline [opp_hsb_to_rgb $routline]}
//               set rwidth [lindex $tags($rtag) 3]
//               if {$rwidth == ""} {set rwidth 1}
//               if {$zoom != ""} {
//                   set radius [expr $zoom*$radius]
//               }
//               set radius [expr $radius-$rwidth/2]

//               set x1 [expr $x - $radius]
//               set y1 [expr $y - $radius]
//               set x2 [expr $x + $radius]
//               set y2 [expr $y + $radius]

//               set circle [$c create circle $x $y -r $radius -fillopacity 0.5 \
//                   -fill $rfill -strokewidth $rwidth -stroke $routline -tags "dx range submodext"]
//               # has been moved to the beginning of ModuleInspector:drawEnclosingModule to maintain relative z order of range indicators
//               # $c lower $circle
//           }
    //    }
}

QColor SubmoduleItemUtil::parseColor(const QString &name, const QColor &fallbackColor) {
    if (name.isEmpty())
        return fallbackColor;

    if (name == "-")
        return QColor("transparent");

    QColor::setAllowX11ColorNames(true); // XXX remove later?

    QColor color;
    if (name[0] == '@') {  // HSB ( == HSV)
        bool ok;
        int hue = name.mid(1, 2).toInt(&ok, 16) / 255.0f * 360;
        int sat = name.mid(3, 2).toInt(&ok, 16);
        int val = name.mid(5, 2).toInt(&ok, 16);
        color.setHsv(hue, sat, val);
    } else { // #RRGGBB or name
        color = QColor(name);
        // XXX Tkenv recognised X color names, listed in ccanvas.cc,
        // but Qt recognizes SVG color names, see the documentation.
        // We could enable the allowX11ColorNames property, but that
        // only works on X11...
    }

    return color.isValid() ? color : fallbackColor;
}

void SubmoduleItem::updateNameItem() {
    if (nameItem) {
        QString label = name;
        if (vectorIndex >= 0) {
            label += "[" + QString::number(vectorIndex) + "]";
        }
        nameItem->setText(label);
        nameItem->setPos(-nameItem->boundingRect().width() / 2, shapeImageBoundingRect().height() / 2);
    }
}

void SubmoduleItem::realignAnchoredItems() {
    auto mainBounds = shapeImageBoundingRect();

    // the info text label
    if (textItem) {
        auto textBounds = textItem->boundingRect();
        QPointF pos;

        switch (textPos) {
        case TEXTPOS_LEFT:
            pos.setX(mainBounds.left() - textBounds.width());
            pos.setY(mainBounds.top());
            break;
        case TEXTPOS_RIGHT:
            pos.setX(mainBounds.right());
            pos.setY(mainBounds.top());
            break;
        case TEXTPOS_TOP:
            pos.setX(mainBounds.center().x() - textBounds.width() / 2.0f);
            pos.setY(mainBounds.top() - textBounds.height());
            break;
        }

        textItem->setPos(pos);
    }

    // the queue length
    if (queueItem)
        queueItem->setPos(mainBounds.width() / 2, -mainBounds.height() / 2);

    // the icon in the corner
    if (decoratorImageItem)
        decoratorImageItem->setPos(mainBounds.width() / 2, -mainBounds.height() / 2);

    updateNameItem(); // the name is anchored too in some sense
}

void SubmoduleItem::updateShapeItem() {
    if (shapeItem) {
        shapeItem->setBrush(shapeFillColor.isValid() ? shapeFillColor : QColor("#8080ff"));
        auto pen = shapeOutlineWidth == 0
                     ? Qt::NoPen
                     : QPen(shapeOutlineColor.isValid()
                              ? shapeOutlineColor
                              : QColor("black"),
                            shapeOutlineWidth);
        pen.setJoinStyle(Qt::MiterJoin);
        shapeItem->setPen(pen);

        auto w = shapeWidth * zoomFactor - shapeOutlineWidth; // so the outline grows inwards
        auto h = shapeHeight * zoomFactor - shapeOutlineWidth;
        if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem *>(shapeItem))
            ellipseItem->setRect(-w / 2.0, -h / 2.0, w, h);
        if (auto rectItem = dynamic_cast<QGraphicsRectItem *>(shapeItem))
            rectItem->setRect(-w / 2.0, -h / 2.0, w, h);
        updateNameItem();
        realignAnchoredItems();
    }
}

void SubmoduleItem::adjustRangeItem(int i) {
    RangeData &data = ranges[i];
    QGraphicsEllipseItem *range = rangeItems[i];
    // the actual item radius, correcting for zoom and to make the outline grow inwards only
    double corrR = data.radius * zoomFactor - data.outlineWidth / 2.0;
    range->setRect(-corrR, -corrR, 2 * corrR, 2 * corrR);
    range->setZValue(-data.radius); // bigger ranges go under smaller ones
    QColor fillColorTransp = data.fillColor;
    fillColorTransp.setAlphaF(data.fillColor.alphaF() * 0.5);
    range->setBrush(data.fillColor.isValid() ? QBrush(fillColorTransp) : Qt::NoBrush);
    range->setPen(data.outlineColor.isValid() ? QPen(data.outlineColor, data.outlineWidth) : Qt::NoPen);
    range->setPos(pos());
}

QRectF SubmoduleItem::shapeImageBoundingRect() const {
    QRectF rect;
    if (imageItem) {
        QRectF imageRect = imageItem->boundingRect();
        // Image scaling is done with a transformation, and boundingRect does
        // not factor that in, so we have to account the factor in here.
        imageRect.setTopLeft(imageRect.topLeft() * imageSizeFactor);
        imageRect.setBottomRight(imageRect.bottomRight() * imageSizeFactor);
        rect = rect.united(imageRect);
    }
    if (shapeItem) {
        QRectF shapeRect = shapeItem->boundingRect();
        // Shape size is modulated by the zoom via adjusting its defining rectangle,
        // (otherwise the outline would change width too), so its boundingRect is good as it is.
        rect = rect.united(shapeRect);
    }

    if (rect.width() < minimumRectSize) {
        rect.setLeft(-minimumRectSize / 2);
        rect.setRight(minimumRectSize / 2);
    }

    if (rect.height() < minimumRectSize) {
        rect.setTop(-minimumRectSize / 2);
        rect.setBottom(minimumRectSize / 2);
    }

    return rect;
}

SubmoduleItem::SubmoduleItem(cModule *mod, GraphicsLayer *rangeLayer)
    : module(mod), rangeLayer(rangeLayer)
{
    nameItem = new OutlinedTextItem(this, scene());
    queueItem = new OutlinedTextItem(this, scene());
    textItem = new OutlinedTextItem(this, scene());

    connect(this, SIGNAL(xChanged()), this, SLOT(onPositionChanged()));
    connect(this, SIGNAL(yChanged()), this, SLOT(onPositionChanged()));
    connect(this, SIGNAL(zChanged()), this, SLOT(onPositionChanged()));
}

SubmoduleItem::~SubmoduleItem()
{
    for (auto i : rangeItems) {
        delete i;
    }
}

void SubmoduleItem::setZoomFactor(double zoom) {
    if (zoomFactor != zoom) {
        zoomFactor = zoom;
        updateShapeItem();

        for (int i = 0; i < ranges.length(); ++i) {
            adjustRangeItem(i);
        }
    }
}

void SubmoduleItem::setImageSizeFactor(double imageSize) {
    if (imageSizeFactor != imageSize) {
        imageSizeFactor = imageSize;
        if (imageItem)
            imageItem->setScale(imageSizeFactor);
        if (decoratorImageItem)
            decoratorImageItem->setScale(imageSizeFactor);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setShape(Shape shape) {
    if (shape != this->shape) {
        this->shape = shape;

        delete shapeItem;
        shapeItem = nullptr;

        switch (shape) {
        case SHAPE_NONE: /* nothing to do here */                             break;
        case SHAPE_OVAL: shapeItem = new QGraphicsEllipseItem(this, scene()); break;
        case SHAPE_RECT: shapeItem = new QGraphicsRectItem(this, scene());    break;
        }

        updateShapeItem();
    }
}

void SubmoduleItem::setWidth(double width) {
    if (this->shapeWidth != width) {
        this->shapeWidth = width;
        updateShapeItem();
    }
}

void SubmoduleItem::setHeight(double height) {
    if (this->shapeHeight != height) {
        this->shapeHeight = height;
        updateShapeItem();
    }
}

void SubmoduleItem::setFillColor(const QColor &color) {
    if (shapeFillColor != color) {
        shapeFillColor = color;
        updateShapeItem();
    }
}

void SubmoduleItem::setOutlineColor(const QColor &color) {
    if (shapeOutlineColor != color) {
        shapeOutlineColor = color;
        updateShapeItem();
    }
}

void SubmoduleItem::setOutlineWidth(double width) {
    if (shapeOutlineWidth != width) {
        shapeOutlineWidth = width;
        updateShapeItem();
    }
}

void SubmoduleItem::setImage(QImage *image) {
    if (this->image != image) {
        this->image = image;
        delete imageItem;
        imageItem = nullptr;
        colorizeEffect = nullptr;
        if (image) {
            imageItem = new QGraphicsPixmapItem(QPixmap::fromImage(*image), this, scene());
            imageItem->setOffset(-image->width() / 2.0f, -image->height() / 2.0f);
            colorizeEffect = new QGraphicsColorizeEffect(this);
            colorizeEffect->setStrength(0);
            imageItem->setGraphicsEffect(colorizeEffect);
            imageItem->setScale(imageSizeFactor);
            imageItem->setTransformationMode(Qt::SmoothTransformation);
            // XXX the colorize effect makes it pixely again, if in effect...
        }
        updateNameItem();
        realignAnchoredItems();
    }
}

void SubmoduleItem::setImageColor(const QColor &color) {
    if (colorizeEffect) {
        colorizeEffect->setColor(color);
    }
}

void SubmoduleItem::setImageColorPercentage(int percent) {
    if (colorizeEffect) {
        colorizeEffect->setStrength(percent / 100.0f);
    }
}

void SubmoduleItem::setDecoratorImage(QImage *decoratorImage) {
    if (this->decoratorImage != decoratorImage) {
        this->decoratorImage = decoratorImage;
        delete decoratorImageItem;
        decoratorImageItem = nullptr;
        decoratorColorizeEffect = nullptr;
        if (decoratorImage) {
            decoratorImageItem = new QGraphicsPixmapItem(QPixmap::fromImage(*decoratorImage), this, scene());
            // It is easier to position using its (almost) upper right corner.
            // The 2 pixel offset moves it a bit to the right and up.
            decoratorImageItem->setOffset(-decoratorImage->width() + 2, -2);
            decoratorColorizeEffect = new QGraphicsColorizeEffect(this);
            decoratorColorizeEffect->setStrength(0);
            decoratorImageItem->setGraphicsEffect(decoratorColorizeEffect);
            decoratorImageItem->setTransformationMode(Qt::SmoothTransformation);
        }
        realignAnchoredItems();
    }
}

void SubmoduleItem::setDecoratorImageColor(const QColor &color) {
    if (decoratorColorizeEffect) {
        decoratorColorizeEffect->setColor(color);
    }
}

void SubmoduleItem::setDecoratorImageColorPercentage(int percent) {
    if (decoratorColorizeEffect) {
        decoratorColorizeEffect->setStrength(percent / 100.0f);
    }
}

void SubmoduleItem::setName(const QString &text) {
    if (name != text) {
        name = text;
        updateNameItem();
    }
}

void SubmoduleItem::setVectorIndex(int index) {
    if (vectorIndex != index) {
        vectorIndex = index;
        updateNameItem();
    }
}

void SubmoduleItem::setQueueText(const QString &queueText) {
    if (this->queueText != queueText) {
        this->queueText = queueText;
        queueItem->setText(queueText);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setInfoText(const QString &text, TextPos pos, const QColor &color) {
    if (this->text != text || textColor != color || pos != textPos) {
        this->text = text;
        textPos = pos;
        textColor = color;
        textItem->setText(text);
        textItem->setBrush(color);
        realignAnchoredItems();
    }
}

void SubmoduleItem::setRangeLayer(GraphicsLayer *layer) {
    if (layer != rangeLayer) {
        rangeLayer = layer;
        for (auto &i : rangeItems) {
            rangeLayer->addItem(i);
        }
    }
}

int SubmoduleItem::addRangeItem(double radius, double outlineWidth, const QColor &fillColor, const QColor &outlineColor) {
    return addRangeItem(RangeData{radius, outlineWidth, fillColor, outlineColor});
}

int SubmoduleItem::addRangeItem(const RangeData &data) {
    auto range = new QGraphicsEllipseItem();

    ranges.append(data);
    rangeItems.append(range);
    rangeLayer->addItem(range);

    adjustRangeItem(rangeItems.length() - 1);

    return rangeItems.length() - 1;
}

QRectF SubmoduleItem::boundingRect() const {
    return shapeImageBoundingRect();
}

void SubmoduleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // nothing to do here, the child items are painted anyways
}

void SubmoduleItem::onPositionChanged() {
    for (auto i : rangeItems) {
        i->setPos(pos());
    }
}

} // namespace qtenv
} // namespace omnetpp
