//==========================================================================
//   INSPECTOR.CC -
//            part of the Tcl/Tk environment of
//                             OMNeT++
//
//  Implementation of
//    inspectors
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <math.h>

#include "omnetpp.h"
#include "tkapp.h"
#include "tklib.h"
#include "inspector.h"

//
// Inspector type code <--> string conversions
//
struct InspTypeName {int code; char *namestr;} insp_typenames[] =
{
     { INSP_DEFAULT,          "(default)"     },
     { INSP_OBJECT,           "As Object"     },
     { INSP_GRAPHICAL,        "As Graphics"   },
     { INSP_MODULEOUTPUT,     "Module output" },
     { -1,                     NULL           }
};

const char *insptype_name_from_code(int code)
{
   for (int i=0; insp_typenames[i].namestr!=NULL; i++)
      if (insp_typenames[i].code == code)
         return insp_typenames[i].namestr;
   return NULL;
}

int insptype_code_from_name(const char *namestr)
{
   for (int i=0; insp_typenames[i].namestr!=NULL; i++)
      if (strcmp(insp_typenames[i].namestr, namestr)==0)
         return insp_typenames[i].code;
   return -1;
}

//=======================================================================
// TInspector: base class for all inspector types
//             member functions

TInspector::TInspector(cObject *obj, int typ, const char *geom, void *dat) : cObject()
{
   setOwner(NULL);

   object = obj;
   type = typ;
   data = dat;

   windowname[0] = '\0'; // no window exists
   windowtitle[0] = '\0';
   geometry[0] = '\0';
   if (geom) strcpy(geometry, geom);
}

TInspector::~TInspector()
{
   if (windowname[0])
   {
      Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
      CHK(Tcl_VarEval(interp, "destroy_inspector_toplevel ", windowname, NULL ));
   }
}

void TInspector::createWindow()
{
   windowname[0] = '.';
   ptrToStr( object, windowname+1 );
   sprintf( windowname+strlen(windowname), "-%d",type);

   // derived classes will also call Tcl_Eval() to actually create the
   // Tk window by invoking a procedure in inspect.tcl
}

bool TInspector::windowExists()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "winfo exists ", windowname, NULL ));
   return Tcl_GetStringResult(interp)[0]=='1';
}

void TInspector::showWindow()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "wm deiconify ", windowname, NULL ));
   CHK(Tcl_VarEval(interp, "raise ", windowname, NULL ));
   CHK(Tcl_VarEval(interp, "focus ", windowname, NULL ));
}

void TInspector::hostObjectDeleted()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "inspector_hostobjectdeleted ", windowname, NULL ));
}

void TInspector::update()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();

   // update window title (only if changed)
   //  (always updating the title produced many unnecessary redraws under fvwm2-95)
   char fullpath[300];
   object->fullPath(fullpath,300);
   int l = strlen(fullpath);
   char newtitle[128];
   if (l<=37)
       sprintf(newtitle, "(%.40s) %s",object->className(), fullpath);
   else
       sprintf(newtitle, "(%.40s) ...%s",object->className(), fullpath+l-35);
   if (strcmp(newtitle, windowtitle)!=0)
   {
       strcpy( windowtitle, newtitle);
       CHK(Tcl_VarEval(interp, "wm title ",windowname," {",windowtitle,"}",NULL));
   }

   // update object type and name info
   char newname[MAX_OBJECTFULLPATH+MAX_CLASSNAME+40];
   char buf[30];
   cModule *mod = dynamic_cast<cModule *>(object);
   if (mod)
       sprintf(newname, "(%s) %s  (id=%d)  (%s)",object->className(),
                        object->fullPath(), mod->id(), ptrToStr(object,buf));
   else
       sprintf(newname, "(%s) %s  (%s)",object->className(),
                        object->fullPath(), ptrToStr(object,buf));
   CHK(Tcl_VarEval(interp, windowname,".infobar.name config -text {",newname,"}",NULL));
}

void TInspector::setEntry(const char *entry, const char *val)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",val,"}",NULL));
}

void TInspector::setEntry( const char *entry, long l )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",buf,"}",NULL));
}

void TInspector::setEntry( const char *entry, double d )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   char buf[24];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",buf,"}",NULL));
}

void TInspector::setLabel( const char *label, const char *val )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",val,"}",NULL));
}

void TInspector::setLabel( const char *label, long l )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",buf,"}",NULL));
}

void TInspector::setLabel( const char *label, double d )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   char buf[16];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",buf,"}",NULL));
}

const char *TInspector::getEntry( const char *entry )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, windowname,entry," get",NULL));
   return Tcl_GetStringResult(interp);
}

void TInspector::setInspectButton(const char *button, cObject *object, bool displayfullpath, int inspectortype)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   if (object)
   {
      char buf[16];
      sprintf(buf, "%d", inspectortype);
      const char *text = displayfullpath ? object->fullPath() : object->name();
      char text2[30] = "";
      if (dynamic_cast<cModule *>(object))
      {
          sprintf(text2, " (id=%d)", ((cModule *)object)->id());
      }
      CHK(Tcl_VarEval(interp, windowname, button,".e config -state normal ",
                              "-text {", text, text2, "} ",
                              "-command {opp_inspect ",ptrToStr(object)," ",buf,"}",
                              NULL));
   }
   else
   {
      CHK(Tcl_VarEval(interp, windowname,button,".e config -state disabled -text {n/a}",NULL));
   }
}

void TInspector::setToolbarInspectButton(const char *button, cObject *object, int type)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   if (object)
   {
      char buf[16];
      sprintf(buf, "%d", type );
      CHK(Tcl_VarEval(interp, windowname,button," config -state normal -command"
                              " {opp_inspect ",ptrToStr(object)," ",buf,"}",NULL));
   }
   else
   {
      CHK(Tcl_VarEval(interp, windowname,button," config -state disabled",NULL));
   }
}

void TInspector::deleteInspectorListbox( const char *listbox)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "multicolumnlistbox_deleteall ", windowname,listbox,".main.list",NULL));
}

void TInspector::fillInspectorListbox(const char *listbox, cObject *object,
                             InfoFunc infofunc,bool deep)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowname,listbox);
   int n = fillListboxWithChildObjects(object, interp, w, infofunc, deep);

   // set "number of items" display
   sprintf(w, "%s.label", listbox);
   sprintf(buf,"%d objects", n);
   setLabel(w, buf);
}

void TInspector::fillModuleListbox(const char *listbox, cModule *parent,
                                InfoFunc infofunc,bool simpleonly,bool deep )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowname,listbox);
   int n = fillListboxWithChildModules(parent, interp, w, infofunc, simpleonly, deep);

   // set "number of items" display
   sprintf(w, "%s.label", listbox);
   sprintf(buf,"%d modules", n);
   setLabel(w, buf);
}

//=======================================================================

TInspectorPanel::TInspectorPanel(const char *widgetname, cObject *obj)
{
   strcpy(this->widgetname, widgetname);
   object = obj;
}


