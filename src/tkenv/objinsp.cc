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
            dynamic_cast<cSimulation *>(object);
        bool focuscontentspage =
            dynamic_cast<cArray *>(object) || dynamic_cast<cQueue *>(object) ||
            dynamic_cast<cMessageHeap *>(object) || dynamic_cast<cDefaultList *>(object);
        insp->setContentsPage(showcontentspage, focuscontentspage);
        return insp;
    }
};

Register_InspectorFactory(TGenericObjectInspectorFactory);


//=======================================================================

//
// class TObjInspectorFactory : public cInspectorFactory
// {
//   public:
//     TObjInspectorFactory(const char *name) : cInspectorFactory(name) {}
//
//     bool supportsObject(cObject *obj) {return true;}
//     int inspectorType() {return INSP_OBJECT;}
//     double qualityAsDefault(cObject *object) {return 1.0;}
//
//     TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
//         return new TObjInspector(object, type, geom, data);
//     }
// };
//
// Register_InspectorFactory(TObjInspectorFactory);
//
//
// TObjInspector::TObjInspector(cObject *obj,int typ,const char *geom,void *dat) :
//     TInspector(obj,typ,geom,dat)
// {
// }
//
// void TObjInspector::createWindow()
// {
//    TInspector::createWindow(); // create window name etc.
//
//    // create inspector window by calling the specified proc with
//    // the object's pointer. Window name will be like ".ptr80003a9d-1"
//    Tcl_Interp *interp = getTkApplication()->getInterp();
//    CHK(Tcl_VarEval(interp, "create_objinspector ", windowname, " \"", geometry, "\"", NULL ));
// }
//
// void TObjInspector::update()
// {
//    TInspector::update();
//
//    setLabel(".nb.info.name.e",object->name());
//    setLabel(".nb.info.fullpath.e",object->fullPath().c_str());
//    setLabel(".nb.info.class.e",object->className());
//
//    setLabel(".nb.info.info.e",object->info().c_str());
//    setReadonlyText(".nb.info.details.t",object->detailedInfo().c_str());
// }
//
// void TObjInspector::writeBack()
// {
//    TInspector::writeBack();    // must be there after all changes
// }
//
// int TObjInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
// {
//    return TCL_ERROR;
// }
//
// TObjInspector::~TObjInspector()
// {
// }

//=======================================================================

//
// class TContainerInspectorFactory : public cInspectorFactory
// {
//   public:
//     TContainerInspectorFactory(const char *name) : cInspectorFactory(name) {}
//
//     bool supportsObject(cObject *obj) {return true;}
//     int inspectorType() {return INSP_OBJECT;}
//
//     double qualityAsDefault(cObject *object) {
//         if (dynamic_cast<cArray *>(object) || dynamic_cast<cQueue *>(object) ||
//             dynamic_cast<cMessageHeap *>(object) || dynamic_cast<cDefaultList *>(object) ||
//             dynamic_cast<cSimulation *>(object)
//            )
//             return 2.0;
//         else
//             return 0.9;
//     }
//
//     TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
//         return new TContainerInspector(object, type, geom, data);
//     }
// };
//
// Register_InspectorFactory(TContainerInspectorFactory);
//
//
// TContainerInspector::TContainerInspector(cObject *obj,int typ,const char *geom,void *dat) :
//     TInspector(obj,typ,geom,dat)
// {
//    char *opt = (char *)dat;
//    deep = opt && opt[0]=='d';
// }
//
// void TContainerInspector::createWindow()
// {
//    TInspector::createWindow(); // create window name etc.
//    strcpy(listbox,windowname); strcat(listbox,".nb.contents.main.list");
//
//    // create inspector window by calling the specified proc with
//    // the object's pointer. Window name will be like ".ptr80003a9d-1"
//    Tcl_Interp *interp = getTkApplication()->getInterp();
//    char *typelist = " (default) object container graphical";
//    CHK(Tcl_VarEval(interp, "create_containerinspector ", windowname, " \"", geometry, "\"", typelist, NULL));
// }
//
// void TContainerInspector::update()
// {
//    TInspector::update();
//
//    deleteInspectorListbox(".nb.contents");
//    fillInspectorListbox(".nb.contents", object, deep);
// }

//=======================================================================

//
// class TMessageInspectorFactory : public cInspectorFactory
// {
//   public:
//     TMessageInspectorFactory(const char *name) : cInspectorFactory(name) {}
//
//     bool supportsObject(cObject *obj) {return dynamic_cast<cMessage *>(obj)!=NULL;}
//     int inspectorType() {return INSP_OBJECT;}
//     double qualityAsDefault(cObject *object) {return 2.0;}
//
//     TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
//         return new TMessageInspector(object, type, geom, data);
//     }
// };
//
// Register_InspectorFactory(TMessageInspectorFactory);
//
//
// TMessageInspector::TMessageInspector(cObject *obj,int typ,const char *geom,void *dat) :
//     TInspector(obj,typ,geom,dat)
// {
//    controlinfopage = NULL;
// }
//
// void TMessageInspector::createWindow()
// {
//    TInspector::createWindow(); // create window name etc.
//
//    // create inspector window by calling the specified proc with
//    // the object's pointer. Window name will be like ".ptr80003a9d-1"
//    Tcl_Interp *interp = getTkApplication()->getInterp();
//    CHK(Tcl_VarEval(interp, "create_messageinspector ", windowname, " \"", geometry, "\"", NULL ));
//
//    char controlinfopagewidget[256];
//    sprintf(controlinfopagewidget, "%s.nb.controlinfo", windowname);
//    controlinfopage = new TStructPanel(controlinfopagewidget, NULL);
//
// }
//
// void TMessageInspector::update()
// {
//    TInspector::update();
//
//    cMessage *msg = static_cast<cMessage *>(object);
//
//    setEntry(".nb.info.name.e", msg->name() );
//    setEntry(".nb.info.kind.e", (long)msg->kind() );
//    setEntry(".nb.info.length.e", (long)msg->length() );
//    setLabel(".nb.info.lengthb.e", (long)msg->byteLength() );
//    setEntry(".nb.info.prio.e", (long)msg->priority() );
//    setEntry(".nb.info.error.e", (long)msg->hasBitError() );
//    setEntry(".nb.send.tstamp.e", msg->timestamp() );
//    setLabel(".nb.send.created.e", simtimeToStr(msg->creationTime()) );
//    setLabel(".nb.send.sent.e", simtimeToStr(msg->sendingTime()) );
//    setLabel(".nb.send.arrived.e", simtimeToStr(msg->arrivalTime()) );
//
//    setInspectButton(".nb.info.encapmsg", msg->encapsulatedMsg(), false, INSP_DEFAULT);
//    setInspectButton(".nb.send.owner", msg->owner(), true, INSP_DEFAULT);
//    setInspectButton(".nb.send.src",simulation.module(msg->senderModuleId()), true, INSP_DEFAULT);
//    setInspectButton(".nb.send.dest",simulation.module(msg->arrivalModuleId()), true, INSP_DEFAULT);
//
//    controlinfopage->setObject(msg->controlInfo());
//    controlinfopage->update();
//
//    deleteInspectorListbox(".nb.params");
//    fillInspectorListbox(".nb.params", &msg->parList(), false);
// }
//
// void TMessageInspector::writeBack()
// {
//    cMessage *msg = static_cast<cMessage *>(object);
//
//    msg->setName( getEntry(".nb.info.name.e") );
//    msg->setKind( atol(getEntry(".nb.info.kind.e")) );
//    msg->setLength( atol( getEntry(".nb.info.length.e")) );
//    msg->setPriority( atol( getEntry(".nb.info.prio.e")) );
//    msg->setBitError( atol( getEntry(".nb.info.error.e"))!=0 );
//    msg->setTimestamp( atof( getEntry(".nb.send.tstamp.e")) );
//
//    controlinfopage->writeBack();
//
//    TInspector::writeBack();    // must be there after all changes
// }
//
// int TMessageInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
// {
//    return TCL_ERROR;
// }
//
// TMessageInspector::~TMessageInspector()
// {
//    delete controlinfopage;
// }

//=======================================================================

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

//=======================================================================

class TParInspectorFactory : public cInspectorFactory
{
  public:
    TParInspectorFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cPar *>(obj)!=NULL;}
    int inspectorType() {return INSP_OBJECT;}
    double qualityAsDefault(cObject *object) {return 2.0;}

    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TParInspector(object, type, geom, data);
    }
};

Register_InspectorFactory(TParInspectorFactory);


TParInspector::TParInspector(cObject *obj,int typ,const char *geom,void *dat) :
    TInspector(obj,typ,geom,dat)
{
}

void TParInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "create_parinspector ", windowname, " \"", geometry, "\"", NULL ));
}

void TParInspector::update()
{
   TInspector::update();

   cPar *p = static_cast<cPar *>(object);
   setEntry(".main.value.e", p->getAsText().c_str());

   char *t;
   switch (p->type())
   {
       case 'C': t=" C character"   ; break;
       case 'S': t=" S string"      ; break;
       case 'B': t=" B boolean"     ; break;
       case 'L': t=" L long int"    ; break;
       case 'D': t=" D double"      ; break;
       case 'F': t=" F function"    ; break;
       case 'X': t=" X expression"  ; break;
       case 'T': t=" T distribution"; break;
       case 'P': t=" P void* ptr"   ; break;
       case 'O': t=" O object ptr"  ; break;
       case 'M': t=" M XML element" ; break;
       default:  t=" ??? unknown";
   }
   setLabel(".main.type.e", t );
   setEntry(".main.newtype.e", "" );
   setEntry(".main.prompt.e", correct(p->prompt()) );
   setEntry(".main.input.e", p->isInput() ? "1" : "0" );
   setInspectButton(".main.indirection",p->redirection(), true, INSP_DEFAULT);
}

void TParInspector::writeBack()
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   cPar *p = static_cast<cPar *>(object);

   char newtype = getEntry(".main.newtype.e")[0];
   if (!newtype) newtype = '?';

   bool ok = false;
   try {
      ok = p->setFromText(getEntry(".main.value.e"), newtype);
      if (!ok)
         throw new cException(newtype=='?' ? "Syntax error, value not changed." : "Syntax error or wrong type, value not changed.");
   } catch (cException *e) {
      TclQuotedString msg(e->message());
      delete e;
      CHK(Tcl_VarEval(interp,"messagebox {Error} ", msg.get(), " error ok", NULL));
   }
   if (ok)
      setEntry(".main.newtype.e", "");
   p->setPrompt( getEntry(".main.prompt.e") );
   p->setInput( getEntry(".main.input.e")[0]=='1' );

   TInspector::writeBack();    // must be there after all changes
}

