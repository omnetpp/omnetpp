//==========================================================================
//  MODULECANVASVIEWER.CC - part of
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

#include "modulecanvasviewer.h"
#include "canvasrenderer.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cfutureeventset.h"
#include "common/stlutil.h"
#include "qtenv.h"
#include "mainwindow.h"
#include "layouterenv.h"
#include "figurerenderers.h"
#include "messageanimator.h"
#include "compoundmoduleitem.h"
#include "connectionitem.h"
#include "submoduleitem.h"
#include "graphicsitems.h"
#include "areaselectordialog.h"
#include "arrow.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGraphicsWidget>
#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>
#include <QtWidgets/QApplication>
#include <QtWidgets/QAction>
#include <QtWidgets/QRubberBand>
#include <QtWidgets/QToolTip>
#include <QtPrintSupport/QPrinter>
#include <QtGui/QImageWriter>
#include <QtPrintSupport/QPrintDialog>
#include <QtWidgets/QFileDialog>

#define emit

using namespace omnetpp::common;
using namespace omnetpp::layout;

namespace omnetpp {
namespace qtenv {

ModuleCanvasViewer::ModuleCanvasViewer()
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

    // This avoids a crash inside Qt when traversing the tree.
    // The proper solution would be to pedantically call
    // prepareGeometryChange() in every custom item when needed.
    // However, this might also improve performance.
    // XXX is this still necessary?
    moduleScene->setItemIndexMethod(QGraphicsScene::NoIndex);

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

    QStringList tip;

    // The individial items' setToolTip() method cannot be used
    // because that way they are stealing the ToolTip event,
    // and then the viewer itself doesn't have a chance to collect
    // all the relevant tooltips for every component/message/figure
    // under the cursor, and aggregate them.
    // So we have to store their tooltips as custom user data.
    for (auto i : items) {
        QString itemTip = i->data(ITEMDATA_TOOLTIP).toString();

        if (itemTip.isEmpty() && getQtenv()->inspectorsAreFresh()) {
            cObject *itemObject = i->data(ITEMDATA_COBJECT).value<cObject *>();

            if (itemObject && itemObject != object)
                itemTip = makeObjectTooltip(itemObject);
        }

        // skipping empties, deduplication
        if (!itemTip.isEmpty() && !tip.contains(itemTip))
            tip += itemTip;
    }

    return tip.join("\n");
}

void ModuleCanvasViewer::setZoomLabelVisible(bool visible)
{
    zoomLabel->setVisible(visible);
}

void ModuleCanvasViewer::displayStringChanged()
{
    compoundModuleChanged = true;
}

void ModuleCanvasViewer::displayStringChanged(cModule *submod)
{
    ASSERT(submod->getParentModule() == object);
    changedSubmodules.insert(submod);
}

void ModuleCanvasViewer::displayStringChanged(cGate *gate)
{
    ASSERT(gate->getOwnerModule() == object || gate->getOwnerModule()->getParentModule() == object);
    changedConnections.insert(gate);
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
    else if (event->modifiers() & Qt::ShiftModifier) {
        auto objects = getObjectsAt(event->pos());
        for (cObject *o : objects)
            if (dynamic_cast<cModule *>(o)) {
                cModule *sm = dynamic_cast<cModule *>(o);
                if (sm->getParentModule() == object) {
                    draggedSubmod = sm;
                    break;
                }
            }
    }
    else
        QGraphicsView::mousePressEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void ModuleCanvasViewer::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    draggedSubmod = nullptr;
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
    else if (draggedSubmod && (event->modifiers() & Qt::ShiftModifier)) {
        cDisplayString ds = draggedSubmod->getDisplayString();
        QPointF sp = mapToScene(event->localPos().toPoint());
        ds.setTagArg("p", 0, std::to_string(sp.rx() / zoomFactor).c_str());
        ds.setTagArg("p", 1, std::to_string(sp.ry() / zoomFactor).c_str());
        ds.setTagArg("p", 2, nullptr);
        ds.setTagArg("p", 3, nullptr);
        ds.setTagArg("p", 4, nullptr);
        ds.setTagArg("p", 5, nullptr);

        draggedSubmod->setDisplayString(ds);
        getQtenv()->getModuleLayouter()->refreshPositionFromDS(draggedSubmod);
        submoduleGraphicsItems[draggedSubmod]->setPos(getSubmodCoords(draggedSubmod));
        getQtenv()->callRefreshDisplaySafe();
        getQtenv()->refreshInspectors();
    }
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

void ModuleCanvasViewer::exportToImage()
{
    QRectF sceneRect = askExportArea();

    if (sceneRect.isNull())
        return;

    QString filter;
    for (auto f : QImageWriter::supportedImageFormats()) {
        if (!filter.isEmpty())
            filter += ";;";
        filter += QString(f).toUpper() + "(*." + QString(f) + ")";
    }

    QString selF = "PNG(*.png)";
    QString fileName = QString(getQtenv()->cEnvir::getConfigEx()->getActiveConfigName()) + "_" + getObjectShortTypeName(object) + QString(".png");
    fileName = QFileDialog::getSaveFileName(this, "Export to Image", fileName, filter, &selF);

    if (fileName.isNull())
        return; // the file selection dialog got cancelled

    QImage image(sceneRect.size().toSize(), QImage::Format_RGBA8888); // Premultiplied?

    renderToPaintDevice(image, sceneRect, QRectF(QPointF(0, 0), sceneRect.size().toSize()));

    image.save(fileName);
}

void ModuleCanvasViewer::exportToPdf()
{
    QRectF exportArea = askExportArea();

    if (exportArea.isNull())
        return;

    QString fileName = QString(getQtenv()->cEnvir::getConfigEx()->getActiveConfigName()) + "_" + getObjectShortTypeName(object) + QString(".pdf");
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

    renderToPrinter(printer, exportArea);
}

void ModuleCanvasViewer::print()
{
    QRectF exportArea = askExportArea();

    if (exportArea.isNull())
        return;

    QPrinter printer;

    // the user can override this in the dialog...
    printer.setColorMode(QPrinter::Color);

    QPrintDialog printDialog(&printer);
    printDialog.setOptions(QAbstractPrintDialog::PrintToFile);
    if (printDialog.exec() != QDialog::Accepted)
        return;

    renderToPrinter(printer, exportArea);
}

QRectF ModuleCanvasViewer::askExportArea()
{
    AreaSelectorDialog dialog(this);

    QRectF exportArea;

    if (!object || !compoundModuleItem)
        return exportArea;
    if (dialog.exec() != QDialog::Accepted)
        return exportArea;

    switch (dialog.getArea()) {
        case AreaSelectorDialog::BOUNDING_BOX:
            exportArea = scene()->itemsBoundingRect();
            break;
        case AreaSelectorDialog::MODULE_RECTANGLE:
            exportArea = compoundModuleItem->boundingRect();
            break;
        case AreaSelectorDialog::VIEWPORT:
            exportArea = mapToScene(viewport()->rect()).boundingRect();
            break;
    }

    int margin = dialog.getMargin();

    QMarginsF margins(margin, margin, margin, margin);
    return exportArea.marginsAdded(margins);
}

void ModuleCanvasViewer::renderToPrinter(QPrinter &printer, const QRectF& sceneRect)
{
    printer.setPageMargins(QMargins(0, 0, 0, 0));
    printer.setPaperSize(sceneRect.size() / printer.resolution(), QPrinter::Inch);
    printer.setFullPage(true);

    renderToPaintDevice(printer, sceneRect, printer.pageRect());
}

void ModuleCanvasViewer::renderToPaintDevice(QPaintDevice &printer, const QRectF& sceneRect, const QRectF& pageRect)
{
    scene()->setSceneRect(sceneRect);

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
    painter.fillRect(pageRect, backgroundBrush()); // green background

    QRect viewport = scene()->sceneRect().toAlignedRect();
    scene()->render(&painter, pageRect, viewport);

    painter.end();

    setZoomLabelVisible(true);

    for (auto p : cacheModes)
        p.first->setCacheMode(p.second);
}

void ModuleCanvasViewer::redrawFigures()
{
    canvasRenderer->redraw(makeFigureRenderingHints());
}

void ModuleCanvasViewer::refreshFigures()
{
    canvasRenderer->refresh(makeFigureRenderingHints());
}

FigureRenderingHints ModuleCanvasViewer::makeFigureRenderingHints()
{
    FigureRenderingHints hints;
    QFont canvasFont = getQtenv()->getCanvasFont();
    hints.defaultFont = canvasFont.family().toStdString();
    hints.defaultFontSize = canvasFont.pointSize();
    hints.defaultZoom = zoomFactor;
    return hints;
}

// requires either recalculateLayout() or refreshLayout() called before!
void ModuleCanvasViewer::redrawModules()
{
    submoduleLayer->clear();
    submoduleGraphicsItems.clear();
    connectionGraphicsItems.clear();

    cModule *parentModule = object;

    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it)
        drawSubmodule(*it);

    if (!containsKey(submoduleGraphicsItems, draggedSubmod))
        draggedSubmod = nullptr;

    // draw the inside of the inspected module
    redrawEnclosingModule();

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
}

void ModuleCanvasViewer::drawSubmodule(cModule *submod)
{
    ASSERT(submod->getParentModule() == object);
    ASSERT(!containsKey(submoduleGraphicsItems, submod));

    auto item = new SubmoduleItem(submod, rangeLayer);
    try {
        SubmoduleItemUtil::setupFromDisplayString(item, submod);
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error drawing submodule %s: %s", submod->getNedTypeAndFullPath().c_str(), e.what());
    }
    item->setData(ITEMDATA_COBJECT, QVariant::fromValue(dynamic_cast<cObject *>(submod)));
    item->setZoomFactor(zoomFactor);
    item->setImageSizeFactor(imageSizeFactor);
    item->setPos(getSubmodCoords(submod));
    item->setNameFormat(submoduleNameFormat);
    item->setNameVisible(showModuleNames);
    submoduleGraphicsItems[submod] = item;
    item->setParentItem(submoduleLayer);
    item->update();
}

void ModuleCanvasViewer::redrawEnclosingModule()
{
    backgroundLayer->clear();
    compoundModuleItem = new CompoundModuleItem();
    backgroundLayer->addItem(compoundModuleItem);

    try {
        CompoundModuleItemUtil::setupFromDisplayString(compoundModuleItem, object, zoomFactor, getSubmodulesRect());
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error redrawing module %s: %s", object->getNedTypeAndFullPath().c_str(), e.what());
    }

    compoundModuleChanged = false;
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
    ASSERT(gate->getOwnerModule() == object || gate->getOwnerModule()->getParentModule() == object);
    ASSERT(!containsKey(connectionGraphicsItems, gate));

    auto item = new ConnectionItem(submoduleLayer);
    try {
        item->setLine(getConnectionLine(gate));
        ConnectionItemUtil::setupFromDisplayString(item, gate, showArrowHeads);
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error drawing connection originating in gate %s: %s", gate->getFullPath().c_str() , e.what());
    }

    connectionGraphicsItems[gate] = item;
    item->setZValue(-1);
}

QPointF ModuleCanvasViewer::getSubmodCoords(cModule *mod)
{
    ASSERT(mod->getParentModule() == object);
    return getQtenv()->getModuleLayouter()->getModulePosition(mod, zoomFactor);
}

QRectF ModuleCanvasViewer::getSubmodRect(cModule *mod)
{
    // This should not be enforced, as we should handle invalid connections
    // (like one between a sibling of a module and one of its submodules) gracefully.
    // ASSERT(mod->getParentModule() == object);

    if (!containsKey(submoduleGraphicsItems, mod) && compoundModuleItem)
        return compoundModuleItem->getArea();

    return getQtenv()->getModuleLayouter()->getModuleRectangle(mod, zoomFactor, imageSizeFactor);
}

QRectF ModuleCanvasViewer::getModuleRect(bool includeBorder)
{
    return includeBorder ? compoundModuleItem->boundingRect() : compoundModuleItem->getArea();
}

QLineF ModuleCanvasViewer::getConnectionLine(cGate *gate)
{
    ASSERT(gate->getOwnerModule() == object || gate->getOwnerModule()->getParentModule() == object);

    cGate *nextGate = gate ? gate->getNextGate() : nullptr;
    if (!nextGate)
        return QLineF();

    char mode = 'a';

    QPointF srcAnch(50, 50);
    QPointF destAnch(50, 50);

    auto channel = gate->getChannel();
    if (channel) {
        cDisplayString ds = channel->getDisplayString();

        std::string buffer;
        ds = substituteDisplayStringParamRefs(ds, buffer, channel, true);

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
        if (gate->isVector()) {
            src_i = gate->getIndex();
            src_n = gate->getVectorSize();
        }
        if (nextGate->isVector()) {
            dest_i = nextGate->getIndex();
            dest_n = nextGate->getVectorSize();
        }
    }

    return arrowcoords(getSubmodRect(gate->getOwnerModule()), getSubmodRect(nextGate->getOwnerModule()),
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

            if (assocObj) {
                if (!contains(objects, assocObj))
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
    compoundModuleItem = nullptr;

    compoundModuleChanged = false;
    changedSubmodules.clear();
    changedConnections.clear();

    needsRedraw = false;
    notDrawn = false;
}

void ModuleCanvasViewer::refreshLayout()
{
    getQtenv()->getModuleLayouter()->ensureLayouted(object);
}

void ModuleCanvasViewer::refreshSubmodule(cModule *submod)
{
    ASSERT(submod->getParentModule() == object);
    ASSERT(containsKey(submoduleGraphicsItems, submod));

    auto item = submoduleGraphicsItems[submod];
    try {
        SubmoduleItemUtil::setupFromDisplayString(item, submod);
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error redrawing submodule %s: %s", submod->getNedTypeAndFullPath().c_str(), e.what());
    }

    // Do not forget to make the layouter reread the "p" tag if needed
    // by doing a "forgetPosition" or "refreshLayout(parent)" before this.
    item->setPos(getSubmodCoords(submod));
    item->setZoomFactor(zoomFactor);
    item->update();
}

void ModuleCanvasViewer::refreshSubmodules()
{
    if (object)
        for (cModule::SubmoduleIterator it(object); !it.end(); ++it)
            refreshSubmodule(*it);
    changedSubmodules.clear();
}

void ModuleCanvasViewer::refreshQueueSizes()
{
    for (auto p : submoduleGraphicsItems)
        SubmoduleItemUtil::updateQueueSizeLabel(p.second, p.first);
}

void ModuleCanvasViewer::refreshConnection(cGate *gate)
{
    ASSERT(gate->getOwnerModule() == object || gate->getOwnerModule()->getParentModule() == object);
    ASSERT(containsKey(connectionGraphicsItems, gate));

    ConnectionItem *item = connectionGraphicsItems[gate];
    try {
        ConnectionItemUtil::setupFromDisplayString(item, gate, showArrowHeads);
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error refreshing connection originating in gate %s: %s", gate->getFullPath().c_str(), e.what());
    }

    item->setLine(getConnectionLine(gate));
}

void ModuleCanvasViewer::refreshConnections(cModule *module)
{
    ASSERT(module == object || module->getParentModule() == object);

    for (cModule::GateIterator it(module); !it.end(); ++it) {
        cGate *gate = *it;
        if (gate->getNextGate() == nullptr)
            continue;

        if (module == object) {
            if (gate->getType() != cGate::INPUT)
                continue;
        }
        else {
            if (gate->getType() != cGate::OUTPUT)
                continue;
        }

        refreshConnection(*it);
    }
}

void ModuleCanvasViewer::refreshConnections()
{
    if (object) {
        refreshConnections(object);
        for (cModule::SubmoduleIterator it(object); !it.end(); ++it)
            refreshConnections(*it);
        changedConnections.clear();
    }
}

void ModuleCanvasViewer::redraw()
{
    clear();
    if (object == nullptr)
        return;

    zoomLabel->setFont(getQtenv()->getCanvasFont());
    updateZoomLabelPos();

    try {
        refreshLayout();
        redrawModules();
        redrawFigures();
        refreshSubmodules();
        refreshConnections();
    }
    catch (std::exception& e) {
        getQtenv()->showException(e);
    }
    recalcSceneRect();
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

    // redraw from scratch only if really needed
    if (needsRedraw) {
        redraw();
        needsRedraw = false;
    }
    else {
        refreshFigures();
        refreshQueueSizes(); // unconditionally on all submodules, because we don't get change notification

        for (auto s : changedSubmodules) {
            getQtenv()->getModuleLayouter()->refreshPositionFromDS(s);
            refreshSubmodule(s);

            // have to update all incoming and outgoing connections as well,
            // the bounding rect of the submodule might have changed
            for (cModule::GateIterator it(s); !it.end(); ++it) {
                cGate *gate = *it;
                if (gate->getType() == cGate::OUTPUT)
                    changedConnections.insert(gate);

                if (gate->getType() == cGate::INPUT)
                    changedConnections.insert(gate->getPreviousGate());
            }
        }

        // has to be done after the submodules have been positioned, but before connections
        if (compoundModuleChanged || !changedSubmodules.empty()) {
            redrawEnclosingModule();

            for (cModule::GateIterator it(object); !it.end(); ++it) {
                cGate *gate = *it;
                if (gate->getType() == cGate::INPUT)
                    changedConnections.insert(gate);

                // connections on OUTPUT gates are not shown, they are "outside"
            }
        }

        for (auto g : changedConnections)
            if (g && g->getNextGate()) {// if any gate was unconnected above, we added a nullptr
                refreshConnection(g);
                if (isTwoWayConnection(g)) {
                    // if it is two way connection, refresh the "other half" of it as well,
                    // so in case the connection line itself has changed, it isn't split in two
                    cGate *otherDirection = getGateOtherHalf(g->getNextGate());
                    if (!contains(changedConnections, otherDirection)) // don't do it twice
                        refreshConnection(otherDirection);
                }
            }
    }

    compoundModuleChanged = false;
    changedConnections.clear();
    changedSubmodules.clear();

    recalcSceneRect();
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
        zoomLabel->setFont(getQtenv()->getCanvasFont());
        updateZoomLabelPos();

        if (!object || notDrawn)
            return;

        needsRedraw = false;

        refreshLayout();
        refreshSubmodules();
        // has to be done after the submodules have been positioned, but before connections
        redrawEnclosingModule();
        refreshConnections();

        recalcSceneRect();

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

void ModuleCanvasViewer::setSubmoduleNameFormat(SubmoduleNameFormat format)
{
    if (submoduleNameFormat != format) {
        submoduleNameFormat = format;
        for (auto i : submoduleGraphicsItems)
            i.second->setNameFormat(format);
    }
}

void ModuleCanvasViewer::setShowArrowheads(bool show)
{
    if (showArrowHeads != show) {
        showArrowHeads = show;
        refreshConnections();
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
