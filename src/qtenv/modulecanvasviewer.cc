//==========================================================================
//  MODULECANVASVIEWER.CC - part of
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

#include "modulecanvasviewer.h"
#include "canvasrenderer.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cfutureeventset.h"
#include "qtenv.h"
#include "mainwindow.h"
#include "layouterenv.h"
#include "figurerenderers.h"
#include "messageanimator.h"
#include "compoundmoduleitem.h"
#include "connectionitem.h"
#include "submoduleitem.h"
#include "graphicsitems.h"
#include "arrow.h"
#include <QGraphicsScene>
#include <QScrollBar>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QGraphicsWidget>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <QAction>
#include <QRubberBand>
#include <QToolTip>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>

#define emit

using namespace omnetpp::layout;

namespace omnetpp {
namespace qtenv {

ModuleCanvasViewer::ModuleCanvasViewer() :
    object(nullptr),
    notDrawn(false),
    needsRedraw(false)
{
    setFont(getQtenv()->getCanvasFont());

    backgroundLayer = new GraphicsLayer();
    rangeLayer = new GraphicsLayer();
    submoduleLayer = new GraphicsLayer();
    figureLayer = new GraphicsLayer();
    animationLayer = new GraphicsLayer();
    bubbleLayer = new GraphicsLayer();
    zoomLabelLayer = new GraphicsLayer();

    moduleScene = new QGraphicsScene();
    setScene(moduleScene);
    moduleScene->addItem(backgroundLayer);
    moduleScene->addItem(figureLayer);
    moduleScene->addItem(bubbleLayer);
    moduleScene->addItem(zoomLabelLayer);

    networkLayer = new GraphicsLayer();
    networkLayer->addItem(rangeLayer);
    networkLayer->addItem(submoduleLayer);
    networkLayer->addItem(animationLayer);

    zoomLabel = new ZoomLabel();
    zoomLabelLayer->addItem(zoomLabel);
    zoomLabel->setZoomFactor(zoomFactor);
    zoomLabel->setFont(getQtenv()->getCanvasFont());

    canvasRenderer = new CanvasRenderer();
    canvasRenderer->setLayer(figureLayer, nullptr, networkLayer);

    backgroundLayer->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    networkLayer->setFlag(QGraphicsItem::ItemIgnoresTransformations);

    // that beautiful green shade behind everything
    setBackgroundBrush(QColor("#a0e0a0"));
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setDragMode(ScrollHandDrag);
    // otherwise it would be a hand, that's why this is in mousePressEvent and mouseReleaseEvent too
    viewport()->setCursor(Qt::ArrowCursor);
    setResizeAnchor(AnchorViewCenter);

    setMouseTracking(true);

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

#ifdef Q_WS_MAC
    // the zoom label was not correctly drawn without this
    setViewportUpdateMode(FullViewportUpdate);
#endif
}

ModuleCanvasViewer::~ModuleCanvasViewer()
{
    delete canvasRenderer;
}

void ModuleCanvasViewer::setObject(cModule *obj)
{
    clear();
    object = obj;

    cCanvas *canvas = object ? object->getCanvasIfExists() : nullptr;
    canvasRenderer->setCanvas(canvas);
    redraw();
}

void ModuleCanvasViewer::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    updateZoomLabelPos();
}

void ModuleCanvasViewer::updateZoomLabelPos()
{
    QPointF size = mapToScene(viewport()->size().width(), viewport()->size().height());
    zoomLabel->setPos(size.x() - zoomLabel->boundingRect().width() - 4, size.y() - zoomLabel->boundingRect().height() - 4);
}

QString ModuleCanvasViewer::gatherTooltips(const QPoint &pos, int threshold)
{
    QString tip = gatherTooltips(QRect(pos, pos));

    if (tip.isEmpty())
        tip = gatherTooltips(QRect(pos, pos).adjusted(-threshold, -threshold, threshold, threshold));

    return tip;
}

QString ModuleCanvasViewer::gatherTooltips(const QRect& rect)
{
    auto items = scene()->items(mapToScene(rect));

    QString tip;

    // The individial items' setToolTip() method cannot be used
    // because that way they are stealing the ToolTip event,
    // and then the viewer itself doesn't have a chance to collect
    // all the relevant tooltips for every component/message/figure
    // under the cursor, and aggregate them.
    // So we have to store their tooltips as custom user data.
    for (auto i : items) {
        QString itemTip = i->data(ITEMDATA_TOOLTIP).toString();

        if (itemTip.isEmpty()) {
            cObject *itemObject = i->data(ITEMDATA_COBJECT).value<cObject *>();

            if (itemObject && itemObject != object)
                itemTip = makeObjectTooltip(itemObject);
        }

        if (!itemTip.isEmpty()) {
            if (!tip.isEmpty())
                tip += "\n";
            tip += itemTip;
        }
    }

    return tip;
}

void ModuleCanvasViewer::setZoomLabelVisible(bool visible)
{
    zoomLabel->setVisible(visible);
}

void ModuleCanvasViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
        emit doubleClick(event);
}

void ModuleCanvasViewer::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::LeftButton: emit click(event); break;
        case Qt::XButton1:   emit back();       break;
        case Qt::XButton2:   emit forward();    break;
        default:   /* shut up, compiler! */     break;
    }

    if (event->modifiers() & Qt::ControlModifier) {
        rubberBandStartPos = event->pos();
        rubberBand->setGeometry(QRect(rubberBandStartPos, QSize()));
        rubberBand->show();
    }
    else
        QGraphicsView::mousePressEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void ModuleCanvasViewer::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if (rubberBand->isVisible()) {
        QRect rect = rubberBand->geometry().normalized();
        if (event->button() == Qt::RightButton) {
            emit contextMenuRequested(getObjectsAt(rect), event->globalPos());
        }
        else
            emit marqueeZoom(rect);
        rubberBand->hide();
    }
    else {
        emit dragged(mapToScene(viewport()->rect().center()));
    }

    viewport()->setCursor(Qt::ArrowCursor);
}

void ModuleCanvasViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (QToolTip::isVisible())
        QToolTip::showText(event->globalPos(), gatherTooltips(event->pos()), this);

    if (rubberBand->isVisible() && (event->modifiers() & Qt::ControlModifier))
        rubberBand->setGeometry(QRect(rubberBandStartPos, event->pos()).normalized());
    else
        QGraphicsView::mouseMoveEvent(event);
}

void ModuleCanvasViewer::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
        event->ignore();  // scroll wheel zooming is handled by the inspector
    else
        QGraphicsView::wheelEvent(event);
}

void ModuleCanvasViewer::resizeEvent(QResizeEvent *event)
{
    if (isEnabled())
        recalcSceneRect();
    updateZoomLabelPos();
}

bool ModuleCanvasViewer::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::Polish:
        case QEvent::PolishRequest:
            recalcSceneRect(true);
            break;

        case QEvent::ToolTip: {
            auto helpEvent = static_cast<QHelpEvent *>(event);
            QToolTip::showText(helpEvent->globalPos(), gatherTooltips(helpEvent->pos()), this);
            break;
        }

        default:
            break;
    }

    return QGraphicsView::event(event);
}

void ModuleCanvasViewer::contextMenuEvent(QContextMenuEvent *event)
{
    // if Ctrl is held, ignoring the event, so gathering is possible
    if (!(event->modifiers() & Qt::ControlModifier)) {
        auto objects = getObjectsAt(event->pos());
        emit contextMenuRequested(objects, event->globalPos());
    }
}

void ModuleCanvasViewer::exportToPdf()
{
    QString fileName = getObjectShortTypeName(object) + QString(".pdf");
    fileName = QFileDialog::getSaveFileName(this, "Export to PDF", fileName, "PDF files (*.pdf)");

    if (fileName.isNull())
        return; // the file selection dialog got cancelled

    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
        fileName += ".pdf";

    QPrinter printer(QPrinter::ScreenResolution);
    printer.setFontEmbeddingEnabled(true);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setColorMode(QPrinter::Color);
    printer.setOutputFileName(fileName);

    renderToPrinter(printer);
}

void ModuleCanvasViewer::print()
{
    QPrinter printer;

    // the user can override this in the dialog...
    printer.setColorMode(QPrinter::Color);

    QPrintDialog printDialog(&printer);
    printDialog.setOptions(QAbstractPrintDialog::PrintToFile);
    if (printDialog.exec() != QDialog::Accepted)
        return;

    renderToPrinter(printer);
}

void ModuleCanvasViewer::renderToPrinter(QPrinter &printer)
{
    QRectF rect;

    if (object && compoundModuleItem) {
        auto compoundRect = compoundModuleItem->boundingRect()
                .united(getSubmodulesRect());
        auto figuresRect = canvasRenderer->itemsBoundingRect();

        rect = compoundRect.united(figuresRect);

        rect = QRectF(rect.topLeft(), rect.bottomRight());
    }

    const int margin = 20;
    QMarginsF margins(margin, margin, margin, margin);
    rect = rect.marginsAdded(margins);

    // DO NOT CHANGE THE RESOLUTION!
    // I have no idea why it has to be 75, but this is a workaround for
    // https://bugreports.qt.io/browse/QTBUG-57005 because we use the
    // ItemIgnoresTransformations flag extensively.
    printer.setResolution(75);

    printer.setPageMargins(QMargins(0, 0, 0, 0));
    printer.setPaperSize(rect.size() / printer.resolution(), QPrinter::Inch);
    scene()->setSceneRect(rect);


    QPainter painter;
    setZoomLabelVisible(false);

    // we have to disable caching to avoid the text items
    // (mainly cached OutlinedTextItem) being rasterized
    std::map<QGraphicsItem *, QGraphicsItem::CacheMode> cacheModes;

    for (auto i : items()) {
        cacheModes[i] = i->cacheMode();
        i->setCacheMode(QGraphicsItem::NoCache);
    }

    painter.begin(&printer);
    printer.setFullPage(true);
    painter.fillRect(printer.pageRect(), backgroundBrush()); // green background

    QRect viewport = scene()->sceneRect().toAlignedRect();
    scene()->render(&painter, printer.pageRect(), viewport);

    painter.end();

    setZoomLabelVisible(true);

    for (auto p : cacheModes)
        p.first->setCacheMode(p.second);
}

void ModuleCanvasViewer::redrawFigures()
{
    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->redraw(&hints);
}

void ModuleCanvasViewer::refreshFigures()
{
    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->refresh(&hints);
}

void ModuleCanvasViewer::fillFigureRenderingHints(FigureRenderingHints *hints)
{
    QFont canvasFont = getQtenv()->getCanvasFont();
    hints->defaultFont = canvasFont.family().toStdString();
    hints->defaultFontSize = canvasFont.pointSize();
    hints->zoom = zoomFactor;
}

// requires either recalculateLayout() or refreshLayout() called before!
void ModuleCanvasViewer::redrawModules()
{
    cModule *parentModule = object;

    //TODO then display all submodules
    //CHK(Tcl_VarEval(interp, canvas, " delete dx", TCL_NULL));  // NOT "delete all" because that'd remove "bubbles" too!

    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it) {
        cModule *submod = *it;
        //assert(submodPosMap.find(submod) != submodPosMap.end());
        drawSubmodule(submod);
    }

    // draw enclosing module
    drawEnclosingModule(parentModule);

    // loop through all submodules and enclosing module & draw their connections
    bool atParent = false;
    for (cModule::SubmoduleIterator it(parentModule); !atParent; ++it) {
        cModule *mod = !it.end() ? *it : (atParent = true, parentModule);

        for (cModule::GateIterator git(mod); !git.end(); ++git) {
            cGate *gate = *git;
            if (gate->getType() == (atParent ? cGate::INPUT : cGate::OUTPUT) && gate->getNextGate() != nullptr) {
                drawConnection(gate);
            }
        }
    }
    //CHK(Tcl_VarEval(interp, canvas, " raise bubble", TCL_NULL));
    //CHK(Tcl_VarEval(interp, "ModuleInspector:setScrollRegion ", windowName, " 0", TCL_NULL));
}

void ModuleCanvasViewer::drawSubmodule(cModule *submod)
{
    auto item = new SubmoduleItem(submod, rangeLayer);
    SubmoduleItemUtil::setupFromDisplayString(item, submod);
    item->setData(ITEMDATA_COBJECT, QVariant::fromValue(dynamic_cast<cObject *>(submod)));
    item->setZoomFactor(zoomFactor);
    item->setImageSizeFactor(imageSizeFactor);
    item->setPos(getSubmodCoords(submod));
    item->setNameVisible(showModuleNames);
    submoduleGraphicsItems[submod] = item;
    item->setParentItem(submoduleLayer);
    item->update();
}

void ModuleCanvasViewer::drawEnclosingModule(cModule *parentModule)
{
    backgroundLayer->clear();
    compoundModuleItem = new CompoundModuleItem();
    backgroundLayer->addItem(compoundModuleItem);

    CompoundModuleItemUtil::setupFromDisplayString(compoundModuleItem, parentModule, zoomFactor, getSubmodulesRect());

    recalcSceneRect();
}

QRectF ModuleCanvasViewer::getSubmodulesRect()
{
    QRectF submodulesRect;

    if (submoduleGraphicsItems.empty()) {
        submodulesRect.setWidth(300 * zoomFactor);
        submodulesRect.setHeight(200 * zoomFactor);
    }
    else {
        // Neither layer->boundingRect or layer->childrenBoundingRect
        // does what we want here. the former doesn't consider children at all,
        // the latter encloses everything, right down to the leaves.
        // What we want is the bounding box of only the child items themselves, no recursion.
        for (auto gi : submoduleGraphicsItems) {
            submodulesRect = submodulesRect.united(gi.second->mapRectToParent(gi.second->boundingRect()));
        }
    }

    return submodulesRect;
}

void ModuleCanvasViewer::recalcSceneRect(bool alignTopLeft)
{
    const int margin = 10;
    if (object && compoundModuleItem) {
        auto rect = compoundModuleItem->boundingRect()
                      .united(getSubmodulesRect())
                      .adjusted(-margin, -margin, margin, margin); // leaving a bit of a margin

        // how much additional space we need to show both ways in each direction
        // so the compound module can be moved in the viewport even if it is small
        double horizExcess = std::max(0.0, viewport()->width() - rect.width());
        double vertExcess = std::max(0.0, viewport()->height() - rect.height());

        auto figuresRect = canvasRenderer->itemsBoundingRect();

        rect = rect.adjusted(-horizExcess, -vertExcess, horizExcess, vertExcess)
                 .united(figuresRect // including canvas figures
                           .adjusted(-margin, -margin, margin, margin)); // leaving a bit of a margin

        setSceneRect(rect);

        if (alignTopLeft) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
            verticalScrollBar()->setValue(verticalScrollBar()->maximum());
            // making sure the viewpoint is scrolled so the top left
            // corners of the viewport and the compound module are aligned
            auto corner = compoundModuleItem->boundingRect().topLeft();
            ensureVisible(corner.x(), corner.y(), 1, 1, margin, margin);
        }
    }
}

void ModuleCanvasViewer::drawConnection(cGate *gate)
{
    cModule *mod = gate->getOwnerModule();
    cGate *destGate = gate->getNextGate();

    // check if this is a two way connection (an other connection is pointing back
    // to the this gate's pair from the next gate's pair)
    bool twoWayConnection = false;
    // check if this gate is really part of an in/out gate pair
    // gate      o-------------------->o dest_gate
    // gate_pair o<--------------------o dest_gate_pair
    if (gate->getNameSuffix()[0]) {
        const cGate *gatePair = mod->gateHalf(gate->getBaseName(),
                    gate->getType() == cGate::INPUT ? cGate::OUTPUT : cGate::INPUT,
                    gate->isVector() ? gate->getIndex() : -1);

        if (destGate->getNameSuffix()[0]) {
            const cGate *destGatePair = destGate->getOwnerModule()->gateHalf(destGate->getBaseName(),
                        destGate->getType() == cGate::INPUT ? cGate::OUTPUT : cGate::INPUT,
                        destGate->isVector() ? destGate->getIndex() : -1);
            twoWayConnection = destGatePair == gatePair->getPreviousGate();
        }
    }

    auto item = new ConnectionItem(submoduleLayer);
    item->setLine(getConnectionLine(gate));
    ConnectionItemUtil::setupFromDisplayString(item, gate, twoWayConnection);
    connectionGraphicsItems[gate] = item;
    item->setZValue(-1);
    // The !twoWayConnection is a tiny bit of duplicated
    // logic with setupFromDisplayString, but oh well...
    item->setArrowEnabled(!twoWayConnection && showArrowHeads);
}

QPointF ModuleCanvasViewer::getSubmodCoords(cModule *mod)
{
    return getQtenv()->getModuleLayouter()->getModulePosition(mod) * zoomFactor;
}

QRectF ModuleCanvasViewer::getSubmodRect(cModule *mod)
{
    if (submoduleGraphicsItems.count(mod) == 0) {
        return compoundModuleItem->getArea();
    }

    return submoduleGraphicsItems[mod]->boundingRect()
                   .translated(getSubmodCoords(mod));
}

QLineF ModuleCanvasViewer::getConnectionLine(cGate *gate)
{
    char mode = 'a';

    QPointF srcAnch(50, 50);
    QPointF destAnch(50, 50);

    auto channel = gate->getChannel();
    if (channel) {
        cDisplayString ds = channel->getDisplayString();

        const char *modeString = ds.getTagArg("m", 0);
        if (modeString[0] && QString("amnews").contains(modeString[0]))
            mode = modeString[0];

        bool xOk, yOk;
        int x = QString(ds.getTagArg("m", 1)).toInt(&xOk);
        int y = QString(ds.getTagArg("m", 2)).toInt(&yOk);

        if (xOk)
            srcAnch.setX(x);
        if (yOk)
            srcAnch.setY(y);

        x = QString(ds.getTagArg("m", 3)).toInt(&xOk);
        y = QString(ds.getTagArg("m", 4)).toInt(&yOk);

        if (xOk)
            destAnch.setX(x);
        if (yOk)
            destAnch.setY(y);
    }

    int src_i = 0, src_n = 1, dest_i = 0, dest_n = 1;

    if (getQtenv()->opt->arrangeVectorConnections) {
        src_i = gate->getIndex();
        src_n = gate->getVectorSize();
        dest_i = gate->getNextGate()->getIndex();
        dest_n = gate->getNextGate()->getVectorSize();
    }

    return arrowcoords(getSubmodRect(gate->getOwnerModule()), getSubmodRect(gate->getNextGate()->getOwnerModule()),
                       src_i, src_n, dest_i, dest_n, mode, srcAnch, destAnch);
}

void ModuleCanvasViewer::setLayoutingScene(QGraphicsScene *layoutingScene)
{
    if (layoutingScene) {
        ASSERT(!this->layoutingScene);
        this->layoutingScene = layoutingScene;
        setScene(layoutingScene);
        setSceneRect(QRectF());
    }
    else {
        ASSERT(this->layoutingScene);
        this->layoutingScene = nullptr;
        setScene(moduleScene);
    }
}

std::vector<cObject *> ModuleCanvasViewer::getObjectsAt(const QPoint& pos, int threshold)
{
    std::vector<cObject *> objects = getObjectsAt(QRect(pos, pos));

    if (objects.empty()  // if nothing, or only the inspected module (big grey rect) was clicked
            || (objects.size() == 1 && objects[0] == object)) {
        // then trying again, this time with a small square around pos
        QPoint offset(threshold, threshold);
        QRect rect(pos - offset, pos + offset);
        objects = getObjectsAt(rect);
    }

    return objects;
}

std::vector<cObject *> ModuleCanvasViewer::getObjectsAt(const QRect& rect)
{
    QList<QGraphicsItem *> items = scene()->items(mapToScene(rect));
    std::vector<cObject *> objects;

    for (auto item : items) {
        auto obj = item->data(ITEMDATA_COBJECT).value<cObject *>();

        if (!obj)
            continue;

        for (auto fig = dynamic_cast<cFigure *>(obj); fig; fig = fig->getParentFigure()) {
            auto assocObj = fig->getAssociatedObject();

            if (assocObj && std::find(objects.begin(), objects.end(), assocObj) == objects.end()) {
                objects.push_back(assocObj);
                break;
            }
        }

        if (std::find(objects.begin(), objects.end(), obj) == objects.end())
            objects.push_back(obj);
    }

    return objects;
}

void ModuleCanvasViewer::clear()
{
    // everything on the animationLayer is handled by the Animator, so don't touch that!
    backgroundLayer->clear();
    submoduleLayer->clear();
    bubbleLayer->clear();
    submoduleGraphicsItems.clear();
    connectionGraphicsItems.clear();
    nextEventMarker = nullptr;  // because it is on the submodule layer, it has been deleted by that
}

void ModuleCanvasViewer::redrawNextEventMarker()
{
    delete nextEventMarker;
    nextEventMarker = nullptr;

    cModule *mod = object;

    // this thingy is only needed if animation is going on
    if (!getQtenv()->animating || !getQtenv()->opt->showNextEventMarkers)
        return;

    // if any parent of the module containing the next event is on this canvas, draw marker
    cModule *nextMod = getSimulation()->guessNextModule();
    cModule *nextModParent = nextMod;
    while (nextModParent && nextModParent->getParentModule() != mod)
        nextModParent = nextModParent->getParentModule();

    if (nextModParent) {
        // XXX maybe move this to the animation layer?
        nextEventMarker = new QGraphicsRectItem(submoduleLayer);

        auto item = submoduleGraphicsItems[nextModParent];
        if (item) {
            nextEventMarker->setRect(item->mapRectToParent(item->boundingRect()).adjusted(-2, -2, 2, 2));
            nextEventMarker->setBrush(Qt::NoBrush);
            nextEventMarker->setPen(QPen(QColor("red"), nextMod == nextModParent ? 2 : 1));
            nextEventMarker->setZValue(10);
        }
    }
}

void ModuleCanvasViewer::refreshLayout()
{
    getQtenv()->getModuleLayouter()->ensureLayouted(object);
}

void ModuleCanvasViewer::refreshSubmodule(cModule *submod)
{
    if (submoduleGraphicsItems.count(submod)) {
        auto item = submoduleGraphicsItems[submod];
        SubmoduleItemUtil::setupFromDisplayString(item, submod);
        /*bool explicitCoords, obeys;
        double x, y, sx, sy;
        getSubmoduleCoords(submod, explicitCoords, obeys, x, y, sx, sy);
        if (explicitCoords)
            submodPosMap[submod] = QPointF(x, y);*/
        item->setPos(getSubmodCoords(submod));
        item->update();
    }
}

void ModuleCanvasViewer::refreshSubmodules()
{
    if (object)
        for (cModule::SubmoduleIterator it(object); !it.end(); ++it)
            refreshSubmodule(*it);
}

void ModuleCanvasViewer::refreshConnection(cGate *gate)
{
    if (connectionGraphicsItems.count(gate)) {
        ConnectionItem *item = connectionGraphicsItems[gate];
        ConnectionItemUtil::setupFromDisplayString(item, gate, item->isHalfLength());
        item->setLine(getConnectionLine(gate));
    }
}

void ModuleCanvasViewer::refreshConnections(cModule *module)
{
    for (cModule::GateIterator it(module); !it.end(); ++it)
        refreshConnection(*it);
}

void ModuleCanvasViewer::refreshConnections()
{
    if (object) {
        refreshConnections(object);
        for (cModule::SubmoduleIterator it(object); !it.end(); ++it)
            refreshConnections(*it);
    }
}

void ModuleCanvasViewer::redraw()
{
    clear();
    if (object == nullptr)
        return;

    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->redraw(&hints);

    zoomLabel->setFont(getQtenv()->getCanvasFont());
    updateZoomLabelPos();

    refreshLayout();
    redrawModules();
    redrawFigures();
    redrawNextEventMarker();
    refreshSubmodules();
    refreshConnections();
}

void ModuleCanvasViewer::refresh()
{
    if (!object) {
        clear();
        return;
    }

    if (notDrawn)
        return;

    cCanvas *canvas = object->getCanvasIfExists();
    if (canvas != nullptr && !canvasRenderer->hasCanvas())  // canvas was recently created
        canvasRenderer->setCanvas(canvas);

    // must not give control to the model after an error, it might be inconsistent
    if (canvas && getQtenv()->getSimulationState() != Qtenv::SIM_ERROR)
        canvas->getRootFigure()->callRefreshDisplay();

    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->refresh(&hints);

    // redraw modules only if really needed
    if (needsRedraw) {
        needsRedraw = false;
        redraw();
    }
    else {
        refreshFigures();
        redrawNextEventMarker();
        refreshSubmodules();
    }
}

void ModuleCanvasViewer::setZoomFactor(double zoomFactor)
{
    if (this->zoomFactor != zoomFactor) {
        // just moving the bubbles where they belong after the zoom, because it's easy
        double ratio = zoomFactor / this->zoomFactor;
        for (auto i : bubbleLayer->childItems())
            i->setPos(i->pos() * ratio);

        this->zoomFactor = zoomFactor;
        zoomLabel->setZoomFactor(zoomFactor);
        redraw();
        viewport()->update();
    }
}

void ModuleCanvasViewer::setImageSizeFactor(double imageSizeFactor)
{
    if (this->imageSizeFactor != imageSizeFactor) {
        this->imageSizeFactor = imageSizeFactor;
        redraw();
    }
}

void ModuleCanvasViewer::setShowModuleNames(bool show)
{
    if (showModuleNames != show) {
        showModuleNames = show;
        for (auto i : submoduleGraphicsItems) {
            i.second->setNameVisible(show);
        }
    }
}

void ModuleCanvasViewer::setShowArrowheads(bool show)
{
    if (showArrowHeads != show) {
        showArrowHeads = show;
        for (auto i : connectionGraphicsItems) {
            // not enabling arrowheads on two-way connections
            i.second->setArrowEnabled(show && !i.second->isHalfLength());
        }
    }
}

void ModuleCanvasViewer::bubble(cComponent *subcomponent, const char *text)
{
    if (!subcomponent->isModule())
        return;  // channel bubbles not yet supported

    // if submod position is not yet known (because e.g. we're in fast mode
    // and it was dynamically created since the last update), refresh layout
    // so that we can get coordinates for it
    cModule *submod = (cModule *)subcomponent;
    if (std::isnan(getSubmodCoords(submod).x()))
        refreshLayout();

    // will delete itself after a timeout
    bubbleLayer->addItem(new BubbleItem(getSubmodCoords(submod), text));
}

}  // namespace qtenv
}  // namespace omnetpp

