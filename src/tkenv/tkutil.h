//==========================================================================
//  TKUTIL.H - part of
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

#ifndef __TKUTIL_H
#define __TKUTIL_H

#define WIN32_LEAN_AND_MEAN
#include "platmisc.h"   // must precede tk.h otherwise Visual Studio 2013 fails to compile
#include <tk.h>

#include "tkdefs.h"
#include "cobject.h"
#include "visitor.h"

NAMESPACE_BEGIN

class cPar;
class cComponent;

//
// In some installations Tcl headers files have 'char*' without 'const char*'
// in arg lists -- we have to cast away 'const char*' from args in our Tcl calls.
//
#define TCLCONST(x)   const_cast<char*>(x)

//
// Print error message on console if Tcl code returns error
//
#ifdef _NDEBUG
#define CHK(tcl_eval_statement)   tcl_eval_statement
#else
#define CHK(tcl_eval_statement)    \
  do { if (tcl_eval_statement==TCL_ERROR) \
        logTclError(__FILE__,__LINE__, interp); \
  } while(0)
#endif

//
// Turns exceptions into Tcl errors
//
#define TRY(code) \
  try {code;} catch (std::exception& e) { \
      Tcl_SetResult(interp, TCLCONST(e.what()), TCL_VOLATILE); \
      return TCL_ERROR; \
  }

#define E_TRY   try {

#define E_CATCH \
  } catch (std::exception& e) { \
      Tcl_SetResult(interp, TCLCONST(e.what()), TCL_VOLATILE); \
      return TCL_ERROR; \
  }


/**
 * Quoting strings for Tcl_Eval
 */
class TKENV_API TclQuotedString
{
  private:
    char buf[80];  // used only if string is short, to spare alloc/dealloc
    char *quotedstr;
  public:
    TclQuotedString();
    TclQuotedString(const char *s);
    TclQuotedString(const char *s, int n);

    void set(const char *s);
    void set(const char *s, int n);

    ~TclQuotedString();
    const char *get() {return quotedstr;}
    operator const char *() {return quotedstr;}
};

/**
 * Find objects by full path, and optionally also matching class name and/or Id.
 */
class TKENV_API cFindByPathVisitor : public cCollectObjectsVisitor
{
  private:
    const char *fullPath;
    const char *className; // optional
    long objectId; // optional message or component Id; use -1 for none
  protected:
    virtual void visit(cObject *obj);
    bool idMatches(cObject *obj);
  public:
    cFindByPathVisitor(const char *fullPath, const char *className=NULL, long objectId=-1) :
        fullPath(fullPath), className(className), objectId(objectId) {}
    ~cFindByPathVisitor() {}
};

//
// Utility functions:
//
const char *getObjectShortTypeName(cObject *object);
const char *getObjectFullTypeName(cObject *object);

char *voidPtrToStr(void *ptr, char *buffer=NULL);
void *strToVoidPtr(const char *s);

inline char *ptrToStr(cObject *ptr, char *buffer=NULL) {return voidPtrToStr((void *)ptr, buffer);}
inline cObject *strToPtr(const char *s) {return (cObject *)strToVoidPtr(s);}

cModule *findCommonAncestor(cModule *src, cModule *dest);

void resolveSendDirectHops(cModule *src, cModule *dest, std::vector<cModule*>& hops);

bool isAPL();

std::string getObjectIcon(Tcl_Interp *interp, cObject *object);

void setObjectListResult(Tcl_Interp *interp, cCollectObjectsVisitor *visitor);

void insertIntoInspectorListbox(Tcl_Interp *interp, const char *listbox, cObject *obj, bool fullpath);

void feedCollectionIntoInspectorListbox(cCollectObjectsVisitor *visitor, Tcl_Interp *interp, const char *listbox, bool fullpath);

int fillListboxWithChildObjects(cObject *object, Tcl_Interp *interp, const char *listbox, bool deep);

/**
 * Looks up the given parameter
 */
cPar *displayStringPar(const char *parname, cComponent *component, bool searchparent);

/**
 * Returns true if the given (partial or full) display string contains
 * module or channel parameter references ("$param" or "${param}")
 */
bool displayStringContainsParamRefs(const char *dispstr);

/**
 * If the given string is a single parameter reference in the syntax "$name" or
 * "${name}", returns the parameter, otherwise returns NULL.
 */
cPar *resolveDisplayStringParamRef(const char *dispstr, cComponent *component, bool searchparent);

/**
 * Substitutes module/channel param references into the given (partial)
 * display string. The returned string may be the same as the one passed in,
 * or may point into the buffer std::string.
 */
const char *substituteDisplayStringParamRefs(const char *dispstr, std::string& buffer, cComponent *component, bool searchparent);

bool resolveBoolDispStrArg(const char *arg, cComponent *component, bool defaultValue);
long resolveLongDispStrArg(const char *arg, cComponent *component, int defaultValue);
double resolveDoubleDispStrArg(const char *arg, cComponent *component, double defaultValue);

void logTclError(const char *file, int line, Tcl_Interp *interp);

NAMESPACE_END


#endif



