//==========================================================================
//   STRUCTINSP.CC -
//            part of the Tcl/Tk environment of
//                             OMNeT++
//
//  Implementation of
//    cStructDescriptor-based inspector
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <math.h>

#include "cstruct.h"

#include "tkapp.h"
#include "tklib.h"
#include "tkinsp.h"

//=======================================================================

TStructInspector::TStructInspector(cObject *obj,int typ,void *dat) :
    TInspector(obj,typ,dat)
{
}

void TStructInspector::createWindow()
{
   TInspector::createWindow(); // create window name etc.

   // create inspector window by calling the specified proc with
   // the object's pointer. Window name will be like ".ptr80003a9d-1"
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;
   CHK(Tcl_VarEval(interp, "create_structinspector ", windowname, NULL ));
}

void TStructInspector::update()
{
   TInspector::update();

   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->interp;

   // get descriptor object
   cStructDescriptor *sd = cStructDescriptor::createDescriptorFor( object );
   if (!sd)
   {
       CHK(Tcl_VarEval(interp, windowname, ".main.txt delete 1.0 end", NULL));
       CHK(Tcl_VarEval(interp, windowname, ".main.txt insert 1.0 {No cStructDescriptor registered for this class!}", NULL));
   }

   // print everything in a 4K buffer, then display it in a text control.
   // this is a temporary solution, should be replaced by something more professional!
   char buf[4000]; // FIXME! hope 4K will be enough
   char val[128];
   char *s = buf;
   for (int fld=0; fld<sd->getFieldCount(); fld++)
   {
       int type = sd->getFieldType(fld);
       bool isarray = type==cStructDescriptor::FT_BASIC_ARRAY ||
                      type==cStructDescriptor::FT_SPECIAL_ARRAY ||
                      type==cStructDescriptor::FT_STRUCT_ARRAY;

       if (!isarray)
       {
           switch(type)
           {
               // FIXME: handle enumnames too!
               case cStructDescriptor::FT_BASIC:
                   sd->getFieldAsString(fld, 0, val, 128); // FIXME: error handling!
                   sprintf(s,"%s%s = %s\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), val);
                   s+=strlen(s);
                   break;
               case cStructDescriptor::FT_SPECIAL:
                   sprintf(s,"%s%s = ...\n", sd->getFieldTypeString(fld), sd->getFieldName(fld)); //FIXME
                   s+=strlen(s);
                   break;
               case cStructDescriptor::FT_STRUCT:
                   sprintf(s,"%s%s = {...}\n", sd->getFieldTypeString(fld), sd->getFieldName(fld)); //FIXME
                   s+=strlen(s);
                   break;
               default:
                   sprintf(s,"%s%s = (unknown type)\n", sd->getFieldTypeString(fld), sd->getFieldName(fld));
                   s+=strlen(s);
           }
       }
       else
       {
           int size = sd->getArraySize(fld);
           for (int i=0; i<size; i++)
           {
               switch(type)
               {
                   // FIXME: handle enumnames too!
                   case cStructDescriptor::FT_BASIC_ARRAY:
                       sd->getFieldAsString(fld, i, val, 128); // FIXME: error handling!
                       sprintf(s,"%s%s[%d] = %s\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), i, val);
                       s+=strlen(s);
                       break;
                   case cStructDescriptor::FT_SPECIAL_ARRAY:
                       sprintf(s,"%s%s[%d] = ...\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), i); //FIXME
                       s+=strlen(s);
                       break;
                   case cStructDescriptor::FT_STRUCT_ARRAY:
                       sprintf(s,"%s%s[%d] = {...}\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), i); //FIXME
                       s+=strlen(s);
                       break;
                   default:
                       sprintf(s,"%s%s[%d] = (unknown type)\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), i);
                       s+=strlen(s);
               }
           }
       }
   }
   delete sd;

   CHK(Tcl_VarEval(interp, windowname, ".main.txt delete 1.0 end", NULL));
   CHK(Tcl_VarEval(interp, windowname, ".main.txt insert 1.0 {", buf, "}", NULL));

}

void TStructInspector::writeBack()
{
   // TBD
}

int TStructInspector::inspectorCommand(Tcl_Interp *interp, int argc, char **argv)
{
   // a Tcl-based struct inspector might use this...

   if (argc<1) return TCL_ERROR;

   cStructDescriptor *sd = cStructDescriptor::createDescriptorFor( object ); // FIXME: not very effective!

   if (strcmp(argv[0],"count")==0)   // 'opp_inspectorcommand <inspector> fieldcount ...'
   {
      if (argc!=1) return TCL_ERROR;
      sprintf(interp->result, "%d", sd->getFieldCount());
      return TCL_OK;
   }

   if (strcmp(argv[0],"type")==0)   // 'opp_inspectorcommand <inspector> fieldtype ...'
   {
      if (argc!=2) return TCL_ERROR;
      int fld = atoi(argv[1]);
      int type = sd->getFieldType(fld);
      switch(type)
      {
          case cStructDescriptor::FT_BASIC:
              interp->result = "basic"; break;
          case cStructDescriptor::FT_SPECIAL:
              interp->result = "special"; break;
          case cStructDescriptor::FT_STRUCT:
              interp->result = "struct"; break;
          case cStructDescriptor::FT_BASIC_ARRAY:
              interp->result = "basic array"; break;
          case cStructDescriptor::FT_SPECIAL_ARRAY:
              interp->result = "special array"; break;
          case cStructDescriptor::FT_STRUCT_ARRAY:
              interp->result = "struct array"; break;
          default:
              interp->result = "invalid";
      }
      return TCL_OK;
   }

   if (strcmp(argv[0],"name")==0)   // 'opp_inspectorcommand <inspector> fieldname <fldid> ...'
   {
      if (argc!=2) return TCL_ERROR;
      int fld = atoi(argv[1]);
      strcpy(interp->result, sd->getFieldName(fld));
      return TCL_OK;
   }

   if (strcmp(argv[0],"typename")==0)   // 'opp_inspectorcommand <inspector> fieldtypename <fldid> ...'
   {
      if (argc!=2) return TCL_ERROR;
      int fld = atoi(argv[1]);
      strcpy(interp->result, sd->getFieldTypeString(fld));
      return TCL_OK;
   }

   if (strcmp(argv[0],"arraysize")==0)   // 'opp_inspectorcommand <inspector> fieldarraysize <fldid> ...'
   {
      if (argc!=2) return TCL_ERROR;
      int fld = atoi(argv[1]);
      sprintf(interp->result, "%d", sd->getArraySize(fld));
      return TCL_OK;
   }

   if (strcmp(argv[0],"value")==0)   // 'opp_inspectorcommand <inspector> fieldvalue <fldid> ?index?...'
   {
      if (argc!=2 && argc!=3) return TCL_ERROR;
      int fld = atoi(argv[1]);
      int i=0;
      if (argc==4)
         i = atoi(argv[2]);
      if (!sd->getFieldAsString(fld, i, interp->result, 128))
         return TCL_ERROR;
      return TCL_OK;
   }

   if (strcmp(argv[0],"enumname")==0)   // 'opp_inspectorcommand <inspector> fieldenumname <fldid> ?index?...'
   {
      if (argc!=2) return TCL_ERROR;
      int fld = atoi(argv[1]);
      strcpy(interp->result, sd->getFieldEnumName(fld));
      return TCL_OK;
   }

   if (strcmp(argv[0],"structname")==0)   // 'opp_inspectorcommand <inspector> fieldenumname <fldid> ?index?...'
   {
      if (argc!=2) return TCL_ERROR;
      int fld = atoi(argv[1]);
      strcpy(interp->result, sd->getFieldStructName(fld));
      return TCL_OK;
   }

   return TCL_ERROR;
}

