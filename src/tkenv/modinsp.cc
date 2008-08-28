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
#include "inspfactory.h"
#include "arrow.h"
#include "graphlayouter.h"
#include "layouterenv.h"
#include "forcedirectedgraphlayouter.h"
#include "basicspringembedderlayout.h"

USING_NAMESPACE

#define UNKNOWNICON_WIDTH  32
#define UNKNOWNICON_HEIGHT 32

void _dummy_for_modinsp() {}

static cPar *displayStringPar(const char *parname, cModule *mod, bool searchparent)
{
   cPar *par = NULL;
   int k = mod->findPar(parname);
   if (k>=0)
      par = &(mod->par(k));

   if (!par && searchparent && mod->getParentModule())
   {
      k = mod->getParentModule()->findPar( parname );
      if (k>=0)
         par = &(mod->getParentModule()->par(k));
   }
   if (!par)
   {
      if (!searchparent)
          throw cRuntimeError("module `%s' has no parameter `%s'", mod->getFullPath().c_str(), parname);
      else
          throw cRuntimeError("module `%s' and its parent have no parameter `%s'", mod->getFullPath().c_str(), parname);
   }
   return par;
}

bool resolveBoolDispStrArg(const char *s, cModule *mod, bool defaultValue)
{
   if (!s || !*s)
       return defaultValue;
   if (*s=='$')
       return displayStringPar(s+1, mod, true)->boolValue();
   return !strcmp("1", s) || !strcmp("true", s);
}

long resolveLongDispStrArg(const char *s, cModule *mod, int defaultValue)
{
   if (!s || !*s)
       return defaultValue;
   if (*s=='$')
       return displayStringPar(s+1, mod, true)->longValue();
   return (long) atol(s);
}

double resolveDoubleDispStrArg(const char *s, cModule *mod, double defaultValue)
{
   if (!s || !*s)
       return defaultValue;
   if (*s=='$')
       return displayStringPar(s+1, mod, true)->doubleValue();
   return atof(s);
}

//=======================================================================

class TModuleWindowFactory : public cInspectorFactory
{
  public:
    TModuleWindowFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cModule *>(obj)!=NULL;}
    int inspectorType() {return INSP_MODULEOUTPUT;}
    double qualityAsDefault(cObject *object) {return 0.5;}

    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TModuleWindow(object, type, geom, data);
    }
};

Register_InspectorFactory(TModuleWindowFactory);


TModuleWindow::TModuleWindow(cObject *obj,int typ,const char *geom,void *dat) :
    TInspector(obj,typ,geom,dat)
{
}

void TModuleWindow::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkenv()->getInterp();
   cModule *mod = static_cast<cModule *>(object);
   const char *createcommand = mod->isSimple() ?
            "create_simplemodulewindow " : "create_compoundmodulewindow ";
   CHK(Tcl_VarEval(interp, createcommand, windowname, " \"", geometry, "\"", NULL ));
}

void TModuleWindow::update()
{
   TInspector::update();

   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "modulewindow_trimlines ", windowname, NULL));
}

//=======================================================================

class TGraphicalModWindowFactory : public cInspectorFactory
{
  public:
    TGraphicalModWindowFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cModule *>(obj)!=NULL;}
    int inspectorType() {return INSP_GRAPHICAL;}
    double qualityAsDefault(cObject *object) {
        return dynamic_cast<cSimpleModule *>(object) ? 0.9 : 3.0;
    }

    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TGraphicalModWindow(object, type, geom, data);
    }
};

Register_InspectorFactory(TGraphicalModWindowFactory);


TGraphicalModWindow::TGraphicalModWindow(cObject *obj,int typ,const char *geom,void *dat) :
    TInspector(obj,typ,geom,dat)
{
   needs_redraw = false;
   not_drawn = false;
   random_seed = 1;
}

TGraphicalModWindow::~TGraphicalModWindow()
{
}

void TGraphicalModWindow::createWindow()
{
   TInspector::createWindow(); // create window name etc.
   strcpy(canvas,windowname); strcat(canvas,".c");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "create_graphicalmodwindow ", windowname, " \"", geometry, "\"", NULL ));
}

void TGraphicalModWindow::update()
{
   TInspector::update();

   if (not_drawn) return;

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

void TGraphicalModWindow::relayoutAndRedrawAll()
{
   cModule *mod = (cModule *)object;
   int submodcount = 0;
   int gatecountestimate = mod->gateCount();
   for (cModule::SubmoduleIterator submod(mod); !submod.end(); submod++)
   {
       submodcount++;
       // note: gatecountestimate will count unconnected gates in the gate array as well
       gatecountestimate += submod()->gateCount();
   }

   not_drawn = false;
   if (submodcount>1000 || gatecountestimate>4000)
   {
       Tcl_Interp *interp = getTkenv()->getInterp();
       char problem[200];
       if (submodcount>1000)
           sprintf(problem, "contains more than 1000 submodules (exactly %d)", submodcount);
       else
           sprintf(problem, "may contain a lot of connections (modules have a large number of gates)");
       CHK(Tcl_VarEval(interp,"tk_messageBox -parent ",windowname," -type yesno -title Warning -icon question "
                              "-message {Module '", object->getFullName(), "' ", problem,
                              ", it may take a long time to display the graphics. "
                              "Do you want to proceed with drawing?}", NULL));
       bool answer = (Tcl_GetStringResult(interp)[0]=='y');
       if (answer==false)
       {
           not_drawn = true;
           CHK(Tcl_VarEval(interp, canvas, " delete all",NULL)); // this must be done, still
           return;
       }
   }

   // go to next seed
   random_seed++;
   recalculateLayout();
   redrawModules();
   redrawNextEventMarker();
   redrawMessages();
   updateSubmodules();
}

void TGraphicalModWindow::redrawAll()
{
   refreshLayout();
   redrawModules();
   redrawNextEventMarker();
   redrawMessages();
   updateSubmodules();
}

void TGraphicalModWindow::getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeyslayout,
                                                              int& x, int& y, int& sx, int& sy)
{
    const cDisplayString blank;
    const cDisplayString& ds = submod->hasDisplayString() ? submod->getDisplayString() : blank;

    // get size -- we'll need to return that too, and may be needed for matrix, ring etc. layout
    int boxsx=0, boxsy=0, iconsx=0, iconsy=0;
    if (ds.containsTag("b") || !ds.containsTag("i"))
    {
        boxsx = resolveLongDispStrArg(ds.getTagArg("b",0), submod, 40);
        boxsy = resolveLongDispStrArg(ds.getTagArg("b",1), submod, 24);
    }
    if (ds.containsTag("i"))
    {
        const char *imgname = ds.getTagArg("i",0);
        const char *imgsize = ds.getTagArg("is",0);
        if (!imgname || !*imgname)
        {
            iconsx = UNKNOWNICON_WIDTH;
            iconsy = UNKNOWNICON_HEIGHT;
        }
        else
        {
            Tcl_Interp *interp = getTkenv()->getInterp();
            Tcl_VarEval(interp, "lookup_image ", imgname, " ", imgsize, NULL);
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
    x = resolveLongDispStrArg(ds.getTagArg("p",0), submod, -1);
    y = resolveLongDispStrArg(ds.getTagArg("p",1), submod, -1);
    explicitcoords = x!=-1 && y!=-1;

    // set missing coordinates to zero
    if (x==-1) x = 0;
    if (y==-1) y = 0;

    const char *layout = ds.getTagArg("p",2); // matrix, row, column, ring, exact etc.
    obeyslayout = (layout && *layout);

    // modify x,y using predefined layouts
    if (!layout || !*layout)
    {
        // we're happy
    }
    else if (!strcmp(layout,"e") || !strcmp(layout,"x") || !strcmp(layout,"exact"))
    {
        int dx = resolveLongDispStrArg(ds.getTagArg("p",3), submod, 0);
        int dy = resolveLongDispStrArg(ds.getTagArg("p",4), submod, 0);
        x += dx;
        y += dy;
    }
    else if (!strcmp(layout,"r") || !strcmp(layout,"row"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        int dx = resolveLongDispStrArg(ds.getTagArg("p",3), submod, 2*sx);
        x += submod->getIndex()*dx;
    }
    else if (!strcmp(layout,"c") || !strcmp(layout,"col") || !strcmp(layout,"column"))
    {
        int dy = resolveLongDispStrArg(ds.getTagArg("p",3), submod, 2*sy);
        y += submod->getIndex()*dy;
    }
    else if (!strcmp(layout,"m") || !strcmp(layout,"matrix"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        int columns = resolveLongDispStrArg(ds.getTagArg("p",3), submod, 5);
        int dx = resolveLongDispStrArg(ds.getTagArg("p",4), submod, 2*sx);
        int dy = resolveLongDispStrArg(ds.getTagArg("p",5), submod, 2*sy);
        x += (submod->getIndex() % columns)*dx;
        y += (submod->getIndex() / columns)*dy;
    }
    else if (!strcmp(layout,"i") || !strcmp(layout,"ri") || !strcmp(layout,"ring"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        int rx = resolveLongDispStrArg(ds.getTagArg("p",3), submod, (sx+sy)*submod->size()/4);
        int ry = resolveLongDispStrArg(ds.getTagArg("p",4), submod, rx);

        x += (int) floor(rx - rx*sin(submod->getIndex()*2*PI/submod->size()));
        y += (int) floor(ry - ry*cos(submod->getIndex()*2*PI/submod->size()));
    }
    else
    {
        Tcl_Interp *interp = getTkenv()->getInterp();
        CHK(Tcl_VarEval(interp,"messagebox {Error} "
                        "{Error: invalid layout `", layout, "' in `p' tag "
                        "of display string \"", ds.str(), "\"} error ok", NULL));
    }
}

void TGraphicalModWindow::recalculateLayout()
{
    // refresh layout with empty submodPosMap -- everything layouted
    submodPosMap.clear();
    refreshLayout();
}

void TGraphicalModWindow::refreshLayout()
{
    // recalculate layout, using coordinates in submodPosMap as "fixed" nodes --
    // only new nodes are re-layouted

    cModule *parentmodule = static_cast<cModule *>(object);

    // Note trick avoid calling getDisplayString() directly because it'd cause
    // the display string object inside cModule to spring into existence
    const cDisplayString blank;
    const cDisplayString& ds = parentmodule->hasDisplayString() ? parentmodule->getDisplayString() : blank;

    // create and configure layouter object
    GraphLayouter *layouter = getTkenv()->opt_usenewlayouter ?
                                    (GraphLayouter *) new ForceDirectedGraphLayouter() :
                                    (GraphLayouter *) new BasicSpringEmbedderLayout();

    int32 seed = resolveLongDispStrArg(ds.getTagArg("bgl",4), parentmodule, random_seed);
    layouter->setSeed(seed);

    Tcl_Interp *interp = getTkenv()->getInterp();

    TGraphLayouterEnvironment environment(parentmodule, ds);
    environment.setInterpreter(interp);

    // enable graphics only if full re-layouting (no cached coordinates in submodPosMap)
    if (submodPosMap.empty() && getTkenv()->opt_showlayouting)
        environment.setCanvas(canvas);
    layouter->setEnvironment(&environment);

    // size
    int sx = resolveLongDispStrArg(ds.getTagArg("bgb",0), parentmodule, 740);
    int sy = resolveLongDispStrArg(ds.getTagArg("bgb",1), parentmodule, 500);
    layouter->setScaleToArea(sx,sy,50); // FIXME position "bgp" is ignored here...

    // loop through all submodules, get their sizes and positions and feed them into layouting engine
    for (cModule::SubmoduleIterator it(parentmodule); !it.end(); it++)
    {
        cModule *submod = it();

        bool explicitcoords, obeyslayout;
        int x, y, sx, sy;
        getSubmoduleCoords(submod, explicitcoords, obeyslayout, x, y, sx, sy);

        // add node into layouter:
        if (explicitcoords)
        {
            // e.g. "p=120,70" or "p=140,30,ring"
            layouter->addFixedNode(submod, x, y, sx, sy);
        }
        else if (submodPosMap.find(submod)!=submodPosMap.end())
        {
            // reuse coordinates from previous layout
            Point pos = submodPosMap[submod];
            layouter->addFixedNode(submod, pos.x, pos.y, sx, sy);
        }
        else if (obeyslayout)
        {
            // all modules are anchored to the anchor point with the vector's name
            // e.g. "p=,,ring"
            layouter->addAnchoredNode(submod, submod->getName(), x, y, sx, sy);
        }
        else
        {
            layouter->addMovableNode(submod, sx, sy);
        }
    }

    // add connections into the layouter, too
    bool parent = false;
    for (cModule::SubmoduleIterator it(parentmodule); !parent; it++)
    {
        cModule *mod = !it.end() ? it() : (parent=true,parentmodule);

        for (cModule::GateIterator i(mod); !i.end(); i++)
        {
            cGate *gate = i();
            cGate *destgate = gate->getNextGate();
            if (gate->getType()==(parent ? cGate::INPUT : cGate::OUTPUT) && destgate)
            {
                cModule *destmod = destgate->getOwnerModule();
                if (mod==parentmodule && destmod==parentmodule) {
                    // nop
                } else if (destmod==parentmodule) {
                    layouter->addEdgeToBorder(mod);
                } else if (destmod->getParentModule()!=parentmodule) {
                    // connection goes to a module under a different parent!
                    // this in fact violates module encapsulation, but let's
                    // accept it nevertheless. Just skip this connection.
                } else if (mod==parentmodule) {
                    layouter->addEdgeToBorder(destmod);
                } else {
                    layouter->addEdge(mod,destmod);
                }
            }
        }
    }

    bool isFullLayout = submodPosMap.empty();
    if (isFullLayout) {
        Tcl_SetVar(interp, "stoplayouting", "0", TCL_GLOBAL_ONLY);
        Tcl_VarEval(interp, "layouter_startgrab ", windowName(), ".toolbar.stop", NULL);
    }
    // layout the graph -- should be VERY fast if most nodes are fixed!
    layouter->execute();

    if (isFullLayout) {
        Tcl_VarEval(interp, "layouter_releasegrab ", windowName(), ".toolbar.stop", NULL);
    }

    // fill the map with the results
    submodPosMap.clear();
    for (cModule::SubmoduleIterator it(parentmodule); !it.end(); it++)
    {
        cModule *submod = it();

        Point pos;
        layouter->getNodePosition(submod, pos.x, pos.y);
        submodPosMap[submod] = pos;
    }

    environment.cleanup();

    delete layouter;
}

// requires either recalculateLayout() or refreshLayout() called before!
void TGraphicalModWindow::redrawModules()
{
    cModule *parentmodule = static_cast<cModule *>(object);
    Tcl_Interp *interp = getTkenv()->getInterp();

    // then display all submodules
    CHK(Tcl_VarEval(interp, canvas, " delete dx",NULL)); // NOT "delete all" because that'd remove "bubbles" too!
    const cDisplayString blank;
    const char *scaling = parentmodule->hasDisplayString() ? parentmodule->getDisplayString().getTagArg("bgs",0) : "";
    for (cModule::SubmoduleIterator it(parentmodule); !it.end(); it++)
    {
        cModule *submod = it();
        const cDisplayString& ds = submod->hasDisplayString() ? submod->getDisplayString() : blank;

        // call Tcl procedure to draw the submodule
        assert(submodPosMap.find(submod)!=submodPosMap.end());
        Point pos = submodPosMap[submod];
        char coords[32];
        sprintf(coords,"%d %d ", pos.x, pos.y);
        CHK(Tcl_VarEval(interp, "draw_submod ",
                        canvas, " ",
                        ptrToStr(submod), " ",
                        coords,
                        "{", submod->getFullName(), "} ",
                        "{", ds.str(), "} ",
                        "{", scaling, "} ",
                        NULL ));
    }

    // draw enclosing module
    const char *dispstr = parentmodule->hasDisplayString() ? parentmodule->getDisplayString().str() : "";
    CHK(Tcl_VarEval(interp, "draw_enclosingmod ",
                       canvas, " ",
                       ptrToStr(parentmodule), " ",
                       "{", parentmodule->getFullPath().c_str(), "} ",
                       "{", dispstr, "} ",
                       "{", scaling, "} ",
                       NULL ));

    // loop through all submodules and enclosing module & draw their connections
    bool parent = false;
    for (cModule::SubmoduleIterator it(parentmodule); !parent; it++)
    {
        cModule *mod = !it.end() ? it() : (parent=true,parentmodule);

        for (cModule::GateIterator i(mod); !i.end(); i++)
        {
            cGate *gate = i();
            cGate *dest_gate = gate->getNextGate();
            if (gate->getType()==(parent ? cGate::INPUT: cGate::OUTPUT) && dest_gate!=NULL)
            {
                char gateptr[32], srcptr[32], destptr[32], indices[32];
                ptrToStr(gate, gateptr);
                ptrToStr(mod, srcptr);
                ptrToStr(dest_gate->getOwnerModule(), destptr);
                sprintf(indices,"%d %d %d %d",
                        gate->getIndex(), gate->size(),
                        dest_gate->getIndex(), dest_gate->size());
                cChannel *chan = gate->getChannel();
                const char *dispstr = (chan && chan->hasDisplayString()) ? chan->getDisplayString().str() : "";

                CHK(Tcl_VarEval(interp, "draw_connection ",
                        canvas, " ",
                        gateptr, " ",
                        "{", dispstr, "} ",
                        srcptr, " ",
                        destptr, " ",
                        indices,
                        NULL ));
           }
        }
    }
    CHK(Tcl_VarEval(interp, canvas, " raise bubble",NULL));
}

void TGraphicalModWindow::redrawMessages()
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

      cModule *arrivalmod = simulation.getModule( msg()->getArrivalModuleId() );
      if (arrivalmod &&
          arrivalmod->getParentModule()==static_cast<cModule *>(object) &&
          msg()->getArrivalGateId()>=0)
      {
         cGate *arrivalGate = msg()->getArrivalGate();

         // if arrivalgate is connected, msg arrived on a connection, otherwise via sendDirect()
         if (arrivalGate->getPreviousGate())
         {
             cGate *gate = arrivalGate->getPreviousGate();
             CHK(Tcl_VarEval(interp, "graphmodwin_draw_message_on_gate ",
                             canvas, " ",
                             ptrToStr(gate), " ",
                             msgptr,
                             NULL));
         }
         else
         {
             CHK(Tcl_VarEval(interp, "graphmodwin_draw_message_on_module ",
                             canvas, " ",
                             ptrToStr(arrivalmod), " ",
                             msgptr,
                             NULL));
         }
      }
   }
   CHK(Tcl_VarEval(interp, canvas, " raise bubble",NULL));
}

void TGraphicalModWindow::redrawNextEventMarker()
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   cModule *mod = static_cast<cModule *>(object);

   // removing marker from previous event
   CHK(Tcl_VarEval(interp, canvas, " delete nexteventmarker", NULL));

   // this thingy is only needed if animation is going on
   if (!getTkenv()->animating || !getTkenv()->opt_nexteventmarkers)
       return;

   // if any parent of the module containing the next event is on this canvas, draw marker
   cModule *nextmod = simulation.guessNextModule();
   cModule *nextmodparent = nextmod;
   while (nextmodparent && nextmodparent->getParentModule()!=mod)
       nextmodparent = nextmodparent->getParentModule();
   if (nextmodparent)
   {
       CHK(Tcl_VarEval(interp, "graphmodwin_draw_nexteventmarker ",
                       canvas, " ",
                       ptrToStr(nextmodparent), " ",
                       (nextmod==nextmodparent ? "2" : "1"),
                       NULL));
   }
}

void TGraphicalModWindow::updateSubmodules()
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   for (cModule::SubmoduleIterator submod(static_cast<cModule *>(object)); !submod.end(); submod++)
   {
       CHK(Tcl_VarEval(interp, "graphmodwin_update_submod ",
                       canvas, " ",
                       ptrToStr(submod()),
                       NULL));
   }
}


void TGraphicalModWindow::submoduleCreated(cModule *newmodule)
{
   needs_redraw = true;
}

void TGraphicalModWindow::submoduleDeleted(cModule *module)
{
   needs_redraw = true;
}

void TGraphicalModWindow::connectionCreated(cGate *srcgate)
{
   needs_redraw = true;
}

void TGraphicalModWindow::connectionDeleted(cGate *srcgate)
{
   needs_redraw = true;
}

void TGraphicalModWindow::displayStringChanged(cModule *)
{
   needs_redraw = true;
}

void TGraphicalModWindow::displayStringChanged()
{
   needs_redraw = true; //TODO check, probably only non-background tags have changed...
}

void TGraphicalModWindow::displayStringChanged(cGate *)
{
   needs_redraw = true;
}

void TGraphicalModWindow::bubble(cModule *mod, const char *text)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "graphmodwin_bubble ",canvas," ",ptrToStr(mod)," {",text,"}",NULL));
}

int TGraphicalModWindow::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   // supported commands:
   //   arrowcoords, relayout, etc...

   if (strcmp(argv[0],"arrowcoords")==0)
   {
      return ::arrowcoords(interp,argc,argv);
   }
   else if (strcmp(argv[0],"relayout")==0)
   {
      TRY( relayoutAndRedrawAll() );
      return TCL_OK;
   }
   else if (strcmp(argv[0],"redraw")==0)
   {
      TRY( redrawAll() );
      return TCL_OK;
   }
   else if (strcmp(argv[0],"dispstrpar")==0)
   {
      return getDisplayStringPar(interp,argc,argv);
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

int TGraphicalModWindow::getDisplayStringPar(Tcl_Interp *interp, int argc, const char **argv)
{
   // args: <module ptr> <parname> <searchparent>
   if (argc!=4) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   bool searchparents = atoi(argv[3])!=0;
   cModule *mod = dynamic_cast<cModule *>(strToPtr( argv[1] ));
   if (!mod) {Tcl_SetResult(interp, TCLCONST("null or malformed pointer"), TCL_STATIC); return TCL_ERROR;}

   const char *parname = argv[2];

   cPar *par;
   TRY( par = displayStringPar(parname, mod, searchparents) );

   if (par->getType()=='S')
   {
      Tcl_SetResult(interp, TCLCONST(par->stdstringValue().c_str()), TCL_VOLATILE);
   }
   else
   {
      char buf[30];
      sprintf(buf, "%g", par->doubleValue());
      Tcl_SetResult(interp, buf, TCL_VOLATILE);
   }
   return TCL_OK;
}

int TGraphicalModWindow::getSubmoduleCount(Tcl_Interp *interp, int argc, const char **argv)
{
   int count = 0;
   for (cModule::SubmoduleIterator submod(static_cast<cModule *>(object)); !submod.end(); submod++)
       count++;
   char buf[20];
   sprintf(buf, "%d", count);
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}


int TGraphicalModWindow::getSubmodQ(Tcl_Interp *interp, int argc, const char **argv)
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

int TGraphicalModWindow::getSubmodQLen(Tcl_Interp *interp, int argc, const char **argv)
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

//
// class TCompoundModInspectorFactory : public cInspectorFactory
// {
//   public:
//     TCompoundModInspectorFactory(const char *name) : cInspectorFactory(name) {}
//
//     bool supportsObject(cObject *obj) {return dynamic_cast<cModule *>(obj)!=NULL;}
//     int inspectorType() {return INSP_OBJECT;}
//     double qualityAsDefault(cObject *object) {return 2.9;}
//
//     TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
//         return new TCompoundModInspector(object, type, geom, data);
//     }
// };
//
// Register_InspectorFactory(TCompoundModInspectorFactory);
//
//
// TCompoundModInspector::TCompoundModInspector(cObject *obj,int typ,const char *geom,void *dat) :
//     TInspector(obj,typ,geom,dat)
// {
// }
//
// void TCompoundModInspector::createWindow()
// {
//    TInspector::createWindow(); // create window name etc.
//
//    // create inspector window by calling the specified proc with
//    // the object's pointer. Window name will be like ".ptr80003a9d-1"
//    Tcl_Interp *interp = getTkenv()->getInterp();
//    CHK(Tcl_VarEval(interp, "create_compoundmodinspector ", windowname, " \"", geometry, "\"", NULL ));
// }
//
// void TCompoundModInspector::update()
// {
//    TInspector::update();
//
//    cCompoundModule *mod = static_cast<cCompoundModule *>(object);
//
//    //setToolbarInspectButton(".toolbar.parent", mod->getParentModule(),INSP_DEFAULT);
//
//    setEntry(".nb.info.name.e", mod->getName());
//    char id[16]; sprintf(id,"%ld", (long)mod->getId());
//    setLabel(".nb.info.id.e", id);
//    setEntry(".nb.info.dispstr.e", mod->getDisplayString());
//    setEntry(".nb.info.dispstrpt.e", mod->backgroundDisplayString());
//
//    deleteInspectorListbox(".nb.contents");
//    fillInspectorListbox(".nb.contents", mod, false);
// }
//
// void TCompoundModInspector::writeBack()
// {
//    cCompoundModule *mod = static_cast<cCompoundModule *>(object);
//    mod->setName(getEntry(".nb.info.name.e"));
//    mod->getDisplayString().parse(getEntry(".nb.info.dispstr.e"));
//    mod->backgroundDisplayString().parse(getEntry(".nb.info.dispstrpt.e"));
//
//    TInspector::writeBack();    // must be there after all changes
// }
//

//=======================================================================

//
// class TSimpleModInspectorFactory : public cInspectorFactory
// {
//   public:
//     TSimpleModInspectorFactory(const char *name) : cInspectorFactory(name) {}
//
//     bool supportsObject(cObject *obj) {return dynamic_cast<cSimpleModule *>(obj)!=NULL;}
//     int inspectorType() {return INSP_OBJECT;}
//     double qualityAsDefault(cObject *object) {return 4.0;}
//
//     TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
//         return new TSimpleModInspector(object, type, geom, data);
//     }
// };
//
// Register_InspectorFactory(TSimpleModInspectorFactory);
//
//
// TSimpleModInspector::TSimpleModInspector(cObject *obj,int typ,const char *geom,void *dat) :
//     TInspector(obj,typ,geom,dat)
// {
// }
//
// void TSimpleModInspector::createWindow()
// {
//    TInspector::createWindow(); // create window name etc.
//
//    // create inspector window by calling the specified proc with
//    // the object's pointer. Window name will be like ".ptr80003a9d-1"
//    Tcl_Interp *interp = getTkenv()->getInterp();
//    CHK(Tcl_VarEval(interp, "create_simplemodinspector ", windowname, " \"", geometry, "\"", NULL ));
// }
//
// void TSimpleModInspector::update()
// {
//    TInspector::update();
//
//    cSimpleModule *mod = static_cast<cSimpleModule *>(object);
//
//    char buf[40];
//    setEntry(".nb.info.name.e", mod->getName());
//    sprintf(buf,"%ld", (long)mod->getId());
//    setLabel(".nb.info.id.e", buf);
//    setEntry(".nb.info.dispstr.e", mod->getDisplayString());
//    setEntry(".nb.info.dispstrpt.e", mod->backgroundDisplayString());
//    setLabel(".nb.info.state.e",  modstate[ mod->moduleState() ]  );
//    if (mod->usesActivity())
//    {
//       unsigned stk = mod->getStackSize();
//       unsigned extra = ev.getExtraStackForEnvir();
//       unsigned used = mod->getStackUsage();
//       sprintf(buf,"%u + %u = %u bytes", stk-extra, extra, stk);
//       setLabel(".nb.info.stacksize.e", buf );
//       sprintf(buf,"approx. %u bytes", used);
//       setLabel(".nb.info.stackused.e", buf );
//    }
//    else
//    {
//       setLabel(".nb.info.stacksize.e", "n/a" );
//       setLabel(".nb.info.stackused.e", "n/a" );
//    }
//
//    deleteInspectorListbox(".nb.contents");
//    fillInspectorListbox(".nb.contents", mod, false);
// }
//
// void TSimpleModInspector::writeBack()
// {
//    cSimpleModule *mod = static_cast<cSimpleModule *>(object);
//    mod->setName(getEntry(".nb.info.name.e"));
//    mod->getDisplayString().parse(getEntry(".nb.info.dispstr.e"));
//    mod->backgroundDisplayString().parse(getEntry(".nb.info.dispstrpt.e"));
//
//    TInspector::writeBack();    // must be there after all changes
// }
//

//=======================================================================

//
// class TGateInspectorFactory : public cInspectorFactory
// {
//   public:
//     TGateInspectorFactory(const char *name) : cInspectorFactory(name) {}
//
//     bool supportsObject(cObject *obj) {return dynamic_cast<cGate *>(obj)!=NULL;}
//     int inspectorType() {return INSP_OBJECT;}
//     double qualityAsDefault(cObject *object) {return 2.9;}
//
//     TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
//         return new TGateInspector(object, type, geom, data);
//     }
// };
//
// Register_InspectorFactory(TGateInspectorFactory);
//
// TGateInspector::TGateInspector(cObject *obj,int typ,const char *geom,void *dat) :
//     TInspector(obj,typ,geom,dat)
// {
// }
//
// void TGateInspector::createWindow()
// {
//    TInspector::createWindow(); // create window name etc.
//
//    // create inspector window by calling the specified proc with
//    // the object's pointer. Window name will be like ".ptr80003a9d-1"
//    Tcl_Interp *interp = getTkenv()->getInterp();
//    CHK(Tcl_VarEval(interp, "create_gateinspector ", windowname, " \"", geometry, "\"", NULL ));
// }
//
// void TGateInspector::update()
// {
//    TInspector::update();
//
//    cGate *g = static_cast<cGate *>(object);
//
//    setEntry(".nb.info.name.e", g->getName());
//    char buf[64];
//    sprintf(buf,"#%d", g->getId());
//    setLabel(".nb.info.id.e", buf);
//    setEntry(".nb.info.dispstr.e", g->getDisplayString().str());
//    cDatarateChannel *ch = dynamic_cast<cDatarateChannel*>(g->getChannel());
//    if (ch)
//    {
//        setEntry(".nb.info.delay.e", ch->getDelay());
//        setEntry(".nb.info.error.e", ch->getError());
//        setEntry(".nb.info.datarate.e", ch->getDatarate());
//    }
//    else
//    {
//        setEntry(".nb.info.delay.e", 0.0);
//        setEntry(".nb.info.error.e", 0.0);
//        setEntry(".nb.info.datarate.e", 0.0);
//    }
//    setLabel(".nb.info.trfinish.e", g->getTransmissionFinishTime());
//
//    setInspectButton(".nb.info.from", g->getPreviousGate(), true, INSP_DEFAULT);
//    setInspectButton(".nb.info.to", g->getNextGate(), true, INSP_DEFAULT);
// }
//
// void TGateInspector::writeBack()
// {
//    cGate *g = static_cast<cGate *>(object);
//    g->setName(getEntry(".nb.info.name.e"));
//    g->getDisplayString().parse(getEntry(".nb.info.dispstr.e"));
//    cDatarateChannel *ch = dynamic_cast<cDatarateChannel*>(g->getChannel());
//    double delay = atof(getEntry(".nb.info.delay.e"));
//    double error = atof(getEntry(".nb.info.error.e"));
//    double datarate = atof(getEntry(".nb.info.datarate.e"));
//    if (delay!=0 || error!=0 || datarate!=0 || ch!=NULL)
//    {
//        if (!ch)
//        {
//            ch = new cDatarateChannel("channel");
//            g->setChannel(ch);
//        }
//        ch->setDelay(delay<0 ? 0 : delay);
//        ch->setError(error<0 ? 0 : error>1 ? 1 : error);
//        ch->setDatarate(datarate<0 ? 0 : datarate);
//    }
//
//    TInspector::writeBack();    // must be there after all changes
// }
//

//=======================================================================
class TGraphicalGateWindowFactory : public cInspectorFactory
{
  public:
    TGraphicalGateWindowFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cGate *>(obj)!=NULL;}
    int inspectorType() {return INSP_GRAPHICAL;}
    double qualityAsDefault(cObject *object) {return 3.0;}

    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TGraphicalGateWindow(object, type, geom, data);
    }
};

Register_InspectorFactory(TGraphicalGateWindowFactory);


TGraphicalGateWindow::TGraphicalGateWindow(cObject *obj,int typ,const char *geom,void *dat) :
    TInspector(obj,typ,geom,dat)
{
}

void TGraphicalGateWindow::createWindow()
{
   TInspector::createWindow(); // create window name etc.
   strcpy(canvas,windowname); strcat(canvas,".c");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "create_graphicalgatewindow ", windowname, " \"", geometry, "\"", NULL ));
}

int TGraphicalGateWindow::redraw(Tcl_Interp *interp, int, const char **)
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
        CHK(Tcl_VarEval(interp, "draw_module_gate ",
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
        cChannel *channel = g->getChannel();
        char srcgateptr[32], destgateptr[32];
        ptrToStr(g,srcgateptr);
        ptrToStr(g->getNextGate(),destgateptr);
        cChannel *chan = g->getChannel();
        const char *dispstr = (chan && chan->hasDisplayString()) ? chan->getDisplayString().str() : "";
        CHK(Tcl_VarEval(interp, "draw_conn ",
                      canvas, " ",
                      srcgateptr, " ",
                      destgateptr, " ",
                      "{", (channel?channel->info().c_str():""), "} ",
                      "{", dispstr,"} ",
                      NULL ));
   }

   // loop through all messages in the event queue
   update();

   return TCL_OK;
}

void TGraphicalGateWindow::update()
{
   TInspector::update();

   Tcl_Interp *interp = getTkenv()->getInterp();
   cGate *gate = static_cast<cGate *>(object);

   // redraw modules only on explicit request

   // loop through all messages in the event queue
   CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", NULL));
   cGate *destgate = gate->getPathEndGate();
   for (cMessageHeap::Iterator msg(simulation.msgQueue); !msg.end(); msg++)
   {
      char gateptr[32], msgptr[32];
      ptrToStr(msg(),msgptr);

      if (msg()->getArrivalGate()== destgate)
      {
         cGate *gate = msg()->getArrivalGate();
         if (gate) gate = gate->getPreviousGate();
         if (gate)
         {
             CHK(Tcl_VarEval(interp, "graphmodwin_draw_message_on_gate ",
                             canvas, " ",
                             ptrToStr(gate,gateptr), " ",
                             msgptr,
                             NULL));
         }
      }
   }
}

int TGraphicalGateWindow::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
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

void TGraphicalGateWindow::displayStringChanged(cGate *gate)
{
   //XXX should defer redraw (via redraw_needed) to avoid "flickering"
}



