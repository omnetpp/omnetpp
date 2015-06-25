#include "modulegraphicsview.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cdisplaystring.h"
#include "layout/graphlayouter.h"
#include "layout/basicspringembedderlayout.h"
#include "layout/forcedirectedgraphlayouter.h"
#include "qtenv.h"
#include "qtenv/moduleinspector.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include <QDebug>

#define UNKNOWNICON_WIDTH     32
#define UNKNOWNICON_HEIGHT    32

using namespace OPP::layout;

namespace omnetpp {
namespace qtenv {

ModuleGraphicsView::ModuleGraphicsView() :
    object(nullptr),
    layoutSeed(1),
    notDrawn(false)
{
}

void ModuleGraphicsView::setObject(cModule *obj)
{
    object = obj;
}

void ModuleGraphicsView::setEventListener(ModuleInspector *insp)
{
    eventListener = insp;
}

void ModuleGraphicsView::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(eventListener)
        eventListener->mouseDoubleClick(event);
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
        //TODO MessageBox
//        CHK(Tcl_VarEval(interp, "tk_messageBox -parent ", windowName, " -type yesno -title Warning -icon question "
//                                                                      "-message {Module '", object->getFullName(), "' ", problem,
//                        ", it may take a long time to display the graphics. "
//                        "Do you want to proceed with drawing?}", TCL_NULL));
//        bool answer = (Tcl_GetStringResult(interp)[0] == 'y');
//        if (answer == false) {
//            notDrawn = true;
//            CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));  // this must be done, still
//            return;
//        }
    }

    recalculateLayout();
    redrawFigures();
    redrawModules();
    redrawNextEventMarker();
    redrawMessages();
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

}

void ModuleGraphicsView::redrawModules()
{
    cModule *parentModule = static_cast<cModule *>(object);

    // then display all submodules
    //CHK(Tcl_VarEval(interp, canvas, " delete dx", TCL_NULL));  // NOT "delete all" because that'd remove "bubbles" too!

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
    //CHK(Tcl_VarEval(interp, canvas, " raise bubble", TCL_NULL));
    //CHK(Tcl_VarEval(interp, "ModuleInspector:setScrollRegion ", windowName, " 0", TCL_NULL));
}

void ModuleGraphicsView::drawSubmodule(cModule *submod, double x, double y)
{
    const char *iconName = submod->getDisplayString().getTagArg("i", 0);
    if (!iconName || !iconName[0])
        iconName = "block/process";

    // TODO context menu + doubleclick
    QImage *img = getQtenv()->icons.getImage(iconName);
    QPixmap icon = QPixmap::fromImage(*img);
    submoduleGraphicsItems[submod->getId()] = scene()->addPixmap(icon);
    submoduleGraphicsItems[submod->getId()]->setPos(x, y);
    // TODO key
    submoduleGraphicsItems[submod->getId()]->setData(1, QVariant::fromValue(static_cast<cObject *>(submod)));

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

    char gateptr[32], srcptr[32], destptr[32], chanptr[32], indices[32];

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

    scene()->addLine(src_rect.x(), src_rect.y(), dest_rect.x(), dest_rect.y());

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
    if (submoduleGraphicsItems.find(mod->getId()) == submoduleGraphicsItems.end())
        return QPointF(0, 0);
    QPixmap icon = submoduleGraphicsItems[mod->getId()]->pixmap();
    QPointF pos = submoduleGraphicsItems[mod->getId()]->pos();
    pos.setX(pos.x() + icon.width() / 2);
    pos.setY(pos.y() + icon.height() / 2);
    return pos;
}

QGraphicsItem *ModuleGraphicsView::getItemAt(qreal x, qreal y)
{
    return scene()->itemAt(mapToScene(x, y), QTransform());
}

void ModuleGraphicsView::clear()
{
    scene()->clear();
    submoduleGraphicsItems.clear();
}

void ModuleGraphicsView::redrawNextEventMarker()
{

}

void ModuleGraphicsView::redrawMessages()
{

}

void ModuleGraphicsView::refreshSubmodules()
{

}

void ModuleGraphicsView::adjustSubmodulesZOrder()
{

}

} // namespace qtenv
} // namespace omnetpp
