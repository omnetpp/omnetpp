//==========================================================================
//  INSPECTOR.CC - part of
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
#include <math.h>
#include <assert.h>

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

const char *insptypeNameFromCode(int code)
{
   for (int i=0; insp_typenames[i].namestr!=NULL; i++)
      if (insp_typenames[i].code == code)
         return insp_typenames[i].namestr;
   return NULL;
}

int insptypeCodeFromName(const char *namestr)
{
   for (int i=0; insp_typenames[i].namestr!=NULL; i++)
      if (strcmp(insp_typenames[i].namestr, namestr)==0)
         return insp_typenames[i].code;
   return -1;
}

//=================================================================
// a utility function:

void splitInspectorName(const char *namestr, cObject *&object, int& type)
{
   // namestr is the window path name, sth like ".ptr80005a31-2"
   // split it into pointer string ("ptr80005a31") and inspector type ("2")
   assert(namestr!=0); // must exist
   assert(namestr[0]=='.');  // must begin with a '.'

   // find '-' and temporarily replace it with EOS
   char *s;
   for (s=const_cast<char *>(namestr); *s!='-' && *s!='\0'; s++);
   assert(*s=='-');  // there must be a '-' in the string
   *s = '\0';

   object = dynamic_cast<cObject *>(strToPtr(namestr+1));
   type = atoi(s+1);
   *s = '-';  // restore '-'
}

//=======================================================================
// TInspector: base class for all inspector types
//             member functions

TInspector::TInspector(cObject *obj, int typ, const char *geom, void *dat)
{
   object = obj;
   type = typ;
   data = dat;
   toBeDeleted = false;

   windowname[0] = '\0'; // no window exists
   windowtitle[0] = '\0';
   geometry[0] = '\0';
   if (geom) strcpy(geometry, geom);
}

TInspector::~TInspector()
{
   if (windowname[0])
   {
      Tcl_Interp *interp = getTkApplication()->getInterp();
      CHK(Tcl_VarEval(interp, "destroy_inspector_toplevel ", windowname, NULL ));
   }
}

void TInspector::createWindow()
{
   windowname[0] = '.';
   ptrToStr(object, windowname+1 );
   sprintf(windowname+strlen(windowname), "-%d",type);

   // derived classes will also call Tcl_Eval() to actually create the
   // Tk window by invoking a procedure in inspect.tcl
}

bool TInspector::windowExists()
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "winfo exists ", windowname, NULL ));
   return Tcl_GetStringResult(interp)[0]=='1';
}

void TInspector::showWindow()
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "inspector_show ", windowname, NULL ));
}

void TInspector::hostObjectDeleted()
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "inspector_hostobjectdeleted ", windowname, NULL ));
}

void TInspector::update()
{
   Tcl_Interp *interp = getTkApplication()->getInterp();

   // update window title (only if changed)
   //  (always updating the title produces many unnecessary redraws under some window mgrs)
   char newtitle[128];
   const char *prefix = getTkApplication()->getWindowTitlePrefix();
   char fullpath[300];
   object->fullPath(fullpath,300);
   int len = strlen(fullpath);
   if (len<=45)
       sprintf(newtitle, "%s(%.40s) %s", prefix, object->className(), fullpath);
   else
       sprintf(newtitle, "%s(%.40s) ...%s", prefix, object->className(), fullpath+len-40);

   if (strcmp(newtitle, windowtitle)!=0)
   {
       strcpy(windowtitle, newtitle);
       CHK(Tcl_VarEval(interp, "wm title ",windowname," {",windowtitle,"}",NULL));
   }

   // update object type and name info
   char newname[MAX_OBJECTFULLPATH+MAX_CLASSNAME+40];
   char buf[30];
   cModule *mod = dynamic_cast<cModule *>(object);
   if (mod)
       sprintf(newname, "(%s) %s  (id=%d)  (%s)",object->className(),
                        object->fullPath().c_str(), mod->id(), ptrToStr(object,buf));
   else
       sprintf(newname, "(%s) %s  (%s)",object->className(),
                        object->fullPath().c_str(), ptrToStr(object,buf));
   CHK(Tcl_VarEval(interp, windowname,".infobar.name config -text {",newname,"}",NULL));

   // owner button on toolbar
   setToolbarInspectButton(".toolbar.owner",mod ? mod->parentModule() : object->owner(), INSP_DEFAULT);
}

void TInspector::setEntry(const char *entry, const char *val)
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   if (!val) val="";
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",val,"}",NULL));
}

void TInspector::setEntry( const char *entry, long l )
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",buf,"}",NULL));
}

void TInspector::setEntry( const char *entry, double d )
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   char buf[24];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",buf,"}",NULL));
}

void TInspector::setLabel( const char *label, const char *val )
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   if (!val) val="";
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",val,"}",NULL));
}

void TInspector::setLabel( const char *label, long l )
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",buf,"}",NULL));
}

void TInspector::setLabel( const char *label, double d )
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   char buf[16];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",buf,"}",NULL));
}

void TInspector::setText(const char *entry, const char *val)
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   if (!val) val="";
   CHK(Tcl_VarEval(interp, windowname,entry," delete 1.0 end", NULL));
   CHK(Tcl_VarEval(interp, windowname,entry," insert 1.0 {",val,"}",NULL));
}

void TInspector::setReadonlyText(const char *entry, const char *val)
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, windowname,entry," config -state normal", NULL));
   setText(entry, val);
   CHK(Tcl_VarEval(interp, windowname,entry," config -state disabled", NULL));
}

const char *TInspector::getEntry( const char *entry )
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, windowname,entry," get",NULL));
   return Tcl_GetStringResult(interp);
}

void TInspector::setInspectButton(const char *button, cObject *object, bool displayfullpath, int inspectortype)
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   if (object)
   {
      char buf[16];
      sprintf(buf, "%d", inspectortype);
      char idtext[30] = "";
      if (dynamic_cast<cModule *>(object))
      {
          sprintf(idtext, " (id=%d)", static_cast<cModule *>(object)->id());
      }
      CHK(Tcl_VarEval(interp, windowname, button,".e config -state normal ",
                              "-text {(", object->className(), ") ",
                              (displayfullpath ? object->fullPath().c_str() : object->fullName()),
                              idtext, "} ",
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
   Tcl_Interp *interp = getTkApplication()->getInterp();
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

void TInspector::deleteInspectorListbox(const char *listbox)
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "multicolumnlistbox_deleteall ", windowname,listbox,".main.list",NULL));
}

void TInspector::fillInspectorListbox(const char *listbox, cObject *object, bool deep)
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowname,listbox);
   int n = fillListboxWithChildObjects(object, interp, w, deep);

   // The following is needed because BLT tends to crash when item count goes
   // from 3 to 0 (e.g. in samples/fifo, Fast mode). Adding a dummy line when
   // listbox is empty solves the problem...
   if (n==0)
       CHK(Tcl_VarEval(interp, "multicolumnlistbox_adddummyline ", w, NULL));

   // set "number of items" display
   sprintf(w, "%s.label", listbox);
   sprintf(buf,"%d objects", n);
   setLabel(w, buf);
}

void TInspector::fillListboxWithSubmodules(const char *listbox, cModule *parent)
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowname,listbox);

   // feed into listbox
   int n=0;
   for (cSubModIterator submod(*parent); !submod.end(); submod++, n++)
        insertIntoInspectorListbox(interp, w, submod(), false);

   // set "number of items" display
   sprintf(w, "%s.label", listbox);
   sprintf(buf,"%d modules", n);
   setLabel(w, buf);
}

//=======================================================================

TInspectorPanel::TInspectorPanel(const char *widgetname, cPolymorphic *obj)
{
   strcpy(this->widgetname, widgetname);
   object = obj;
}

void TInspectorPanel::setObject(cPolymorphic *obj)
{
   object=obj;
}


