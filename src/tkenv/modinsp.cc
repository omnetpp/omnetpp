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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "csimplemodule.h"
#include "cchannel.h"
#include "cgate.h"
#include "cmessage.h"
#include "cpar.h"
#include "carray.h"
#include "coutvect.h"
#include "cstat.h"
#include "cdensity.h"
#include "cdispstr.h"

#include "modinsp.h"
#include "tkapp.h"
#include "tklib.h"
#include "inspfactory.h"
#include "arrow.h"
#include "graphlayout.h"


#define UNKNOWNICON_WIDTH  32
#define UNKNOWNICON_HEIGHT 32


// Module state as text:
char *modstate[] = {
  "Ended",    // sENDED
  "Ready" ,   // sREADY
  ""
};


void _dummy_for_modinsp() {}

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
   Tcl_Interp *interp = getTkApplication()->getInterp();
   cModule *mod = static_cast<cModule *>(object);
   const char *createcommand = mod->isSimple() ?
            "create_simplemodulewindow " : "create_compoundmodulewindow ";
   CHK(Tcl_VarEval(interp, createcommand, windowname, " \"", geometry, "\"", NULL ));
}

void TModuleWindow::update()
{
   TInspector::update();

   Tcl_Interp *interp = getTkApplication()->getInterp();
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
   Tcl_Interp *interp = getTkApplication()->getInterp();
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

static cPar *displayStringPar(const char *parname, cModule *mod, bool searchparent)
{
   cPar *par = NULL;
   int k = mod->findPar(parname);
   if (k>=0)
      par = &(mod->par(k));

   if (!par && searchparent && mod->parentModule())
   {
      k = mod->parentModule()->findPar( parname );
      if (k>=0)
         par = &(mod->parentModule()->par(k));
   }
   if (!par)
   {
      if (!searchparent)
          throw new cRuntimeError("module `%s' has no parameter `%s'", mod->fullPath().c_str(), parname);
      else
          throw new cRuntimeError("module `%s' and its parent have no parameter `%s'", mod->fullPath().c_str(), parname);
   }
   return par;
}

static long resolveNumericDispStrArg(const char *s, cModule *mod, int defaultval)
{
   if (!s || !*s)
       return defaultval;
   if (*s=='$')
       return displayStringPar(s+1, mod, true)->longValue();
   return (long) atol(s);
}

void TGraphicalModWindow::relayoutAndRedrawAll()
{
   int submodcount = 0;
   int gatecountestimate = static_cast<cModule *>(object)->gates();
   for (cSubModIterator submod(*static_cast<cModule *>(object)); !submod.end(); submod++)
   {
       submodcount++;
       // note: gatecountestimate will count unconnected gates and "holes" in the gate array as well
       gatecountestimate += submod()->gates();
   }

   not_drawn = false;
   if (submodcount>1000 || gatecountestimate>4000)
   {
       Tcl_Interp *interp = getTkApplication()->getInterp();
       char problem[200];
       if (submodcount>1000)
           sprintf(problem,"contains more than 1000 submodules (exactly %d)", submodcount);
       else
           sprintf(problem,"may contain a lot of connections (modules have a large number of gates)");
       CHK(Tcl_VarEval(interp,"tk_messageBox -parent ",windowname," -type yesno -title Warning -icon question "
                              "-message {Module '", object->fullName(), "' ", problem,
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
    const cDisplayString& ds = submod->hasDisplayString() ? submod->displayString() : blank;

    // get size -- we'll need to return that too, and may be needed for matrix, ring etc. layout
    int boxsx=0, boxsy=0, iconsx=0, iconsy=0;
    if (ds.existsTag("b") || !ds.existsTag("i"))
    {
        boxsx = resolveNumericDispStrArg(ds.getTagArg("b",0), submod, 40);
        boxsy = resolveNumericDispStrArg(ds.getTagArg("b",1), submod, 24);
    }
    if (ds.existsTag("i"))
    {
        const char *imgname = ds.getTagArg("i",0);
        if (!imgname || !*imgname)
        {
            iconsx = UNKNOWNICON_WIDTH;
            iconsy = UNKNOWNICON_HEIGHT;
        }
        else
        {
            Tcl_Interp *interp = getTkApplication()->getInterp();
            Tk_Image img = Tk_GetImage(interp, Tk_MainWindow(interp), TCLCONST(imgname), NULL, NULL);
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
    x = resolveNumericDispStrArg(ds.getTagArg("p",0), submod, -1);
    y = resolveNumericDispStrArg(ds.getTagArg("p",1), submod, -1);
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
        int dx = resolveNumericDispStrArg(ds.getTagArg("p",3), submod, 0);
        int dy = resolveNumericDispStrArg(ds.getTagArg("p",4), submod, 0);
        x += dx;
        y += dy;
    }
    else if (!strcmp(layout,"r") || !strcmp(layout,"row"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        int dx = resolveNumericDispStrArg(ds.getTagArg("p",3), submod, 2*sx);
        x += submod->index()*dx;
    }
    else if (!strcmp(layout,"c") || !strcmp(layout,"col") || !strcmp(layout,"column"))
    {
        int dy = resolveNumericDispStrArg(ds.getTagArg("p",3), submod, 2*sy);
        y += submod->index()*dy;
    }
    else if (!strcmp(layout,"m") || !strcmp(layout,"matrix"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        int columns = resolveNumericDispStrArg(ds.getTagArg("p",3), submod, 5);
        int dx = resolveNumericDispStrArg(ds.getTagArg("p",4), submod, 2*sx);
        int dy = resolveNumericDispStrArg(ds.getTagArg("p",5), submod, 2*sy);
        x += (submod->index() % columns)*dx;
        y += (submod->index() / columns)*dy;
    }
    else if (!strcmp(layout,"i") || !strcmp(layout,"ri") || !strcmp(layout,"ring"))
    {
        // perhaps we should use the size of the 1st element in the vector?
        int rx = resolveNumericDispStrArg(ds.getTagArg("p",3), submod, (sx+sy)*submod->size()/4);
        int ry = resolveNumericDispStrArg(ds.getTagArg("p",4), submod, rx);

        x += (int) floor(rx - rx*sin(submod->index()*2*PI/submod->size()));
        y += (int) floor(ry - ry*cos(submod->index()*2*PI/submod->size()));
    }
    else
    {
        Tcl_Interp *interp = getTkApplication()->getInterp();
        CHK(Tcl_VarEval(interp,"messagebox {Error} "
                        "{Error: invalid layout `", layout, "' in `p' tag "
                        "of display string \"", ds.getString(), "\"} error ok", NULL));
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

    // create and configure layouter object
    BasicSpringEmbedderLayout layouter;
    layouter.setSeed(random_seed);

    // enable graphics only if full re-layouting (no cached coordinates in submodPosMap)
    if (submodPosMap.empty() && getTkApplication()->opt_showlayouting)
        layouter.setCanvas(getTkApplication()->getInterp(), canvas);

    // Note trick avoid calling backgroundDisplayString() directly because it'd cause
    // the display string object inside cModule to spring into existence
    const cDisplayString blank;
    const cDisplayString& ds = parentmodule->hasBackgroundDisplayString() ? parentmodule->backgroundDisplayString() : blank;
    int sx = resolveNumericDispStrArg(ds.getTagArg("b",0), parentmodule, 740);
    int sy = resolveNumericDispStrArg(ds.getTagArg("b",1), parentmodule, 500);
    layouter.setScaleToArea(sx,sy,50); // FIXME enclosing module's p= is ignored here...

    int repf = resolveNumericDispStrArg(ds.getTagArg("l",0), parentmodule, -1);
    if (repf>0) layouter.setRepulsiveForce(repf);

    int attf = resolveNumericDispStrArg(ds.getTagArg("l",1), parentmodule, -1);
    if (attf>0) layouter.setAttractionForce(attf);

    int edgelen = resolveNumericDispStrArg(ds.getTagArg("l",2), parentmodule, -1);
    if (edgelen>0) layouter.setDefaultEdgeLength(edgelen); // this should come before adding edges

    int maxiter = resolveNumericDispStrArg(ds.getTagArg("l",3), parentmodule, -1);
    if (maxiter>0) layouter.setMaxIterations(maxiter);

    int seed = resolveNumericDispStrArg(ds.getTagArg("l",4), parentmodule, -1);
    if (seed>0) layouter.setSeed(seed);

#ifdef USE_CONTRACTING_BOX
    layouter.boxContractionForce = resolveNumericDispStrArg(ds.getTagArg("bpars",0), parentmodule, 100);
    layouter.boxRepulsiveForce = resolveNumericDispStrArg(ds.getTagArg("bpars",1), parentmodule, 100);
    layouter.boxRepForceRatio = resolveNumericDispStrArg(ds.getTagArg("bpars",2), parentmodule, 1);
#endif

    cSubModIterator it(*parentmodule);
    bool parent;

    // loop through all submodules, get their sizes and positions and feed them into layouting engine
    for (it.init(*parentmodule); !it.end(); it++)
    {
        cModule *submod = it();

        bool explicitcoords, obeyslayout;
        int x, y, sx, sy;
        getSubmoduleCoords(submod, explicitcoords, obeyslayout, x, y, sx, sy);

        // add node into layouter:
        if (explicitcoords)
        {
            // e.g. "p=120,70" or "p=140,30,ring"
            layouter.addFixedNode(submod, x, y, sx, sy);
        }
        else if (submodPosMap.find(submod)!=submodPosMap.end())
        {
            // reuse coordinates from previous layout
            Point pos = submodPosMap[submod];
            layouter.addFixedNode(submod, pos.x, pos.y, sx, sy);
        }
        else if (obeyslayout)
        {
            // all modules are anchored to the anchor point with the vector's name
            // e.g. "p=,,ring"
            layouter.addAnchoredNode(submod, submod->name(), x, y, sx, sy);
        }
        else
        {
            layouter.addMovableNode(submod, sx, sy);
        }
    }

    // add connections into the layouter, too
    parent=false;
    for (it.init(*parentmodule); !parent; it++)
    {
        cModule *mod = !it.end() ? it() : (parent=true,parentmodule);

        int n = mod->gates();
        for (int i=0; i<n; i++)
        {
            cGate *gate = mod->gate(i);
            if (!gate) continue;

            cGate *destgate = gate->toGate();
            if (gate->type()==(parent?'I':'O') && destgate)
            {
                cModule *destmod = destgate->ownerModule();
                if (mod==parentmodule && destmod==parentmodule) {
                    // nop
                } else if (mod==parentmodule) {
                    layouter.addEdgeToBorder(destmod);
                } else if (destmod==parentmodule) {
                    layouter.addEdgeToBorder(mod);
                } else {
                    layouter.addEdge(mod,destmod);
                }
            }
        }
    }

    // layout the graph -- should be VERY fast if most nodes are fixed!
    layouter.execute();

    // fill the map with the results
    submodPosMap.clear();
    for (it.init(*parentmodule); !it.end(); it++)
    {
        cModule *submod = it();

        Point pos;
        layouter.getNodePosition(submod, pos.x, pos.y);
        submodPosMap[submod] = pos;
    }
}

// requires either recalculateLayout() or refreshLayout() called before!
void TGraphicalModWindow::redrawModules()
{
    cModule *parentmodule = static_cast<cModule *>(object);
    Tcl_Interp *interp = getTkApplication()->getInterp();

    // then display all submodules
    CHK(Tcl_VarEval(interp, canvas, " delete dx",NULL)); // NOT "delete all" because that'd remove "bubbles" too!
    const cDisplayString blank;
    cSubModIterator it(*parentmodule);
    for (it.init(*parentmodule); !it.end(); it++)
    {
        cModule *submod = it();
        const cDisplayString& ds = submod->hasDisplayString() ? submod->displayString() : blank;

        // call Tcl procedure to draw the submodule
        assert(submodPosMap.find(submod)!=submodPosMap.end());
        Point pos = submodPosMap[submod];
        char coords[32];
        sprintf(coords,"%d %d ", pos.x, pos.y);
        CHK(Tcl_VarEval(interp, "draw_submod ",
                        canvas, " ",
                        ptrToStr(submod), " ",
                        coords,
                        "{", submod->fullName(), "} ",
                        "{", ds.getString(), "} ",
                        NULL ));
    }

    // draw enclosing module
    const char *dispstr = parentmodule->hasBackgroundDisplayString() ? parentmodule->backgroundDisplayString().getString() : "";
    CHK(Tcl_VarEval(interp, "draw_enclosingmod ",
                       canvas, " ",
                       ptrToStr(parentmodule), " ",
                       "{", parentmodule->fullPath().c_str(), "} ",
                       "{", dispstr, "}",
                       NULL ));

    // loop through all submodules and enclosing module & draw their connections
    bool parent=false;
    for (it.init(*parentmodule); !parent; it++)
    {
        cModule *mod = !it.end() ? it() : (parent=true,parentmodule);

        int n = mod->gates();
        for (int i=0; i<n; i++)
        {
            cGate *gate = mod->gate(i);
            if (!gate) continue;

            cGate *dest_gate = gate->toGate();
            if (gate->type()==(parent?'I':'O') && dest_gate!=NULL)
            {
                char gateptr[32], srcptr[32], destptr[32], indices[32];
                ptrToStr(gate, gateptr);
                ptrToStr(mod, srcptr);
                ptrToStr(dest_gate->ownerModule(), destptr);
                sprintf(indices,"%d %d %d %d",
                        gate->index(), gate->size(),
                        dest_gate->index(), dest_gate->size());
                const char *dispstr = gate->hasDisplayString() ? gate->displayString().getString() : "";

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
   Tcl_Interp *interp = getTkApplication()->getInterp();

   // refresh & cleanup from prev. events
   CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", NULL));

   // this thingy is only needed if animation is going on
   if (!getTkApplication()->animating)
       return;

   // loop through all messages in the event queue and display them
   for (cMessageHeap::Iterator msg(simulation.msgQueue); !msg.end(); msg++)
   {
      char msgptr[32];
      ptrToStr(msg(),msgptr);

      cModule *arrivalmod = simulation.module( msg()->arrivalModuleId() );
      if (arrivalmod &&
          arrivalmod->parentModule()==static_cast<cModule *>(object) &&
          msg()->arrivalGateId()>=0)
      {
         cGate *arrivalGate = msg()->arrivalGate();

         // if arrivalgate is connected, msg arrived on a connection, otherwise via sendDirect()
         if (arrivalGate->fromGate())
         {
             cGate *gate = arrivalGate->fromGate();
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
   Tcl_Interp *interp = getTkApplication()->getInterp();
   cModule *mod = static_cast<cModule *>(object);

   // removing marker from previous event
   CHK(Tcl_VarEval(interp, canvas, " delete nexteventmarker", NULL));

   // this thingy is only needed if animation is going on
   if (!getTkApplication()->animating || !getTkApplication()->opt_nexteventmarkers)
       return;

   // if any parent of the module containing the next event is on this canvas, draw marker
   cModule *nextmod = simulation.guessNextModule();
   cModule *nextmodparent = nextmod;
   while (nextmodparent && nextmodparent->parentModule()!=mod)
       nextmodparent = nextmodparent->parentModule();
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
   Tcl_Interp *interp = getTkApplication()->getInterp();
   for (cSubModIterator submod(*static_cast<cModule *>(object)); !submod.end(); submod++)
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

void TGraphicalModWindow::connectionRemoved(cGate *srcgate)
{
   needs_redraw = true;
}

void TGraphicalModWindow::displayStringChanged(cModule *)
{
   needs_redraw = true;
}

void TGraphicalModWindow::backgroundDisplayStringChanged()
{
   needs_redraw = true;
}

void TGraphicalModWindow::displayStringChanged(cGate *)
{
   needs_redraw = true;
}

void TGraphicalModWindow::bubble(cModule *mod, const char *text)
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "graphmodwin_bubble ",canvas," ",ptrToStr(mod)," {",text,"}",NULL));
}

int TGraphicalModWindow::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<1) {Tcl_SetResult(interp, "wrong number of args", TCL_STATIC); return TCL_ERROR;}

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
   if (argc!=4) {Tcl_SetResult(interp, "wrong number of args", TCL_STATIC); return TCL_ERROR;}

   bool searchparents = atoi(argv[3])!=0;
   cModule *mod = dynamic_cast<cModule *>(strToPtr( argv[1] ));
   if (!mod) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   const char *parname = argv[2];

   cPar *par;
   TRY( par = displayStringPar(parname, mod, searchparents) );

   if (par->type()=='S')
   {
      Tcl_SetResult(interp, TCLCONST(par->stringValue()), TCL_VOLATILE);
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
   for (cSubModIterator submod(*static_cast<cModule *>(object)); !submod.end(); submod++)
       count++;
   char buf[20];
   sprintf(buf, "%d", count);
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}


int TGraphicalModWindow::getSubmodQ(Tcl_Interp *interp, int argc, const char **argv)
{
   // args: <module ptr> <qname>
   if (argc!=3) {Tcl_SetResult(interp, "wrong number of args", TCL_STATIC); return TCL_ERROR;}

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
   if (argc!=3) {Tcl_SetResult(interp, "wrong number of args", TCL_STATIC); return TCL_ERROR;}

   cModule *mod = dynamic_cast<cModule *>(strToPtr( argv[1] ));
   const char *qname = argv[2];
   cQueue *q = dynamic_cast<cQueue *>(mod->findObject(qname));
   if (!q) {Tcl_SetResult(interp, "", TCL_STATIC); return TCL_OK;}

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
//    Tcl_Interp *interp = getTkApplication()->getInterp();
//    CHK(Tcl_VarEval(interp, "create_compoundmodinspector ", windowname, " \"", geometry, "\"", NULL ));
// }
//
// void TCompoundModInspector::update()
// {
//    TInspector::update();
//
//    cCompoundModule *mod = static_cast<cCompoundModule *>(object);
//
//    //setToolbarInspectButton(".toolbar.parent", mod->parentModule(),INSP_DEFAULT);
//
//    setEntry(".nb.info.name.e", mod->name());
//    char id[16]; sprintf(id,"%ld", (long)mod->id());
//    setLabel(".nb.info.id.e", id);
//    setEntry(".nb.info.dispstr.e", mod->displayString());
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
//    mod->displayString().parse(getEntry(".nb.info.dispstr.e"));
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
//    Tcl_Interp *interp = getTkApplication()->getInterp();
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
//    setEntry(".nb.info.name.e", mod->name());
//    sprintf(buf,"%ld", (long)mod->id());
//    setLabel(".nb.info.id.e", buf);
//    setEntry(".nb.info.dispstr.e", mod->displayString());
//    setEntry(".nb.info.dispstrpt.e", mod->backgroundDisplayString());
//    setLabel(".nb.info.state.e",  modstate[ mod->moduleState() ]  );
//    if (mod->usesActivity())
//    {
//       unsigned stk = mod->stackSize();
//       unsigned extra = ev.extraStackForEnvir();
//       unsigned used = mod->stackUsage();
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
//    mod->displayString().parse(getEntry(".nb.info.dispstr.e"));
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
//    Tcl_Interp *interp = getTkApplication()->getInterp();
//    CHK(Tcl_VarEval(interp, "create_gateinspector ", windowname, " \"", geometry, "\"", NULL ));
// }
//
// void TGateInspector::update()
// {
//    TInspector::update();
//
//    cGate *g = static_cast<cGate *>(object);
//
//    setEntry(".nb.info.name.e", g->name());
//    char buf[64];
//    sprintf(buf,"#%d", g->id());
//    setLabel(".nb.info.id.e", buf);
//    setEntry(".nb.info.dispstr.e", g->displayString().getString());
//    cBasicChannel *ch = dynamic_cast<cBasicChannel*>(g->channel());
//    if (ch)
//    {
//        setEntry(".nb.info.delay.e", ch->delay());
//        setEntry(".nb.info.error.e", ch->error());
//        setEntry(".nb.info.datarate.e", ch->datarate());
//    }
//    else
//    {
//        setEntry(".nb.info.delay.e", 0.0);
//        setEntry(".nb.info.error.e", 0.0);
//        setEntry(".nb.info.datarate.e", 0.0);
//    }
//    setLabel(".nb.info.trfinish.e", g->transmissionFinishes());
//
//    setInspectButton(".nb.info.from", g->fromGate(), true, INSP_DEFAULT);
//    setInspectButton(".nb.info.to", g->toGate(), true, INSP_DEFAULT);
// }
//
// void TGateInspector::writeBack()
// {
//    cGate *g = static_cast<cGate *>(object);
//    g->setName(getEntry(".nb.info.name.e"));
//    g->displayString().parse(getEntry(".nb.info.dispstr.e"));
//    cBasicChannel *ch = dynamic_cast<cBasicChannel*>(g->channel());
//    double delay = atof(getEntry(".nb.info.delay.e"));
//    double error = atof(getEntry(".nb.info.error.e"));
//    double datarate = atof(getEntry(".nb.info.datarate.e"));
//    if (delay!=0 || error!=0 || datarate!=0 || ch!=NULL)
//    {
//        if (!ch)
//        {
//            ch = new cBasicChannel("channel");
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
   Tcl_Interp *interp = getTkApplication()->getInterp();
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
   for(g = gate->sourceGate(); g!=NULL; g=g->toGate(),k++)
   {
        if (g->type()==prevdir)
             xsiz += (g->type()=='O') ? 1 : -1;
        else
             prevdir = g->type();

        char modptr[32], gateptr[32], kstr[16], xstr[16], dir[2];
        ptrToStr(g->ownerModule(),modptr);
        ptrToStr(g,gateptr);
        sprintf(kstr,"%d",k);
        sprintf(xstr,"%d",xsiz);
        dir[0] = g->type(); dir[1]=0;
        CHK(Tcl_VarEval(interp, "draw_module_gate ",
                      canvas, " ",
                      modptr, " ",
                      gateptr, " ",
                      "{",g->ownerModule()->fullPath().c_str(), "} ",
                      "{",g->fullName(), "} ",
                      kstr," ",
                      xstr," ",
                      dir, " ",
                      g==gate?"1":"0",
                      NULL ));
   }

   // draw connections
   for (g = gate->sourceGate(); g->toGate()!=NULL; g=g->toGate())
   {
        cChannel *channel = g->channel();
        char srcgateptr[32], destgateptr[32];
        ptrToStr(g,srcgateptr);
        ptrToStr(g->toGate(),destgateptr);
        const char *dispstr = g->hasDisplayString() ? g->displayString().getString() : "";
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

   Tcl_Interp *interp = getTkApplication()->getInterp();
   cGate *gate = static_cast<cGate *>(object);

   // redraw modules only on explicit request

   // loop through all messages in the event queue
   CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", NULL));
   cGate *destgate = gate->destinationGate();
   for (cMessageHeap::Iterator msg(simulation.msgQueue); !msg.end(); msg++)
   {
      char gateptr[32], msgptr[32];
      ptrToStr(msg(),msgptr);

      if (msg()->arrivalGate()== destgate)
      {
         cGate *gate = msg()->arrivalGate();
         if (gate) gate = gate->fromGate();
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
   if (argc<1) {Tcl_SetResult(interp, "wrong number of args", TCL_STATIC); return TCL_ERROR;}

   // supported commands:
   //   redraw

   if (strcmp(argv[0],"redraw")==0)
   {
      return redraw(interp,argc,argv);
   }

   Tcl_SetResult(interp, "invalid arg: must be 'redraw'", TCL_STATIC);
   return TCL_ERROR;
}

void TGraphicalGateWindow::displayStringChanged(cGate *gate)
{
   //XXX should defer redraw (via redraw_needed) to avoid "flickering"
}


