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
  Copyright (C) 1992-2002 Andras Varga
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
#include "inspfactory.h"
#include "structinsp.h"


void _dummy_for_structinsp() {}


TStructPanel::TStructPanel(const char *widgetname, cObject *obj) :
    TInspectorPanel(widgetname,obj)
{
}

static void flush_if_needed(char *buf, char *&s, int limit, Tcl_Interp *interp, const char *widgetname)
{
    // if there are more than limit chars in the buffer
    if (s-buf>=limit)
    {
        CHK(Tcl_VarEval(interp, widgetname, ".txt insert insert {", buf, "}", NULL));
        s = buf; // reset buffer
    }
}

void TStructPanel::update()
{
   Tcl_Interp *interp = ((TOmnetTkApp *)ev.app)->getInterp();

   // delete display
   CHK(Tcl_VarEval(interp, widgetname, ".txt delete 1.0 end", NULL));

   // get descriptor object
   cStructDescriptor *sd = cStructDescriptor::createDescriptorFor( object );
   if (!sd)
   {
       CHK(Tcl_VarEval(interp, widgetname, ".txt insert 1.0 {No cStructDescriptor registered for this class!}", NULL));
   }

   // print everything in a buffer, and periodically display it as the buffer gets full.
   // this is a temporary solution, should be replaced by something more professional!

   const int bufsize = 2048;     // buffer size
   const int flushlimit = bufsize-256; // one sprintf() should be less than 256 chars
   char buf[bufsize];

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
               case cStructDescriptor::FT_BASIC:
                   sd->getFieldAsString(fld, 0, val, 128); // FIXME: error handling!
                   if (sd->getFieldEnumName(fld))
                   {
                       // display enum value as int and as string
                       cEnum* enm = findEnum(sd->getFieldEnumName(fld));
                       if (enm)
                       {
                           int key = atol(val);
                           sprintf(val, "%d (%s)", key, enm->stringFor(key));
                       }
                   }
                   sprintf(s,"%s\t%s = \t%s\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), val);
                   s+=strlen(s);
                   flush_if_needed(buf, s, flushlimit, interp, widgetname);

                   break;
               case cStructDescriptor::FT_SPECIAL:
                   sprintf(s,"%s\t%s = \t...\n", sd->getFieldTypeString(fld), sd->getFieldName(fld)); //FIXME
                   s+=strlen(s);
                   flush_if_needed(buf, s, flushlimit, interp, widgetname);
                   break;
               case cStructDescriptor::FT_STRUCT:
                   sprintf(s,"%s\t%s = \t{...}\n", sd->getFieldTypeString(fld), sd->getFieldName(fld)); //FIXME
                   s+=strlen(s);
                   flush_if_needed(buf, s, flushlimit, interp, widgetname);
                   break;
               default:
                   sprintf(s,"%s\t%s = \t(unknown type)\n", sd->getFieldTypeString(fld), sd->getFieldName(fld));
                   s+=strlen(s);
                   flush_if_needed(buf, s, flushlimit, interp, widgetname);
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
                       if (sd->getFieldEnumName(fld))
                       {
                           // display enum value as int and as string
                           cEnum* enm = findEnum(sd->getFieldEnumName(fld));
                           if (enm)
                           {
                               int key = atol(val);
                               sprintf(val, "%d (%s)", key, enm->stringFor(key));
                           }
                       }
                       sprintf(s,"%s\t%s[%d] = \t%s\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), i, val);
                       s+=strlen(s);
                       flush_if_needed(buf, s, flushlimit, interp, widgetname);
                       break;
                   case cStructDescriptor::FT_SPECIAL_ARRAY:
                       sprintf(s,"%s\t%s[%d] = \t...\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), i); //FIXME
                       s+=strlen(s);
                       flush_if_needed(buf, s, flushlimit, interp, widgetname);
                       break;
                   case cStructDescriptor::FT_STRUCT_ARRAY:
                       sprintf(s,"%s\t%s[%d] = \t{...}\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), i); //FIXME
                       s+=strlen(s);
                       flush_if_needed(buf, s, flushlimit, interp, widgetname);
                       break;
                   default:
                       sprintf(s,"%s\t%s[%d] = \t(unknown type)\n", sd->getFieldTypeString(fld), sd->getFieldName(fld), i);
                       s+=strlen(s);
                       flush_if_needed(buf, s, flushlimit, interp, widgetname);
               }
           }
       }
   }
   delete sd;

   // flush the rest
   flush_if_needed(buf, s, 0, interp, widgetname);
}

void TStructPanel::writeBack()
{
   // TBD
}

int TStructPanel::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   // a Tcl-based struct inspector might use this...

   if (argc<1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   cStructDescriptor *sd = cStructDescriptor::createDescriptorFor( object ); // FIXME: not very effective!

   if (strcmp(argv[0],"count")==0)   // 'opp_inspectorcommand <inspector> fieldcount ...'
   {
      if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      sprintf(interp->result, "%d", sd->getFieldCount()); // FIXME use Tcl_SetResult()
      return TCL_OK;
   }

   if (strcmp(argv[0],"type")==0)   // 'opp_inspectorcommand <inspector> fieldtype ...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      int type = sd->getFieldType(fld);
      switch(type)
      {
          case cStructDescriptor::FT_BASIC:
              Tcl_SetResult(interp, "basic", TCL_STATIC); break;
          case cStructDescriptor::FT_SPECIAL:
              Tcl_SetResult(interp, "special", TCL_STATIC); break;
          case cStructDescriptor::FT_STRUCT:
              Tcl_SetResult(interp, "struct", TCL_STATIC); break;
          case cStructDescriptor::FT_BASIC_ARRAY:
              Tcl_SetResult(interp, "basic array", TCL_STATIC); break;
          case cStructDescriptor::FT_SPECIAL_ARRAY:
              Tcl_SetResult(interp, "special array", TCL_STATIC); break;
          case cStructDescriptor::FT_STRUCT_ARRAY:
              Tcl_SetResult(interp, "struct array", TCL_STATIC); break;
          default:
              Tcl_SetResult(interp, "invalid", TCL_STATIC);
      }
      return TCL_OK;
   }

   if (strcmp(argv[0],"name")==0)   // 'opp_inspectorcommand <inspector> fieldname <fldid> ...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      strcpy(interp->result, sd->getFieldName(fld)); // FIXME use Tcl_SetResult()
      return TCL_OK;
   }

   if (strcmp(argv[0],"typename")==0)   // 'opp_inspectorcommand <inspector> fieldtypename <fldid> ...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      strcpy(interp->result, sd->getFieldTypeString(fld)); // FIXME use Tcl_SetResult()
      return TCL_OK;
   }

   if (strcmp(argv[0],"arraysize")==0)   // 'opp_inspectorcommand <inspector> fieldarraysize <fldid> ...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      sprintf(interp->result, "%d", sd->getArraySize(fld)); // FIXME use Tcl_SetResult()
      return TCL_OK;
   }

   if (strcmp(argv[0],"value")==0)   // 'opp_inspectorcommand <inspector> fieldvalue <fldid> ?index?...'
   {
      if (argc!=2 && argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      int i=0;
      if (argc==4)
         i = atoi(argv[2]);
      if (!sd->getFieldAsString(fld, i, interp->result, 128))  // FIXME use Tcl_SetResult()
         return TCL_ERROR;
      return TCL_OK;
   }

   if (strcmp(argv[0],"enumname")==0)   // 'opp_inspectorcommand <inspector> fieldenumname <fldid> ?index?...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      strcpy(interp->result, sd->getFieldEnumName(fld));   // FIXME use Tcl_SetResult()
      return TCL_OK;
   }

   if (strcmp(argv[0],"structname")==0)   // 'opp_inspectorcommand <inspector> fieldenumname <fldid> ?index?...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      strcpy(interp->result, sd->getFieldStructName(fld));   // FIXME use Tcl_SetResult()
      return TCL_OK;
   }

   return TCL_ERROR;
}

