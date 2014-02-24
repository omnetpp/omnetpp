//==========================================================================
//  MODINSP.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Implementation of
//    inspectors
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

#include "modinsp.h"
#include "cchannel.h"
#include "tkenv.h"
#include "tklib.h"
#include "tkutil.h"
#include "inspfactory.h"
#include "arrow.h"
#include "graphlayouter.h"
#include "layouterenv.h"
#include "forcedirectedgraphlayouter.h"
#include "basicspringembedderlayout.h"
#include "stringtokenizer.h"

NAMESPACE_BEGIN

#define UNKNOWNICON_WIDTH  32
#define UNKNOWNICON_HEIGHT 32

void _dummy_for_modinsp() {}


class TModuleWindowFactory : public InspectorFactory
{
  public:
    TModuleWindowFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cModule *>(obj)!=NULL;}
    int getInspectorType() {return INSP_MODULEOUTPUT;}
    double getQualityAsDefault(cObject *object) {return 0.5;}

    Inspector *createInspector() {
        return prepare(new LogInspector());
    }
};

Register_InspectorFactory(TModuleWindowFactory);


LogInspector::LogInspector() : Inspector()
{
}

void LogInspector::createWindow(const char *window, const char *geometry)
{
    Inspector::createWindow(window, geometry);

    strcpy(textWidget,windowName);
    strcat(textWidget, ".main.text");

    Tcl_Interp *interp = getTkenv()->getInterp();
    cModule *mod = static_cast<cModule *>(object);
    const char *createcommand = mod->isSimple() ?
             "createSimpleModuleWindow " : "createCompoundModuleWindow ";
    CHK(Tcl_VarEval(interp, createcommand, windowName, " \"", geometry, "\"", NULL ));
    redisplay(getTkenv()->getLogBuffer());
}

void LogInspector::update()
{
    if (!object)
        return;

    Inspector::update();

    Tcl_Interp *interp = getTkenv()->getInterp();
    CHK(Tcl_VarEval(interp, "moduleWindow:trimlines ", windowName, NULL));
}

void LogInspector::printLastLineOf(const LogBuffer& logBuffer)
{
    printLastLineOf(getTkenv()->getInterp(), textWidget, logBuffer, excludedModuleIds);
}

void LogInspector::redisplay(const LogBuffer& logBuffer)
{
    redisplay(getTkenv()->getInterp(), textWidget, logBuffer, static_cast<cModule *>(object), excludedModuleIds);
}

void LogInspector::printLastLineOf(Tcl_Interp *interp, const char *textWidget, const LogBuffer& logBuffer, const std::set<int>& excludedModuleIds)
{
    const LogBuffer::Entry& entry = logBuffer.getEntries().back();
    if (!entry.moduleIds)
    {
        if (entry.lines.empty())
            textWidget_insert(interp, textWidget, entry.banner, "log");
        else
            textWidget_insert(interp, textWidget, entry.lines.back());
    }
    else if (excludedModuleIds.find(entry.moduleIds[0])==excludedModuleIds.end())
    {
        if (entry.lines.empty())
            textWidget_insert(interp, textWidget, entry.banner, "event");
        else
            textWidget_insert(interp, textWidget, entry.lines.back());
    }
    textWidget_gotoEnd(interp, textWidget);
}

void LogInspector::redisplay(Tcl_Interp *interp, const char *textWidget, const LogBuffer& logBuffer, cModule *mod, const std::set<int>& excludedModuleIds)
{
    textWidget_clear(interp, textWidget);

    if (!mod)
        return;

    int inspModuleId = mod->getId();
    const std::list<LogBuffer::Entry>& entries = logBuffer.getEntries();
    for (std::list<LogBuffer::Entry>::const_iterator it=entries.begin(); it!=entries.end(); it++)
    {
        const LogBuffer::Entry& entry = *it;
        if (!entry.moduleIds)
        {
            textWidget_insert(interp, textWidget, entry.banner, "log");
            for (int i=0; i<(int)entry.lines.size(); i++)
                textWidget_insert(interp, textWidget, entry.lines[i]); //?
        }
        else
        {
            // check that this module is covered in entry.moduleIds[] (module path up to the root)
            bool found = false;
            for (int *p = entry.moduleIds; !found && *p; p++)
                if (*p == inspModuleId)
                    found = true;

            // if so, and is not excluded, display log
            if (found && excludedModuleIds.find(entry.moduleIds[0])==excludedModuleIds.end())
            {
                textWidget_insert(interp, textWidget, entry.banner, "event");
                for (int i=0; i<(int)entry.lines.size(); i++)
                    textWidget_insert(interp, textWidget, entry.lines[i]);
            }
        }
    }
    textWidget_gotoEnd(interp, textWidget);
}

int LogInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc<1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

    // supported commands: redisplay, getexcludedmoduleids, setexcludedmoduleids

    if (strcmp(argv[0],"redisplay")==0)
    {
       if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       TRY(redisplay(getTkenv()->getLogBuffer()));
       return TCL_OK;
    }
    else if (strcmp(argv[0],"getexcludedmoduleids")==0)
    {
       if (argc!=1) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       Tcl_Obj *listobj = Tcl_NewListObj(0, NULL);
       for (std::set<int>::iterator it=excludedModuleIds.begin(); it!=excludedModuleIds.end(); it++)
           Tcl_ListObjAppendElement(interp, listobj, Tcl_NewIntObj(*it));
       Tcl_SetObjResult(interp, listobj);
       return TCL_OK;
    }
    else if (strcmp(argv[0],"setexcludedmoduleids")==0)
    {
       if (argc!=2) {Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC); return TCL_ERROR;}
       excludedModuleIds.clear();
       StringTokenizer tokenizer(argv[1]);
       while (tokenizer.hasMoreTokens())
           excludedModuleIds.insert(atoi(tokenizer.nextToken()));
       return TCL_OK;
    }
    return TCL_ERROR;
}

//=======================================================================

class TGraphicalModWindowFactory : public InspectorFactory
{
  public:
    TGraphicalModWindowFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cModule *>(obj)!=NULL;}
    int getInspectorType() {return INSP_GRAPHICAL;}
    double getQualityAsDefault(cObject *object) {
        return dynamic_cast<cSimpleModule *>(object) ? 0.9 : 3.0;
    }

    Inspector *createInspector() {
        return prepare(new ModuleInspector());
    }
};

Register_InspectorFactory(TGraphicalModWindowFactory);


ModuleInspector::ModuleInspector() : Inspector()
{
   needs_redraw = false;
   notDrawn = false;
   randomSeed = 0;
}

ModuleInspector::~ModuleInspector()
{
}

void ModuleInspector::setObject(cObject *obj)
{
    Inspector::setObject(obj);

    const cDisplayString blank;
    cModule *parentModule = static_cast<cModule *>(object);
    const cDisplayString& ds = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString() : blank;
    randomSeed = resolveLongDispStrArg(ds.getTagArg("bgl",4), parentModule, 1);
}

void ModuleInspector::createWindow(const char *window, const char *geometry)
{
   Inspector::createWindow(window, geometry);

   strcpy(canvas,windowName);
   strcat(canvas,".c");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "createGraphicalModWindow ", windowName, " \"", geometry, "\"", NULL ));
}

void ModuleInspector::useWindow(const char *widget)
{
    Inspector::useWindow(widget);
    strcpy(canvas,windowName);
    strcat(canvas,".c");
}

void ModuleInspector::update()
{
   if (!object)
       return;

   Inspector::update();

   if (notDrawn) return;

   // redraw modules only if really needed
   if (needs_redraw)
   {
       needs_redraw = false;
       redrawAll();
   }
   else
   {
       redrawNextEventMarker();
       redrawMessages();
       updateSubmodules();
   }
}

void ModuleInspector::relayoutAndRedrawAll()
{
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
       Tcl_Interp *interp = getTkenv()->getInterp();
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

   // go to next seed
   randomSeed++;
   recalculateLayout();
   redrawModules();
   redrawNextEventMarker();
   redrawMessages();
   updateSubmodules();
}

void ModuleInspector::redrawAll()
{
   refreshLayout();
   redrawModules();
   redrawNextEventMarker();
   redrawMessages();
   updateSubmodules();
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
            Tcl_Interp *interp = getTkenv()->getInterp();
            Tcl_VarEval(interp, "lookupImage ", imgName, " ", imgSize, NULL);
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
        Tcl_Interp *interp = getTkenv()->getInterp();
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

    layouter->setSeed(randomSeed);

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
    Tcl_Interp *interp = getTkenv()->getInterp();
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

    randomSeed = layouter->getSeed();

    delete layouter;
}

// requires either recalculateLayout() or refreshLayout() called before!
void ModuleInspector::redrawModules()
{
    cModule *parentModule = static_cast<cModule *>(object);
    Tcl_Interp *interp = getTkenv()->getInterp();

    // then display all submodules
    CHK(Tcl_VarEval(interp, canvas, " delete dx",NULL)); // NOT "delete all" because that'd remove "bubbles" too!
    const cDisplayString blank;
    std::string buffer;
    const char *rawScaling = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString().getTagArg("bgs",0) : "";
    const char *scaling = substituteDisplayStringParamRefs(rawScaling, buffer, parentModule, true);

    for (cModule::SubmoduleIterator it(parentModule); !it.end(); it++)
    {
        cModule *submod = it();
        assert(submodPosMap.find(submod)!=submodPosMap.end());
        Point& pos = submodPosMap[submod];
        drawSubmodule(interp, submod, pos.x, pos.y, scaling);
    }

    // draw enclosing module
    drawEnclosingModule(interp, parentModule, scaling);

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
                drawConnection(interp, gate);
            }
        }
    }
    CHK(Tcl_VarEval(interp, canvas, " raise bubble",NULL));
    CHK(Tcl_VarEval(interp, "graphicalModuleWindow:setScrollRegion ", windowName, " 0",NULL));
}

void ModuleInspector::drawSubmodule(Tcl_Interp *interp, cModule *submod, double x, double y, const char *scaling)
{
    char coords[64];
    sprintf(coords,"%g %g ", x, y);
    const char *dispstr = submod->hasDisplayString() && submod->parametersFinalized() ? submod->getDisplayString().str() : "";

    CHK(Tcl_VarEval(interp, "graphicalModuleWindow:drawSubmodule ",
                    canvas, " ",
                    ptrToStr(submod), " ",
                    coords,
                    "{", submod->getFullName(), "} ",
                    TclQuotedString(dispstr).get(), " ",
                    "{", scaling, "} ",
                    (submod->isPlaceholder() ? "1" : "0"),
                    NULL));
}

void ModuleInspector::drawEnclosingModule(Tcl_Interp *interp, cModule *parentModule, const char *scaling)
{
    const char *displayString = parentModule->hasDisplayString() && parentModule->parametersFinalized() ? parentModule->getDisplayString().str() : "";
    CHK(Tcl_VarEval(interp, "graphicalModuleWindow:drawEnclosingModule ",
                       canvas, " ",
                       ptrToStr(parentModule), " ",
                       "{", parentModule->getFullPath().c_str(), "} ",
                       TclQuotedString(displayString).get(), " ",
                       "{", scaling, "} ",
                       NULL ));
}

void ModuleInspector::drawConnection(Tcl_Interp *interp, cGate *gate)
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

    CHK(Tcl_VarEval(interp, "graphicalModuleWindow:drawConnection ",
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
   Tcl_Interp *interp = getTkenv()->getInterp();

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
             CHK(Tcl_VarEval(interp, "graphicalModuleWindow:drawMessageOnGate ",
                             canvas, " ",
                             ptrToStr(gate), " ",
                             msgptr,
                             NULL));
         }
         else
         {
             CHK(Tcl_VarEval(interp, "graphicalModuleWindow:drawMessageOnModule ",
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
   Tcl_Interp *interp = getTkenv()->getInterp();
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
       CHK(Tcl_VarEval(interp, "graphicalModuleWindow:drawNextEventMarker ",
                       canvas, " ",
                       ptrToStr(nextModParent), " ",
                       (nextMod==nextModParent ? "2" : "1"),
                       NULL));
   }
}

void ModuleInspector::updateSubmodules()
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   for (cModule::SubmoduleIterator submod(static_cast<cModule *>(object)); !submod.end(); submod++)
   {
       CHK(Tcl_VarEval(interp, "graphicalModuleWindow:updateSubmodule ",
                       canvas, " ",
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

void ModuleInspector::bubble(cModule *submod, const char *text)
{
    Tcl_Interp *interp = getTkenv()->getInterp();

    // if submod position is not yet known (because e.g. we're in fast mode
    // and it was dynamically created since the last update), refresh layout
    // so that we can get coordinates for it
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
    CHK(Tcl_VarEval(interp, "graphicalModuleWindow:bubble ", canvas, coords, " ", TclQuotedString(scaling).get(), " ", TclQuotedString(text).get(), NULL));
}

int ModuleInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   // supported commands:
   //   arrowcoords, relayout, etc...

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
   else if (strcmp(argv[0],"submodulecount")==0)
   {
      return getSubmoduleCount(interp,argc,argv);
   }
   else if (strcmp(argv[0],"getsubmodq")==0)
   {
      return getSubmodQ(interp,argc,argv);
   }
   else if (strcmp(argv[0],"getsubmodqlen")==0)
   {
      return getSubmodQLen(interp,argc,argv);
   }
   return TCL_ERROR;
}

int ModuleInspector::getSubmoduleCount(Tcl_Interp *interp, int argc, const char **argv)
{
   int count = 0;
   for (cModule::SubmoduleIterator submod(static_cast<cModule *>(object)); !submod.end(); submod++)
       count++;
   char buf[20];
   sprintf(buf, "%d", count);
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}

int ModuleInspector::getSubmodQ(Tcl_Interp *interp, int argc, const char **argv)
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

int ModuleInspector::getSubmodQLen(Tcl_Interp *interp, int argc, const char **argv)
{
   // args: <module ptr> <qname>
   if (argc!=3) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   cModule *mod = dynamic_cast<cModule *>(strToPtr( argv[1] ));
   const char *qname = argv[2];
   cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname)); //FIXME THIS MUST BE REFINED! SEARCHES WAY TOO DEEEEEP!!!!
   if (!q) {Tcl_SetResult(interp, TCLCONST(""), TCL_STATIC); return TCL_OK;}

   char buf[20];
   sprintf(buf, "%d", q->length());
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}


//=======================================================================
class TGraphicalGateWindowFactory : public InspectorFactory
{
  public:
    TGraphicalGateWindowFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cGate *>(obj)!=NULL;}
    int getInspectorType() {return INSP_GRAPHICAL;}
    double getQualityAsDefault(cObject *object) {return 3.0;}

    Inspector *createInspector() {
        return prepare(new GateInspector());
    }
};

Register_InspectorFactory(TGraphicalGateWindowFactory);


GateInspector::GateInspector() : Inspector()
{
}

void GateInspector::createWindow(const char *window, const char *geometry)
{
   Inspector::createWindow(window, geometry);

   strcpy(canvas,windowName);
   strcat(canvas,".c");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "createGraphicalGateWindow ", windowName, " \"", geometry, "\"", NULL ));
}

int GateInspector::redraw(Tcl_Interp *interp, int, const char **)
{
   cGate *gate = (cGate *)object;

   CHK(Tcl_VarEval(interp, canvas, " delete all",NULL));

   // draw modules
   int k = 0;
   int xsiz = 0;
   char prevdir = ' ';
   cGate *g;
   for (g = gate->getPathStartGate(); g!=NULL; g=g->getNextGate(),k++)
   {
        if (g->getType()==prevdir)
             xsiz += (g->getType()==cGate::OUTPUT) ? 1 : -1;
        else
             prevdir = g->getType();

        char modptr[32], gateptr[32], kstr[16], xstr[16], dir[2];
        ptrToStr(g->getOwnerModule(),modptr);
        ptrToStr(g,gateptr);
        sprintf(kstr,"%d",k);
        sprintf(xstr,"%d",xsiz);
        dir[0] = g->getType(); dir[1]=0;
        CHK(Tcl_VarEval(interp, "graphicalModuleWindow:drawModuleGate ",
                      canvas, " ",
                      modptr, " ",
                      gateptr, " ",
                      "{",g->getOwnerModule()->getFullPath().c_str(), "} ",
                      "{",g->getFullName(), "} ",
                      kstr," ",
                      xstr," ",
                      dir, " ",
                      g==gate?"1":"0",
                      NULL ));
   }

   // draw connections
   for (g = gate->getPathStartGate(); g->getNextGate()!=NULL; g=g->getNextGate())
   {
        char srcgateptr[32], destgateptr[32], chanptr[32];
        ptrToStr(g,srcgateptr);
        ptrToStr(g->getNextGate(),destgateptr);
        cChannel *chan = g->getChannel();
        ptrToStr(chan,chanptr);
        const char *dispstr = (chan && chan->hasDisplayString() && chan->parametersFinalized() ) ? chan->getDisplayString().str() : "";
        CHK(Tcl_VarEval(interp, "graphGateWin:drawConnection ",
                      canvas, " ",
                      srcgateptr, " ",
                      destgateptr, " ",
                      chanptr, " ",
                      TclQuotedString(chan?chan->info().c_str():"").get(), " ",
                      TclQuotedString(dispstr).get(), " ",
                      NULL ));
   }

   // loop through all messages in the event queue
   update();

   return TCL_OK;
}

void GateInspector::update()
{
   Inspector::update();

   Tcl_Interp *interp = getTkenv()->getInterp();
   cGate *gate = static_cast<cGate *>(object);

   // redraw modules only on explicit request

   // loop through all messages in the event queue
   CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", NULL));
   cGate *destGate = gate->getPathEndGate();
   for (cMessageHeap::Iterator msg(simulation.msgQueue); !msg.end(); msg++)
   {
      char gateptr[32], msgptr[32];
      ptrToStr(msg(),msgptr);

      if (msg()->getArrivalGate()== destGate)
      {
         cGate *gate = msg()->getArrivalGate();
         if (gate) gate = gate->getPreviousGate();
         if (gate)
         {
             CHK(Tcl_VarEval(interp, "graphicalModuleWindow:drawMessageOnGate ",
                             canvas, " ",
                             ptrToStr(gate,gateptr), " ",
                             msgptr,
                             NULL));
         }
      }
   }
}

int GateInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   // supported commands:
   //   redraw

   if (strcmp(argv[0],"redraw")==0)
   {
      return redraw(interp,argc,argv);
   }

   Tcl_SetResult(interp, TCLCONST("invalid arg: must be 'redraw'"), TCL_STATIC);
   return TCL_ERROR;
}

void GateInspector::displayStringChanged(cGate *gate)
{
   //XXX should defer redraw (via redraw_needed) to avoid "flickering"
}

NAMESPACE_END

