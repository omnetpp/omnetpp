//==========================================================================
//  TKUTIL.CC -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <tk.h>

#include "cobject.h"
#include "cmodule.h"
#include "csimul.h"
#include "tkapp.h"
#include "patmatch.h"
#include "visitor.h"
#include "tkutil.h"

//=======================================================================

char *ptrToStr(void *ptr, char *buffer)
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

void *strToPtr(const char *s)
{
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
   char *buf = Tcl_Alloc(ptrsize*n);
   char *s=buf;
   for (int i=0; i<n; i++)
   {
       ptrToStr(objs[i],s);
       s+=strlen(s);
       assert(strlen(s)<=20);
       *s++ = ' ';
   }
   *s='\0';
   Tcl_SetResult(interp, buf, TCL_DYNAMIC);
}


// helper
char *my_itol(long l, char *buf)
{
    sprintf(buf, "%ld", l);
    return buf;
}


//-----------------------------------------------------------------------

static void insert_into_inspectorlistbox(Tcl_Interp *interp, const char *listbox, cObject *obj)
{
    const char *ptr = ptrToStr(obj);
    static char buf[MAX_OBJECTINFO];
    obj->info(buf);
    CHK(Tcl_VarEval(interp, "multicolumnlistbox_insert ",listbox," ",ptr," {"
                            "ptr {",ptr,"} "
                            "name {",obj->fullName(),"} "
                            "class {",obj->className(),"} "
                            "info {",buf,"}"
                            "}",NULL));
}

static bool do_fill_listbox( cObject *obj, bool beg, Tcl_Interp *intrp, const char *lstbox, InfoFunc f, bool dp)
{
    static const char *listbox;
    static Tcl_Interp *interp;
    static InfoFunc infofunc;
    static bool deep;
    static int ctr;
    if (!obj) {       // setup
         listbox = lstbox;
         interp = intrp;
         infofunc = f;
         deep = dp;
         ctr  = 0;
         return false;
    }
    if( !beg ) return false;
    if( (deep || ctr>0) && !memoryIsLow() ) // if deep=false, exclude owner object
    {
        insert_into_inspectorlistbox(interp, listbox, obj);
    }
    return deep || ctr++ == 0;
}

int fillListboxWithChildObjects( cObject *object, Tcl_Interp *interp, const char *listbox, InfoFunc infofunc, bool deep)
{
    // feeds all children of 'object' into the listbox
    // CHK(Tcl_VarEval(interp, listbox, " delete 0 end", NULL ));
    do_fill_listbox(NULL,false, interp, listbox, infofunc, deep);
    object->forEach( (ForeachFunc)do_fill_listbox );
    return 0; //FIXME!!!!!!!!!
}

static void do_fill_module_listbox(cModule *parent, Tcl_Interp *interp, const char *listbox, InfoFunc infofunc, bool simpleonly, bool deep )
{
    // loop through module vector
    for (int i=0; i<=simulation.lastModuleId() && !memoryIsLow(); i++ )
    {
        cModule *mod = simulation.module(i);
        if (mod && mod!=simulation.systemModule() && mod->parentModule()==parent)
        {
           if (!simpleonly || mod->isSimple())
              insert_into_inspectorlistbox(interp, listbox, mod);

           // handle 'deep' option using recursivity
           if (deep)
              do_fill_module_listbox(mod,interp,listbox,infofunc,simpleonly,deep);
        }
    }
}

int fillListboxWithChildModules(cModule *parent, Tcl_Interp *interp, const char *listbox, InfoFunc infofunc, bool simpleonly, bool deep )
{
    // CHK(Tcl_VarEval(interp, listbox, " delete 0 end", NULL ));
    if (deep)
    {
        if (!simpleonly || parent->isSimple())
            insert_into_inspectorlistbox(interp, listbox, parent);
    }
    do_fill_module_listbox(parent,interp,listbox,infofunc,simpleonly,deep);
    return 0; //FIXME!!!!!!!!!
}

//-----------------------------------------------------------------------

char *printptr(cObject *object, char *buf)
{
    // produces string like: "ptr80004e1f  ".
    // returns a pointer to terminating '\0' char
    ptrToStr(object, buf);
    char *s = buf+strlen(buf);
    s[0]=' '; s[1]=' '; s[2]='\0'; // append two spaces
    return s+2;
}

char *infofunc_infotext( cObject *object)
{
    static char buf[MAX_OBJECTINFO];
    char *d = printptr(object,buf);
    object->info( d );
    return buf;
}

char *infofunc_module( cObject *object)
{
    static char buf[128];
    char *d = printptr(object,buf);
    cModule *mod = (cModule *)object;
    const char *path = mod->fullPath();
    const char *clname = mod->className();
    int padding = 16-strlen(clname); if (padding<1) padding=1;
    sprintf(d, "(%s)%*s %.80s (id=%d)", clname, padding,"", path, mod->id());
    return buf;
}

//----------------------------------------------------------------------

static int inspmatch_ctr;
static bool do_inspect_matching( cObject *obj, bool beg, short *patt, int typ, bool co)
{
    static bool deep;
    static bool countonly;
    static short *pattern;
    static int type;
    static int ctr;
    if (!obj) {       // setup
         pattern = patt;
         type = typ;
         deep = true;
         countonly=co;
         ctr  = 0;
         return false;
    }
    if( !beg ) return false;
    if( (deep || ctr>0) && !memoryIsLow() ) // if deep=false, exclude owner object
    {
         const char *fullpath = obj->fullPath();
         if (stringmatch(pattern,fullpath))
         {
           if (!countonly)
               ((TOmnetTkApp *)(ev.app))->inspect(obj,type,"",NULL);
           inspmatch_ctr++;
         }
    }
    return deep || ctr++ == 0;
}

int inspectMatchingObjects(cObject *object, Tcl_Interp *, char *pattern, int type, bool countonly)
{
    // open inspectors for children of 'object' whose fullpath matches pattern
    short trf_pattern[512];
    if (transform_pattern(pattern, trf_pattern)==false)
       return 0; // bad pattern: unmatched '{'
    inspmatch_ctr=0;
    do_inspect_matching(NULL,false, trf_pattern, type, countonly);
    object->forEach( (ForeachFunc)do_inspect_matching );
    return inspmatch_ctr;
}

//----------------------------------------------------------------------

static bool do_inspect_by_name( cObject *obj, bool beg, const char *_fullpath, const char *_classname,
                                int _insptype, const char *_geometry)
{
    static const char *fullpath;
    static const char *classname;
    static int insptype;
    static const char *geometry;

    static int ctr;
    if (!obj) {       // setup
        fullpath = _fullpath;
        classname = _classname;
        insptype = _insptype;
        geometry = _geometry;
        ctr  = 0;
        return false;
    }

    if (!beg)
        return false;

    // we have to do exhaustive search here... optimization, such as checking
    // if objpath matches beginning of fullpath to see if we're on the
    // right track is not usable, because some objects (simulation, modules'
    // paramv, gatev members) don't appear in object's fullPath()...

    const char *objpath = obj->fullPath();

    // however, a module's name and the future event set's name is not hidden,
    // so if this obj is a module (or cMessageHeap) and its name doesn't match
    // the beginning of fullpath, we can cut the search here.
    if ((dynamic_cast<cModule *>(obj) || dynamic_cast<cMessageHeap *>(obj))
        && strncmp(objpath, fullpath, strlen(objpath))!=0)
    {
        // skip (do not search) this subtree
        return false;
    }

    // found it?
    if (!strcmp(fullpath,objpath) && !strcmp(classname,obj->className()))
    {
        // found: inspect if inspector is not open
        TOmnetTkApp *app = (TOmnetTkApp *)(ev.app);
        if (!app->findInspector(obj, insptype))
            app->inspect(obj, insptype, geometry, NULL);
        // makes no sense to go further down
        return false;
    }

    // just search further...
    return true;
}


void inspectObjectByName(const char *fullpath, const char *classname, int insptype, const char *geometry)
{
    // open inspectors for object whose is the same as fullpath
    do_inspect_by_name(NULL,false, fullpath, classname, insptype, geometry);
    simulation.forEach( (ForeachFunc)do_inspect_by_name);
}


