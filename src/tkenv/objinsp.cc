//==========================================================================
//   OBJINSP.CC -
//            part of the Tcl/Tk environment of
//                             OMNeT++
//
//  Implementation of
//    inspectors
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "create_objinspector ", windowname, " \"", geometry, "\"", NULL ));

   if (cStructDescriptor::hasDescriptor(object->className()))
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
   setLabel(".nb.info.fullpath.e",object->fullPath());
   setLabel(".nb.info.class.e",object->className());

   static char buf[MAX_OBJECTINFO];
   object->info(buf);
   setLabel(".nb.info.info.e",buf);

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
            dynamic_cast<cMessageHeap *>(object) || dynamic_cast<cHead *>(object) ||
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


TContainerInspector::TContainerInspector(cObject *obj,int typ,const char *geom,void *dat,InfoFunc f) :
    TInspector(obj,typ,geom,dat)
{
   char *opt = (char *)dat;
   deep = opt && opt[0]=='d';

   infofunc = f;
   if (!infofunc)
       infofunc = deep ? infofunc_typeandfullpath : infofunc_infotext;
}

void TContainerInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.
   strcpy(listbox,windowname); strcat(listbox,".main.list");

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   char *typelist = " (default) object container graphical";
   CHK(Tcl_VarEval(interp, "create_containerinspector ", windowname, " \"", geometry, "\"", typelist, NULL));
}

void TContainerInspector::update()
{
   TInspector::update();

   deleteInspectorListbox( "" );
   fillInspectorListbox("", object, infofunc, deep);
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
}

void TMessageInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "create_messageinspector ", windowname, " \"", geometry, "\"", NULL ));

   if (cStructDescriptor::hasDescriptor(object->className()))
   {
       char fieldspagewidget[256];
       sprintf(fieldspagewidget, "%s.nb.fields", windowname);
       fieldspage = new TStructPanel(fieldspagewidget, object);
   }
}

void TMessageInspector::update()
{
   TInspector::update();

   cMessage *msg = (cMessage *)object;

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

   deleteInspectorListbox( ".nb.params" );
   fillInspectorListbox(".nb.params", &msg->parList(), infofunc_infotext, false);
}

void TMessageInspector::writeBack()
{
   cMessage *msg = (cMessage *)object;

   msg->setName( getEntry(".nb.info.name.e") );
   msg->setKind( atol(getEntry(".nb.info.kind.e")) );
   msg->setLength( atol( getEntry(".nb.info.length.e")) );
   msg->setPriority( atol( getEntry(".nb.info.prio.e")) );
   msg->setBitError( atol( getEntry(".nb.info.error.e"))!=0 );
   msg->setTimestamp( atof( getEntry(".nb.send.tstamp.e")) );

   if (fieldspage)
       fieldspage->writeBack();

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
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "create_watchinspector ", windowname, " \"", geometry, "\"", NULL ));
}

void TWatchInspector::update()
{
   TInspector::update();

   cWatch *watch = (cWatch *)object;

   char *type,val[128];
   void *p = watch->pointer();
   switch (watch->typeChar())
   {
         case 'c':  sprintf(val, "int'%c' (%d,0x%x)",
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
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   cWatch *watch = (cWatch *)object;

   const char *s = getEntry(".main.name.e");
   void *p = watch->pointer();
   switch (watch->typeChar())
   {
         case 'c':  *(char *)p = s[1]; break;
         case 'i':  sscanf(s,"%d", (int *)p); break;
         case 'l':  sscanf(s,"%ld",(long *)p); break;
         case 'd':  sscanf(s,"%lf",(double *)p); break;
         default:   CHK(Tcl_Eval(interp,"messagebox {Warning}"
                  " {You can only change char, int, long or double watches} info ok"));
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
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "create_parinspector ", windowname, " \"", geometry, "\"", NULL ));
}

void TParInspector::update()
{
   TInspector::update();

   char buf[129];
   cPar *p = (cPar *)object;

   p->getAsText(buf,128);
   setEntry(".main.value.e", buf );

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
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   cPar *p = (cPar *)object;

   char newtype = getEntry(".main.newtype.e")[0];
   if (!newtype) newtype = '?';

   if( p->setFromText( getEntry(".main.value.e"), newtype) )
   {
      setEntry(".main.newtype.e", "" );
   }
   else
   {
      if (newtype=='?')
      {
         CHK(Tcl_Eval(interp,"messagebox {Warning}"
                " {Incorrect setting, value not changed} info ok"));
      }
      else
      {
         CHK(Tcl_Eval(interp,"messagebox {Warning}"
                " {Incorrect setting. Try auto type selection?} question yesno"));
         if (interp->result[0]=='y')
         {
            if( p->setFromText( getEntry(".main.value.e"), '?' ))
                setEntry(".main.newtype.e", "" );
            else
                CHK(Tcl_Eval(interp,"messagebox {Warning}"
                  " {Incorrect setting, value not changed} info ok"));
         }
      }
   }
   p->setPrompt( getEntry(".main.prompt.e") );
   p->setInput( getEntry(".main.input.e")[0]=='1' );

   TInspector::writeBack();    // must be there after all changes
}


//=======================================================================

class TPacketInspectorFactory : public cInspectorFactory
{
  public:
    TPacketInspectorFactory(const char *name) : cInspectorFactory(name) {}

    bool supportsObject(cObject *obj) {return dynamic_cast<cPacket *>(obj)!=NULL;}
    int inspectorType() {return INSP_OBJECT;}
    double qualityAsDefault(cObject *object) {return 3.0;}

    TInspector *createInspectorFor(cObject *object,int type,const char *geom,void *data) {
        return new TPacketInspector(object, type, geom, data);
    }
};

Register_InspectorFactory(TPacketInspectorFactory);


TPacketInspector::TPacketInspector(cObject *obj,int typ,const char *geom,void *dat) :
    TMessageInspector(obj,typ,geom,dat)
{
}

void TPacketInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();
   CHK(Tcl_VarEval(interp, "create_packetinspector ", windowname, " \"", geometry, "\"", NULL ));
}

void TPacketInspector::update()
{
   TMessageInspector::update();

   cPacket *pkt = (cPacket *)object;

   setEntry(".nb.info.protocol.e", (long)pkt->protocol() );
   setEntry(".nb.info.pdu.e",      (long)pkt->pdu() );
}

void TPacketInspector::writeBack()
{
   cPacket *pkt = (cPacket *)object;

   pkt->setProtocol( (short)atol( getEntry(".nb.info.protocol.e")) );
   pkt->setPdu( (short)atol( getEntry(".nb.info.pdu.e")) );

   TMessageInspector::writeBack();    // must be there after all changes
}




