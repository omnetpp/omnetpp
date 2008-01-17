//==========================================================================
//  OBJINSP.CC - part of
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

#include "omnetpp.h"

#include "tkapp.h"
#include "tklib.h"
#include "inspfactory.h"
#include "objinsp.h"

USING_NAMESPACE


void _dummy_for_objinsp() {}

TGenericObjectInspector::TGenericObjectInspector(cObject *obj, int typ, const char *geom, void *dat) :
TInspector(obj,typ,geom,dat)
{
    hascontentspage = false;
    focuscontentspage = false;
}

TGenericObjectInspector::~TGenericObjectInspector()
{
}

void TGenericObjectInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "create_genericobjectinspector ", windowname, " \"", geometry, "\"", " ",
                   (hascontentspage ? "1" : "0"), " ",  (focuscontentspage ? "1" : "0"), " ", NULL));
}

void TGenericObjectInspector::update()
{
   TInspector::update();

   // refresh "fields" page
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "refresh_fields2page ", windowname, NULL));

   // refresh "contents" page
   if (hascontentspage)
   {
       deleteInspectorListbox(".nb.contents");
       fillInspectorListbox(".nb.contents", object, false);
   }
}

void TGenericObjectInspector::writeBack()
{
   TInspector::writeBack();
}

int TGenericObjectInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   return TCL_ERROR;
}

class TGenericObjectInspectorFactory : public cInspectorFactory
{
  public:
    TGenericObjectInspectorFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return true;}
    int inspectorType() {return INSP_OBJECT;}
    double qualityAsDefault(cObject *object) {return 1.0;}

    TInspector *createInspectorFor(cObject *object, int type, const char *geom, void *data) {
        TGenericObjectInspector *insp = new TGenericObjectInspector(object, type, geom, data);
        bool showcontentspage =
            dynamic_cast<cArray *>(object) || dynamic_cast<cQueue *>(object) ||
            dynamic_cast<cMessageHeap *>(object) || dynamic_cast<cDefaultList *>(object) ||
            dynamic_cast<cSimulation *>(object) || dynamic_cast<cSymTable *>(object);
        bool focuscontentspage =
            dynamic_cast<cArray *>(object) || dynamic_cast<cQueue *>(object) ||
            dynamic_cast<cMessageHeap *>(object) || dynamic_cast<cDefaultList *>(object) ||
            dynamic_cast<cSymTable *>(object);
        insp->setContentsPage(showcontentspage, focuscontentspage);
        return insp;
    }
};

Register_InspectorFactory(TGenericObjectInspectorFactory);

//----

class TWatchInspectorFactory : public cInspectorFactory
{
  public:
    TWatchInspectorFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {
        // Return true if it's a watch for a simple type (int, double, string etc).
        // For structures, we prefer the normal TGenericObjectInspector.
        // Currently we're prepared for cStdVectorWatcherBase.
        return dynamic_cast<cWatchBase *>(obj) && !dynamic_cast<cStdVectorWatcherBase *>(obj);
    }
    int inspectorType() {return INSP_OBJECT;}
    double qualityAsDefault(cObject *object) {return 2.0;}
    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TWatchInspector(object, type, geom, data);
    }
};

Register_InspectorFactory(TWatchInspectorFactory);


TWatchInspector::TWatchInspector(cObject *obj,int typ,const char *geom,void *dat) :
    TInspector(obj,typ,geom,dat)
{
}

void TWatchInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "create_watchinspector ", windowname, " \"", geometry, "\"", NULL ));
}

void TWatchInspector::update()
{
   TInspector::update();

   cWatchBase *watch = static_cast<cWatchBase *>(object);
   setLabel(".main.name.l", (std::string(watch->className())+" "+watch->name()).c_str());
   setEntry(".main.name.e", watch->info().c_str());
}

void TWatchInspector::writeBack()
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   cWatchBase *watch = static_cast<cWatchBase *>(object);
   const char *s = getEntry(".main.name.e");
   if (watch->supportsAssignment())
       watch->assign(s);
   else
      CHK(Tcl_VarEval(interp,"messagebox {Warning} {This inspector doesn't support changing the value.} warning ok", NULL));
   TInspector::writeBack();    // must be there after all changes
}


