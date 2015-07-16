//==========================================================================
//  MODULEGRAPHICSVIEW.CC - part of
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

#include "modulegraphicsview.h"
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
#include "inspector.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <canvasrenderer.h>
#include "animator.h"
#include <QDebug>

#define emit

#define UNKNOWNICON_WIDTH     32
#define UNKNOWNICON_HEIGHT    32

using namespace OPP::layout;

namespace omnetpp {
namespace qtenv {

ModuleGraphicsView::ModuleGraphicsView(CanvasRenderer *canvasRenderer) :
    object(nullptr),
    layoutSeed(0),
    notDrawn(false),
    needs_redraw(false),
    canvasRenderer(canvasRenderer)
{
}

void ModuleGraphicsView::setObject(cModule *obj)
{
    object = obj;
}

void ModuleGraphicsView::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
        emit doubleClick(event);
}

void ModuleGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
        emit click(event);
}

void ModuleGraphicsView::contextMenuEvent(QContextMenuEvent * event)
{
    emit contextMenuRequested(event);
}

void ModuleGraphicsView::relayoutAndRedrawAll()
{
    ASSERT(object != nullptr);

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
        QMessageBox::StandardButton answer = QMessageBox::warning(this, windowName, message, QMessageBox::Yes | QMessageBox::No);
        if(answer == QMessageBox::Yes)
        {
            notDrawn = true;
            clear();  // this must be done, still
            return;
        }
    }

    layer->clear();
    recalculateLayout();
    redrawFigures();
    redrawModules();
    getQtenv()->getAnimator()->redrawMessages();
    redrawNextEventMarker();
    refreshSubmodules();
    adjustSubmodulesZOrder();
}

void ModuleGraphicsView::recalculateLayout()
{
    // refresh layout with empty submodPosMap -- everything layouted
    submodPosMap.clear();
    refreshLayout();
}

void ModuleGraphicsView::refreshLayout()
{
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

        QPointF pos;
        double x, y;
        layouter->getNodePosition(submod->getId(), x, y);
        pos.setX(x);
        pos.setY(y);
        submodPosMap[submod] = pos;
    }

    layoutSeed = layouter->getSeed();

    delete layouter;
}

void ModuleGraphicsView::getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
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
            //CHK(Tcl_VarEval(interp, "lookupImage ", imgName, " ", imgSize, TCL_NULL));
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
        //CHK(Tcl_VarEval(interp, "messagebox {Error} "
        //                        "{Error: invalid layout `", layout, "' in `p' tag "
        //                                                            "of display string \"", ds.str(), "\"} error ok", TCL_NULL));
    }
}

void ModuleGraphicsView::redrawFigures()
{
    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->redraw(&hints);
}

void ModuleGraphicsView::refreshFigures()
{
    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->refresh(&hints);
}

void ModuleGraphicsView::fillFigureRenderingHints(FigureRenderingHints *hints)
{
    QString prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":zoomfactor";
    QVariant variant = getQtenv()->getPref(prefName);
    hints->zoom = variant.isValid() ? variant.value<double>() : 0;

    prefName = object->getFullName() + QString(":") + INSP_DEFAULT + ":imagesizefactor";
    variant = getQtenv()->getPref(prefName);
    hints->iconMagnification = variant.isValid() ? variant.value<double>() : 0;

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

// requires either recalculateLayout() or refreshLayout() called before!
void ModuleGraphicsView::redrawModules()
{
    cModule *parentModule = object;

    //TODO then display all submodules
    //CHK(Tcl_VarEval(interp, canvas, " delete dx", TCL_NULL));  // NOT "delete all" because that'd remove "bubbles" too!

    for (cModule::SubmoduleIterator it(parentModule); !it.end(); ++it) {
        cModule *submod = *it;
        assert(submodPosMap.find(submod) != submodPosMap.end());
        QPointF& pos = submodPosMap[submod];
        drawSubmodule(submod, pos.x(), pos.y());
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

void ModuleGraphicsView::drawSubmodule(cModule *submod, double x, double y)
{
    const char *iconName = submod->getDisplayString().getTagArg("i", 0);
    if (!iconName || !iconName[0])
        iconName = "block/process";

    QImage *img = getQtenv()->icons.getImage(iconName);
    if (!img) {
        img = getQtenv()->icons.getImage("block/process");
    }
    QPixmap icon = QPixmap::fromImage(*img);
    submoduleGraphicsItems[submod->getId()] = scene()->addPixmap(icon);
    submoduleGraphicsItems[submod->getId()]->setPos(x - icon.size().width() / 2, y - icon.size().height() / 2);
    submoduleGraphicsItems[submod->getId()]->setData(1, QVariant::fromValue(static_cast<cObject *>(submod)));
    submoduleGraphicsItems[submod->getId()]->setParentItem(layer);

    char coords[64];
    const char *dispstr = submod->hasDisplayString() && submod->parametersFinalized() ? submod->getDisplayString().str() : "";

//    CHK(Tcl_VarEval(interp, "ModuleInspector:drawSubmodule ",
//                    canvas, " ",
//                    ptrToStr(submod), " ",
//                    coords,
//                    "{", submod->getFullName(), "} ",
//                    TclQuotedString(dispstr).get(), " ",
//                    (submod->isPlaceholder() ? "1" : "0"),
//                    TCL_NULL));

//    proc ModuleInspector:drawSubmodule {c submodptr x y name dispstr isplaceholder} {
//       #puts "DEBUG: ModuleInspector:drawSubmodule $c $submodptr $x $y $name $dispstr $isplaceholder"
//       global icons inspectordata tkpFont canvasTextOptions

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

void ModuleGraphicsView::drawEnclosingModule(cModule *parentModule)
{
    const char *displayString = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString().str() : "";
//    CHK(Tcl_VarEval(interp, "ModuleInspector:drawEnclosingModule ",
//                    canvas, " ",
//                    ptrToStr(parentModule), " ",
//                    "{", parentModule->getFullPath().c_str(), "} ",
//                    TclQuotedString(displayString).get(),
//                    TCL_NULL));
}

void ModuleGraphicsView::drawConnection(cGate *gate)
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

    QPointF src = getSubmodCoords(mod);
    QPointF dest = getSubmodCoords(destGate->getOwnerModule());

    auto item = new QGraphicsLineItem();
    item->setLine(src.x(), src.y(), dest.x(), dest.y());
    item->setZValue(-1);
    item->setParentItem(layer);



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

QPointF ModuleGraphicsView::getSubmodCoords(cModule *mod)
{
    return QPointF(submodPosMap[mod]);
}

QPointF ModuleGraphicsView::getMessageEndPos(const QPointF &src, const QPointF &dest)
{
    auto delta = dest - src;
    auto len = sqrt(delta.x() * delta.x() + delta.y() * delta.y());
    if (len == 0) len = 1;
    delta /= len;
    len -= 20;
    if (len < 1) len = 1;
    delta *= len;
    return src + delta;
}

cObject *ModuleGraphicsView::getObjectAt(qreal x, qreal y)
{
    QGraphicsItem *item = scene()->itemAt(mapToScene(x, y), QTransform());
    if(item == nullptr)
        return nullptr;

    QVariant variant = item->data(1);
    cObject *object = nullptr;
    if(variant.isValid())
         object = variant.value<cObject*>();

    return object;
}

QList<cObject*> ModuleGraphicsView::getObjectsAt(qreal x, qreal y)
{
    QList<QGraphicsItem*> items = scene()->items(mapToScene(x, y));
    QList<cObject*> objects;
    if(items.size() == 0)
        return objects;

    for(auto item : items)
    {
        QVariant variant = item->data(1);
        if(!variant.isValid())
        {
            //TODO
            qDebug() << "getObjectsAt: item has invalid data.";
            continue;
        }

        objects.push_back(variant.value<cObject*>());

    }

    return objects;
}

void ModuleGraphicsView::setLayer(GraphicsLayer *layer)
{
    clear();
    this->layer = layer;
    redraw();
}

void ModuleGraphicsView::clear()
{
    if (layer) layer->clear();
    submoduleGraphicsItems.clear();
}

void ModuleGraphicsView::redrawNextEventMarker()
{
    cModule *mod = object;

    // removing marker from previous event
    //CHK(Tcl_VarEval(interp, canvas, " delete nexteventmarker", TCL_NULL));

    // this thingy is only needed if animation is going on
    if (!getQtenv()->animating || !getQtenv()->opt->showNextEventMarkers)
        return;

    // if any parent of the module containing the next event is on this canvas, draw marker
    cModule *nextMod = getSimulation()->guessNextModule();
    cModule *nextModParent = nextMod;
    while (nextModParent && nextModParent->getParentModule() != mod)
        nextModParent = nextModParent->getParentModule();
    if (nextModParent) {
//        CHK(Tcl_VarEval(interp, "ModuleInspector:drawNextEventMarker ",
//                        canvas, " ",
//                        ptrToStr(nextModParent), " ",
//                        (nextMod == nextModParent ? "2" : "1"),
//                        TCL_NULL));
    }
}

void ModuleGraphicsView::refreshSubmodules()
{
    for (cModule::SubmoduleIterator it(object); !it.end(); ++it) {
//        CHK(Tcl_VarEval(interp, "ModuleInspector:refreshSubmodule ",
//                        windowName, " ",
//                        ptrToStr(*it),
//                        TCL_NULL));
    }
}

void ModuleGraphicsView::adjustSubmodulesZOrder()
{
//    cCanvas *canvas = getCanvas();
//    if (canvas) {
//        cFigure *submodulesLayer = canvas->getSubmodulesLayer();
//        if (submodulesLayer) {
//            char tag[32];
//            sprintf(tag, "f%d", submodulesLayer->getId());
//            //CHK(Tcl_VarEval(interp, this->canvas, " lower submodext ", tag, TCL_NULL));
//            //CHK(Tcl_VarEval(interp, this->canvas, " raise submodext ", tag, TCL_NULL));
//        }
//    }
}

void ModuleGraphicsView::redraw()
{
    clear();
    if (object == nullptr) {
        return;
    }

    updateBackgroundColor();

    refreshLayout();
    redrawModules();
    redrawFigures();
    redrawNextEventMarker();
    refreshSubmodules();
    adjustSubmodulesZOrder();
}

void ModuleGraphicsView::updateBackgroundColor()
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

void ModuleGraphicsView::refresh()
{
    if (!object) {
        clear();
        return;
    }

    if (notDrawn)
        return;

    //TODO
//    cCanvas *canvas = getCanvas();
//    if (canvas != nullptr && !canvasRenderer->hasCanvas())  // canvas was recently created
//        canvasRenderer->setCanvas(canvas);

    updateBackgroundColor();

    // redraw modules only if really needed
    if (needs_redraw) {
        needs_redraw = false;
        redraw();
    }
    else {
        refreshFigures();
        redrawNextEventMarker();
        refreshSubmodules();
        adjustSubmodulesZOrder();
    }
}

void ModuleGraphicsView::bubble(cComponent *subcomponent, const char *text)
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
    QPointF& pos = submodPosMap[submod];
    sprintf(coords, " %g %g ", pos.x(), pos.y());
    //CHK(Tcl_VarEval(interp, "ModuleInspector:bubble ", canvas, coords, " ", TclQuotedString(text).get(), TCL_NULL));
}

} // namespace qtenv
} // namespace omnetpp
