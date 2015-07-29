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
#include "submoduleitem.h"
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
    switch (event->button()) {
    case Qt::LeftButton: emit click(event); break;
    case Qt::XButton1:   emit back();       break;
    case Qt::XButton2:   emit forward();    break;
    default:   /* shut up, compiler! */     break;
    }
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
        QMessageBox::StandardButton answer = QMessageBox::warning(this, "Warning", message, QMessageBox::Yes | QMessageBox::No);
        if(answer == QMessageBox::Yes)
        {
            notDrawn = true;
            clear();  // this must be done, still
            return;
        }
    }

    submoduleLayer->clear();
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
    auto item = new SubmoduleItem(submod);
    item->setRangeLayer(rangeLayer);
    SubmoduleItemUtil::setupFromDisplayString(item, submod);
    item->setData(1, QVariant::fromValue(dynamic_cast<cObject *>(submod)));
    item->setPos(submodPosMap[submod]);
    submoduleGraphicsItems[submod->getId()] = item;
    item->setParentItem(submoduleLayer);
}

void ModuleGraphicsView::drawEnclosingModule(cModule *parentModule)
{
    cDisplayString ds = parentModule->hasDisplayString() && parentModule->parametersFinalized()
            ? parentModule->getDisplayString()
            : cDisplayString();

    // replacing $param args with the actual parameter values
    std::string buffer;
    ds.updateWith(substituteDisplayStringParamRefs(ds, buffer, parentModule, true));

    QRectF border;

    if (ds.containsTag("bgb")) {
        border.setLeft(QString(ds.getTagArg("bgb", 0)).toInt());
        border.setTop(QString(ds.getTagArg("bgb", 1)).toInt());
        // TODO: zoom
    }

    if (submoduleLayer->childItems().isEmpty()) {
        border.setWidth(300);
        border.setHeight(200);
    } else {
        // Neither layer->boundingRect or layer->childrenBoundingRect
        // does what we want here. the former doesn't consider children at all,
        // the latter encloses everything, right down to the leaves.
        // What we want is the bounding box of only the child items themselves, no recursion.
        for (auto c : submoduleLayer->childItems()) {
            border = border.united(c->mapRectToParent(c->boundingRect()));
        }
        border = border.adjusted(-10, -10, 10, 10); // adding a bit of a margin
    }

    auto background = ds.getTagArg("bgb", 2);
    QColor::setAllowX11ColorNames(true); // XXX only works on X11, remove later!
    QColor backgroundColor("gray82");
    if (background[0]) {
        backgroundColor = SubmoduleItemUtil::parseColor(background);
    }

    auto outline = ds.getTagArg("bgb", 3);
    QColor outlineColor("black");
    if (outline[0]) {
        outlineColor = SubmoduleItemUtil::parseColor(outline);
    }

    int outlineWidth;
    bool ok;
    outlineWidth = QString(ds.getTagArg("bgb", 4)).toInt(&ok);
    if (!ok)
        outlineWidth = 2;


//$c create ptext [expr $bx+3] [expr $by+3] -text $name -textanchor nw {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx tooltip modname $ptr"


    setBackgroundBrush(QColor("#a0e0a0"));
    delete rectangleItem;
    rectangleItem = new QGraphicsRectItem(border.adjusted(-outlineWidth/2.0f, -outlineWidth/2.0f, outlineWidth/2.0f, outlineWidth/2.0f));
    rectangleItem->setPen(QPen(outlineColor, outlineWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    rectangleItem->setBrush(backgroundColor);
    backgroundLayer->addItem(rectangleItem);
    rectangleItem->setZValue(-10);
    setSceneRect(border.adjusted(-10, -10, 10, 10));

    /*

if {$sx=="" || $sy==""} {
    set bb [$c bbox submod]
    if {$bb==""} {
        if {$zoom==""} {
            set bb [list $bx $by 300 200]
        } else {
            set bb [list $bx $by [expr $zoom*300] [expr $zoom*200]]
        }
    }
    if {$sx==""} {set sx [expr [lindex $bb 2]+[lindex $bb 0]-2*$bx]}
    if {$sy==""} {set sy [expr [lindex $bb 3]+[lindex $bb 1]-2*$by]}
}


        */



    /*
    proc ModuleInspector:drawEnclosingModule {c ptr name dispstr} {
       global icons bitmaps inspectordata tkpFont canvasTextOptions
       # puts "DEBUG: ModuleInspector:drawEnclosingModule $c $ptr $name $dispstr"

       set zoom $inspectordata($c:zoomfactor)

       if [catch {

           # lower all range indicators below the icons
           $c lower "range"

           # parse display string. note: we need "1" as last parameter (search
           # for $params in parent module too), because all tags get resolved
           # not only bg* ones.
           opp_displaystring $dispstr parse tags $ptr 1

           # determine top-left origin (bgp tag; currently not supported)
           #if {![info exists tags(bgp)]} {set tags(bgp) {}}
           #set bx [lindex $tags(bgp) 0]
           #set by [lindex $tags(bgp) 1]
           #if {$bx==""} {set bx 0}
           #if {$by==""} {set by 0}
           #if {$zoom != ""} {
           #    set bx [expr $zoom*$bx]
           #    set by [expr $zoom*$by]
           #}
           set bx 0
           set by 0

           # determine size
           if {![info exists tags(bgb)]} {set tags(bgb) {{} {} {}}}
           set sx [lindex $tags(bgb) 0]
           set sy [lindex $tags(bgb) 1]
           if {$zoom != ""} {
               if {$sx!=""} {set sx [expr $zoom*$sx]}
               if {$sy!=""} {set sy [expr $zoom*$sy]}
           }

           if {$sx=="" || $sy==""} {
               set bb [$c bbox submod]
               if {$bb==""} {
                   if {$zoom==""} {
                       set bb [list $bx $by 300 200]
                   } else {
                       set bb [list $bx $by [expr $zoom*300] [expr $zoom*200]]
                   }
               }
               if {$sx==""} {set sx [expr [lindex $bb 2]+[lindex $bb 0]-2*$bx]}
               if {$sy==""} {set sy [expr [lindex $bb 3]+[lindex $bb 1]-2*$by]}
           }

           # determine colors and line width
           set fill [lindex $tags(bgb) 2]
           if {$fill == ""} {set fill grey82}
           if {$fill == "-"} {set fill ""}
           if {[string index $fill 0]== "@"} {set fill [opp_hsb_to_rgb $fill]}
           set outline [lindex $tags(bgb) 3]
           if {$outline == ""} {set outline black}
           if {$outline == "-"} {set outline ""}
           if {[string index $outline 0]== "@"} {set outline [opp_hsb_to_rgb $outline]}
           set width [lindex $tags(bgb) 4]
           if {$width == ""} {set width 2}

           # draw (note: width should grow *outside* the $sx-by-$sy inner rectangle)
           $c create prect [expr $bx-$width/2] [expr $by-$width/2] [expr $bx+$sx+$width/2] [expr $by+$sy+$width/2] \
               -fill $fill -strokewidth $width -stroke $outline -strokelinejoin miter -tags "dx mod $ptr"
           $c create ptext [expr $bx+3] [expr $by+3] -text $name -textanchor nw {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx tooltip modname $ptr"

           # background image
           if {![info exists tags(bgi)]} {set tags(bgi) {}}
           set imgname [lindex $tags(bgi) 0]
           set imgmode [lindex $tags(bgi) 1]
           if {$imgname!=""} {
              if {[catch {set img $bitmaps($imgname)}]} {
                  set img $icons(unknown)
              }
              set iwidth [image width $img]
              set iheight [image height $img]
              set isx [expr $iwidth*$zoom]
              set isy [expr $iheight*$zoom]
              if {[string index $imgmode 0]== "c"} {
                  # image centered
                  set imgx [expr $bx+$sx/2]
                  set imgy [expr $by+$sy/2]
                  set croppedsx [mathMin $sx $isx]
                  set croppedsy [mathMin $sy $isy]
                  if {$sx >= $isx} {
                      set isrcx1 0
                      set isrcx2 $iwidth
                  } else {
                      set isrcx1 [expr ($isx-$sx)/2/$zoom]
                      set isrcx2 [expr $iwidth - $isrcx1]
                  }
                  if {$sy >= $isy} {
                      set isrcy1 0
                      set isrcy2 $iheight
                  } else {
                      set isrcy1 [expr ($isy-$sy)/2/$zoom]
                      set isrcy2 [expr $iheight - $isrcy1]
                  }
                  $c create pimage $imgx $imgy -image $img -anchor c -width $croppedsx -height $croppedsy -srcregion [list $isrcx1 $isrcy1 $isrcx2 $isrcy2] -tags "dx mod $ptr"

              } elseif {[string index $imgmode 0]== "s"} {
                  # image stretched to fill the background area
                  $c create pimage $bx $by -image $img -anchor nw -width $sx -height $sy -tags "dx mod $ptr"
              } elseif {[string index $imgmode 0]== "t"} {
                  # image "tile" mode
                  set tx [expr $sx / $zoom]
                  set ty [expr $sy / $zoom]
                  $c create pimage $bx $by -image $img -anchor nw -width $sx -height $sy -srcregion [list 0 0 $tx $ty] -tags "dx mod $ptr"
              } else {
                  # default mode: image top-left corner gets aligned to background top-left corner
                  # we need cropping
                  set croppedsx [mathMin $sx $isx]
                  set croppedsy [mathMin $sy $isy]
                  set tx [expr $croppedsx / $zoom]
                  set ty [expr $croppedsy / $zoom]
                  $c create pimage $bx $by -image $img -anchor nw -width $croppedsx -height $croppedsy -srcregion [list 0 0 $tx $ty] -tags "dx mod $ptr"
              }
           }

           # grid display
           if {![info exists tags(bgg)]} {set tags(bgg) {}}
           set gdist [lindex $tags(bgg) 0]
           set gminor [lindex $tags(bgg) 1]
           set gcolor [lindex $tags(bgg) 2]
           if {$gcolor == ""} {set gcolor grey}
           if {$gcolor == "-"} {set gcolor ""}
           if {[string index $gcolor 0]== "@"} {set gcolor [opp_hsb_to_rgb $gcolor]}
           if {$gdist!=""} {
               if {$zoom != ""} {
                   set gdist [expr $zoom*$gdist]
               }
               if {$gminor=="" || $gminor < 1} {set gminor 1}
               for {set x $bx} {$x < $bx+$sx} {set x [expr $x+$gdist]} {
                   set coords [list $x $by $x [expr $by+$sy]]
                   $c create pline $coords -strokewidth 1 -stroke $gcolor -tags "dx mod $ptr"
                   # create minor ticks
                   set i 1
                   for {set minorx [expr int($x+$gdist/$gminor)]} {$i < $gminor && $minorx < $bx+$sx} {
                                                 set i [expr $i+1]} {
                       set minorx [expr int($x+$i*$gdist/$gminor)]
                       if {$minorx < $bx+$sx} {
                           set coords [list $minorx $by $minorx [expr $by+$sy]]
                           $c create pline $coords -strokewidth 1 -strokedasharray {2 3} -stroke $gcolor -tags "dx mod $ptr"
                       }
                   }
               }
               for {set y $by} {$y < $by+$sy} {set y [expr $y+$gdist]} {
                   set coords [list $bx $y [expr $bx+$sx] $y]
                   $c create pline $coords -strokewidth 1 -stroke $gcolor -tags "dx mod $ptr"
                   # create minor ticks
                   set i 1
                   for {set minory [expr int($y+$gdist/$gminor)]} {$i < $gminor && $minory < $by+$sy} {
                                                 set i [expr $i+1]} {
                       set minory [expr int($y+$i*$gdist/$gminor)]
                       if {$minory < $by+$sy} {
                           set coords [list $bx $minory [expr $bx+$sx] $minory]
                           $c create pline $coords -strokewidth 1 -strokedasharray {2 3} -stroke $gcolor -tags "dx mod $ptr"
                       }
                   }
               }
           }

           # text: bgt=<x>,<y>,<text>,<color>; multiple bgt tags supported (bgt1,bgt2,etc)
           foreach {bgttag} [array names tags -regexp {^bgt\d*$} ] {
               set x [lindex $tags($bgttag) 0]
               set y [lindex $tags($bgttag) 1]
               if {$x==""} {set x 0}
               if {$y==""} {set y 0}
               set txt [lindex $tags($bgttag) 2]
               set color [lindex $tags($bgttag) 3]
               if {$color == ""} {set color black}
               if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}
               $c create ptext $x $y -text $txt -fill $color -textanchor nw {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx"
           }

           $c lower mod

       } errmsg] {
           tk_messageBox -type ok -title "Error" -icon error -parent [winfo toplevel [focusOrRoot]] \
                         -message "Error in display string of $name: $errmsg"
       }
    }
    */
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
    item->setParentItem(submoduleLayer);



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

    for (auto item : items) {
        QVariant variant = item->data(1);
        if (variant.isValid())
            objects.push_back(variant.value<cObject*>());
    }

    return objects;
}

void ModuleGraphicsView::setBackgroundLayer(GraphicsLayer *layer)
{
    clear();
    backgroundLayer = layer;
    redraw();
}

void ModuleGraphicsView::setSubmoduleLayer(GraphicsLayer *layer) {
    clear();
    submoduleLayer = layer;
    redraw();
}

void ModuleGraphicsView::setRangeLayer(GraphicsLayer *layer) {
    clear();
    this->rangeLayer = layer;
    for (auto &i : submoduleGraphicsItems) {
        i.second->setRangeLayer(layer);
    }
    redraw();
}

void ModuleGraphicsView::clear() {
    if (submoduleLayer) submoduleLayer->clear();
    submoduleGraphicsItems.clear();
    nextEventMarker = nullptr; // because it is on the submodule layer, it has been deleted by that
}

void ModuleGraphicsView::redrawNextEventMarker() {
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
        nextEventMarker = new QGraphicsRectItem(submoduleLayer, scene());

        auto item = submoduleGraphicsItems[nextModParent->getId()];
        if (item) {
            nextEventMarker->setRect(item->mapRectToParent(item->boundingRect()).adjusted(-2, -2, 2, 2));
            nextEventMarker->setBrush(Qt::NoBrush);
            nextEventMarker->setPen(QPen(QColor("red"), nextMod == nextModParent ? 2 : 1));
            nextEventMarker->setZValue(10);
        }
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
