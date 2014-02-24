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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <math.h>
#include <assert.h>

#include "omnetpp.h"
#include "tkenv.h"
#include "tklib.h"
#include "inspector.h"
#include "stringutil.h"

NAMESPACE_BEGIN

//
// Inspector type code <--> string conversions
//
struct InspTypeName {int code; const char *namestr;} insp_typenames[] =
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


//----

Inspector::Inspector()
{
   object = NULL;
   type = -1;
   ownsWindow = false;
   toBeDeleted = false;

   windowname[0] = '\0'; // no window exists
   windowtitle[0] = '\0';
}

Inspector::~Inspector()
{
   if (ownsWindow && windowname[0])
   {
      Tcl_Interp *interp = getTkenv()->getInterp();
      CHK(Tcl_VarEval(interp, "inspector:destroy ", windowname, NULL ));
   }
}

std::string Inspector::makeWindowName()
{
   static int counter = 0;
   return opp_stringf(".inspector%d", counter++);
}

void Inspector::createWindow(const char *window, const char *geometry)
{
    strcpy(windowname, window);
    ownsWindow = true;
}

void Inspector::useWindow(const char *widget)
{
    strcpy(windowname, widget);
    windowtitle[0] = '\0';
    ownsWindow = false;
}

bool Inspector::windowExists()
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "winfo exists ", windowname, NULL )); //FIXME what if not toplevel?
   return Tcl_GetStringResult(interp)[0]=='1';
}

void Inspector::showWindow()
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "inspector:show ", windowname, NULL ));  //FIXME what if not toplevel?
}

void Inspector::hostObjectDeleted()
{
   //FIXME for embedded inspectors, call setObject(NULL) !!!!
   Tcl_Interp *interp = getTkenv()->getInterp(); //FIXME only if ownsWindow?
   CHK(Tcl_VarEval(interp, "inspector:hostObjectDeleted ", windowname, NULL )); //FIXME needed? this Tcl proc is currently empty!!!
}

void Inspector::update()
{
   //FIXME only if there is infobar and toolbar! (that is, !embedded; or hasToolbar && hasInfobar)
   Tcl_Interp *interp = getTkenv()->getInterp();

   // update window title (only if changed)
   //  (always updating the title produces many unnecessary redraws under some window mgrs)
   char newtitle[128];
   const char *prefix = getTkenv()->getWindowTitlePrefix();
   if (!object)
   {
       sprintf(newtitle, "%s n/a", prefix);
   }
   else
   {
       char fullpath[300];
       strncpy(fullpath, object->getFullPath().c_str(), 300);
       fullpath[299] = 0;
       int len = strlen(fullpath);
       if (len<=45)
           sprintf(newtitle, "%s(%.40s) %s", prefix, getObjectShortTypeName(object), fullpath);
       else
           sprintf(newtitle, "%s(%.40s) ...%s", prefix, getObjectShortTypeName(object), fullpath+len-40);
   }

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
       sprintf(newname, "(%s) %s  (id=%d)  (%s)", getObjectFullTypeName(object),
               object->getFullPath().c_str(), mod->getId(), ptrToStr(object,buf));
   else if (object)
       sprintf(newname, "(%s) %s  (%s)", getObjectFullTypeName(object),
               object->getFullPath().c_str(), ptrToStr(object,buf));
   else
       sprintf(newname, "n/a");
   CHK(Tcl_VarEval(interp, windowname,".infobar.name config -text {",newname,"}",NULL));

   // owner button on toolbar
   setToolbarInspectButton(".toolbar.owner", mod ? mod->getParentModule() : object ? object->getOwner() : NULL, INSP_DEFAULT);
}

void Inspector::setEntry(const char *entry, const char *val)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   if (!val) val="";
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",val,"}",NULL));
}

void Inspector::setEntry( const char *entry, long l )
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",buf,"}",NULL));
}

void Inspector::setEntry( const char *entry, double d )
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   char buf[24];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",buf,"}",NULL));
}

void Inspector::setLabel( const char *label, const char *val )
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   if (!val) val="";
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",val,"}",NULL));
}

void Inspector::setLabel( const char *label, long l )
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",buf,"}",NULL));
}

void Inspector::setLabel( const char *label, double d )
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   char buf[16];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",buf,"}",NULL));
}

void Inspector::setText(const char *entry, const char *val)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   if (!val) val="";
   CHK(Tcl_VarEval(interp, windowname,entry," delete 1.0 end", NULL));
   CHK(Tcl_VarEval(interp, windowname,entry," insert 1.0 {",val,"}",NULL));
}

void Inspector::setReadonlyText(const char *entry, const char *val)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, windowname,entry," config -state normal", NULL));
   setText(entry, val);
   CHK(Tcl_VarEval(interp, windowname,entry," config -state disabled", NULL));
}

const char *Inspector::getEntry( const char *entry )
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, windowname,entry," get",NULL));
   return Tcl_GetStringResult(interp);
}

void Inspector::setInspectButton(const char *button, cObject *object, bool displayfullpath, int inspectortype)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   if (object)
   {
      char buf[16];
      sprintf(buf, "%d", inspectortype);
      char idtext[30] = "";
      if (dynamic_cast<cModule *>(object))
      {
          sprintf(idtext, " (id=%d)", static_cast<cModule *>(object)->getId());
      }
      CHK(Tcl_VarEval(interp, windowname, button,".e config -state normal ",
                              "-text {(", getObjectShortTypeName(object), ") ",
                              (displayfullpath ? object->getFullPath().c_str() : object->getFullName()),
                              idtext, "} ",
                              "-command {opp_inspect ",ptrToStr(object)," ",buf,"}",
                              NULL));
   }
   else
   {
      CHK(Tcl_VarEval(interp, windowname,button,".e config -state disabled -text {n/a}",NULL));
   }
}

void Inspector::setToolbarInspectButton(const char *button, cObject *object, int type)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
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

void Inspector::deleteInspectorListbox(const char *listbox)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   CHK(Tcl_VarEval(interp, "multicolumnlistbox:deleteAll ", windowname,listbox,".main.list",NULL));
}

void Inspector::fillInspectorListbox(const char *listbox, cObject *object, bool deep)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowname,listbox);
   int n = fillListboxWithChildObjects(object, interp, w, deep);

   // The following is needed because BLT tends to crash when item count goes
   // from 3 to 0 (e.g. in samples/fifo, Fast mode). Adding a dummy line when
   // listbox is empty solves the problem...
   if (n==0)
       CHK(Tcl_VarEval(interp, "multicolumnlistbox:addDummyLine ", w, NULL));

   // set "number of items" display
   sprintf(w, "%s.label", listbox);
   sprintf(buf,"%d objects", n);
   setLabel(w, buf);
}

void Inspector::fillListboxWithSubmodules(const char *listbox, cModule *parent)
{
   Tcl_Interp *interp = getTkenv()->getInterp();
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowname,listbox);

   // feed into listbox
   int n = 0;
   for (cModule::SubmoduleIterator submod(parent); !submod.end(); submod++, n++)
        insertIntoInspectorListbox(interp, w, submod(), false);

   // set "number of items" display
   sprintf(w, "%s.label", listbox);
   sprintf(buf,"%d modules", n);
   setLabel(w, buf);
}

//=======================================================================

GenericObjectViewer::GenericObjectViewer(const char *widgetname, cObject *obj)
{
   strcpy(this->widgetname, widgetname);
   object = obj;
}

void GenericObjectViewer::setObject(cObject *obj)
{
   object=obj;
}

NAMESPACE_END

