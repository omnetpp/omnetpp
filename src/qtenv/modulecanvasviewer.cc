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
#include "qtutil.h"
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
#include <QtGui/QAction>
#include <QtWidgets/QRubberBand>
#include <QtWidgets/QToolTip>
#include <QtPrintSupport/QPrinter>
#include <QtGui/QImageWriter>
#include <QtGui/QPageSize>
#include <QtPrintSupport/QPrintDialog>
#include <QtWidgets/QFileDialog>

using namespace omnetpp::common;
using namespace omnetpp::layout;

namespace omnetpp {
namespace qtenv {

bool VisibilityWatcher::eventFilter(QObject *obj, QEvent *event)
{
    bool result = QObject::eventFilter(obj, event);
    if (event->type() == QEvent::Show || event->type() == QEvent::Hide)
        Q_EMIT visibilityChanged();
    return result;
}

ModuleCanvasViewer::ModuleCanvasViewer()
{
    setFont(getQtenv()->getCanvasFont());

    backgroundLayer = new GraphicsLayer();
    rangeLayer = new GraphicsLayer();
    submoduleLayer = new GraphicsLayer();
    figureLayer = new GraphicsLayer();
    animationLayer = new GraphicsLayer();
    bubbleLayer = new GraphicsLayer();

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

    networkLayer = new GraphicsLayer();
    networkLayer->addItem(rangeLayer);
    networkLayer->addItem(submoduleLayer);
    networkLayer->addItem(animationLayer);

    canvasRenderer = new CanvasRenderer();
    canvasRenderer->setLayer(figureLayer, nullptr, networkLayer);

    // that beautiful green shade behind everything
    setBackgroundBrush(colors::LIGHTGREEN);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setDragMode(ScrollHandDrag);
    // otherwise it would be a hand, that's why this is in mousePressEvent and mouseReleaseEvent too
    viewport()->setCursor(Qt::ArrowCursor);
    setResizeAnchor(AnchorViewCenter);

    setMouseTracking(true);

    // otherwise if the content fits exactly, these will disappear only for a moment during resizing
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QGridLayout *overlayLayout = new QGridLayout(this);

    VisibilityWatcher *watcher = new VisibilityWatcher(this);
    connect(watcher, &VisibilityWatcher::visibilityChanged, [this, overlayLayout]() {
        // While the scrollbars may not ever be dynamically shown/hidden,
        // their sizes are reported incorrectly before they are first shown,
        // so we have to adjust the margins only after they become visible.
        const int toolbarSpacing = 4; // from the edges, in pixels
        overlayLayout->setContentsMargins(
                toolbarSpacing, toolbarSpacing,
                verticalScrollBar()->width() + toolbarSpacing,
                horizontalScrollBar()->height() + toolbarSpacing);
    });

    horizontalScrollBar()->installEventFilter(watcher);
    verticalScrollBar()->installEventFilter(watcher);

    zoomLabel = new QLabel(this);
    zoomLabel->setAutoFillBackground(true);
    zoomLabel->setBackgroundRole(QPalette::Mid);
    zoomLabel->setContentsMargins(4, 2, 4, 2);
    zoomLabel->setText("Zoom: " + QString::number(zoomFactor, 'f', 2) + "x");

    // the optional floating toolbar will occupy the first row
    overlayLayout->addWidget(zoomLabel, 1, 0, Qt::AlignRight | Qt::AlignBottom);

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
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

QString ModuleCanvasViewer::tooltipAt(const QPoint &pos, int threshold)
{
    QString tooltip = tooltipAt(QRect(pos, pos));

    if (tooltip.isEmpty())
        tooltip = tooltipAt(QRect(pos, pos).adjusted(-threshold, -threshold, threshold, threshold));

    return tooltip;
}

QString ModuleCanvasViewer::tooltipAt(const QRect& rect)
{
    // Note: The individual items' setToolTip() method cannot be used
    // because that way they are stealing the ToolTip event,
    // and then the viewer itself doesn't have a chance to collect
    // all the relevant tooltips for every component/message/figure
    // under the cursor, and aggregate them.
    // So we have to store their tooltips as custom user data.

    QPolygonF sceneRect = mapToScene(rect);
    auto items = scene()->items(sceneRect);
    QPainterPath area;
    area.addPolygon(sceneRect);
    removeClippedItemsOutsideShape(items, area);
    auto objects = gatherObjects(items);
    bool verboseTooltip = (objects.size() == 1);  // initially

    auto tips = gatherTooltips(objects, verboseTooltip);
    if (tips.empty())
        return "";

    // if we got the flag wrong, redo it with the flag flipped
    if ((tips.size() <= 1) != verboseTooltip) {
        tips = gatherTooltips(objects, !verboseTooltip);
    }

    // multi-object tooltips should not be wrapped (they're one line per object)
    if (tips.size() > 1 || !tips[0].contains("\n"))
        tips[0] = "<p style='white-space:pre'>" + tips[0];

    QString joined;
    for (int i = 0; i < tips.size(); ++i) {
        if (i > 0)
            joined += "<br>";
        joined += tips[i].replace("\n", "<br>");
    }
    return joined;
}

std::vector<cObject *> ModuleCanvasViewer::gatherObjects(const QList<QGraphicsItem*>& items)
{
    std::vector<cObject *> objects;
    for (auto i : items) {
        cObject *itemObject = i->data(ITEMDATA_COBJECT).value<cObject *>();

        if (itemObject && itemObject != object) {
            // deduplication
            if (std::find(objects.begin(), objects.end(), itemObject) == objects.end())
                objects.push_back(itemObject);
        }
    }
    return objects;
}

QStringList ModuleCanvasViewer::gatherTooltips(const std::vector<cObject*>& objects, bool singleObjectTooltip)
{
    QStringList tips;
    for (cObject *obj : objects) {
        QString tip = makeObjectTooltip(obj, singleObjectTooltip, object);
        if (!tip.isEmpty() && !tips.contains(tip))
            tips += tip;
    }
    return tips;
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
        Q_EMIT doubleClick(event);
}

void ModuleCanvasViewer::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::LeftButton: Q_EMIT click(event); break;
        case Qt::XButton1:   Q_EMIT back();       break;
        case Qt::XButton2:   Q_EMIT forward();    break;
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
            Q_EMIT contextMenuRequested(getObjectsAt(rect), event->globalPos());
        }
        else
            Q_EMIT marqueeZoom(rect);
        rubberBand->hide();
    }
    else {
        Q_EMIT dragged(mapToScene(viewport()->rect().center()));
    }

    viewport()->setCursor(Qt::ArrowCursor);
}

void ModuleCanvasViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (QToolTip::isVisible())
        QToolTip::showText(event->globalPos(), tooltipAt(event->pos()), this);

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
    QGraphicsView::resizeEvent(event);

    if (isEnabled())
        recalcSceneRect();
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
            QToolTip::showText(helpEvent->globalPos(), tooltipAt(helpEvent->pos()), this);
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
        Q_EMIT contextMenuRequested(objects, event->globalPos());
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

    while (true) {
        fileName = QFileDialog::getSaveFileName(this, "Export to Image", fileName, filter, &selF);

        if (fileName.isNull())
            return; // the file selection dialog got cancelled

        bool extensionFound = false;
        for (auto f : QImageWriter::supportedImageFormats())
            if (fileName.endsWith("." + QString(f), Qt::CaseInsensitive))
                extensionFound = true;
        if (extensionFound)
            break;

        QMessageBox::warning(this, "Export to Image", "Please enter a file name ending with a supported extension (e.g. .png, .jpg, .bmp).");
    }

    QImage image(sceneRect.size().toSize(), QImage::Format_RGBA8888); // Premultiplied?

    renderToPaintDevice(image, sceneRect, QRectF(QPointF(0, 0), sceneRect.size().toSize()));

    if (!image.save(fileName))
        QMessageBox::warning(this, "Export to Image", "Failed to save image to file: " + fileName);
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

    QPrintDialog printDialog(&printer, this);
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
    printer.setPageSize(QPageSize(sceneRect.size() / printer.resolution(), QPageSize::Inch));
    printer.setFullPage(true);

    renderToPaintDevice(printer, sceneRect, printer.pageRect(QPrinter::DevicePixel));
}

void ModuleCanvasViewer::renderToPaintDevice(QPaintDevice &printer, const QRectF& sceneRect, const QRectF& pageRect)
{
    scene()->setSceneRect(sceneRect);

    QPainter painter;

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

    // Build connection grouping if the feature is enabled
    if (distributeConnectionArrows)
        buildConnectionGrouping();

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

using ConnBundleKey = std::tuple<cModule *, cModule*, char> ;

ConnBundleKey makeConnBundleKey(cGate *gate)
{
    cModule *srcMod = gate->getOwnerModule();
    cModule *destMod = gate->getNextGate()->getOwnerModule();

    char dir = 'a';
    if (cChannel *chan = gate->getChannel()) {
        // TODO: need to use DisplayStringAccess ?
        if (chan->hasDisplayString()) {
            const char *v = chan->getDisplayString().getTagArg("m", 0);
            if (v[0])
                dir = v[0];
        }
    }

    // Create a canonical ordering for the module pair (smaller ID first)
    if (srcMod->getId() < destMod->getId())
        return { srcMod, destMod, dir };
    else
        return { destMod, srcMod, dir };
}

void ModuleCanvasViewer::buildConnectionGrouping()
{
    connectionGrouping.clear();

    if (!object)
        return;

    // Map from (srcModule, destModule) pairs to list of gates
    // Using canonical ordering: smaller module ID first
    std::map<ConnBundleKey, std::vector<cGate*>> modulePairConnections;

    // Collect all connections grouped by module pairs
    cModule *parentModule = object;
    bool atParent = false;
    for (cModule::SubmoduleIterator it(parentModule); !atParent; ++it) {
        cModule *mod = !it.end() ? *it : (atParent = true, parentModule);

        for (cModule::GateIterator git(mod); !git.end(); ++git) {
            cGate *gate = *git;

            if (gate->getType() == (atParent ? cGate::INPUT : cGate::OUTPUT) && gate->getNextGate() != nullptr) {
                if (isReverseHalfOfTwoWayConnection(gate)) {
                    // To avoid double-counting bidirectional connections,
                    // only consider the forward half
                    continue;
                }

                auto key = makeConnBundleKey(gate);

                modulePairConnections[key].push_back(gate);
            }
        }
    }

    // Assign indices to connections within each module pair
    for (auto& pair : modulePairConnections) {
        auto& gates = pair.second;

        int index = 0;
        // Assign indices - one per visual line
        for (cGate* gate : gates) {

            // Check if this is a bidirectional connection (between inout gates)
            if (isTwoWayConnection(gate)) {
                // This gate and its reverse connection form a single visual line
                cGate *reverseGate = gate->getNextGate()->getOtherHalf();

                // Assign the same index to both directions
                connectionGrouping[gate] = std::make_pair(index, 0);
                connectionGrouping[reverseGate] = std::make_pair(index, 0);
            }
            else {
                // Unidirectional connection - gets its own visual line
                connectionGrouping[gate] = std::make_pair(index, 0);
            }

            index++;
        }

        // Update all gates with the correct total count (number of visual lines)
        for (auto& entry : connectionGrouping) {
            cGate *gate = entry.first;

            // Check if this gate belongs to the current module pair
            auto key = makeConnBundleKey(gate);

            if (key == pair.first) {
                entry.second.second = index;
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
    catch (cRuntimeError& e) {
        e.prependMessage("Error drawing submodule %s: ", submod->getNedTypeAndFullPath().c_str());
        throw;
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
    catch (cRuntimeError& e) {
        e.prependMessage("Error redrawing module %s: ", object->getNedTypeAndFullPath().c_str());
        throw;
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
            auto corner = compoundModuleItem->boundingRect().topLeft();
            ensureVisible(corner.x()-margin, corner.y()-margin, width(), height(), 0, 0);
        }
    }
}

void ModuleCanvasViewer::drawConnection(cGate *gate)
{
    ASSERT(gate->getOwnerModule() == object || gate->getOwnerModule()->getParentModule() == object);
    ASSERT(!containsKey(connectionGraphicsItems, gate));

    auto item = new ConnectionItem(submoduleLayer);
    try {
        item->setPoints(getConnectionLine(gate));
        ConnectionItemUtil::setupFromDisplayString(item, gate, showArrowHeads);
    }
    catch (cRuntimeError& e) {
        e.prependMessage("Error drawing connection originating in gate %s: ", gate->getFullPath().c_str());
        throw;
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

QPolygonF ModuleCanvasViewer::getConnectionLine(cGate *gate)
{
    ASSERT(gate->getOwnerModule() == object || gate->getOwnerModule()->getParentModule() == object);

    cGate *nextGate = gate ? gate->getNextGate() : nullptr;
    if (!nextGate)
        return QPolygonF();

    // For bidirectional connections, both halves must follow the same polyline path.
    // Canonicalize: always use the forward half.
    // The reverse half reuses the forward half's channel for display string parsing,
    // since NED only sets the display string on the forward channel.
    bool isReverseHalf = isReverseHalfOfTwoWayConnection(gate);

    cGate *canonicalGate = gate;
    if (isReverseHalf) {
        // Get the forward half's gate: nextGate's other half (inout pair)
        canonicalGate = nextGate->getOtherHalf();
    }

    layout::RoutingConstraint constraint;

    auto channel = canonicalGate ? canonicalGate->getChannel() : nullptr;
    if (channel) {
        cDisplayString ds = channel->getDisplayString();

        std::string buffer;
        DisplayStringAccess dsa(&ds, channel);

        const char *arg0 = dsa.getTagArg("m", 0, buffer);
        char ch = arg0[0];
        if (ch == 'a') {
            // legacy 'a' (auto) = both unconstrained (default)
        }
        else if (ch == 'm') {
            // legacy manual mode: m=m,srcAnchX,srcAnchY,destAnchX,destAnchY
            constraint.srcDir = 'm';
            bool ok;
            int v;
            v = dsa.getTagArgAsLong("m", 1, 0.0, &ok); if (ok) constraint.srcAnchX = v;
            v = dsa.getTagArgAsLong("m", 2, 0.0, &ok); if (ok) constraint.srcAnchY = v;
            v = dsa.getTagArgAsLong("m", 3, 0.0, &ok); if (ok) constraint.destAnchX = v;
            v = dsa.getTagArgAsLong("m", 4, 0.0, &ok); if (ok) constraint.destAnchY = v;
        }
        else {
            if (strchr("newshv", ch))
                constraint.srcDir = ch;
            // arg 1 is dest direction (if present and a valid direction letter)
            const char *arg1 = dsa.getTagArg("m", 1, buffer);
            if (arg1[0] && strchr("newshv", arg1[0]))
                constraint.destDir = arg1[0];
        }
    }

    int bundle_i = 0, bundle_n = 1;

    if (distributeConnectionArrows) {
        // Check if we have grouping information for this connection
        auto it = connectionGrouping.find(gate);
        if (it != connectionGrouping.end()) {
            // Use the module-pair-based grouping
            bundle_i = it->second.first;  // index within group
            bundle_n = it->second.second; // total count in group
        }
    }

    cModule *owner = gate->getOwnerModule();
    cModule *nextOwner = nextGate->getOwnerModule();

    QRectF ownerRect = getSubmodRect(owner);
    QRectF nextRect = getSubmodRect(nextOwner);

    QPolygonF poly;
    if (isReverseHalf) {
        // Constraint was parsed from the forward half's channel (src=forward_src, dest=forward_dest).
        // We call arrowcoords with forward direction, then reverse the polyline.
        poly = arrowcoords(nextRect, ownerRect, bundle_i, bundle_n, constraint);
        std::reverse(poly.begin(), poly.end());
    }
    else {
        poly = arrowcoords(ownerRect, nextRect, bundle_i, bundle_n, constraint);
    }

    // Handling degenerate connections (those crossing compound module boundaries without a gate).
    cModule *parent = owner->getParentModule();
    cModule *nextParent = nextOwner->getParentModule();

    bool isIn = gate->getType() == cGate::INPUT;
    bool isOut = gate->getType() == cGate::OUTPUT;
    bool nextIsIn = nextGate->getType() == cGate::INPUT;
    bool nextIsOut = nextGate->getType() == cGate::OUTPUT;

    // The non-degenerate cases, where "parent" is the inspected compound module
    bool isProper = (parent == nextParent && isOut && nextIsIn)   // submodule -> submodule
                 || ( owner == nextParent && isIn  && nextIsIn)   // parent -> submodule
                 || (parent == nextOwner  && isOut && nextIsOut)  // submodule -> parent
                 || ( owner == nextOwner  && isIn  && nextIsOut); // parent input -> parent output

    if (!isProper) {
        // The connection is degenerate, so only drawing a half length line
        // to not make a false impression about where it actually ends.
        // Truncate polyline to half its length
        if (poly.size() >= 2) {
            QPolygonF half;
            half << poly.first();
            QPointF mid = (poly.first() + poly.last()) / 2.0;
            half << mid;
            poly = half;
        }
    }

    return poly;
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
    QPolygonF sceneRect = mapToScene(rect);
    QList<QGraphicsItem *> items = scene()->items(sceneRect);
    QPainterPath area;
    area.addPolygon(sceneRect);
    removeClippedItemsOutsideShape(items, area);
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

void ModuleCanvasViewer::setFloatingToolbar(QToolBar *toolbar)
{
    ((QGridLayout *)layout())->addWidget(toolbar, 0, 0, Qt::AlignRight | Qt::AlignTop);
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
    catch (cRuntimeError& e) {
        e.prependMessage("Error redrawing submodule %s: ", submod->getNedTypeAndFullPath().c_str());
        throw;
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
    catch (cRuntimeError& e) {
        e.prependMessage("Error refreshing connection originating in gate %s: ", gate->getFullPath().c_str());
        throw;
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error refreshing connection originating in gate %s: %s", gate->getFullPath().c_str(), e.what());
    }

    item->setPoints(getConnectionLine(gate));
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
                    cGate *otherDirection = g->getNextGate()->getOtherHalf();
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
        zoomLabel->setText("Zoom: " + QString::number(zoomFactor, 'f', 2) + "x");

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

void ModuleCanvasViewer::setDistributeConnectionArrows(bool distribute)
{
    if (distributeConnectionArrows != distribute) {
        distributeConnectionArrows = distribute;
        redrawModules();
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
