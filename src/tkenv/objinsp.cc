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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <math.h>

#include "omnetpp.h"

#include "tkapp.h"
#include "tklib.h"
#include "tkinsp.h"
#include "arrow.h"

//=======================================================================
// Inspector type code <--> string conversions

struct InspTypeName {int code; char *namestr;} insp_typenames[] =
{
     { INSP_DEFAULT,          "(default)"     },
     { INSP_OBJECT,           "As Object"     },
     { INSP_GRAPHICAL,        "As Graphics"   },
     { INSP_CONTAINER,        "As Container"  },
     { INSP_MODULEOUTPUT,     "Module output" },
     { INSP_LOCALVARS,        "Local vars"    },
     { INSP_CLASSMEMBERS,     "Class members" },
     { INSP_PARAMETERS,       "Parameters"    },
     { INSP_GATES,            "Gates"         },
     { INSP_SUBMODS,          "Submodules"    },
     { -1,                     NULL           }
};

char *insptype_name_from_code( int code )
{
   for (int i=0; insp_typenames[i].namestr!=NULL; i++)
      if (insp_typenames[i].code == code)
         return insp_typenames[i].namestr;
   return NULL;
}

int insptype_code_from_name( char *namestr )
{
   for (int i=0; insp_typenames[i].namestr!=NULL; i++)
      if (strcmp(insp_typenames[i].namestr, namestr)==0)
         return insp_typenames[i].code;
   return -1;
}

//=======================================================================
// TInspector: base class for all inspector types
//             member functions

TInspector::TInspector(cObject *obj, int typ, void *dat) : cObject("",NULL)
{
   object = obj;
   type = typ;
   data = dat;

   windowname[0] = '\0'; // no window exists
   windowtitle[0] = '\0';
}

TInspector::~TInspector()
{
   if (windowname[0])
   {
      Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
      CHK(Tcl_VarEval(interp, "destroy ", windowname, NULL ));
   }
}

void TInspector::createWindow()
{
   windowname[0] = '.';
   ptrToStr( object, windowname+1 );
   sprintf( windowname+strlen(windowname), "-%d",type);

   // derived classes will also call Tcl_Eval() to actually create the
   // Tk window by invoking a procedure in inspect.tcl
}

bool TInspector::windowExists()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "winfo exists ", windowname, NULL ));
   return interp->result[0]=='1';
}

void TInspector::showWindow()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "wm deiconify ", windowname, NULL ));
   CHK(Tcl_VarEval(interp, "raise ", windowname, NULL ));
   CHK(Tcl_VarEval(interp, "focus ", windowname, NULL ));
}

void TInspector::update()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;

   // update window title (only if changed)
   //  (always updating the title produced many unnecessary redraws under fvwm2-95)
   char newtitle[128];
   sprintf(newtitle, "(%.20s) %.100s",object->className(), object->fullPath());
   if (strcmp(newtitle, windowtitle)!=0)
   {
       strcpy( windowtitle, newtitle);
       CHK(Tcl_VarEval(interp, "wm title ",windowname," {",windowtitle,"}",NULL));
   }
}

void TInspector::setEntry(const char *entry, const char *val)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",val,"}",NULL));
}

void TInspector::setEntry( const char *entry, long l )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",buf,"}",NULL));
}

void TInspector::setEntry( const char *entry, double d )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   char buf[24];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowname,entry," delete 0 end;",
                           windowname,entry," insert 0 {",buf,"}",NULL));
}

void TInspector::setLabel( const char *label, const char *val )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",val,"}",NULL));
}

void TInspector::setLabel( const char *label, long l )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   char buf[16];
   sprintf(buf, "%ld", l );
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",buf,"}",NULL));
}

void TInspector::setLabel( const char *label, double d )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   char buf[16];
   sprintf(buf, "%g", d );
   CHK(Tcl_VarEval(interp, windowname,label," config -text {",buf,"}",NULL));
}

const char *TInspector::getEntry( const char *entry )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, windowname,entry," get",NULL));
   return interp->result;
}

void TInspector::setInspectButton( const char *button, cObject *object, int type )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
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

void TInspector::setButtonText( const char *button, const char *text)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, windowname,button," config -text {",text,"}",NULL));
}

void TInspector::deleteInspectorListbox( const char *listbox)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp,windowname,listbox,".main.list delete 0 end",NULL));
}

void TInspector::fillInspectorListbox(const char *listbox, cObject *object,
                             InfoFunc infofunc,bool deep)
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowname,listbox);
   collection( object, interp, w, infofunc, deep);

   // set "number of items" display
   CHK(Tcl_VarEval(interp,w," index end",NULL));
   sprintf(w, "%s.label", listbox);
   sprintf(buf,"%s objects in (%s) %s:", interp->result,object->className(),object->fullPath());
   setLabel(w, buf);
}

void TInspector::fillModuleListbox(const char *listbox, cModule *parent,
                                InfoFunc infofunc,bool simpleonly,bool deep )
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   char w[256], buf[256];
   sprintf(w, "%s%s.main.list", windowname,listbox);
   modcollection( parent, interp, w, infofunc, simpleonly, deep);

   // set "number of items" display
   CHK(Tcl_VarEval(interp,w," index end",NULL));
   sprintf(w, "%s.label", listbox);
   sprintf(buf,"%s modules", interp->result);
   setLabel(w, buf);
}


//=======================================================================
TObjInspector::TObjInspector(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TObjInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_objinspector ", windowname, NULL ));
}

void TObjInspector::update()
{
   TInspector::update();

   char buf[129];
   object->info(buf);
   setLabel(".main.info",buf);
}

//=======================================================================
TContainerInspector::TContainerInspector(cObject *obj,int typ,void *dat,InfoFunc f) :
    TInspector(obj,typ,dat)
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
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   char *typelist = " (default) object container graphical Module@output";
   CHK(Tcl_VarEval(interp, "create_containerinspector ", windowname, typelist, NULL));
}

void TContainerInspector::update()
{
   TInspector::update();

   deleteInspectorListbox( "" );
   fillInspectorListbox("", object, infofunc, deep);
}

//=======================================================================
TMessageInspector::TMessageInspector(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TMessageInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_messageinspector ", windowname, NULL ));
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

   char buf[64];

   cMessage *emsg = msg->encapsulatedMsg();
   setInspectButton(".nb.info.encapmsg", emsg, INSP_DEFAULT);
   if (emsg) sprintf(buf,"Encapsulated: %.50s", emsg->name());
       else strcpy(buf,"-no encapsulated msg-");
   setButtonText(".nb.info.encapmsg", buf);

   cObject *obj;
   setInspectButton(".nb.send.owner", obj=msg->owner(), INSP_DEFAULT);
   if (obj) sprintf(buf,"Owner: %.50s", obj->fullPath());
       else strcpy(buf,"-no owner-");
   setButtonText(".nb.send.owner", buf);

   cModule *mod;
   setInspectButton(".nb.send.src",mod=&(simulation[msg->senderModuleId()]), INSP_DEFAULT);
   if (mod) sprintf(buf,"Src: #%d %.50s", mod->id(), mod->fullPath());
       else strcpy(buf,"Src: n/a");
   setButtonText(".nb.send.src", buf);
   setInspectButton(".nb.send.dest",mod=&(simulation[msg->arrivalModuleId()]), INSP_DEFAULT);
   if (mod) sprintf(buf,"Dest: #%d %.50s", mod->id(), mod->fullPath());
       else strcpy(buf,"Dest: n/a");
   setButtonText(".nb.send.dest", buf);

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

   TInspector::writeBack();    // must be there after all changes
}

//=======================================================================

TWatchInspector::TWatchInspector(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TWatchInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_watchinspector ", windowname, NULL ));
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
         case 'l':  sprintf(val, "%dL (%uLU, 0x%x)",
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
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   cWatch *watch = (cWatch *)object;

   const char *s = getEntry(".main.name.e");
   void *p = watch->pointer();
   switch (watch->typeChar())
   {
         case 'c':  *(char *)p = s[1]; break;
         case 'i':  sscanf(s,"%d",p); break;
         case 'l':  sscanf(s,"%ld",p); break;
         case 'd':  sscanf(s,"%f",p); break;
         default:   CHK(Tcl_Eval(interp,"messagebox {Warning}"
                  " {You can only change char, int, long or double watches} info ok"));
   }
   TInspector::writeBack();    // must be there after all changes
}

//=======================================================================

TParInspector::TParInspector(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TParInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_parinspector ", windowname, NULL ));
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

   cObject *obj;
   setInspectButton(".bot.indirection",obj=p->redirection(),INSP_DEFAULT);
   if (obj) sprintf(buf,"Value taken from: %.30s",obj->fullPath());
   setButtonText(".bot.indirection", obj ? buf : "-not redirected-");
}

void TParInspector::writeBack()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
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
TPacketInspector::TPacketInspector(cObject *obj,int typ,void *dat) :
    TMessageInspector(obj,typ,dat)
{
}

void TPacketInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_packetinspector ", windowname, NULL ));
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


