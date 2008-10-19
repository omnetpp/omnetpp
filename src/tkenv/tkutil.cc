//==========================================================================
//  TKUTIL.CC - part of
//
//                    OMNeT++/OMNEST
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// Work around an MSVC .NET bug: WinSCard.h causes compilation
// error due to faulty LPCVOID declaration, so prevent it from
// being pulled in (we don't need the SmartCard API here anyway ;-)
#define _WINSCARD_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <tk.h>

#include "cownedobject.h"
#include "cmodule.h"
#include "csimulation.h"

#include "tkenv.h"
#include "patternmatcher.h"
#include "visitor.h"
#include "tkutil.h"

NAMESPACE_BEGIN

#define INSPECTORLISTBOX_MAX_ITEMS   100000


TclQuotedString::TclQuotedString()
{
    quotedstr = NULL;
    buf[0] = '\0';
}

TclQuotedString::TclQuotedString(const char *s)
{
    int flags;
    int quotedlen = Tcl_ScanElement(TCLCONST(s), &flags);
    quotedstr = quotedlen<80 ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertElement(TCLCONST(s), quotedstr, flags);
}

TclQuotedString::TclQuotedString(const char *s, int n)
{
    int flags;
    int quotedlen = Tcl_ScanCountedElement(TCLCONST(s), n, &flags);
    quotedstr = quotedlen<80 ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertCountedElement(TCLCONST(s), n, quotedstr, flags);
}

void TclQuotedString::set(const char *s)
{
    int flags;
    int quotedlen = Tcl_ScanElement(TCLCONST(s), &flags);
    quotedstr = quotedlen<80 ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertElement(TCLCONST(s), quotedstr, flags);
}

void TclQuotedString::set(const char *s, int n)
{
    int flags;
    int quotedlen = Tcl_ScanCountedElement(TCLCONST(s), n, &flags);
    quotedstr = quotedlen<80 ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertCountedElement(TCLCONST(s), n, quotedstr, flags);
}

TclQuotedString::~TclQuotedString()
{
    if (quotedstr!=buf)
        Tcl_Free(quotedstr);
}

//=======================================================================

const char *getObjectShortTypeName(cObject *object)
{
   if (dynamic_cast<cComponent*>(object))
       return ((cComponent*)object)->getComponentType()->getName();
   else
       return object->getClassName();
}

const char *getObjectFullTypeName(cObject *object)
{
   if (dynamic_cast<cComponent*>(object))
       return ((cComponent*)object)->getComponentType()->getFullName();
   else
       return object->getClassName();
}

char *voidPtrToStr(void *ptr, char *buffer)
{
    static char staticbuf[20];
    if (buffer==NULL)
           buffer = staticbuf;

    if (ptr==0)
           strcpy(buffer,"ptr0");  // GNU C++'s sprintf() says "nil"
    else
           sprintf(buffer,"ptr%p", ptr );
    return buffer;
}

void *strToVoidPtr(const char *s)
{
    // Note: must accept "" and malformed strings too, and return them as NULL.
    if (s[0]!='p' || s[1]!='t' || s[2]!='r')
       return NULL;

    void *ptr;
    sscanf(s+3,"%p",&ptr);
    return ptr;
}


void setObjectListResult(Tcl_Interp *interp, cCollectObjectsVisitor *visitor)
{
   int n = visitor->getArraySize();
   cObject **objs = visitor->getArray();
   const int ptrsize = 21; // one ptr should be max 20 chars (good for even 64bit-ptrs)
   char *buf = Tcl_Alloc(ptrsize*n+1);
   char *s=buf;
   for (int i=0; i<n; i++)
   {
       ptrToStr(objs[i],s);
       assert(strlen(s)<=20);
       s+=strlen(s);
       *s++ = ' ';
   }
   *s='\0';
   Tcl_SetResult(interp, buf, TCL_DYNAMIC);
}


//-----------------------------------------------------------------------

void insertIntoInspectorListbox(Tcl_Interp *interp, const char *listbox, cObject *obj, bool fullpath)
{
    const char *ptr = ptrToStr(obj);
    CHK(Tcl_VarEval(interp, "multicolumnlistbox_insert ",listbox," ",ptr," {"
                            "ptr {",ptr,"} "
                            "name {",(fullpath ? obj->getFullPath().c_str() : obj->getFullName()),"} "
                            "class {",getObjectShortTypeName(obj),"} "
                            "info ",TclQuotedString(obj->info().c_str()).get(),
                            "} ",
                            "[get_icon_for_object {",ptr,"}]",
                            NULL));
}

void feedCollectionIntoInspectorListbox(cCollectObjectsVisitor *visitor, Tcl_Interp *interp, const char *listbox, bool fullpath)
{
    int n = visitor->getArraySize();
    cObject **objs = visitor->getArray();

    for (int i=0; i<n; i++)
    {
        // insert into listbox
        insertIntoInspectorListbox(interp, listbox, objs[i], fullpath);
    }
}

int fillListboxWithChildObjects(cObject *object, Tcl_Interp *interp, const char *listbox, bool deep)
{
    int n;
    if (deep)
    {
        cCollectObjectsVisitor visitor;
        visitor.setSizeLimit(INSPECTORLISTBOX_MAX_ITEMS);
        visitor.process(object);
        n = visitor.getArraySize();
        feedCollectionIntoInspectorListbox(&visitor, interp, listbox, true);
    }
    else
    {
        cCollectChildrenVisitor visitor(object);
        visitor.setSizeLimit(INSPECTORLISTBOX_MAX_ITEMS);
        visitor.process(object);
        n = visitor.getArraySize();
        feedCollectionIntoInspectorListbox(&visitor, interp, listbox, false);
    }
    return n;
}

//----------------------------------------------------------------------

/**
 * Recursively finds an object by full path.
 */
class cInspectByNameVisitor : public cVisitor
{
  protected:
    const char *fullpath;
    const char *classname;
    int insptype;
    const char *geometry;
  public:
    cInspectByNameVisitor(const char *_fullpath, const char *_classname,
                          int _insptype, const char *_geometry) {
        fullpath = _fullpath;
        classname = _classname;
        insptype = _insptype;
        geometry = _geometry;
    }
    virtual void visit(cObject *obj) {
        // we have to do exhaustive search here... optimization, such as checking
        // if objpath matches beginning of fullpath to see if we're on the
        // right track is not usable, because some objects (simulation, modules'
        // paramv, gatev members) don't appear in object's getFullPath()...

        std::string objpath = obj->getFullPath();

        // however, a module's name and the future event set's name is not hidden,
        // so if this obj is a getModule(or cMessageHeap) and its name doesn't match
        // the beginning of fullpath, we can cut the search here.
        if ((dynamic_cast<cModule *>(obj) || dynamic_cast<cMessageHeap *>(obj))
            && strncmp(objpath.c_str(), fullpath, strlen(objpath.c_str()))!=0)
        {
            // skip (do not search) this subtree
            return;
        }

        // found it?
        if (!strcmp(fullpath,objpath.c_str()) && !strcmp(classname,getObjectShortTypeName(obj)))
        {
            // found: inspect if inspector is not open
            Tkenv *app = getTkenv();
            if (!app->findInspector(obj, insptype))
                app->inspect(obj, insptype, geometry, NULL);

            // makes no sense to go further down
            return;
        }

        // search recursively
        obj->forEachChild(this);
    }
};

void inspectObjectByName(const char *fullpath, const char *classname, int insptype, const char *geometry)
{
    // open inspectors for object whose is the same as fullpath
    cInspectByNameVisitor v(fullpath, classname, insptype, geometry);
    v.process(&simulation);
}

NAMESPACE_END


