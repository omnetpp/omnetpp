//==========================================================================
//  CANVASVIEWER.CC - part of
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

#include "canvasviewer.h"
#include "canvasrenderer.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/csimplemodule.h"
#include "moduleinspector.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cfutureeventset.h"
#include "layout/graphlayouter.h"
#include "layout/basicspringembedderlayout.h"
#include "layout/forcedirectedgraphlayouter.h"
#include "qtenv.h"
#include "figurerenderers.h"
#include "omnetpp/cmodule.h"
#include "inspector.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <canvasrenderer.h>
#include "animator.h"
#include "compoundmoduleitem.h"
#include "connectionitem.h"
#include "submoduleitem.h"
#include <QDebug>
#include "arrow.h"

#define emit

#define UNKNOWNICON_WIDTH     32
#define UNKNOWNICON_HEIGHT    32

using namespace OPP::layout;

namespace omnetpp {
namespace qtenv {

CanvasViewer::CanvasViewer() :
    object(nullptr)
{
    figureLayer = new GraphicsLayer();

    canvasRenderer = new CanvasRenderer();
    canvasRenderer->setLayer(figureLayer, nullptr);

    setScene(new QGraphicsScene());
    scene()->addItem(figureLayer);

    // that beautiful green shade behind everything
    setBackgroundBrush(QColor("#a0e0a0"));
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

CanvasViewer::~CanvasViewer()
{
    delete canvasRenderer;
}

void CanvasViewer::setObject(cCanvas *obj)
{
    clear();
    object = obj;

    canvasRenderer->setCanvas(object);
    redraw();
}

void CanvasViewer::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit click(event);
}

void CanvasViewer::contextMenuEvent(QContextMenuEvent * event)
{
    emit contextMenuRequested(event);
}

void CanvasViewer::relayoutAndRedrawAll()
{
//    ASSERT(object != nullptr);

//    cModule *mod = object;
//    int submoduleCount = 0;
//    int estimatedGateCount = mod->gateCount();
//    for (cModule::SubmoduleIterator it(mod); !it.end(); ++it) {
//        submoduleCount++;
//        // note: estimatedGateCount will count unconnected gates in the gate array as well
//        estimatedGateCount += (*it)->gateCount();
//    }

//    notDrawn = false;
//    if (submoduleCount > 1000 || estimatedGateCount > 4000) {
//        char problem[200];
//        if (submoduleCount > 1000)
//            sprintf(problem, "contains more than 1000 submodules (exactly %d)", submoduleCount);
//        else
//            sprintf(problem, "may contain a lot of connections (modules have a large number of gates)");

//        QString message = "Module '" + QString(object->getFullName()) + "' " + problem +
//                ", it may take a long time to display the graphics.\n\nDo you want to proceed with drawing?";
//        QMessageBox::StandardButton answer = QMessageBox::warning(this, "Warning", message, QMessageBox::Yes | QMessageBox::No);
//        if(answer == QMessageBox::Yes)
//        {
//            notDrawn = true;
//            clear();  // this must be done, still
//            return;
//        }
//    }

    clear();
    redraw();
}

void CanvasViewer::recalculateLayout()
{
//    // refresh layout with empty submodPosMap -- everything layouted
//    submodPosMap.clear();
//    refreshLayout();
}

void CanvasViewer::refreshLayout()
{
//    // recalculate layout, using coordinates in submodPosMap as "fixed" nodes --
//    // only new nodes are re-layouted

//    cModule *parentModule = object;

//    // Note trick avoid calling getDisplayString() directly because it'd cause
//    // the display string object inside cModule to spring into existence
//    const cDisplayString blank;
//    const cDisplayString& ds = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString() : blank;

//    // create and configure layouter object
//    LayouterChoice choice = getQtenv()->opt->layouterChoice;
//    if (choice == LAYOUTER_AUTO) {
//        const int LIMIT = 20;  // note: on test/anim/dynamic2, Advanced is already very slow with 30-40 modules
//        int submodCountLimited = 0;
//        for (cModule::SubmoduleIterator it(parentModule); !it.end() && submodCountLimited < LIMIT; ++it)
//            submodCountLimited++;
//        choice = submodCountLimited >= LIMIT ? LAYOUTER_FAST : LAYOUTER_ADVANCED;
//    }
//    GraphLayouter *layouter = choice == LAYOUTER_FAST ?
//        (GraphLayouter *)new BasicSpringEmbedderLayout() :
//        (GraphLayouter *)new ForceDirectedGraphLayouter();

//    layouter->setSeed(layoutSeed);

//    // background size
//    int sx = resolveLongDispStrArg(ds.getTagArg("bgb", 0), parentModule, 0);
//    int sy = resolveLongDispStrArg(ds.getTagArg("bgb", 1), parentModule, 0);
//    int border = 30;
//    if (sx != 0 && sx < 2*border)
//        border = sx/2;
//    if (sy != 0 && sy < 2*border)
//        border = sy/2;
//    layouter->setSize(sx, sy, border);
//    // TODO support "bgp" tag ("background position")

//    // loop through all submodules, get their sizes and positions and feed them into layouting engine
//    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it) {
//        cModule *submod = *it;

//        bool explicitCoords, obeysLayout;
//        double x, y, sx, sy;
//        getSubmoduleCoords(submod, explicitCoords, obeysLayout, x, y, sx, sy);

//        // add node into layouter:
//        if (explicitCoords) {
//            // e.g. "p=120,70" or "p=140,30,ring"
//            layouter->addFixedNode(submod->getId(), x, y, sx, sy);
//        }
//        else if (submodPosMap.find(submod) != submodPosMap.end()) {
//            // reuse coordinates from previous layout
//            QPointF pos = submodPosMap[submod];
//            layouter->addFixedNode(submod->getId(), pos.x(), pos.y(), sx, sy);
//        }
//        else if (obeysLayout) {
//            // all modules are anchored to the anchor point with the vector's name
//            // e.g. "p=,,ring"
//            layouter->addAnchoredNode(submod->getId(), submod->getName(), x, y, sx, sy);
//        }
//        else {
//            layouter->addMovableNode(submod->getId(), sx, sy);
//        }
//    }

//    // add connections into the layouter, too
//    bool atParent = false;
//    for (cModule::SubmoduleIterator it(parentModule); !atParent; ++it) {
//        cModule *mod = !it.end() ? *it : (atParent = true, parentModule);

//        for (cModule::GateIterator git(mod); !git.end(); ++git) {
//            cGate *gate = *git;
//            cGate *destGate = gate->getNextGate();
//            if (gate->getType() == (atParent ? cGate::INPUT : cGate::OUTPUT) && destGate) {
//                cModule *destMod = destGate->getOwnerModule();
//                if (mod == parentModule && destMod == parentModule) {
//                    // nop
//                }
//                else if (destMod == parentModule) {
//                    layouter->addEdgeToBorder(mod->getId());
//                }
//                else if (destMod->getParentModule() != parentModule) {
//                    // connection goes to a module under a different parent!
//                    // this in fact violates module encapsulation, but let's
//                    // accept it nevertheless. Just skip this connection.
//                }
//                else if (mod == parentModule) {
//                    layouter->addEdgeToBorder(destMod->getId());
//                }
//                else {
//                    layouter->addEdge(mod->getId(), destMod->getId());
//                }
//            }
//        }
//    }

//    // set up layouter environment (responsible for "Stop" button handling and visualizing the layouting process)
//    // TODO TkenvGraphLayouterEnvironment
//    BasicGraphLayouterEnvironment environment;

//    // TODO
////    std::string stopButton = std::string(getWindowName()) + ".toolbar.stop";
////    bool isExpressMode = getTkenv()->getSimulationRunMode() == Qtenv::RUNMODE_EXPRESS;
////    if (!isExpressMode)
////        environment.setWidgetToGrab(stopButton.c_str());

////    // enable visualizing only if full re-layouting (no cached coordinates in submodPosMap)
////    // if (getTkenv()->opt->showlayouting)  // for debugging
////    if (submodPosMap.empty() && getTkenv()->opt->showLayouting)
////        environment.setCanvas(canvas);

//    layouter->setEnvironment(&environment);
//    layouter->execute();
//    // environment.cleanup();

//    // fill the map with the results
//    submodPosMap.clear();
//    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it) {
//        cModule *submod = *it;

//        QPointF pos;
//        double x, y;
//        layouter->getNodePosition(submod->getId(), x, y);
//        pos.setX(x);
//        pos.setY(y);
//        submodPosMap[submod] = pos;
//    }

//    layoutSeed = layouter->getSeed();

//    delete layouter;
}

void CanvasViewer::fillFigureRenderingHints(FigureRenderingHints *hints)
{
    QString prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":zoomfactor";
    QVariant variant = getQtenv()->getPref(prefName);
    hints->zoom = variant.isValid() ? variant.value<double>() : 1;

    prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":imagesizefactor";
    variant = getQtenv()->getPref(prefName);
    hints->iconMagnification = variant.isValid() ? variant.value<double>() : 1;

    prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":showlabels";
    variant = getQtenv()->getPref(prefName);
    hints->showSubmoduleLabels = variant.isValid() ? variant.value<bool>() : true;

    prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":showarrowheads";
    variant = getQtenv()->getPref(prefName);
    hints->showArrowHeads = variant.isValid() ? variant.value<bool>() : false;

    hints->defaultFont = scene()->font().family().toStdString();

    //TODO pixelSize() or pointSize()
    hints->defaultFontSize = scene()->font().pointSize() * 16 / 10;  // FIXME figure out conversion factor (point to pixel?)...
}

std::vector<cObject*> CanvasViewer::getObjectsAt(const QPoint &pos)
{
    QList<QGraphicsItem*> items = scene()->items(mapToScene(pos));
    std::vector<cObject*> objects;

    for (auto item : items) {
        QVariant variant = item->data(1);
        if (variant.isValid())
            objects.push_back(variant.value<cObject*>());
    }

    return objects;
}

void CanvasViewer::redraw()
{
    clear();
    if (object == nullptr)
        return;

    updateBackgroundColor();

    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    getCanvasRenderer()->redraw(&hints);
}

void CanvasViewer::updateBackgroundColor()
{
    //TODO szukseges-e a getCanvas() a ModuleInspector classban?
    /*
    cCanvas *canvas = getCanvas();
    if (canvas) {
        char buf[16];
        cFigure::Color color = canvas->getBackgroundColor();
        sprintf(buf, "#%2.2x%2.2x%2.2x", color.red, color.green, color.blue);
        //CHK(Tcl_VarEval(interp, this->canvas, " config -bg {", buf, "}", TCL_NULL));
    }
    */
}

void CanvasViewer::refresh()
{
    if (!object) {
        clear();
        return;
    }

    updateBackgroundColor();

    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    getCanvasRenderer()->refresh(&hints);
}

void CanvasViewer::setZoomFactor(double zoomFactor) {
    if (this->zoomFactor != zoomFactor) {
        this->zoomFactor = zoomFactor;
        redraw();
        viewport()->update();
    }
}

void CanvasViewer::clear()
{
    figureLayer->clear();
}

} // namespace qtenv
} // namespace omnetpp
