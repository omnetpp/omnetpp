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


class TObjInspectorFactory : public cInspectorFactory
{
  public:
    TObjInspectorFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return true;}
    int inspectorType() {return INSP_OBJECT;}
    double qualityAsDefault(cObject *object) {return 1.0;}

    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TObjInspector(object, type, geom, data);
    }
};

Register_InspectorFactory(TObjInspectorFactory);


TObjInspector::TObjInspector(cObject *obj,int typ,const char *geom,void *dat) :
    TInspector(obj,typ,geom,dat)
{
    fieldspage = NULL;
}

void TObjInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "create_objinspector ", windowname, " \"", geometry, "\"", NULL ));

   // try if it has a descriptor
   cStructDescriptor *sd = object->createDescriptor();
   delete sd;
   if (sd)
   {
       char fieldspagewidget[256];
       sprintf(fieldspagewidget, "%s.nb.fields", windowname);
       fieldspage = new TStructPanel(fieldspagewidget, object);
   }
}

void TObjInspector::update()
{
   TInspector::update();

   setLabel(".nb.info.name.e",object->name());
   setLabel(".nb.info.fullpath.e",object->fullPath().c_str());
   setLabel(".nb.info.class.e",object->className());

   setLabel(".nb.info.info.e",object->info().c_str());
   setLabel(".nb.info.details.e",object->detailedInfo().c_str());

   if (fieldspage)
       fieldspage->update();
}

void TObjInspector::writeBack()
{
   if (fieldspage)
       fieldspage->writeBack();

   TInspector::writeBack();    // must be there after all changes
}

int TObjInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   if (fieldspage)
       return fieldspage->inspectorCommand(interp, argc, argv);
   return TCL_ERROR;
}

TObjInspector::~TObjInspector()
{
   delete fieldspage;
}

//=======================================================================

class TContainerInspectorFactory : public cInspectorFactory
{
  public:
    TContainerInspectorFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return true;}
    int inspectorType() {return INSP_OBJECT;}

    double qualityAsDefault(cObject *object) {
        if (dynamic_cast<cArray *>(object) || dynamic_cast<cQueue *>(object) ||
            dynamic_cast<cMessageHeap *>(object) || dynamic_cast<cDefaultList *>(object) ||
            dynamic_cast<cSimulation *>(object)
           )
            return 2.0;
        else
            return 0.9;
    }

    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TContainerInspector(object, type, geom, data);
    }
};

Register_InspectorFactory(TContainerInspectorFactory);


TContainerInspector::TContainerInspector(cObject *obj,int typ,const char *geom,void *dat) :
    TInspector(obj,typ,geom,dat)
{
   char *opt = (char *)dat;
   deep = opt && opt[0]=='d';
}

void TContainerInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.
   strcpy(listbox,windowname); strcat(listbox,".main.list");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkApplication()->getInterp();
   char *typelist = " (default) object container graphical";
   CHK(Tcl_VarEval(interp, "create_containerinspector ", windowname, " \"", geometry, "\"", typelist, NULL));
}

void TContainerInspector::update()
{
   TInspector::update();

   deleteInspectorListbox( "" );
   fillInspectorListbox("", object, deep);
}

//=======================================================================

class TMessageInspectorFactory : public cInspectorFactory
{
  public:
    TMessageInspectorFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cMessage *>(obj)!=NULL;}
    int inspectorType() {return INSP_OBJECT;}
    double qualityAsDefault(cObject *object) {return 2.0;}

    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TMessageInspector(object, type, geom, data);
    }
};

Register_InspectorFactory(TMessageInspectorFactory);


TMessageInspector::TMessageInspector(cObject *obj,int typ,const char *geom,void *dat) :
    TInspector(obj,typ,geom,dat)
{
   fieldspage = NULL;
   controlinfopage = NULL;
}

void TMessageInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = getTkApplication()->getInterp();
   CHK(Tcl_VarEval(interp, "create_messageinspector ", windowname, " \"", geometry, "\"", NULL ));

   // try if it has a descriptor
   cStructDescriptor *sd = object->createDescriptor();
   delete sd;
   if (sd)
   {
       char fieldspagewidget[256];
       sprintf(fieldspagewidget, "%s.nb.fields", windowname);
       fieldspage = new TStructPanel(fieldspagewidget, object);
   }

   char controlinfopagewidget[256];
   sprintf(controlinfopagewidget, "%s.nb.controlinfo", windowname);
   controlinfopage = new TStructPanel(controlinfopagewidget, NULL);

}

void TMessageInspector::update()
{
   TInspector::update();

   cMessage *msg = static_cast<cMessage *>(object);

   setEntry(".nb.info.name.e", msg->name() );
   setEntry(".nb.info.kind.e", (long)msg->kind() );
   setEntry(".nb.info.length.e", (long)msg->length() );
   setEntry(".nb.info.prio.e", (long)msg->priority() );
   setEntry(".nb.info.error.e", (long)msg->hasBitError() );
   setEntry(".nb.send.tstamp.e", msg->timestamp() );
   setLabel(".nb.send.created.e", simtimeToStr(msg->creationTime()) );
   setLabel(".nb.send.sent.e", simtimeToStr(msg->sendingTime()) );
   setLabel(".nb.send.arrived.e", simtimeToStr(msg->arrivalTime()) );

   setInspectButton(".nb.info.encapmsg", msg->encapsulatedMsg(), false, INSP_DEFAULT);
   setInspectButton(".nb.send.owner", msg->owner(), true, INSP_DEFAULT);
   setInspectButton(".nb.send.src",simulation.module(msg->senderModuleId()), true, INSP_DEFAULT);
   setInspectButton(".nb.send.dest",simulation.module(msg->arrivalModuleId()), true, INSP_DEFAULT);

   if (fieldspage)
       fieldspage->update();

   controlinfopage->setObject(msg->controlInfo());
   controlinfopage->update();

   deleteInspectorListbox( ".nb.params" );
   fillInspectorListbox(".nb.params", &msg->parList(), false);
}

void TMessageInspector::writeBack()
{
   cMessage *msg = static_cast<cMessage *>(object);

   msg->setName( getEntry(".nb.info.name.e") );
   msg->setKind( atol(getEntry(".nb.info.kind.e")) );
   msg->setLength( atol( getEntry(".nb.info.length.e")) );
   msg->setPriority( atol( getEntry(".nb.info.prio.e")) );
   msg->setBitError( atol( getEntry(".nb.info.error.e"))!=0 );
   msg->setTimestamp( atof( getEntry(".nb.send.tstamp.e")) );

   if (fieldspage)
       fieldspage->writeBack();

   controlinfopage->writeBack();

   TInspector::writeBack();    // must be there after all changes
}

int TMessageInspector::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   if (fieldspage)
       return fieldspage->inspectorCommand(interp, argc, argv);
   return TCL_ERROR;
}

TMessageInspector::~TMessageInspector()
{
   delete fieldspage;
   delete controlinfopage;
}


//=======================================================================
class TWatchInspectorFactory : public cInspectorFactory
{
  public:
    TWatchInspectorFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cWatch *>(obj)!=NULL;}
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

   cWatch *watch = static_cast<cWatch *>(object);

   char *type,val[128];
   void *p = watch->pointer();
   switch (watch->typeChar())
   {
         case 'c':  sprintf(val, "'%c' (%d,0x%x)",
                                 *(char *)p, *(char *)p, *(char *)p );
                    type = "char ";
                    break;
         case 'i':  sprintf(val, "%d (%uU, 0x%x)",
                                 *(int *)p, *(int *)p, *(int *)p );
                    type = "int ";
                    break;
         case 'l':  sprintf(val, "%ldL (%luLU, 0x%lx)",
                                 *(long *)p, *(long *)p, *(long *)p );
                    type = "long ";
                    break;
         case 'd':  sprintf(val, "%f",
                                 *(double *)p );
                    type = "double ";
                    break;
         case 's':  if (*(char **)p==NULL)
                       sprintf(val, "NULL");
                    else
                       sprintf(val, "\"%.120s\"", *(char **)p);
                    type = "char *";
                    break;
         case 'b':  sprintf(val, "%s", (*(bool *)p ? "true" : "false"));
                    type = "bool ";
                    break;
         case 'o':  if (*(cObject **)p==NULL)
                       sprintf(val, "NULL");
                    else
                       sprintf(val, "not NULL");
                    type = "cObject *";
                    break;
         default:   strcpy(val,"???");
                    type = "? ";
   }
   char name[64];
   sprintf(name,"%s%.50s = ",type,watch->name());
   setLabel(".main.name.l", name );
   setEntry(".main.name.e", val );
}

void TWatchInspector::writeBack()
{
   Tcl_Interp *interp = getTkApplication()->getInterp();
   cWatch *watch = static_cast<cWatch *>(object);

   const char *s = getEntry(".main.name.e");
   void *p = watch->pointer();
   switch (watch->typeChar())
   {
         case 'c':  *(char *)p = s[1]; break;
         case 'i':  sscanf(s,"%d", (int *)p); break;
         case 'l':  sscanf(s,"%ld",(long *)p); break;
         case 'd':  sscanf(s,"%lf",(double *)p); break;
         case 'b':  *(bool *)p = (s[0]=='1' || s[0]=='t' || s[0]=='y' || s[0]=='T' || s[0]=='Y'); break;
         default:   CHK(Tcl_Eval(interp,"messagebox {Warning}"
                  " {You can only change char, int, bool, long or double watches} info ok"));
   }
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

