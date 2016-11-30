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
#include "layout/graphlayouter.h"
#include "layout/basicspringembedderlayout.h"
#include "layout/forcedirectedgraphlayouter.h"
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

#define emit

#define UNKNOWNICON_WIDTH     32
#define UNKNOWNICON_HEIGHT    32

using namespace omnetpp::layout;

namespace omnetpp {
namespace qtenv {

ModuleCanvasViewer::ModuleCanvasViewer() :
    object(nullptr),
    layoutSeed(1),
    notDrawn(false),
    needs_redraw(false)
{
    setFont(getQtenv()->getCanvasFont());

    backgroundLayer = new GraphicsLayer();
    rangeLayer = new GraphicsLayer();
    submoduleLayer = new GraphicsLayer();
    figureLayer = new GraphicsLayer();
    animationLayer = new GraphicsLayer();
    bubbleLayer = new GraphicsLayer();
    zoomLabelLayer = new GraphicsLayer();

    setScene(new QGraphicsScene());
    scene()->addItem(backgroundLayer);
    scene()->addItem(figureLayer);
    scene()->addItem(bubbleLayer);
    scene()->addItem(zoomLabelLayer);

    GraphicsLayer *networkLayer = new GraphicsLayer();
    networkLayer->addItem(rangeLayer);
    networkLayer->addItem(submoduleLayer);
    networkLayer->addItem(animationLayer);

    zoomLabel = new ZoomLabel();
    zoomLabelLayer->addItem(zoomLabel);
    zoomLabel->setZoomFactor(zoomFactor);
    zoomLabel->setFont(getQtenv()->getCanvasFont());

    canvasRenderer = new CanvasRenderer();
    canvasRenderer->setLayer(figureLayer, nullptr, networkLayer);
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

void ModuleCanvasViewer::relayoutAndRedrawAll()
{
    if (!object)
        return;

    cModule *mod = object;
    int submoduleCount = 0;
    int estimatedGateCount = mod->gateCount();
    for (cModule::SubmoduleIterator it(mod); !it.end(); ++it) {
        submoduleCount++;
        // note: estimatedGateCount will count unconnected gates in the gate array as well
        estimatedGateCount += (*it)->gateCount();
    }

    notDrawn = false;
    if (submoduleCount > 1000 || estimatedGateCount > 4000) {
        char problem[200];
        if (submoduleCount > 1000)
            sprintf(problem, "contains more than 1000 submodules (exactly %d)", submoduleCount);
        else
            sprintf(problem, "may contain a lot of connections (modules have a large number of gates)");

        QString message = "Module '" + QString(object->getFullName()) + "' " + problem +
                ", it may take a long time to display the graphics.\n\nDo you want to proceed with drawing?";
        QMessageBox::StandardButton answer = QMessageBox::warning(this, "Warning", message, QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::Yes) {
            notDrawn = true;
            clear();  // this must be done, still
            return;
        }
    }

    zoomLabel->setFont(getQtenv()->getCanvasFont());
    updateZoomLabelPos();

    clear();
    recalculateLayout();
    redrawFigures();
    redrawModules();
    getQtenv()->getMessageAnimator()->redrawMessages();
    redrawNextEventMarker();
    refreshSubmodules();
}

void ModuleCanvasViewer::recalculateLayout()
{
    // refresh layout with empty submodPosMap -- everything layouted
    submodPosMap.clear();
    refreshLayout();
}

void ModuleCanvasViewer::refreshLayout()
{
    static bool inProgress = false;

    if (inProgress || !object)
        return;

    inProgress = true;

    // recalculate layout, using coordinates in submodPosMap as "fixed" nodes --
    // only new nodes are re-layouted

    cModule *parentModule = object;

    // Note trick avoid calling getDisplayString() directly because it'd cause
    // the display string object inside cModule to spring into existence
    const cDisplayString blank;
    const cDisplayString& ds = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString() : blank;

    // create and configure layouter object
    LayouterChoice choice = getQtenv()->opt->layouterChoice;
    if (choice == LAYOUTER_AUTO) {
        const int LIMIT = 20;  // note: on test/anim/dynamic2, Advanced is already very slow with 30-40 modules
        int submodCountLimited = 0;
        for (cModule::SubmoduleIterator it(parentModule); !it.end() && submodCountLimited < LIMIT; ++it)
            submodCountLimited++;
        choice = submodCountLimited >= LIMIT ? LAYOUTER_FAST : LAYOUTER_ADVANCED;
    }
    GraphLayouter *layouter = choice == LAYOUTER_FAST ?
        (GraphLayouter *)new BasicSpringEmbedderLayout() :
        (GraphLayouter *)new ForceDirectedGraphLayouter();

    layouter->setSeed(layoutSeed);

    // background size
    int sx = resolveLongDispStrArg(ds.getTagArg("bgb", 0), parentModule, 0);
    int sy = resolveLongDispStrArg(ds.getTagArg("bgb", 1), parentModule, 0);
    int border = 30;
    if (sx != 0 && sx < 2*border)
        border = sx/2;
    if (sy != 0 && sy < 2*border)
        border = sy/2;
    layouter->setSize(sx, sy, border);
    // TODO support "bgp" tag ("background position")

    // loop through all submodules, get their sizes and positions and feed them into layouting engine
    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it) {
        cModule *submod = *it;

        bool explicitCoords, obeysLayout;
        double x, y, sx, sy;
        getSubmoduleCoords(submod, explicitCoords, obeysLayout, x, y, sx, sy);

        // add node into layouter:
        if (explicitCoords) {
            // e.g. "p=120,70" or "p=140,30,ring"
            layouter->addFixedNode(submod->getId(), x, y, sx, sy);
        }
        else if (submodPosMap.find(submod) != submodPosMap.end()) {
            // reuse coordinates from previous layout
            QPointF pos = submodPosMap[submod];
            layouter->addFixedNode(submod->getId(), pos.x(), pos.y(), sx, sy);
        }
        else if (obeysLayout) {
            // all modules are anchored to the anchor point with the vector's name
            // e.g. "p=,,ring"
            layouter->addAnchoredNode(submod->getId(), submod->getName(), x, y, sx, sy);
        }
        else {
            layouter->addMovableNode(submod->getId(), sx, sy);
        }
    }

    // add connections into the layouter, too
    bool atParent = false;
    for (cModule::SubmoduleIterator it(parentModule); !atParent; ++it) {
        cModule *mod = !it.end() ? *it : (atParent = true, parentModule);

        for (cModule::GateIterator git(mod); !git.end(); ++git) {
            cGate *gate = *git;
            cGate *destGate = gate->getNextGate();
            if (gate->getType() == (atParent ? cGate::INPUT : cGate::OUTPUT) && destGate) {
                cModule *destMod = destGate->getOwnerModule();
                if (mod == parentModule && destMod == parentModule) {
                    // nop
                }
                else if (destMod == parentModule) {
                    layouter->addEdgeToBorder(mod->getId());
                }
                else if (destMod->getParentModule() != parentModule) {
                    // connection goes to a module under a different parent!
                    // this in fact violates module encapsulation, but let's
                    // accept it nevertheless. Just skip this connection.
                }
                else if (mod == parentModule) {
                    layouter->addEdgeToBorder(destMod->getId());
                }
                else {
                    layouter->addEdge(mod->getId(), destMod->getId());
                }
            }
        }
    }

    // set up layouter environment (responsible for "Stop" button handling and visualizing the layouting process)
    qtenv::QtenvGraphLayouterEnvironment qtenvEnvironment(parentModule, ds);
    connect(getQtenv()->getMainWindow()->getStopAction(), SIGNAL(triggered()), &qtenvEnvironment, SLOT(stop()));

    // we still have to set something for the layouter if visualisation is disabled.
    BasicGraphLayouterEnvironment basicEnvironment;

    // we are replacing the scene in the view with a temporary one which is used only
    // for visualising the layouting process, and is managed by the qtenvEnvironment
    // so we don't ruin the layer structure in the existing scene

    auto moduleScene = scene();
    QGraphicsScene *layoutScene = nullptr;

    // enable visualizing only if full re-layouting (no cached coordinates in submodPosMap)
    if (submodPosMap.empty() && getQtenv()->opt->showLayouting) {  // for realz
        // if (getQtenv()->opt->showLayouting) { // for debugging
        layoutScene = new QGraphicsScene(this);

        setSceneRect(QRectF());
        setScene(layoutScene);
        qtenvEnvironment.setView(this);

        layoutScene->setBackgroundBrush(QColor("#a0e0a0"));

        getQtenv()->getMainWindow()->enterLayoutingMode();

        layouter->setEnvironment(&qtenvEnvironment);
    }
    else {
        layouter->setEnvironment(&basicEnvironment);
    }

    layouter->execute();
    qtenvEnvironment.cleanup();

    if (moduleScene != scene())
        setScene(moduleScene);

    delete layoutScene;

    getQtenv()->getMainWindow()->exitLayoutingMode();

    // fill the map with the results
    submodPosMap.clear();
    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it) {
        cModule *submod = *it;

        QPointF pos;
        double x, y;
        layouter->getNodePosition(submod->getId(), x, y);
        pos.setX(x);
        pos.setY(y);
        submodPosMap[submod] = pos;
    }

    layoutSeed = layouter->getSeed();

    delete layouter;

    inProgress = false;
}

void ModuleCanvasViewer::getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
        double& x, double& y, double& sx, double& sy)
{
    const cDisplayString blank;
    const cDisplayString& ds = submod->hasDisplayString() && submod->parametersFinalized() ? submod->getDisplayString() : blank;

    // get size -- we'll need to return that too, and may be needed for matrix, ring etc. layout
    double boxsx = 0, boxsy = 0;
    int iconsx = 30, iconsy = 30;
    if (ds.containsTag("b") || !ds.containsTag("i")) {
        boxsx = resolveDoubleDispStrArg(ds.getTagArg("b", 0), submod, 40);
        boxsy = resolveDoubleDispStrArg(ds.getTagArg("b", 1), submod, 24);
    }/*
    if (ds.containsTag("i")) {
        const char *imgName = ds.getTagArg("i", 0);
        const char *imgSize = ds.getTagArg("is", 0);
        if (!imgName || !*imgName) {
            iconsx = UNKNOWNICON_WIDTH;
            iconsy = UNKNOWNICON_HEIGHT;
        }
        else {
            //CHK(Tcl_VarEval(interp, "lookupImage ", imgName, " ", imgSize, TCL_NULL));
            iconsx = iconsy = 30;  // TODO
            Tk_Image img = Tk_GetImage(interp, Tk_MainWindow(interp), Tcl_GetStringResult(interp), nullptr, nullptr);
            if (!img)
            {
                iconsx = UNKNOWNICON_WIDTH;
                iconsy = UNKNOWNICON_HEIGHT;
            }
            else
            {
                Tk_SizeOfImage(img, &iconsx, &iconsy);
                Tk_FreeImage(img);
            }
        }
    }*/
    sx = (boxsx > iconsx) ? boxsx : iconsx;
    sy = (boxsy > iconsy) ? boxsy : iconsy;

    // first, see if there's an explicit position ("p=" tag) given
    x = resolveDoubleDispStrArg(ds.getTagArg("p", 0), submod, -1);
    y = resolveDoubleDispStrArg(ds.getTagArg("p", 1), submod, -1);
    explicitcoords = x != -1 && y != -1;

    // set missing coordinates to zero
    if (x == -1)
        x = 0;
    if (y == -1)
        y = 0;

    const char *layout = ds.getTagArg("p", 2);  // matrix, row, column, ring, exact etc.
    obeysLayout = (layout && *layout);

    // modify x,y using predefined layouts
    if (!layout || !*layout) {
        // we're happy
    }
    else if (!strcmp(layout, "e") || !strcmp(layout, "x") || !strcmp(layout, "exact")) {
        double dx = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, 0);
        double dy = resolveDoubleDispStrArg(ds.getTagArg("p", 4), submod, 0);
        x += dx;
        y += dy;
    }
    else if (!strcmp(layout, "r") || !strcmp(layout, "row")) {
        // perhaps we should use the size of the 1st element in the vector?
        double dx = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, 2*sx);
        x += submod->getIndex()*dx;
    }
    else if (!strcmp(layout, "c") || !strcmp(layout, "col") || !strcmp(layout, "column")) {
        double dy = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, 2*sy);
        y += submod->getIndex()*dy;
    }
    else if (!strcmp(layout, "m") || !strcmp(layout, "matrix")) {
        // perhaps we should use the size of the 1st element in the vector?
        int columns = resolveLongDispStrArg(ds.getTagArg("p", 3), submod, 5);
        double dx = resolveDoubleDispStrArg(ds.getTagArg("p", 4), submod, 2*sx);
        double dy = resolveDoubleDispStrArg(ds.getTagArg("p", 5), submod, 2*sy);
        if (columns < 1)
            columns = 1;
        x += (submod->getIndex() % columns)*dx;
        y += (submod->getIndex() / columns)*dy;
    }
    else if (!strcmp(layout, "i") || !strcmp(layout, "ri") || !strcmp(layout, "ring")) {
        // perhaps we should use the size of the 1st element in the vector?
        int vectorSize = submod->getVectorSize();
        double rx = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, (sx+sy)*vectorSize/4);
        double ry = resolveDoubleDispStrArg(ds.getTagArg("p", 4), submod, rx);

        x += rx - rx*sin(submod->getIndex()*2*PI/vectorSize);
        y += ry - ry*cos(submod->getIndex()*2*PI/vectorSize);
    }
    else {
        //CHK(Tcl_VarEval(interp, "messagebox {Error} "
        //                        "{Error: invalid layout '", layout, "' in 'p' tag "
        //                                                            "of display string \"", ds.str(), "\"} error ok", TCL_NULL));
    }
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
        assert(submodPosMap.find(submod) != submodPosMap.end());
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

        auto figuresRect = figureLayer->mapToScene(figureLayer->childrenBoundingRect()).boundingRect();

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
    return submodPosMap[mod] * zoomFactor;
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

void ModuleCanvasViewer::refreshSubmodule(cModule *submod)
{
    if (submoduleGraphicsItems.count(submod)) {
        auto item = submoduleGraphicsItems[submod];
        SubmoduleItemUtil::setupFromDisplayString(item, submod);
        bool xplct, obeys;
        double x, y, sx, sy;
        getSubmoduleCoords(submod, xplct, obeys, x, y, sx, sy);
        if (xplct)
            submodPosMap[submod] = QPointF(x, y);
        item->setPos(getSubmodCoords(submod));
        item->update();
    }
}

void ModuleCanvasViewer::refreshSubmodules()
{
    if (object) {
        refreshLayout();
        for (cModule::SubmoduleIterator it(object); !it.end(); ++it)
            refreshSubmodule(*it);
    }
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
    if (needs_redraw) {
        needs_redraw = false;
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
    if (!submodPosMap.count(submod))
        refreshLayout();

    // will delete itself after a timeout
    bubbleLayer->addItem(new BubbleItem(getSubmodCoords(submod), text));
}

}  // namespace qtenv
}  // namespace omnetpp

