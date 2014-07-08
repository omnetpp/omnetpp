//==========================================================================
//  MODULEINSPECTOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "moduleinspector.h"
#include "figurerenderers.h"
#include "tkenv.h"
#include "tklib.h"
#include "tkutil.h"
#include "inspectorfactory.h"
#include "arrow.h"
#include "graphlayouter.h"
#include "layouterenv.h"
#include "forcedirectedgraphlayouter.h"
#include "basicspringembedderlayout.h"
#include "stringtokenizer.h"
#include "cdisplaystring.h"
#include "cqueue.h"
#include "cmessage.h"
#include "cgate.h"
#include "cchannel.h"
#include "csimplemodule.h"
#include "stringutil.h"

NAMESPACE_BEGIN

#define UNKNOWNICON_WIDTH  32
#define UNKNOWNICON_HEIGHT 32

void _dummy_for_moduleinspector() {}


class ModuleInspectorFactory : public InspectorFactory
{
  public:
    ModuleInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cModule *>(obj)!=NULL;}
    int getInspectorType() {return INSP_GRAPHICAL;}
    double getQualityAsDefault(cObject *object) {return dynamic_cast<cSimpleModule *>(object) ? 0.9 : 3.0;}
    Inspector *createInspector() {return new ModuleInspector(this);}
};

Register_InspectorFactory(ModuleInspectorFactory);


ModuleInspector::ModuleInspector(InspectorFactory *f) : Inspector(f)
{
   needs_redraw = false;
   notDrawn = false;
   layoutSeed = 0;
}

ModuleInspector::~ModuleInspector()
{
}

void ModuleInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    CHK(Tcl_VarEval(interp, canvas, " delete all",NULL));

    if (object)
    {
        layoutSeed = 0; // we'll read the "bgl" display string tag from Tcl
        CHK(Tcl_VarEval(interp, "ModuleInspector:onSetObject ", windowName, NULL ));
    }
}

void ModuleInspector::createWindow(const char *window, const char *geometry)
{
   Inspector::createWindow(window, geometry);

   strcpy(canvas,windowName);
   strcat(canvas,".c");

   CHK(Tcl_VarEval(interp, "createModuleInspector ", windowName, " ", TclQuotedString(geometry).get(), NULL ));
}

void ModuleInspector::useWindow(const char *window)
{
    Inspector::useWindow(window);
    strcpy(canvas,windowName);
    strcat(canvas,".c");
}

void ModuleInspector::refresh()
{
   Inspector::refresh();

   if (!object)
   {
       CHK(Tcl_VarEval(interp, canvas," delete all", NULL));
       return;
   }

   if (notDrawn)
       return;


   // redraw modules only if really needed
   if (needs_redraw)
   {
       needs_redraw = false;
       redrawAll();
   }
   else
   {
       refreshFigures();
       redrawNextEventMarker();
       redrawMessages();
       refreshSubmodules();
   }
}

void ModuleInspector::relayoutAndRedrawAll()
{
   ASSERT(object != NULL);

   cModule *mod = (cModule *)object;
   int submoduleCount = 0;
   int estimatedGateCount = mod->gateCount();
   for (cModule::SubmoduleIterator submod(mod); !submod.end(); submod++)
   {
       submoduleCount++;
       // note: estimatedGateCount will count unconnected gates in the gate array as well
       estimatedGateCount += submod()->gateCount();
   }

   notDrawn = false;
   if (submoduleCount>1000 || estimatedGateCount>4000)
   {
       char problem[200];
       if (submoduleCount>1000)
           sprintf(problem, "contains more than 1000 submodules (exactly %d)", submoduleCount);
       else
           sprintf(problem, "may contain a lot of connections (modules have a large number of gates)");
       CHK(Tcl_VarEval(interp,"tk_messageBox -parent ",windowName," -type yesno -title Warning -icon question "
                              "-message {Module '", object->getFullName(), "' ", problem,
                              ", it may take a long time to display the graphics. "
                              "Do you want to proceed with drawing?}", NULL));
       bool answer = (Tcl_GetStringResult(interp)[0]=='y');
       if (answer==false)
       {
           notDrawn = true;
           CHK(Tcl_VarEval(interp, canvas, " delete all",NULL)); // this must be done, still
           return;
       }
   }

   recalculateLayout();
   redrawModules();
   redrawFigures();
   redrawNextEventMarker();
   redrawMessages();
   refreshSubmodules();
}

void ModuleInspector::redrawAll()
{
   if (object == NULL) {
       CHK(Tcl_VarEval(interp, canvas," delete all", NULL));
       return;
   }

   refreshLayout();
   redrawModules();
   redrawFigures();
   redrawNextEventMarker();
   redrawMessages();
   refreshSubmodules();
}

void ModuleInspector::getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
                                                              double& x, double& y, double& sx, double& sy)
{
    const cDisplayString blank;
    const cDisplayString& ds = submod->hasDisplayString() && submod->parametersFinalized() ? submod->getDisplayString() : blank;

    // get size -- we'll need to return that too, and may be needed for matrix, ring etc. layout
    double boxsx=0, boxsy=0;
    int iconsx=0, iconsy=0;
    if (ds.containsTag("b") || !ds.containsTag("i"))
    {
        boxsx = resolveDoubleDispStrArg(ds.getTagArg("b",0), submod, 40);
        boxsy = resolveDoubleDispStrArg(ds.getTagArg("b",1), submod, 24);
    }
    if (ds.containsTag("i"))
    {
        const char *imgName = ds.getTagArg("i",0);
        const char *imgSize = ds.getTagArg("is",0);
        if (!imgName || !*imgName)
        {
            iconsx = UNKNOWNICON_WIDTH;
            iconsy = UNKNOWNICON_HEIGHT;
        }
        else
        {
            CHK(Tcl_VarEval(interp, "lookupImage ", imgName, " ", imgSize, NULL));
            Tk_Image img = Tk_GetImage(interp, Tk_MainWindow(interp), Tcl_GetStringResult(interp), NULL, NULL);
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
    }
    sx = (boxsx>iconsx) ? boxsx : iconsx;
    sy = (boxsy>iconsy) ? boxsy : iconsy;

    // first, see if there's an explicit position ("p=" tag) given
    x = resolveDoubleDispStrArg(ds.getTagArg("p",0), submod, -1);
    y = resolveDoubleDispStrArg(ds.getTagArg("p",1), submod, -1);
    explicitcoords = x!=-1 && y!=-1;

    // set missing coordinates to zero
    if (x==-1) x = 0;
    if (y==-1) y = 0;

    const char *layout = ds.getTagArg("p",2); // matrix, row, column, ring, exact etc.
    obeysLayout = (layout && *layout);

    // modify x,y using predefined layouts
    if (!layout || !*layout)
    {
        // we're happy
    }
    else if (!strcmp(layout,"e") || !strcmp(layout,"x") || !strcmp(layout,"exact"))
    {
        double dx = resolveDoubleDispStrArg(ds.getTagArg("p",3), submod, 0);
        double dy = resolveDoubleDispStrArg(ds.getTagArg("p",4), submod, 0);
        x += dx;
        y += dy;
    }
    else if (!strcmp(layout,"r") || !strcmp(layout,"row"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        double dx = resolveDoubleDispStrArg(ds.getTagArg("p",3), submod, 2*sx);
        x += submod->getIndex()*dx;
    }
    else if (!strcmp(layout,"c") || !strcmp(layout,"col") || !strcmp(layout,"column"))
    {
        double dy = resolveDoubleDispStrArg(ds.getTagArg("p",3), submod, 2*sy);
        y += submod->getIndex()*dy;
    }
    else if (!strcmp(layout,"m") || !strcmp(layout,"matrix"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        int columns = resolveLongDispStrArg(ds.getTagArg("p",3), submod, 5);
        double dx = resolveDoubleDispStrArg(ds.getTagArg("p",4), submod, 2*sx);
        double dy = resolveDoubleDispStrArg(ds.getTagArg("p",5), submod, 2*sy);
        if (columns < 1) columns = 1;
        x += (submod->getIndex() % columns)*dx;
        y += (submod->getIndex() / columns)*dy;
    }
    else if (!strcmp(layout,"i") || !strcmp(layout,"ri") || !strcmp(layout,"ring"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        double rx = resolveDoubleDispStrArg(ds.getTagArg("p",3), submod, (sx+sy)*submod->size()/4);
        double ry = resolveDoubleDispStrArg(ds.getTagArg("p",4), submod, rx);

        x += rx - rx*sin(submod->getIndex()*2*PI/submod->size());
        y += ry - ry*cos(submod->getIndex()*2*PI/submod->size());
    }
    else
    {
        CHK(Tcl_VarEval(interp,"messagebox {Error} "
                        "{Error: invalid layout `", layout, "' in `p' tag "
                        "of display string \"", ds.str(), "\"} error ok", NULL));
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
    LayouterChoice choice = getTkenv()->opt->layouterChoice;
    if (choice==LAYOUTER_AUTO)
    {
        const int LIMIT = 20; // note: on test/anim/dynamic2, Advanced is already very slow with 30-40 modules
        int submodCountLimited = 0;
        for (cModule::SubmoduleIterator submod(parentModule); !submod.end() && submodCountLimited<LIMIT; submod++)
            submodCountLimited++;
        choice = submodCountLimited>=LIMIT ? LAYOUTER_FAST : LAYOUTER_ADVANCED;
    }
    GraphLayouter *layouter = choice==LAYOUTER_FAST ?
                                    (GraphLayouter *) new BasicSpringEmbedderLayout() :
                                    (GraphLayouter *) new ForceDirectedGraphLayouter();

    layouter->setSeed(layoutSeed);

    // background size
    int sx = resolveLongDispStrArg(ds.getTagArg("bgb",0), parentModule, 0);
    int sy = resolveLongDispStrArg(ds.getTagArg("bgb",1), parentModule, 0);
    int border = 30;
    if (sx!=0 && sx < 2*border)
        border = sx/2;
    if (sy!=0 && sy < 2*border)
        border = sy/2;
    layouter->setSize(sx, sy, border);
    // TODO support "bgp" tag ("background position")
    // TODO: scaling ("bgs") support for layouter.
    // Layouter algorithm is NOT scale-independent, so we should divide ALL coordinates
    // by "scale" before passing them to the layouter, then multiply back the results.

    // loop through all submodules, get their sizes and positions and feed them into layouting engine
    for (cModule::SubmoduleIterator it(parentModule); !it.end(); it++)
    {
        cModule *submod = it();

        bool explicitCoords, obeysLayout;
        double x, y, sx, sy;
        getSubmoduleCoords(submod, explicitCoords, obeysLayout, x, y, sx, sy);

        // add node into layouter:
        if (explicitCoords)
        {
            // e.g. "p=120,70" or "p=140,30,ring"
            layouter->addFixedNode(submod->getId(), x, y, sx, sy);
        }
        else if (submodPosMap.find(submod)!=submodPosMap.end())
        {
            // reuse coordinates from previous layout
            Point pos = submodPosMap[submod];
            layouter->addFixedNode(submod->getId(), pos.x, pos.y, sx, sy);
        }
        else if (obeysLayout)
        {
            // all modules are anchored to the anchor point with the vector's name
            // e.g. "p=,,ring"
            layouter->addAnchoredNode(submod->getId(), submod->getName(), x, y, sx, sy);
        }
        else
        {
            layouter->addMovableNode(submod->getId(), sx, sy);
        }
    }

    // add connections into the layouter, too
    bool atParent = false;
    for (cModule::SubmoduleIterator it(parentModule); !atParent; it++)
    {
        cModule *mod = !it.end() ? it() : (atParent=true,parentModule);

        for (cModule::GateIterator i(mod); !i.end(); i++)
        {
            cGate *gate = i();
            cGate *destGate = gate->getNextGate();
            if (gate->getType()==(atParent ? cGate::INPUT : cGate::OUTPUT) && destGate)
            {
                cModule *destMod = destGate->getOwnerModule();
                if (mod==parentModule && destMod==parentModule) {
                    // nop
                } else if (destMod==parentModule) {
                    layouter->addEdgeToBorder(mod->getId());
                } else if (destMod->getParentModule()!=parentModule) {
                    // connection goes to a module under a different parent!
                    // this in fact violates module encapsulation, but let's
                    // accept it nevertheless. Just skip this connection.
                } else if (mod==parentModule) {
                    layouter->addEdgeToBorder(destMod->getId());
                } else {
                    layouter->addEdge(mod->getId(), destMod->getId());
                }
            }
        }
    }

    // set up layouter environment (responsible for "Stop" button handling and visualizing the layouting process)
    TkenvGraphLayouterEnvironment environment(interp, parentModule, ds);

    std::string stopButton = std::string(getWindowName()) + ".toolbar.stop";
    bool isExpressMode = getTkenv()->getSimulationRunMode() == Tkenv::RUNMODE_EXPRESS;
    if (!isExpressMode)
        environment.setWidgetToGrab(stopButton.c_str());

    // enable visualizing only if full re-layouting (no cached coordinates in submodPosMap)
    // if (getTkenv()->opt->showlayouting)  // for debugging
    if (submodPosMap.empty() && getTkenv()->opt->showLayouting)
        environment.setCanvas(canvas);

    layouter->setEnvironment(&environment);
    layouter->execute();
    environment.cleanup();

    // fill the map with the results
    submodPosMap.clear();
    for (cModule::SubmoduleIterator it(parentModule); !it.end(); it++)
    {
        cModule *submod = it();

        Point pos;
        layouter->getNodePosition(submod->getId(), pos.x, pos.y);
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
    CHK(Tcl_VarEval(interp, canvas, " delete dx",NULL)); // NOT "delete all" because that'd remove "bubbles" too!
    std::string buffer;
    const char *rawScaling = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString().getTagArg("bgs",0) : "";
    const char *scaling = substituteDisplayStringParamRefs(rawScaling, buffer, parentModule, true);

    for (cModule::SubmoduleIterator it(parentModule); !it.end(); it++)
    {
        cModule *submod = it();
        assert(submodPosMap.find(submod)!=submodPosMap.end());
        Point& pos = submodPosMap[submod];
        drawSubmodule(submod, pos.x, pos.y, scaling);
    }

    // draw enclosing module
    drawEnclosingModule(parentModule, scaling);

    // loop through all submodules and enclosing module & draw their connections
    bool atParent = false;
    for (cModule::SubmoduleIterator it(parentModule); !atParent; it++)
    {
        cModule *mod = !it.end() ? it() : (atParent=true,parentModule);

        for (cModule::GateIterator i(mod); !i.end(); i++)
        {
            cGate *gate = i();
            if (gate->getType()==(atParent ? cGate::INPUT: cGate::OUTPUT) && gate->getNextGate()!=NULL)
            {
                drawConnection(gate);
            }
        }
    }
    CHK(Tcl_VarEval(interp, canvas, " raise bubble",NULL));
    CHK(Tcl_VarEval(interp, "ModuleInspector:setScrollRegion ", windowName, " 0",NULL));
}

void ModuleInspector::drawSubmodule(cModule *submod, double x, double y, const char *scaling)
{
    char coords[64];
    sprintf(coords,"%g %g ", x, y);
    const char *dispstr = submod->hasDisplayString() && submod->parametersFinalized() ? submod->getDisplayString().str() : "";

    CHK(Tcl_VarEval(interp, "ModuleInspector:drawSubmodule ",
                    canvas, " ",
                    ptrToStr(submod), " ",
                    coords,
                    "{", submod->getFullName(), "} ",
                    TclQuotedString(dispstr).get(), " ",
                    "{", scaling, "} ",
                    (submod->isPlaceholder() ? "1" : "0"),
                    NULL));
}

void ModuleInspector::drawEnclosingModule(cModule *parentModule, const char *scaling)
{
    const char *displayString = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString().str() : "";
    CHK(Tcl_VarEval(interp, "ModuleInspector:drawEnclosingModule ",
                       canvas, " ",
                       ptrToStr(parentModule), " ",
                       "{", parentModule->getFullPath().c_str(), "} ",
                       TclQuotedString(displayString).get(), " ",
                       "{", scaling, "} ",
                       NULL ));
}

void ModuleInspector::drawConnection(cGate *gate)
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


    ptrToStr(gate, gateptr);
    ptrToStr(mod, srcptr);
    ptrToStr(destGate->getOwnerModule(), destptr);
    sprintf(indices, "%d %d %d %d",
            gate->getIndex(), gate->size(),
            destGate->getIndex(), destGate->size());
    cChannel *chan = gate->getChannel();
    ptrToStr(chan, chanptr);
    const char *dispstr = (chan && chan->hasDisplayString() && chan->parametersFinalized()) ? chan->getDisplayString().str() : "";

    CHK(Tcl_VarEval(interp, "ModuleInspector:drawConnection ",
            canvas, " ",
            gateptr, " ",
            TclQuotedString(dispstr).get(), " ",
            srcptr, " ",
            destptr, " ",
            chanptr, " ",
            indices, " ",
            twoWayConnection ? "1" : "0",
            NULL
             ));
}

void ModuleInspector::redrawMessages()
{
   // refresh & cleanup from prev. events
   CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", NULL));

   // this thingy is only needed if animation is going on
   if (!getTkenv()->animating)
       return;

   // loop through all messages in the event queue and display them
   for (cMessageHeap::Iterator msg(simulation.msgQueue); !msg.end(); msg++)
   {
      char msgptr[32];
      ptrToStr(msg(),msgptr);

      cModule *arrivalMod = simulation.getModule( msg()->getArrivalModuleId() );
      if (arrivalMod &&
          arrivalMod->getParentModule()==static_cast<cModule *>(object) &&
          msg()->getArrivalGateId()>=0)
      {
         cGate *arrivalGate = msg()->getArrivalGate();

         // if arrivalGate is connected, msg arrived on a connection, otherwise via sendDirect()
         if (arrivalGate->getPreviousGate())
         {
             cGate *gate = arrivalGate->getPreviousGate();
             CHK(Tcl_VarEval(interp, "ModuleInspector:drawMessageOnGate ",
                             canvas, " ",
                             ptrToStr(gate), " ",
                             msgptr,
                             NULL));
         }
         else
         {
             CHK(Tcl_VarEval(interp, "ModuleInspector:drawMessageOnModule ",
                             canvas, " ",
                             ptrToStr(arrivalMod), " ",
                             msgptr,
                             NULL));
         }
      }
   }
   CHK(Tcl_VarEval(interp, canvas, " raise bubble",NULL));
}

void ModuleInspector::redrawNextEventMarker()
{
   cModule *mod = static_cast<cModule *>(object);

   // removing marker from previous event
   CHK(Tcl_VarEval(interp, canvas, " delete nexteventmarker", NULL));

   // this thingy is only needed if animation is going on
   if (!getTkenv()->animating || !getTkenv()->opt->showNextEventMarkers)
       return;

   // if any parent of the module containing the next event is on this canvas, draw marker
   cModule *nextMod = simulation.guessNextModule();
   cModule *nextModParent = nextMod;
   while (nextModParent && nextModParent->getParentModule()!=mod)
       nextModParent = nextModParent->getParentModule();
   if (nextModParent)
   {
       CHK(Tcl_VarEval(interp, "ModuleInspector:drawNextEventMarker ",
                       canvas, " ",
                       ptrToStr(nextModParent), " ",
                       (nextMod==nextModParent ? "2" : "1"),
                       NULL));
   }
}

void ModuleInspector::redrawFigures()
{
   cModule *mod = static_cast<cModule *>(object);

   // remove existing figures
   CHK(Tcl_VarEval(interp, canvas, " delete fig", NULL));

   // draw figures
   cCanvas *canvas = mod->getCanvasIfExists();
   if (canvas)
   {
       LinearCoordMapping mapping;
       initMapping(mapping);
       drawFigureRec(canvas->getRootFigure(), mapping);

       char tag[32];
       cFigure *submodulesLayer = canvas->getSubmodulesLayer();
       ASSERT(submodulesLayer); // should be present!
       sprintf(tag, "f%d", submodulesLayer->getId());
       CHK(Tcl_VarEval(interp, this->canvas, " lower submodext ", tag, NULL));
       CHK(Tcl_VarEval(interp, this->canvas, " raise submodext ", tag, NULL));
   }
}

void ModuleInspector::refreshFigures()
{
   cModule *mod = static_cast<cModule *>(object);
   cCanvas *canvas = mod->getCanvasIfExists();
   if (canvas)
   {
       // if there is a structural change, we rebuild everything;
       // otherwise we only adjust subtree or that particular figure
       cFigure *rootFigure = canvas->getRootFigure();
       if ((rootFigure->getTreeChangeFlags() | rootFigure->getLocalChangeFlags()) & cFigure::CHANGE_STRUCTURAL)
       {
           redrawFigures();
           rootFigure->clearChangeFlags();
       }
       else if ((rootFigure->getTreeChangeFlags() | rootFigure->getLocalChangeFlags()) != 0)
       {
           LinearCoordMapping mapping;
           initMapping(mapping);
           refreshFigureGeometryRec(rootFigure, mapping);
           refreshFigureVisualsRec(rootFigure);
           rootFigure->clearChangeFlags();
       }
   }
}

double ModuleInspector::getScale()
{
    // read scale
    cModule *parentModule = static_cast<cModule *>(object);
    std::string buffer;
    const char *rawScaling = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString().getTagArg("bgs",0) : "";
    const char *scalingStr = substituteDisplayStringParamRefs(rawScaling, buffer, parentModule, true);
    return opp_isblank(scalingStr) ? 1.0 : opp_atof(scalingStr);
}

double ModuleInspector::getZoom()
{
    // read zoom level ($inspectordata($c:zoomfactor)) -- TODO maybe move it into C++?
    const char *zoomStr = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":zoomfactor").c_str()), TCL_GLOBAL_ONLY);
    return opp_atof(zoomStr);
}

void ModuleInspector::initMapping(LinearCoordMapping& mapping)
{
    double scale = getScale();
    double zoom = getZoom();
    mapping.scaleX = mapping.scaleY = scale * zoom;
}

FigureRenderer *ModuleInspector::getRendererFor(cFigure *figure)
{
    return FigureRenderer::getRendererFor(figure);
}

void ModuleInspector::drawFigureRec(cFigure *figure, LinearCoordMapping& mapping)
{
    if (figure->isVisible())
    {
        FigureRenderer *renderer = getRendererFor(figure);
        renderer->render(figure, interp, canvas, &mapping);

        if (figure->containsFigures())
        {
            cFigure::Point offset = figure->getChildOrigin();
            LinearCoordMapping childMapping(mapping.scaleX, mapping.offsetX + mapping.scaleX*offset.x, mapping.scaleY, mapping.offsetY + mapping.scaleY*offset.y);

            for (int i = 0; i < figure->getNumFigures(); i++)
                drawFigureRec(figure->getFigure(i), childMapping);
        }
    }
}

void ModuleInspector::refreshFigureGeometryRec(cFigure *figure, LinearCoordMapping& mapping, bool forceGeometryRefresh)
{
    if (figure->getLocalChangeFlags() & cFigure::CHANGE_GEOMETRY)
        forceGeometryRefresh = true;  // must refresh this figure and its entire subtree

    if (forceGeometryRefresh)
    {
        FigureRenderer *renderer = getRendererFor(figure);
        renderer->refreshGeometry(figure, interp, canvas, &mapping);
    }

    if (forceGeometryRefresh || (figure->getTreeChangeFlags() & cFigure::CHANGE_GEOMETRY))
    {
        cFigure::Point offset = figure->getChildOrigin();
        LinearCoordMapping childMapping(mapping.scaleX, mapping.offsetX + mapping.scaleX*offset.x, mapping.scaleY, mapping.offsetY + mapping.scaleY*offset.y);
        for (int i = 0; i < figure->getNumFigures(); i++)
            refreshFigureGeometryRec(figure->getFigure(i), childMapping, forceGeometryRefresh);
    }
}

void ModuleInspector::refreshFigureVisualsRec(cFigure *figure)
{
    if (figure->getLocalChangeFlags() & cFigure::CHANGE_VISUAL)
    {
        FigureRenderer *renderer = getRendererFor(figure);
        renderer->refreshVisuals(figure, interp, canvas);
    }

    if (figure->getTreeChangeFlags() & cFigure::CHANGE_VISUAL)
        for (int i = 0; i < figure->getNumFigures(); i++)
            refreshFigureVisualsRec(figure->getFigure(i));
}

void ModuleInspector::refreshSubmodules()
{
   for (cModule::SubmoduleIterator submod(static_cast<cModule *>(object)); !submod.end(); submod++)
   {
       CHK(Tcl_VarEval(interp, "ModuleInspector:refreshSubmodule ",
                       windowName, " ",
                       ptrToStr(submod()),
                       NULL));
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
   needs_redraw = true; //TODO check, probably only non-background tags have changed...
}

void ModuleInspector::displayStringChanged(cGate *)
{
   needs_redraw = true;
}

void ModuleInspector::bubble(cComponent *subcomponent, const char *text)
{
    if (!subcomponent->isModule())
        return; // channel bubbles not yet supported

    // if submod position is not yet known (because e.g. we're in fast mode
    // and it was dynamically created since the last update), refresh layout
    // so that we can get coordinates for it
    cModule *submod = (cModule *)subcomponent;
    if (submodPosMap.find(submod)==submodPosMap.end())
        refreshLayout();

    cModule *parentModule = static_cast<cModule *>(object);
    std::string buffer;
    const char *rawScaling = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString().getTagArg("bgs",0) : "";
    const char *scaling = substituteDisplayStringParamRefs(rawScaling, buffer, parentModule, true);

    // invoke Tcl code to display bubble
    char coords[64];
    Point& pos = submodPosMap[submod];
    sprintf(coords, " %g %g ", pos.x, pos.y);
    CHK(Tcl_VarEval(interp, "ModuleInspector:bubble ", canvas, coords, " ", TclQuotedString(scaling).get(), " ", TclQuotedString(text).get(), NULL));
}

const char *ModuleInspector::animModeToStr(SendAnimMode mode)
{
    return mode==ANIM_BEGIN ? "beg" : mode==ANIM_THROUGH ? "thru" : mode==ANIM_END ? "end" : "???";
}

void ModuleInspector::animateMethodcallAscent(cModule *srcSubmod, const char *methodText)
{
    char parentPtr[30], modPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(srcSubmod, modPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallAscent ", getWindowName(), " ", parentPtr, " ", modPtr," ", TclQuotedString(methodText).get(), NULL));
}

void ModuleInspector::animateMethodcallDescent(cModule *destSubmod, const char *methodText)
{
    char parentPtr[30], modPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(destSubmod, modPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallDescent ", getWindowName(), " ", parentPtr, " ", modPtr," ", TclQuotedString(methodText).get(), NULL));
}

void ModuleInspector::animateMethodcallHoriz(cModule *srcSubmod, cModule *destSubmod, const char *methodText)
{
    char srcPtr[30], destPtr[30];
    ptrToStr(srcSubmod, srcPtr);
    ptrToStr(destSubmod, destPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallHoriz ", getWindowName(), " ", srcPtr, " ", destPtr," ", TclQuotedString(methodText).get(), NULL));
}

void ModuleInspector::animateMethodcallDelay(Tcl_Interp *interp)
{
    CHK(Tcl_Eval(interp, "ModuleInspector:animateMethodcallWait"));
}

void ModuleInspector::animateMethodcallCleanup()
{
    CHK(Tcl_VarEval(interp, "ModuleInspector:animateMethodcallCleanup ", getWindowName(), NULL));
}

void ModuleInspector::animateSendOnConn(cGate *srcGate, cMessage *msg, SendAnimMode mode)
{
    char gatePtr[30], msgPtr[30];
    ptrToStr(srcGate, gatePtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateOnConn ", getWindowName(), " ", msgPtr, " ", gatePtr, " ", modeStr, NULL));
}

void ModuleInspector::animateSenddirectAscent(cModule *srcSubmod, cMessage *msg)
{
    char parentPtr[30], modPtr[30], msgPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(srcSubmod, modPtr);
    ptrToStr(msg, msgPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectAscent ", getWindowName(), " ", msgPtr, " ", parentPtr," ", modPtr," ", "thru", NULL));
}

void ModuleInspector::animateSenddirectDescent(cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    char parentPtr[30], modPtr[30], msgPtr[30];
    ptrToStr(object, parentPtr);
    ptrToStr(destSubmod, modPtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectDescent ", getWindowName(), " ", msgPtr, " ", parentPtr," ", modPtr," ", modeStr, NULL));
}

void ModuleInspector::animateSenddirectHoriz(cModule *srcSubmod, cModule *destSubmod, cMessage *msg, SendAnimMode mode)
{
    char srcPtr[30], destPtr[30], msgPtr[30];
    ptrToStr(srcSubmod, srcPtr);
    ptrToStr(destSubmod, destPtr);
    ptrToStr(msg, msgPtr);
    const char *modeStr = animModeToStr(mode);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectHoriz ", getWindowName(), " ", msgPtr, " ", srcPtr," ", destPtr, " ", modeStr, NULL));
}

void ModuleInspector::animateSenddirectCleanup()
{
    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectCleanup ", getWindowName(), NULL));
}

void ModuleInspector::animateSenddirectDelivery(cModule *destSubmod, cMessage *msg)
{
    char destPtr[30], msgPtr[30];
    ptrToStr(destSubmod, destPtr);
    ptrToStr(msg, msgPtr);

    CHK(Tcl_VarEval(interp, "ModuleInspector:animateSenddirectDelivery ", getWindowName(), " ", msgPtr, " ", destPtr, NULL));
}

void ModuleInspector::performAnimations(Tcl_Interp *interp)
{
    CHK(Tcl_VarEval(interp, "performAnimations", NULL));
}

int ModuleInspector::inspectorCommand(int argc, const char **argv)
{
   if (argc<1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   if (strcmp(argv[0],"arrowcoords")==0)
   {
      return arrowcoords(interp,argc,argv);
   }
   else if (strcmp(argv[0],"relayout")==0)
   {
      TRY(relayoutAndRedrawAll());
      return TCL_OK;
   }
   else if (strcmp(argv[0],"redraw")==0)
   {
      TRY(redrawAll());
      return TCL_OK;
   }
   else if (strcmp(argv[0],"getdefaultlayoutseed")==0)
   {
       return getDefaultLayoutSeed(argc, argv);
   }
   else if (strcmp(argv[0],"getlayoutseed")==0)
   {
       return getLayoutSeed(argc, argv);
   }
   else if (strcmp(argv[0],"setlayoutseed")==0)
   {
       return setLayoutSeed(argc, argv);
   }
   else if (strcmp(argv[0],"submodulecount")==0)
   {
      return getSubmoduleCount(argc, argv);
   }
   else if (strcmp(argv[0],"getsubmodq")==0)
   {
      return getSubmodQ(argc, argv);
   }
   else if (strcmp(argv[0],"getsubmodqlen")==0)
   {
      return getSubmodQLen(argc, argv);
   }

   return Inspector::inspectorCommand(argc, argv);
}

int ModuleInspector::getDefaultLayoutSeed(int argc, const char **argv)
{
    if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}
    const cDisplayString blank;
    cModule *parentModule = static_cast<cModule *>(object);
    const cDisplayString& ds = parentModule && parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString() : blank;
    long seed = resolveLongDispStrArg(ds.getTagArg("bgl",4), parentModule, 1);
    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)seed));
    return TCL_OK;
}

int ModuleInspector::getLayoutSeed(int argc, const char **argv)
{
    if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}
    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)layoutSeed));
    return TCL_OK;
}

int ModuleInspector::setLayoutSeed(int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}
    layoutSeed = atol(argv[1]);
    return TCL_OK;
}

int ModuleInspector::getSubmoduleCount(int argc, const char **argv)
{
   if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}
   int count = 0;
   for (cModule::SubmoduleIterator submod(static_cast<cModule *>(object)); !submod.end(); submod++)
       count++;
   Tcl_SetObjResult(interp, Tcl_NewIntObj(count));
   return TCL_OK;
}

int ModuleInspector::getSubmodQ(int argc, const char **argv)
{
   // args: <module ptr> <qname>
   if (argc!=3) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   cModule *mod = dynamic_cast<cModule *>(strToPtr( argv[1] ));
   const char *qname = argv[2];
   cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname));
   char buf[21];
   ptrToStr(q,buf);
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}

int ModuleInspector::getSubmodQLen(int argc, const char **argv)
{
   // args: <module ptr> <qname>
   if (argc!=3) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   cModule *mod = dynamic_cast<cModule *>(strToPtr( argv[1] ));
   const char *qname = argv[2];
   cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname)); //FIXME THIS MUST BE REFINED! SEARCHES WAY TOO DEEEEEP!!!!
   if (!q) {Tcl_SetResult(interp, TCLCONST(""), TCL_STATIC); return TCL_OK;}
   Tcl_SetObjResult(interp, Tcl_NewIntObj(q->length()));
   return TCL_OK;
}


NAMESPACE_END

