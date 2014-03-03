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

Inspector::Inspector(int t)
{
   interp = getTkenv()->getInterp();
   object = NULL;
   type = t;
   isToplevelWindow = false;
   closeRequested = false;

   windowName[0] = '\0'; // no window exists
}

Inspector::~Inspector()
{
   if (isToplevelWindow && windowName[0])
   {
      CHK(Tcl_VarEval(interp, "inspector:destroy ", windowName, NULL ));
   }
}

std::string Inspector::makeWindowName()
{
   static int counter = 0;
   return opp_stringf(".inspector%d", counter++);
}

void Inspector::createWindow(const char *window, const char *geometry)
{
    strcpy(windowName, window);
    isToplevelWindow = true;
}

void Inspector::useWindow(const char *window)
{
    strcpy(windowName, window);
    windowTitle = "";
    isToplevelWindow = false;
}

void Inspector::setObject(cObject *obj)
{
    ASSERT2(windowName[0], "createWindow()/useWindow() needs to be called before setObject()");

    if (obj != object)
    {
        object = obj;
        CHK(Tcl_VarEval(interp, "inspector:onSetObject ", windowName, NULL));
        refresh();
    }
}

void Inspector::showWindow()
{
    if (isToplevelWindow)
        CHK(Tcl_VarEval(interp, "inspector:show ", windowName, NULL ));
}

void Inspector::hostObjectDeleted()
{
    if (isToplevelWindow)
        CHK(Tcl_VarEval(interp, "inspector:hostObjectDeleted ", windowName, NULL )); //FIXME needed? this Tcl proc is currently empty!!!
    else
        setObject(NULL);
}

void Inspector::refresh()
{
    if (isToplevelWindow)
    {
        refreshTitle();
        refreshInfobar();
    }

    // owner button on toolbar
    cModule *mod = dynamic_cast<cModule *>(object);
    setToolbarInspectButton(".toolbar.owner", mod ? mod->getParentModule() : object ? object->getOwner() : NULL, INSP_DEFAULT);
}

void Inspector::refreshTitle()
{
    // update window title (only if changed -- always updating the title produces
    // unnecessary redraws under some window managers
    char newTitle[256];
    const char *prefix = getTkenv()->getWindowTitlePrefix();
    if (!object)
    {
        sprintf(newTitle, "%s n/a", prefix);
    }
    else
    {
        std::string fullPath = object->getFullPath();
        if (fullPath.length()<=60)
            sprintf(newTitle, "%s(%.40s) %s", prefix, getObjectShortTypeName(object), fullPath.c_str());
        else
            sprintf(newTitle, "%s(%.40s) ...%s", prefix, getObjectShortTypeName(object), fullPath.c_str()+fullPath.length()-55);
    }

    if (windowTitle != newTitle)
    {
        windowTitle = newTitle;
        CHK(Tcl_VarEval(interp, "wm title ",windowName," {",windowTitle.c_str(),"}",NULL));
    }
}

void Inspector::refreshInfobar()
{
    char newName[MAX_OBJECTFULLPATH+MAX_CLASSNAME+40]; //TODO std::string!
    cModule *mod = dynamic_cast<cModule *>(object);
    if (mod)
        sprintf(newName, "(%s) %s  (id=%d)", getObjectFullTypeName(object),
                object->getFullPath().c_str(), mod->getId());
    else if (object)
        sprintf(newName, "(%s) %s", getObjectFullTypeName(object),
                object->getFullPath().c_str());
    else
        sprintf(newName, "n/a");
    CHK(Tcl_VarEval(interp, windowName,".infobar.name config -text {",newName,"}",NULL));
}

void Inspector::setEntry(const char *entry, const char *val)
{
    if (!val) val="";
    CHK(Tcl_VarEval(interp, windowName,entry," delete 0 end;",
            windowName,entry," insert 0 {",val,"}",NULL));
}

void Inspector::setEntry( const char *entry, long l )
{
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowName,entry," delete 0 end;",
                           windowName,entry," insert 0 {",buf,"}",NULL));
}

void Inspector::setEntry( const char *entry, double d )
{
   char buf[24];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowName,entry," delete 0 end;",
                           windowName,entry," insert 0 {",buf,"}",NULL));
}

void Inspector::setLabel( const char *label, const char *val )
{
   if (!val) val="";
   CHK(Tcl_VarEval(interp, windowName,label," config -text {",val,"}",NULL));
}

void Inspector::setLabel( const char *label, long l )
{
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowName,label," config -text {",buf,"}",NULL));
}

void Inspector::setLabel( const char *label, double d )
{
   char buf[16];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowName,label," config -text {",buf,"}",NULL));
}

void Inspector::setText(const char *entry, const char *val)
{
   if (!val) val="";
   CHK(Tcl_VarEval(interp, windowName,entry," delete 1.0 end", NULL));
   CHK(Tcl_VarEval(interp, windowName,entry," insert 1.0 {",val,"}",NULL));
}

void Inspector::setReadonlyText(const char *entry, const char *val)
{
   CHK(Tcl_VarEval(interp, windowName,entry," config -state normal", NULL));
   setText(entry, val);
   CHK(Tcl_VarEval(interp, windowName,entry," config -state disabled", NULL));
}

const char *Inspector::getEntry( const char *entry )
{
   CHK(Tcl_VarEval(interp, windowName,entry," get",NULL));
   return Tcl_GetStringResult(interp);
}

void Inspector::setInspectButton(const char *button, cObject *object, bool displayfullpath, int inspectortype)
{
   if (object)
   {
      char buf[16];
      sprintf(buf, "%d", inspectortype);
      char idtext[30] = "";
      if (dynamic_cast<cModule *>(object))
      {
          sprintf(idtext, " (id=%d)", static_cast<cModule *>(object)->getId());
      }
      CHK(Tcl_VarEval(interp, windowName, button,".e config -state normal ",
                              "-text {(", getObjectShortTypeName(object), ") ",
                              (displayfullpath ? object->getFullPath().c_str() : object->getFullName()),
                              idtext, "} ",
                              "-command {opp_inspect ",ptrToStr(object)," ",buf,"}",
                              NULL));
   }
   else
   {
      CHK(Tcl_VarEval(interp, windowName,button,".e config -state disabled -text {n/a}",NULL));
   }
}

void Inspector::setToolbarInspectButton(const char *button, cObject *object, int type)
{
   if (object)
   {
      char buf[16];
      sprintf(buf, "%d", type );
      CHK(Tcl_VarEval(interp, windowName,button," config -state normal -command"
                              " {opp_inspect ",ptrToStr(object)," ",buf,"}",NULL));
   }
   else
   {
      CHK(Tcl_VarEval(interp, windowName,button," config -state disabled",NULL));
   }
}

void Inspector::clearInspectorListbox(const char *listbox)
{
   CHK(Tcl_VarEval(interp, "multicolumnlistbox:deleteAll ", windowName,listbox,".main.list",NULL));
}

void Inspector::fillInspectorListbox(const char *listbox, cObject *object, bool deep)
{
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowName,listbox);
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
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowName,listbox);

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

