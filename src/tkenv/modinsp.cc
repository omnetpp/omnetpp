//==========================================================================
//   MODINSP.CC -
//            part of the Tcl/Tk environment of
//                             OMNeT++
//
//  Implementation of
//    inspectors
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <math.h>

#include "cmodule.h"
#include "cmessage.h"
#include "cpar.h"
#include "carray.h"
#include "cnetmod.h"
#include "coutvect.h"
#include "cstat.h"
#include "cdensity.h"

#include "tkapp.h"
#include "tklib.h"
#include "tkinsp.h"
#include "arrow.h"

//=======================================================================

TModuleWindow::TModuleWindow(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TModuleWindow::createWindow()
{
   TInspector::createWindow(); // create window name etc.
   strcpy(modulename,windowname); strcat(modulename,".statusbar.name");
   strcpy(phase,windowname); strcat(phase,".statusbar.phase");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_modulewindow ", windowname, NULL ));
}

void TModuleWindow::update()
{
   TInspector::update();

   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   cSimpleModule *mod = (cSimpleModule *)object;

   setInspectButton(".toolbar.parent", mod->parentModule(),INSP_DEFAULT);

   char subsc[32];
   sprintf(subsc,"#%ld ",mod->id());
   CHK(Tcl_VarEval(interp, modulename, " config -text {Module ",
                           subsc,mod->fullPath(),"}", NULL));
   CHK(Tcl_VarEval(interp, phase, " config -text {Phase: ",mod->phase(),"}", NULL));
}

//=======================================================================

TGraphicalModWindow::TGraphicalModWindow(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TGraphicalModWindow::createWindow()
{
   TInspector::createWindow(); // create window name etc.
   strcpy(canvas,windowname); strcat(canvas,".c");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_graphicalmodwindow ", windowname, NULL ));
}

int TGraphicalModWindow::redraw(Tcl_Interp *interp, int, char **)
{
   bool w = simulation.warnings(); simulation.setWarnings(FALSE);

   // loop through all submodules and draw them
   for (cSubModIterator submod(*(cModule *)object); !submod.end(); submod++)
   {
      // If there's no explicit position given for a submodule vector, we'll
      // auto assing either row or ring auto layout.
      // We select row if only adjacent modules of the vector are connected;
      // otherwise, we select ring.

      // first, see if there's an explicit position ("p=" tag) given
      int p_tag_present;
      char *dstr = submod()->displayString(dispSUBMOD);
      if (dstr[0]==0)
         p_tag_present = 0;
      else if (dstr[0]=='p' && dstr[1]=='=')
         p_tag_present = 1;
      else
         p_tag_present = strstr(dstr,";p=")!=NULL;

      // decide about ring or row:
      int ringlayout = 0;

      if (!p_tag_present && submod()->size()>1)
      {
         char *name = submod()->name();
         for (cSubModIterator m(*(cModule *)object); !m.end(); m++)
         {
            if (strcmp(m()->name(),name)==0)
            {
               int n = m()->gates();
               for (int i=0; i<n; i++)
               {
                  cGate *gate = m()->gate(i);
                  if (!gate) continue;

                  cGate *dest_gate = gate->toGate();
                  if (gate->type()=='O' && dest_gate!=NULL)
                  {
                     cModule *dest_mod = dest_gate->ownerModule();
                     if (strcmp(dest_mod->name(),name)==0)
                     {
                        int diff = m()->index() - dest_mod->index();
                        if (diff>1 || diff<-1)
                        {
                            ringlayout = 1;
                        }
                     }
                  }
               }
            }
         }
      }

      // call Tcl procedure to draw the submodule
      char index[8];
      sprintf(index,"%d %d ", submod()->index(), submod()->size());
      CHK(Tcl_VarEval(interp, "draw_submod ",
                      canvas, " ",
                      ptrToStr( submod() ), " ",
                      "{", submod()->fullName(), "} ",
                      "{", submod()->displayString(dispSUBMOD), "} ",
                      index, ringlayout?"ring":"row",
                      NULL ));
   }

   // draw enclosing module
   CHK(Tcl_VarEval(interp, "draw_enclosingmod ",
                      canvas, " ",
                      ptrToStr( object ), " ",
                      "{", object->fullPath(), "} ",
                      "{", ((cModule *)object)->displayString( dispENCLOSINGMOD ), "}",
                      NULL ));

   // loop through all submodules and enclosing module & draw their connections
   int parent=0;
   for (cSubModIterator submod2(*(cModule *)object); !parent; submod2++)
   {
      cModule *mod = !submod2.end() ? submod2() : (parent=1,(cModule *)object);

      int n = mod->gates();
      for (int i=0; i<n; i++)
      {
         cGate *gate = mod->gate(i);
         if (!gate) continue;

         cGate *dest_gate = gate->toGate();
         if (gate->type()==(parent?'I':'O') && dest_gate!=NULL)
         {

            char gateptr[32], srcptr[32], destptr[32], indices[32];
            ptrToStr( gate, gateptr );
            ptrToStr( mod, srcptr );
            ptrToStr( dest_gate->ownerModule(), destptr );
            sprintf(indices,"%d %d %d %d",
                  gate->index(), gate->size(),
                  dest_gate->index(), dest_gate->size());

            CHK(Tcl_VarEval(interp, "draw_connection ",
                      canvas, " ",
                      gateptr, " ",
                      "{", gate->displayString(), "} ",
                      srcptr, " ",
                      destptr, " ",
                      indices,
                      NULL ));
         }
      }
   }

   // loop through all messages in the event queue
   update();

   simulation.setWarnings(w);

   return TCL_OK;
}

void TGraphicalModWindow::update()
{
   TInspector::update();

   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   cSimpleModule *mod = (cSimpleModule *)object;

   setInspectButton(".toolbar.parent", mod->parentModule(),INSP_DEFAULT);
   setInspectButton(".toolbar.params", &(mod->paramv),INSP_DEFAULT);
   setInspectButton(".toolbar.gates", &(mod->gatev),INSP_DEFAULT);

   // redraw modules only on explicit request

   // loop through all messages in the event queue
   CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", NULL));
   for (cMessageHeapIterator msg(simulation.msgQueue); !msg.end(); msg++)
   {
      char gateptr[32], msgptr[32], msgkind[16];
      ptrToStr(msg(),msgptr);
      sprintf(msgkind,"%d",msg()->kind());

      cModule *arrivalmod = simulation.module( msg()->arrivalModuleId() );
      if (arrivalmod &&
          arrivalmod->parentModule()==(cModule *)object &&
          msg()->arrivalGateId()>=0)
      {
         cGate *gate = msg()->arrivalGate();
         if (gate) gate = gate->fromGate();
         if (gate)
         {
             CHK(Tcl_VarEval(interp, "draw_message ",
                             canvas, " ",
                             ptrToStr(gate,gateptr), " ",
                             msgptr,
                             " {",msg()->fullName(),"} ",
                             msgkind,
                             NULL));
         }
      }
   }
}

int TGraphicalModWindow::inspectorCommand(Tcl_Interp *interp, int argc, char **argv)
{
   if (argc<1) {interp->result="wrong number of args"; return TCL_ERROR;}

   // supported commands:
   //   arrowcoords, redraw, modpar

   if (strcmp(argv[0],"arrowcoords")==0)
   {
      return arrowcoords(interp,argc,argv);
   }
   else if (strcmp(argv[0],"redraw")==0)
   {
      return redraw(interp,argc,argv);
   }
   else if (strcmp(argv[0],"modpar")==0)
   {
      // args: <module ptr> <parname>
      if (argc!=3) {interp->result="wrong number of args"; return TCL_ERROR;}

      cModule *mod = (cModule *)strToPtr( argv[1] );
      int parindex = mod->findPar( argv[2] );

      if (parindex<0)
      {
         sprintf(interp->result, "module '%s' has no '%s' parameter",
                 mod->fullPath(), argv[2]);
         return TCL_ERROR;
      }

      cPar& par = mod->par(parindex);
      if (par.type()=='S')
        interp->result = par.stringValue();
      else
        sprintf(interp->result, "%lg", par.doubleValue());

      return TCL_OK;
   }
   return TCL_ERROR;
}

//=======================================================================

TCompoundModInspector::TCompoundModInspector(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TCompoundModInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_compoundmodinspector ", windowname, NULL ));
}

void TCompoundModInspector::update()
{
   TInspector::update();

   cCompoundModule *mod = (cCompoundModule *)object;

   setInspectButton(".toolbar.parent", mod->parentModule(),INSP_DEFAULT);

   setLabel(".nb.info.name.e", mod->fullPath());
   char id[16]; sprintf(id,"#%ld", (long)mod->id());
   setLabel(".nb.info.id.e", id);

   deleteInspectorListbox( ".nb.submods" );
   fillModuleListbox(".nb.submods", mod, infofunc_module, FALSE, FALSE);

   deleteInspectorListbox( ".nb.params" );
   fillInspectorListbox(".nb.params", &mod->paramv, infofunc_infotext, FALSE);

   deleteInspectorListbox( ".nb.gates" );
   fillInspectorListbox(".nb.gates", &mod->gatev, infofunc_infotext, FALSE);

}

//=======================================================================
TSimpleModInspector::TSimpleModInspector(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TSimpleModInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_simplemodinspector ", windowname, NULL ));
}

void TSimpleModInspector::update()
{
   TInspector::update();

   cSimpleModule *mod = (cSimpleModule *)object;

   setInspectButton(".toolbar.parent", mod->parentModule(),INSP_DEFAULT);

   char buf[40];
   setLabel(".nb.info.name.e", mod->fullPath());
   sprintf(buf,"#%ld", (long)mod->id());
   setLabel(".nb.info.id.e", buf);
   setLabel(".nb.info.state.e",  modstate[ mod->moduleState() ]  );
   setLabel(".nb.info.phase.e", mod->phase() );
   if (mod->usesActivity())
   {
      unsigned stk = mod->stackSize();
      unsigned extra = ev.extraStackForEnvir();
      unsigned used = mod->stackUsage();
      sprintf(buf,"%u + %u = %u bytes", stk-extra, extra, stk);
      setLabel(".nb.info.stacksize.e", buf );
      sprintf(buf,"approx. %u bytes", used);
      setLabel(".nb.info.stackused.e", buf );
   }
   else
   {
      setLabel(".nb.info.stacksize.e", "n/a" );
      setLabel(".nb.info.stackused.e", "n/a" );
   }

   deleteInspectorListbox( ".nb.params" );
   fillInspectorListbox(".nb.params", &mod->paramv, infofunc_infotext, FALSE);

   deleteInspectorListbox( ".nb.gates" );
   fillInspectorListbox(".nb.gates", &mod->gatev, infofunc_infotext, FALSE);

   deleteInspectorListbox( ".nb.vars" );
   fillInspectorListbox(".nb.vars", &mod->members, infofunc_infotext, FALSE);
   fillInspectorListbox(".nb.vars", &mod->locals, infofunc_infotext, FALSE);

   deleteInspectorListbox( ".nb.paq" );
   fillInspectorListbox(".nb.paq", &mod->putAsideQueue, infofunc_infotext, FALSE);

   deleteInspectorListbox( ".nb.submods" );
   fillModuleListbox(".nb.submods", mod, infofunc_module, FALSE, FALSE);
}

//=======================================================================
TGateInspector::TGateInspector(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TGateInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_gateinspector ", windowname, NULL ));
}

void TGateInspector::update()
{
   TInspector::update();

   cGate *g= (cGate *)object;

   char buf[64];
   sprintf(buf,"#%d  %s", g->id(), g->fullName());
   setLabel(".main.name.e", buf);

   setLabel(".main.mod.e", g->ownerModule()->fullPath());

   if (g->delay()) setLabel(".main.delay.e", (double)(*g->delay()) );
              else setLabel(".main.delay.e", "none" );
   if (g->error()) setLabel(".main.error.e", (double)(*g->error()) );
              else setLabel(".main.error.e", "none" );
   if (g->datarate()) setLabel(".main.datarate.e", (double)(*g->datarate()) );
              else setLabel(".main.datarate.e", "none" );

   bool w = simulation.warnings(); simulation.setWarnings(FALSE);
   setLabel(".main.trfinish.e", g->transmissionFinishes() );

   cGate *gate;
   setInspectButton(".main.from",gate=g->fromGate(), INSP_DEFAULT);
   if (gate) sprintf(buf,"From: %.50s", gate->fullPath());
        else strcpy(buf,"(Path starts at this module)");
   setButtonText(".main.from", buf);
   setInspectButton(".main.to",gate=g->toGate(), INSP_DEFAULT);
   if (gate) sprintf(buf,"To: %.50s", gate->fullPath());
        else strcpy(buf,"(Path ends at this module)");
   setButtonText(".main.to", buf);

   setInspectButton(".toolbar.mod",g->ownerModule(), INSP_DEFAULT);

   simulation.setWarnings(w);
}

//=======================================================================

TGraphicalGateWindow::TGraphicalGateWindow(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TGraphicalGateWindow::createWindow()
{
   TInspector::createWindow(); // create window name etc.
   strcpy(canvas,windowname); strcat(canvas,".c");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_graphicalgatewindow ", windowname, NULL ));
}

int TGraphicalGateWindow::redraw(Tcl_Interp *interp, int, char **)
{
   cGate *gate = (cGate *)object;

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
                      "{",g->ownerModule()->fullPath(), "} ",
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
        char chan[128] = "";
        if (g->datarate())
        {
            strcat(chan,"datarate ");
            g->datarate()->getAsText(chan+strlen(chan),32);
            strcat(chan,"bps  ");
        }
        if (g->delay())
        {
            strcat(chan,"delay ");
            g->delay()->getAsText(chan+strlen(chan),32);
            strcat(chan,"s  ");
        }
        if (g->error())
        {
            strcat(chan,"error ");
            g->error()->getAsText(chan+strlen(chan),32);
            strcat(chan,"  ");
        }
        char srcgateptr[32], destgateptr[32];
        ptrToStr(g,srcgateptr);
        ptrToStr(g->toGate(),destgateptr);
        CHK(Tcl_VarEval(interp, "draw_conn ",
                      canvas, " ",
                      srcgateptr, " ",
                      destgateptr, " ",
                      "{",chan,"} ",
                      "{", g->displayString(),"} ",
                      NULL ));
   }

   // loop through all messages in the event queue
   update();

   return TCL_OK;
}

void TGraphicalGateWindow::update()
{
   TInspector::update();

   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   cGate *gate = (cGate *)object;

   setInspectButton(".toolbar.module", gate->ownerModule(),INSP_DEFAULT);

   // redraw modules only on explicit request

   // loop through all messages in the event queue
   CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", NULL));
   cGate *destgate = gate->destinationGate();
   for (cMessageHeapIterator msg(simulation.msgQueue); !msg.end(); msg++)
   {
      char gateptr[32], msgptr[32], msgkind[16];
      ptrToStr(msg(),msgptr);
      sprintf(msgkind,"%d",msg()->kind());

      if (msg()->arrivalGate()== destgate)
      {
         cGate *gate = msg()->arrivalGate();
         if (gate) gate = gate->fromGate();
         if (gate)
         {
             CHK(Tcl_VarEval(interp, "draw_message ",
                             canvas, " ",
                             ptrToStr(gate,gateptr), " ",
                             msgptr,
                             " {",msg()->fullName(),"} ",
                             msgkind,
                             NULL));
         }
      }
   }
}

int TGraphicalGateWindow::inspectorCommand(Tcl_Interp *interp, int argc, char **argv)
{
   if (argc<1) {interp->result="wrong number of args"; return TCL_ERROR;}

   // supported commands:
   //   redraw

   if (strcmp(argv[0],"redraw")==0)
   {
      return redraw(interp,argc,argv);
   }

   interp->result="invalid arg: must be 'redraw'";
   return TCL_ERROR;
}
