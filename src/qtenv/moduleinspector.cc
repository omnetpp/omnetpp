//==========================================================================
//  MODULEINSPECTOR.CC - part of
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

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QGridLayout>

#include "common/stringtokenizer.h"
#include "common/stringutil.h"
#include "layout/graphlayouter.h"
#include "layout/forcedirectedgraphlayouter.h"
#include "layout/basicspringembedderlayout.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cfutureeventset.h"
#include "moduleinspector.h"
#include "figurerenderers.h"
#include "qtenv.h"
#include "tklib.h"
#include "qtutil.h"
#include "inspectorfactory.h"
#include "arrow.h"
#include "layouterenv.h"
#include "canvasrenderer.h"
#include "graphicsscene.h"
#include "mainwindow.h"

using namespace OPP::common;
using namespace OPP::layout;

namespace omnetpp {
namespace qtenv {

#define UNKNOWNICON_WIDTH     32
#define UNKNOWNICON_HEIGHT    32

void _dummy_for_moduleinspector() {}

class ModuleInspectorFactory : public InspectorFactory
{
  public:
    ModuleInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cModule *>(obj) != nullptr; }
    int getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override
    {
        cModule *mod = dynamic_cast<cModule *>(object);
        return mod && mod->hasSubmodules() ? 3.0 : 0.9;
    }

    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new ModuleInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(ModuleInspectorFactory);

ModuleInspector::ModuleInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    needs_redraw = false;
    notDrawn = false;
    layoutSeed = 0;

    canvasRenderer = new CanvasRenderer();
    createView(this);
    canvasRenderer->setQtCanvas(scene, getCanvas());
}

ModuleInspector::~ModuleInspector()
{
    delete canvasRenderer;
}

void ModuleInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    canvasRenderer->setCanvas(getCanvas());

    scene->clear();
    submoduleGraphicsItems.clear();
    needs_redraw = true;

    if (object) {
        layoutSeed = 1;  // we'll read the "bgl" display string tag from Tcl
        // TODO
        //    ModuleInspector:recallPreferences $insp

        try {
            relayoutAndRedrawAll();
        }
        catch (std::exception& e) {
            QMessageBox::warning(this, QString("Error"), QString("Error displaying network graphics: ") + e.what());
        }
        //    ModuleInspector:updateZoomLabel $insp
        //    ModuleInspector:adjustWindowSizeAndZoom $insp
        // }
    }
}

void ModuleInspector::createView(QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    parent->setLayout(layout);
    QGraphicsView *view = new QGraphicsView();
    layout->addWidget(view);
    layout->setMargin(0);
    scene = new ModuleInspectorScene(this);
    view->setRenderHints(QPainter::Antialiasing);
    view->setScene(scene);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->insertStretch(0);
    view->setLayout(horizontalLayout);

    addToolBar(verticalLayout);
}

void ModuleInspector::addToolBar(QBoxLayout *layout)
{
    QToolBar *toolBar = new QToolBar();
    layout->addWidget(toolBar);
    layout->insertStretch(1);

    toolBar->addAction(QIcon(":/tools/icons/tools/back.png"), "Back",
                       this, SLOT(goBack()));
    toolBar->addAction(QIcon(":/tools/icons/tools/forward.png"), "Forward",
                       this, SLOT(goForward()));
    toolBar->addAction(QIcon(":/tools/icons/tools/parent.png"), "Go to parent module",
                       this, SLOT(inspectParent()));
    toolBar->addSeparator();
    toolBar->addAction(QIcon(":/tools/icons/tools/mrun.png"), "Run until next event in this module",
                       this, SLOT(runUntil()));
    QAction *action = toolBar->addAction(QIcon(":/tools/icons/tools/mfast.png"), "Fast run until next event in this module (Ctrl + F4)",
                       this, SLOT(fastRunUntil()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));

    action = toolBar->addAction(QIcon(":/tools/icons/tools/stop.png"), "Stop the simulation (F8)",
                       this, SLOT(stopSimulation()));
    action->setShortcut(QKeySequence(Qt::Key_F8));

    toolBar->addSeparator();
    action = toolBar->addAction(QIcon(":/tools/icons/tools/redraw.png"), "Re-layout (Ctrl + R)",
                                this, SLOT(relayout()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

    action = toolBar->addAction(QIcon(":/tools/icons/tools/zoomin.png"), "Zoom in (Ctrl + M)",
                                this, SLOT(zoomIn()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));

    action = toolBar->addAction(QIcon(":/tools/icons/tools/zoomout.png"), "Zoom out (Ctrl + N)",
                                this, SLOT(zoomOut()));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));

    toolBar->setAutoFillBackground(true);
}

void ModuleInspector::runUntil()
{
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    mainWindow->runSimulationLocal(this, qtenv::Qtenv::eRunMode::RUNMODE_NORMAL);
}

void ModuleInspector::fastRunUntil()
{
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    mainWindow->runSimulationLocal(this, qtenv::Qtenv::eRunMode::RUNMODE_FAST);
}

void ModuleInspector::stopSimulation()
{
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    mainWindow->on_actionStop_triggered();
}

//Relayout the compound module, and resize the window accordingly.

void ModuleInspector::relayout()
{
    //TODO
//    global config inspectordata

//    set c $insp.c
    ++layoutSeed;

    relayoutAndRedrawAll();

//    if {[opp_inspector_istoplevel $insp] && $config(layout-may-resize-window)} {
//        wm geometry $insp ""
//    }

//    ModuleInspector:adjustWindowSizeAndZoom $insp
}

void ModuleInspector::zoomIn()
{
    //TODO
//    proc ModuleInspector:zoomIn {insp {x ""} {y ""}} {
//        global config
//        ModuleInspector:zoomBy $insp $config(zoomby-factor) 1 $x $y
//    }
}

void ModuleInspector::zoomOut()
{
    //TODO
//    proc ModuleInspector:zoomOut {insp {x ""} {y ""}} {
//        global config
//        ModuleInspector:zoomBy $insp [expr 1.0 / $config(zoomby-factor)] 1 $x $y
//    }
}

void ModuleInspector::zoomBy()
{
    //TODO
//    proc ModuleInspector:zoomBy {insp mult {snaptoone 0}  {x ""} {y ""}} {
//        global inspectordata
//        set c $insp.c
//        if {($mult<1 && $inspectordata($c:zoomfactor)>0.001) || ($mult>1 && $inspectordata($c:zoomfactor)<1000)} {
//            # remember canvas scroll position, we'll need it to zoom in/out around ($x,$y)
//            if {$x == ""} {set x [expr [winfo width $c] / 2]}
//            if {$y == ""} {set y [expr [winfo height $c] / 2]}
//            set origCanvasX [$c canvasx $x]
//            set origCanvasY [$c canvasy $y]
//            set origZoom $inspectordata($c:zoomfactor)

//            # update zoom factor and redraw
//            set inspectordata($c:zoomfactor) [expr $inspectordata($c:zoomfactor) * $mult]

//            # snap to 1 (note: this is not desirable when zoom is set programmatically to fit network into window)
//            if {$snaptoone} {
//                set m [expr $mult < 1 ? 1.0/$mult : $mult]
//                set a [expr  1 - 0.9*(1 - 1.0/$m)]
//                set b [expr  1 + 0.9*($m - 1)]
//                if {$inspectordata($c:zoomfactor) > $a && $inspectordata($c:zoomfactor) < $b} {
//                    set inspectordata($c:zoomfactor) 1
//                }
//            }

//            # clear scrollregion so that it will be set up afresh
//            $c config -scrollregion ""

//            # redraw
//            opp_inspectorcommand $insp redraw
//            ModuleInspector:setScrollRegion $insp 0

//            ModuleInspector:updateZoomLabel $insp

//            # pan the canvas so that we zoom in/out around ($x, $y)
//            set actualMult [expr $inspectordata($c:zoomfactor) / $origZoom]
//            set canvasX [expr $origCanvasX * $actualMult]
//            set canvasY [expr $origCanvasY * $actualMult]
//            set dx [expr int($canvasX - $origCanvasX)]
//            set dy [expr int($canvasY - $origCanvasY)]
//            $c scan mark 0 0
//            $c scan dragto $dx $dy -1
//        }

//        ModuleInspector:updatePreferences $insp

//        ModuleInspector:popOutToolbarButtons $insp
//    }
}

cCanvas *ModuleInspector::getCanvas()
{
    cModule *mod = static_cast<cModule *>(object);
    cCanvas *canvas = mod ? mod->getCanvasIfExists() : nullptr;
    return canvas;
}

void ModuleInspector::refresh()
{
    return;  // FIXME
    Inspector::refresh();

    if (!object) {
        CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));
        return;
    }

    if (notDrawn)
        return;

    cCanvas *canvas = getCanvas();
    if (canvas != nullptr && !canvasRenderer->hasCanvas())  // canvas was recently created
        canvasRenderer->setCanvas(canvas);

    updateBackgroundColor();

    // redraw modules only if really needed
    if (needs_redraw) {
        needs_redraw = false;
        redraw();
    }
    else {
        refreshFigures();
        redrawNextEventMarker();
        redrawMessages();
        refreshSubmodules();
        adjustSubmodulesZOrder();
    }
}

void ModuleInspector::relayoutAndRedrawAll()
{
    ASSERT(object != nullptr);

    cModule *mod = (cModule *)object;
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
        CHK(Tcl_VarEval(interp, "tk_messageBox -parent ", windowName, " -type yesno -title Warning -icon question "
                                                                      "-message {Module '", object->getFullName(), "' ", problem,
                        ", it may take a long time to display the graphics. "
                        "Do you want to proceed with drawing?}", TCL_NULL));
        bool answer = (Tcl_GetStringResult(interp)[0] == 'y');
        if (answer == false) {
            notDrawn = true;
            CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));  // this must be done, still
            return;
        }
    }

    recalculateLayout();
    redrawFigures();
    redrawModules();
    redrawNextEventMarker();
    redrawMessages();
    refreshSubmodules();
    adjustSubmodulesZOrder();
}

void ModuleInspector::redraw()
{
    if (object == nullptr) {
        CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));
        return;
    }

    updateBackgroundColor();

    refreshLayout();
    redrawModules();
    redrawFigures();
    redrawNextEventMarker();
    redrawMessages();
    refreshSubmodules();
    adjustSubmodulesZOrder();
}

void ModuleInspector::adjustSubmodulesZOrder()
{
    cCanvas *canvas = getCanvas();
    if (canvas) {
        cFigure *submodulesLayer = canvas->getSubmodulesLayer();
        if (submodulesLayer) {
            char tag[32];
            sprintf(tag, "f%d", submodulesLayer->getId());
            CHK(Tcl_VarEval(interp, this->canvas, " lower submodext ", tag, TCL_NULL));
            CHK(Tcl_VarEval(interp, this->canvas, " raise submodext ", tag, TCL_NULL));
        }
    }
}

void ModuleInspector::clearObjectChangeFlags()
{
    cCanvas *canvas = getCanvas();
    if (canvas)
        canvas->getRootFigure()->clearChangeFlags();
}

void ModuleInspector::getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
        double& x, double& y, double& sx, double& sy)
{
    const cDisplayString blank;
    const cDisplayString& ds = submod->hasDisplayString() && submod->parametersFinalized() ? submod->getDisplayString() : blank;

    // get size -- we'll need to return that too, and may be needed for matrix, ring etc. layout
    double boxsx = 0, boxsy = 0;
    int iconsx = 0, iconsy = 0;
    if (ds.containsTag("b") || !ds.containsTag("i")) {
        boxsx = resolveDoubleDispStrArg(ds.getTagArg("b", 0), submod, 40);
        boxsy = resolveDoubleDispStrArg(ds.getTagArg("b", 1), submod, 24);
    }
    if (ds.containsTag("i")) {
        const char *imgName = ds.getTagArg("i", 0);
        const char *imgSize = ds.getTagArg("is", 0);
        if (!imgName || !*imgName) {
            iconsx = UNKNOWNICON_WIDTH;
            iconsy = UNKNOWNICON_HEIGHT;
        }
        else {
            CHK(Tcl_VarEval(interp, "lookupImage ", imgName, " ", imgSize, TCL_NULL));
            iconsx = iconsy = 30;  // TODO
/*Qt!
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
 */
        }
    }
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
        double rx = resolveDoubleDispStrArg(ds.getTagArg("p", 3), submod, (sx+sy)*submod->size()/4);
        double ry = resolveDoubleDispStrArg(ds.getTagArg("p", 4), submod, rx);

        x += rx - rx*sin(submod->getIndex()*2*PI/submod->size());
        y += ry - ry*cos(submod->getIndex()*2*PI/submod->size());
    }
    else {
        CHK(Tcl_VarEval(interp, "messagebox {Error} "
                                "{Error: invalid layout `", layout, "' in `p' tag "
                                                                    "of display string \"", ds.str(), "\"} error ok", TCL_NULL));
    }
}

void ModuleInspector::recalculateLayout()
{
    // refresh layout with empty submodPosMap -- everything layouted
    submodPosMap.clear();
    refreshLayout();
}

void ModuleInspector::refreshLayout()
{
    // recalculate layout, using coordinates in submodPosMap as "fixed" nodes --
    // only new nodes are re-layouted

    cModule *parentModule = static_cast<cModule *>(object);

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
    printf("Layout->setSize %d %d \n", sx, sy);
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
            printf("explicitCoords\n");
        }
        else if (submodPosMap.find(submod) != submodPosMap.end()) {
            // reuse coordinates from previous layout
            Point pos = submodPosMap[submod];
            layouter->addFixedNode(submod->getId(), pos.x, pos.y, sx, sy);
            printf("reuse coords\n");
        }
        else if (obeysLayout) {
            // all modules are anchored to the anchor point with the vector's name
            // e.g. "p=,,ring"
            layouter->addAnchoredNode(submod->getId(), submod->getName(), x, y, sx, sy);
            printf("addAnchoredNode\n");
        }
        else {
            layouter->addMovableNode(submod->getId(), sx, sy);
            printf("addMovableNode %g %g \n", sx, sy);
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
    // TODO TkenvGraphLayouterEnvironment
    BasicGraphLayouterEnvironment environment;

    // TODO
//    std::string stopButton = std::string(getWindowName()) + ".toolbar.stop";
//    bool isExpressMode = getTkenv()->getSimulationRunMode() == Qtenv::RUNMODE_EXPRESS;
//    if (!isExpressMode)
//        environment.setWidgetToGrab(stopButton.c_str());

//    // enable visualizing only if full re-layouting (no cached coordinates in submodPosMap)
//    // if (getTkenv()->opt->showlayouting)  // for debugging
//    if (submodPosMap.empty() && getTkenv()->opt->showLayouting)
//        environment.setCanvas(canvas);

    layouter->setEnvironment(&environment);
    layouter->execute();
    // environment.cleanup();

    // fill the map with the results
    submodPosMap.clear();
    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it) {
        cModule *submod = *it;

        Point pos;
        layouter->getNodePosition(submod->getId(), pos.x, pos.y);
        printf("Position: %g %g\n", pos.x, pos.y);
        submodPosMap[submod] = pos;
    }

    layoutSeed = layouter->getSeed();

    delete layouter;
}

// requires either recalculateLayout() or refreshLayout() called before!
void ModuleInspector::redrawModules()
{
    cModule *parentModule = static_cast<cModule *>(object);

    // then display all submodules
    CHK(Tcl_VarEval(interp, canvas, " delete dx", TCL_NULL));  // NOT "delete all" because that'd remove "bubbles" too!

    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it) {
        cModule *submod = *it;
        assert(submodPosMap.find(submod) != submodPosMap.end());
        Point& pos = submodPosMap[submod];
        drawSubmodule(submod, pos.x, pos.y);
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
    CHK(Tcl_VarEval(interp, canvas, " raise bubble", TCL_NULL));
    CHK(Tcl_VarEval(interp, "ModuleInspector:setScrollRegion ", windowName, " 0", TCL_NULL));
}

void ModuleInspector::drawSubmodule(cModule *submod, double x, double y)
{
    const char *iconName = submod->getDisplayString().getTagArg("i", 0);
    if (!iconName || !iconName[0])
        iconName = "block/process";

    // TODO context menu + doubleclick
    QImage *img = getQtenv()->icons.getImage(iconName);
    QPixmap icon = QPixmap::fromImage(*img);
    submoduleGraphicsItems[submod->getId()] = scene->addPixmap(icon);
    submoduleGraphicsItems[submod->getId()]->setPos(x, y);
    // TODO key
    submoduleGraphicsItems[submod->getId()]->setData(1, QVariant::fromValue(static_cast<cObject *>(submod)));

    char coords[64];
    const char *dispstr = submod->hasDisplayString() && submod->parametersFinalized() ? submod->getDisplayString().str() : "";

    CHK(Tcl_VarEval(interp, "ModuleInspector:drawSubmodule ",
                    canvas, " ",
                    ptrToStr(submod), " ",
                    coords,
                    "{", submod->getFullName(), "} ",
                    TclQuotedString(dispstr).get(), " ",
                    (submod->isPlaceholder() ? "1" : "0"),
                    TCL_NULL));
}

void ModuleInspector::drawEnclosingModule(cModule *parentModule)
{
    const char *displayString = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString().str() : "";
    CHK(Tcl_VarEval(interp, "ModuleInspector:drawEnclosingModule ",
                    canvas, " ",
                    ptrToStr(parentModule), " ",
                    "{", parentModule->getFullPath().c_str(), "} ",
                    TclQuotedString(displayString).get(),
                    TCL_NULL));
}

QPointF ModuleInspector::getSubmodCoords(cModule *mod)
{
    if (submoduleGraphicsItems[mod->getId()] == nullptr)
        return QPointF(0, 0);
    QPixmap icon = submoduleGraphicsItems[mod->getId()]->pixmap();
    QPointF pos = submoduleGraphicsItems[mod->getId()]->pos();
    pos.setX(pos.x() + icon.width() / 2);
    pos.setY(pos.y() + icon.height() / 2);
    return pos;
}

void ModuleInspector::drawConnection(cGate *gate)
{
    cModule *mod = gate->getOwnerModule();
    cGate *destGate = gate->getNextGate();

    // char gateptr[32], srcptr[32], destptr[32], chanptr[32], indices[32];

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

    QPointF src_rect = getSubmodCoords(mod);
    QPointF dest_rect = getSubmodCoords(destGate->getOwnerModule());

    scene->addLine(src_rect.x(), src_rect.y(), dest_rect.x(), dest_rect.y());

//    ptrToStr(gate, gateptr);
//    ptrToStr(mod, srcptr);
//    ptrToStr(destGate->getOwnerModule(), destptr);
//    sprintf(indices, "%d %d %d %d",
//            gate->getIndex(), gate->size(),
//            destGate->getIndex(), destGate->size());
//    cChannel *chan = gate->getChannel();
//    ptrToStr(chan, chanptr);
//    const char *dispstr = (chan && chan->hasDisplayString() && chan->parametersFinalized()) ? chan->getDisplayString().str() : "";

//    CHK(Tcl_VarEval(interp, "ModuleInspector:drawConnection ",
//            canvas, " ",
//            gateptr, " ",
//            TclQuotedString(dispstr).get(), " ",
//            srcptr, " ",
//            destptr, " ",
//            chanptr, " ",
//            indices, " ",
//            twoWayConnection ? "1" : "0",
//            nullptr
//             ));

//    proc ModuleInspector:drawConnection {c gateptr dispstr srcptr destptr chanptr src_i src_n dest_i dest_n two_way} {
//        global inspectordata tkpFont canvasTextOptions

//        # puts "DEBUG: ModuleInspector:drawConnection $c $gateptr $dispstr $srcptr $destptr $src_i $src_n $dest_i $dest_n $two_way"

//        if [catch {
//           set src_rect [ModuleInspector:getSubmodCoords $c $srcptr]
//           set dest_rect [ModuleInspector:getSubmodCoords $c $destptr]
//        } errmsg] {
//           # skip this connection if source or destination of the arrow cannot be found
//           return
//        }

//        if [catch {

//           opp_displaystring $dispstr parse tags $chanptr 1

//           if {![info exists tags(m)]} {set tags(m) {a}}

//           set mode [lindex $tags(m) 0]
//           if {$mode==""} {set mode "a"}
//           set src_anch  [list [lindex $tags(m) 1] [lindex $tags(m) 2]]
//           set dest_anch [list [lindex $tags(m) 3] [lindex $tags(m) 4]]

//           # puts "DEBUG: src_rect=($src_rect) dest_rect=($dest_rect)"
//           # puts "DEBUG: src_anch=($src_anch) dest_anch=($dest_anch)"

//           regexp -- {^.[^.]*} $c win

//           # switch off the connection arrangement if the option is not enabled
//           # all connection are treated as the first one in an array with size 1
//           if {![opp_getsimoption arrangevectorconnections]} {
//               set src_n "1"
//               set dest_n "1"
//               set src_i "0"
//               set dest_i "0"
//           }

//           set arrow_coords [eval [concat opp_inspectorcommand $win arrowcoords \
//                      $src_rect $dest_rect $src_i $src_n $dest_i $dest_n \
//                      $mode $src_anch $dest_anch]]

//           # puts "DEBUG: arrow=($arrow_coords)"

//           if {![info exists tags(ls)]} {set tags(ls) {}}
//           set fill [lindex $tags(ls) 0]
//           if {$fill == ""} {set fill black}
//           if {$fill == "-"} {set fill ""}
//           set width [lindex $tags(ls) 1]
//           if {$width == ""} {set width 1}
//           if {$width == "0"} {set fill ""}
//           set style [lindex $tags(ls) 2]
//           switch -glob $style {
//               "da*"   {set pattern "2 2"}
//               "d*"    {set pattern "1 1"}
//               default {set pattern ""}
//           }

//           set state "normal"
//           if {$inspectordata($c:showarrowheads) && !$two_way} {
//               set arrow {-endarrow 1}
//           } else {
//               set arrow {}
//           }

//           $c create pline $arrow_coords {*}$arrow -stroke $fill -strokedasharray $pattern -strokewidth $width -tags "dx tooltip conn submodext $gateptr"

//           # if we have a two way connection we should draw only in one direction
//           # the other line will be hidden (lowered under anything else)
//           if {[string compare $srcptr $destptr] >=0 && $two_way} {
//               $c lower $gateptr "dx"
//           }

//           if {[info exists tags(t)]} {
//               set txt [lindex $tags(t) 0]
//               set pos [lindex $tags(t) 1]
//               if {$pos == ""} {set pos "t"}
//               set color [lindex $tags(t) 2]
//               if {$color == ""} {set color "#005030"}
//               if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}
//               set x1 [lindex $arrow_coords 0]
//               set y1 [lindex $arrow_coords 1]
//               set x2 [lindex $arrow_coords 2]
//               set y2 [lindex $arrow_coords 3]
//               set anch "c"
//               set just "center"
//               if {$pos=="l"} {
//                   # "beginning"
//                   set x [expr (3*$x1+$x2)/4]
//                   set y [expr (3*$y1+$y2)/4]
//               } elseif {$pos=="r"} {
//                   # "end"
//                   set x [expr ($x1+3*$x2)/4]
//                   set y [expr ($y1+3*$y2)/4]
//               } elseif {$pos=="t"} {
//                   # "middle"
//                   set x [expr ($x1+$x2)/2]
//                   set y [expr ($y1+$y2)/2]
//                   if {($x1==$x2)?($y1<$y2):($x1<$x2)} {set anch "n"} else {set anch "s"}
//               } else {
//                   error "wrong position \"$pos\" in connection t= tag, should be `l', `r' or `t' (for beginning, end, or middle, respectively)"
//               }
//               $c create ptext $x $y -text $txt -fill $color -textanchor $anch {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx submodext"
//           }

//        } errmsg] {
//           tk_messageBox -type ok -title "Error" -icon error -parent [winfo toplevel [focusOrRoot]] \
//                         -message "Error in display string of a connection: $errmsg"
//        }
//    }
}

void ModuleInspector::redrawMessages()
{
    // refresh & cleanup from prev. events
    CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", TCL_NULL));

    // this thingy is only needed if animation is going on
    if (!getQtenv()->animating)
        return;

    // loop through all messages in the event queue and display them
    cFutureEventSet *fes = getSimulation()->getFES();
    int fesLen = fes->getLength();
    for (int i = 0; i < fesLen; i++) {
        cEvent *event = fes->get(i);
        if (!event->isMessage())
            continue;
        cMessage *msg = (cMessage *)event;

        cModule *arrivalMod = msg->getArrivalModule();
        if (arrivalMod &&
            arrivalMod->getParentModule() == static_cast<cModule *>(object) &&
            msg->getArrivalGateId() >= 0)
        {
            char msgptr[32];
            ptrToStr(msg, msgptr);
            cGate *arrivalGate = msg->getArrivalGate();

            // if arrivalGate is connected, msg arrived on a connection, otherwise via sendDirect()
            if (arrivalGate->getPreviousGate()) {
                cGate *gate = arrivalGate->getPreviousGate();
                CHK(Tcl_VarEval(interp, "ModuleInspector:drawMessageOnGate ",
                                canvas, " ",
                                ptrToStr(gate), " ",
                                msgptr,
                                TCL_NULL));
            }
            else {
                CHK(Tcl_VarEval(interp, "ModuleInspector:drawMessageOnModule ",
                                canvas, " ",
                                ptrToStr(arrivalMod), " ",
                                msgptr,
                                TCL_NULL));
            }
        }
    }
    CHK(Tcl_VarEval(interp, canvas, " raise bubble", TCL_NULL));
}

void ModuleInspector::redrawNextEventMarker()
{
    cModule *mod = static_cast<cModule *>(object);

    // removing marker from previous event
    CHK(Tcl_VarEval(interp, canvas, " delete nexteventmarker", TCL_NULL));

    // this thingy is only needed if animation is going on
    if (!getQtenv()->animating || !getQtenv()->opt->showNextEventMarkers)
        return;

    // if any parent of the module containing the next event is on this canvas, draw marker
    cModule *nextMod = getSimulation()->guessNextModule();
    cModule *nextModParent = nextMod;
    while (nextModParent && nextModParent->getParentModule() != mod)
        nextModParent = nextModParent->getParentModule();
    if (nextModParent) {
        CHK(Tcl_VarEval(interp, "ModuleInspector:drawNextEventMarker ",
                        canvas, " ",
                        ptrToStr(nextModParent), " ",
                        (nextMod == nextModParent ? "2" : "1"),
                        TCL_NULL));
    }
}

void ModuleInspector::redrawFigures()
{
    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->redraw(&hints);
}

void ModuleInspector::refreshFigures()
{
    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->refresh(&hints);
}

void ModuleInspector::fillFigureRenderingHints(FigureRenderingHints *hints)
{
    const char *s;

    // read $inspectordata($c:zoomfactor)
    s = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":zoomfactor").c_str()), TCL_GLOBAL_ONLY);
    hints->zoom = opp_atof(s);

    // read $inspectordata($c:imagesizefactor)
    s = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":imagesizefactor").c_str()), TCL_GLOBAL_ONLY);
    hints->iconMagnification = opp_atof(s);

    // read $inspectordata($c:showlabels)
    s = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":showlabels").c_str()), TCL_GLOBAL_ONLY);
    hints->showSubmoduleLabels = opp_atol(s) != 0;

    // read $inspectordata($c:showarrowheads)
    s = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":showarrowheads").c_str()), TCL_GLOBAL_ONLY);
    hints->showArrowHeads = opp_atol(s) != 0;

    Tcl_Eval(interp, "font actual CanvasFont -family");
    hints->defaultFont = Tcl_GetStringResult(interp);

    Tcl_Eval(interp, "font actual CanvasFont -size");
    s = Tcl_GetStringResult(interp);
    hints->defaultFontSize = opp_atol(s) * 16 / 10;  // FIXME figure out conversion factor (point to pixel?)...
}

void ModuleInspector::refreshSubmodules()
{
    for (cModule::SubmoduleIterator it(static_cast<cModule *>(object)); !it.end(); ++it) {
        CHK(Tcl_VarEval(interp, "ModuleInspector:refreshSubmodule ",
                        windowName, " ",
                        ptrToStr(*it),
                        TCL_NULL));
    }
}

void ModuleInspector::submoduleCreated(cModule *newmodule)
{
    needs_redraw = true;
}

void ModuleInspector::submoduleDeleted(cModule *module)
{
    needs_redraw = true;
}

void ModuleInspector::connectionCreated(cGate *srcgate)
{
    needs_redraw = true;
}

void ModuleInspector::connectionDeleted(cGate *srcgate)
{
    needs_redraw = true;
}

void ModuleInspector::displayStringChanged(cModule *)
{
    needs_redraw = true;
}

void ModuleInspector::displayStringChanged()
{
    needs_redraw = true;  // TODO check, probably only non-background tags have changed...
}

void ModuleInspector::displayStringChanged(cGate *)
{
    needs_redraw = true;
}

void ModuleInspector::bubble(cComponent *subcomponent, const char *text)
{
    if (!subcomponent->isModule())
        return;  // channel bubbles not yet supported

    // if submod position is not yet known (because e.g. we're in fast mode
    // and it was dynamically created since the last update), refresh layout
    // so that we can get coordinates for it
    cModule *submod = (cModule *)subcomponent;
    if (submodPosMap.find(submod) == submodPosMap.end())
        refreshLayout();

    // invoke Tcl code to display bubble
    char coords[64];
    Point& pos = submodPosMap[submod];
    sprintf(coords, " %g %g ", pos.x, pos.y);
    CHK(Tcl_VarEval(interp, "ModuleInspector:bubble ", canvas, coords, " ", TclQuotedString(text).get(), TCL_NULL));
}

const char *ModuleInspector::animModeToStr(SendAnimMode mode)
{
    return mode == ANIM_BEGIN ? "beg" : mode == ANIM_THROUGH ? "thru" : mode == ANIM_END ? "end" : "???";
}

void ModuleInspector::animateMethodcallAscent(cModule *srcSubmod, const char *methodText)
{
    char parentPtr[30], modPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(srcSubmod, modPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallAscent ", getWindowName(), " ", parentPtr, " ", modPtr, " ", TclQuotedString(methodText).get(), TCL_NULL));
}

void ModuleInspector::animateMethodcallDescent(cModule *destSubmod, const char *methodText)
{
    char parentPtr[30], modPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(destSubmod, modPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallDescent ", getWindowName(), " ", parentPtr, " ", modPtr, " ", TclQuotedString(methodText).get(), TCL_NULL));
}

void ModuleInspector::animateMethodcallHoriz(cModule *srcSubmod, cModule *destSubmod, const char *methodText)
{
    char srcPtr[30], destPtr[30];
    ptrToStr(srcSubmod, srcPtr);
    ptrToStr(destSubmod, destPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallHoriz ", getWindowName(), " ", srcPtr, " ", destPtr, " ", TclQuotedString(methodText).get(), TCL_NULL));
}

void ModuleInspector::animateMethodcallDelay(Tcl_Interp *interp)
{
    CHK(Tcl_Eval(interp, "ModuleInspector:animateMethodcallWait"));
}

void ModuleInspector::animateMethodcallCleanup()
{
    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallCleanup ", getWindowName(), TCL_NULL));
}

void ModuleInspector::animateSendOnConn(cGate *srcGate, cMessage *msg, SendAnimMode mode)
{
    char gatePtr[30], msgPtr[30];
    ptrToStr(srcGate, gatePtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateOnConn ", getWindowName(), " ", msgPtr, " ", gatePtr, " ", modeStr, TCL_NULL));
}

void ModuleInspector::animateSenddirectAscent(cModule *srcSubmod, cMessage *msg)
{
    char parentPtr[30], modPtr[30], msgPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(srcSubmod, modPtr);
    ptrToStr(msg, msgPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectAscent ", getWindowName(), " ", msgPtr, " ", parentPtr, " ", modPtr, " ", "thru", TCL_NULL));
}

void ModuleInspector::animateSenddirectDescent(cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    char parentPtr[30], modPtr[30], msgPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(destSubmod, modPtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectDescent ", getWindowName(), " ", msgPtr, " ", parentPtr, " ", modPtr, " ", modeStr, TCL_NULL));
}

void ModuleInspector::animateSenddirectHoriz(cModule *srcSubmod, cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    char srcPtr[30], destPtr[30], msgPtr[30];
    ptrToStr(srcSubmod, srcPtr);
    ptrToStr(destSubmod, destPtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectHoriz ", getWindowName(), " ", msgPtr, " ", srcPtr, " ", destPtr, " ", modeStr, TCL_NULL));
}

void ModuleInspector::animateSenddirectCleanup()
{
    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectCleanup ", getWindowName(), TCL_NULL));
}

void ModuleInspector::animateSenddirectDelivery(cModule *destSubmod, cMessage *msg)
{
    char destPtr[30], msgPtr[30];
    ptrToStr(destSubmod, destPtr);
    ptrToStr(msg, msgPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectDelivery ", getWindowName(), " ", msgPtr, " ", destPtr, TCL_NULL));
}

void ModuleInspector::performAnimations(Tcl_Interp *interp)
{
    CHK(Tcl_VarEval(interp, "performAnimations", TCL_NULL));
}

void ModuleInspector::updateBackgroundColor()
{
    cCanvas *canvas = getCanvas();
    if (canvas) {
        char buf[16];
        cFigure::Color color = canvas->getBackgroundColor();
        sprintf(buf, "#%2.2x%2.2x%2.2x", color.red, color.green, color.blue);
        CHK(Tcl_VarEval(interp, this->canvas, " config -bg {", buf, "}", TCL_NULL));
    }
}

int ModuleInspector::inspectorCommand(int argc, const char **argv)
{
    if (argc < 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    E_TRY
    if (strcmp(argv[0], "arrowcoords") == 0) {
        return arrowcoords(interp, argc, argv);
    }
    else if (strcmp(argv[0], "relayout") == 0) {
        TRY(relayoutAndRedrawAll());
        return TCL_OK;
    }
    else if (strcmp(argv[0], "redraw") == 0) {
        TRY(redraw());
        return TCL_OK;
    }
//   else if (strcmp(argv[0], "getdefaultlayoutseed") == 0) {
//       return getDefaultLayoutSeed(argc, argv);
//   }
    else if (strcmp(argv[0], "getlayoutseed") == 0) {
        return getLayoutSeed(argc, argv);
    }
    else if (strcmp(argv[0], "setlayoutseed") == 0) {
        return setLayoutSeed(argc, argv);
    }
    else if (strcmp(argv[0], "submodulecount") == 0) {
        return getSubmoduleCount(argc, argv);
    }
    else if (strcmp(argv[0], "getsubmodq") == 0) {
        return getSubmodQ(argc, argv);
    }
    else if (strcmp(argv[0], "getsubmodqlen") == 0) {
        return getSubmodQLen(argc, argv);
    }
    else if (strcmp(argv[0], "hascanvas") == 0) {  // needed because getalltags/getenabledtags/etc throw error if there's no canvas
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->hasCanvas() ? "1" : "0"), TCL_STATIC);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getalltags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getAllTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getenabledtags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getEnabledTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "getexcepttags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getExceptTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "setenabledtags") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
            return TCL_ERROR;
        }
        canvasRenderer->setEnabledTags(argv[1]);
        return TCL_OK;
    }
    else if (strcmp(argv[0], "setexcepttags") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
            return TCL_ERROR;
        }
        canvasRenderer->setExceptTags(argv[1]);
        return TCL_OK;
    }
    E_CATCH

    return Inspector::inspectorCommand(argc, argv);
}

int ModuleInspector::getDefaultLayoutSeed()
{
    const cDisplayString blank;
    cModule *parentModule = static_cast<cModule *>(object);
    const cDisplayString& ds = parentModule && parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString() : blank;
    long seed = resolveLongDispStrArg(ds.getTagArg("bgl",4), parentModule, 1);
    return seed;
}

int ModuleInspector::getLayoutSeed(int argc, const char **argv)
{
    if (argc != 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }
    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)layoutSeed));
    return TCL_OK;
}

int ModuleInspector::setLayoutSeed(int argc, const char **argv)
{
    if (argc != 2) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }
    layoutSeed = atol(argv[1]);
    return TCL_OK;
}

int ModuleInspector::getSubmoduleCount(int argc, const char **argv)
{
    if (argc != 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }
    int count = 0;
    for (cModule::SubmoduleIterator it(static_cast<cModule *>(object)); !it.end(); ++it)
        count++;
    Tcl_SetObjResult(interp, Tcl_NewIntObj(count));
    return TCL_OK;
}

int ModuleInspector::getSubmodQ(int argc, const char **argv)
{
    // args: <module ptr> <qname>
    if (argc != 3) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    cModule *mod = dynamic_cast<cModule *>(strToPtr(argv[1]));
    const char *qname = argv[2];
    cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname));
    char buf[21];
    ptrToStr(q, buf);
    Tcl_SetResult(interp, buf, TCL_VOLATILE);
    return TCL_OK;
}

int ModuleInspector::getSubmodQLen(int argc, const char **argv)
{
    // args: <module ptr> <qname>
    if (argc != 3) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    cModule *mod = dynamic_cast<cModule *>(strToPtr(argv[1]));
    const char *qname = argv[2];
    cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname));  // FIXME THIS MUST BE REFINED! SEARCHES WAY TOO DEEEEEP!!!!
    if (!q) {
        Tcl_SetResult(interp, TCLCONST(""), TCL_STATIC);
        return TCL_OK;
    }
    Tcl_SetObjResult(interp, Tcl_NewIntObj(q->getLength()));
    return TCL_OK;
}

} // namespace qtenv
} // namespace omnetpp

