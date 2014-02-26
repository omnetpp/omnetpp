//==========================================================================
//  GENERICOBJECTINSPECTOR.CC - part of
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
#include <math.h>
#include "tkenv.h"
#include "tklib.h"
#include "inspectorfactory.h"
#include "genericobjectinspector.h"

#include "carray.h"
#include "cqueue.h"
#include "cmessageheap.h"
#include "cdefaultlist.h"
#include "csimulation.h"
#include "cregistrationlist.h"


NAMESPACE_BEGIN

void _dummy_for_genericobjectinspector() {}


class GenericObjectInspectorFactory : public InspectorFactory
{
  public:
    GenericObjectInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return true;}
    int getInspectorType() {return INSP_OBJECT;}
    double getQualityAsDefault(cObject *object) {return 1.0;}

    Inspector *createInspector() {
        return prepare(new GenericObjectInspector());
    }
};

Register_InspectorFactory(GenericObjectInspectorFactory);


GenericObjectInspector::GenericObjectInspector() : Inspector(INSP_OBJECT)
{
    hascontentspage = false;
    focuscontentspage = false;
}

GenericObjectInspector::~GenericObjectInspector()
{
}

void GenericObjectInspector::setObject(cObject *obj)
{
    Inspector::setObject(obj);

    bool showcontentspage =
            dynamic_cast<cArray *>(object) || dynamic_cast<cQueue *>(object) ||
            dynamic_cast<cMessageHeap *>(object) || dynamic_cast<cDefaultList *>(object) ||
            dynamic_cast<cSimulation *>(object) || dynamic_cast<cRegistrationList *>(object);
    bool focuscontentspage =
            dynamic_cast<cArray *>(object) || dynamic_cast<cQueue *>(object) ||
            dynamic_cast<cMessageHeap *>(object) || dynamic_cast<cDefaultList *>(object) ||
            dynamic_cast<cRegistrationList *>(object);
    //FIXME this probably doesn't work now
    setContentsPage(showcontentspage, focuscontentspage);
}

void GenericObjectInspector::createWindow(const char *window, const char *geometry)
{
   Inspector::createWindow(window, geometry);

   CHK(Tcl_VarEval(interp, "createGenericObjectInspector ", windowName, " \"", geometry, "\"", " ",
                   (hascontentspage ? "1" : "0"), " ",  (focuscontentspage ? "1" : "0"), " ", NULL));
}

void GenericObjectInspector::useWindow(const char *window)
{
   Inspector::useWindow(window);
}

void GenericObjectInspector::refresh()
{
   Inspector::refresh();

   // refresh "fields" page
   CHK(Tcl_VarEval(interp, "fields2Page:refresh ", windowName, NULL));

   // refresh "contents" page
   if (hascontentspage)
   {
       clearInspectorListbox(".nb.contents");
       if (object)
           fillInspectorListbox(".nb.contents", object, false);
   }
}

void GenericObjectInspector::commit()
{
   Inspector::commit();
}

int GenericObjectInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   return TCL_ERROR;
}

NAMESPACE_END

