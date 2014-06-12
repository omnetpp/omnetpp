//==========================================================================
//  GATEINSPECTOR.CC - part of
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

#include "gateinspector.h"
#include "tkenv.h"
#include "tklib.h"
#include "tkutil.h"
#include "inspectorfactory.h"
#include "cchannel.h"
#include "cgate.h"
#include "cdisplaystring.h"
#include "cchannel.h"


NAMESPACE_BEGIN

void _dummy_for_gateinspector() {}


class GateInspectorFactory : public InspectorFactory
{
  public:
    GateInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cGate *>(obj)!=NULL;}
    int getInspectorType() {return INSP_GRAPHICAL;}
    double getQualityAsDefault(cObject *object) {return 3.0;}
    Inspector *createInspector() {return new GateInspector(this);}
};

Register_InspectorFactory(GateInspectorFactory);


GateInspector::GateInspector(InspectorFactory *f) : Inspector(f)
{
}

void GateInspector::createWindow(const char *window, const char *geometry)
{
   Inspector::createWindow(window, geometry);

   strcpy(canvas,windowName);
   strcat(canvas,".c");

   CHK(Tcl_VarEval(interp, "createGateInspector ", windowName, " ", TclQuotedString(geometry).get(), NULL ));
}

void GateInspector::useWindow(const char *window)
{
   Inspector::useWindow(window);

   strcpy(canvas,windowName);
   strcat(canvas,".c");
}

void GateInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    redraw();
}

int GateInspector::redraw()
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
        CHK(Tcl_VarEval(interp, "GateInspector:drawModuleGate ",
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
        CHK(Tcl_VarEval(interp, "GateInspector:drawConnection ",
                      canvas, " ",
                      srcgateptr, " ",
                      destgateptr, " ",
                      chanptr, " ",
                      TclQuotedString(chan?chan->info().c_str():"").get(), " ",
                      TclQuotedString(dispstr).get(), " ",
                      NULL ));
   }

   // loop through all messages in the event queue
   refresh();

   return TCL_OK;
}

void GateInspector::refresh()
{
   Inspector::refresh();

   if (!object)
   {
       CHK(Tcl_VarEval(interp, canvas," delete all", NULL));
       return;
   }

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
             CHK(Tcl_VarEval(interp, "ModuleInspector:drawMessageOnGate ",
                             canvas, " ",
                             ptrToStr(gate,gateptr), " ",
                             msgptr,
                             NULL));
         }
      }
   }
}

int GateInspector::inspectorCommand(int argc, const char **argv)
{
   if (argc<1) {Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC); return TCL_ERROR;}

   if (strcmp(argv[0], "redraw")==0)
   {
      redraw();
      return TCL_OK;
   }

   return Inspector::inspectorCommand(argc, argv);
}

void GateInspector::displayStringChanged(cGate *gate)
{
   //XXX should defer redraw (via redraw_needed) to avoid "flickering"
}

NAMESPACE_END

