//==========================================================================
//  STRUCTINSP.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Implementation of
//    cStructDescriptor-based inspector
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <math.h>

#include "cenum.h"
#include "cstruct.h"

#include "tkapp.h"
#include "tklib.h"
#include "inspfactory.h"
#include "structinsp.h"


void _dummy_for_structinsp() {}



TStructPanel::TStructPanel(const char *widgetname, cPolymorphic *obj) :
    TInspectorPanel(widgetname,obj)
{
}

void TStructPanel::flushIfNeeded(int limit)
{
    // bump write pointer
    writeptr += strlen(writeptr);

    // flush if there are more than limit chars in the buffer
    if (writeptr-buf>=limit)
    {
        Tcl_Interp *interp = getTkApplication()->getInterp();
        TclQuotedString quotedstr(buf);
        CHK(Tcl_VarEval(interp, widgetname, ".txt insert end ", quotedstr.get(), " field", NULL));
        writeptr = buf; // reset writeptr to beginning of buffer
    }
}


void TStructPanel::displayStruct(void *object, cStructDescriptor *sd, int level)
{
   // print everything in a buffer, and periodically display it as the buffer gets full.
   // this is a temporary solution, should be replaced by something more professional!

   int indent = level*4;

   for (int fld=0; fld<sd->getFieldCount(object); fld++)
   {
       int type = sd->getFieldType(object, fld);
       bool isarray = type==cStructDescriptor::FT_BASIC_ARRAY ||
                      type==cStructDescriptor::FT_STRUCT_ARRAY;
       cStructDescriptor *sd1;

       if (!isarray)
       {
           switch(type)
           {
               case cStructDescriptor::FT_BASIC:
                   tmpbuf[0]='\0';
                   sd->getFieldAsString(object, fld, 0, tmpbuf, MAXWRITE);
                   if (sd->getFieldEnumName(object, fld))
                   {
                       // display enum value as int and as string
                       cEnum* enm = cEnum::find(sd->getFieldEnumName(object, fld));
                       if (enm)
                       {
                           int key = atol(tmpbuf);
                           sprintf(tmpbuf, "%d (%s)", key, enm->stringFor(key));
                       }
                   }
                   sprintf(writeptr,"%*s%s %s  =  %s\n", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), tmpbuf);
                   flushIfNeeded(FLUSHLIMIT);
                   break;
               case cStructDescriptor::FT_STRUCT:
                   tmpbuf[0]='\0';
                   sd->getFieldAsString(object, fld, 0, tmpbuf, MAXWRITE);
                   sprintf(writeptr,"%*s%s %s  =  %s ", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), tmpbuf);
                   flushIfNeeded(FLUSHLIMIT);

                   sd1 = cStructDescriptor::getDescriptorFor(sd->getFieldStructName(object, fld));
                   if (!sd1)
                   {
                       sprintf(writeptr, (tmpbuf[0] ? "\n" : "{...}\n"));
                       flushIfNeeded(FLUSHLIMIT);
                   }
                   else
                   {
                       sprintf(writeptr,"{\n");
                       flushIfNeeded(FLUSHLIMIT);
                       displayStruct(sd->getFieldStructPointer(object, fld,0), sd1, level+1);
                       sprintf(writeptr,"%*s}\n", indent, "");
                       flushIfNeeded(FLUSHLIMIT);
                   }
                   break;
               default:
                   sprintf(writeptr,"%*s%s %s  =  (unknown type)\n", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld));
                   flushIfNeeded(FLUSHLIMIT);
           }
       }
       else
       {
           int size = sd->getArraySize(object, fld);
           for (int i=0; i<size; i++)
           {
               switch(type)
               {
                   case cStructDescriptor::FT_BASIC_ARRAY:
                       sd->getFieldAsString(object, fld, i, tmpbuf, MAXWRITE); // FIXME: error handling!
                       if (sd->getFieldEnumName(object, fld))
                       {
                           // display enum value as int and as string
                           cEnum* enm = cEnum::find(sd->getFieldEnumName(object, fld));
                           if (enm)
                           {
                               int key = atol(tmpbuf);
                               sprintf(tmpbuf, "%d (%s)", key, enm->stringFor(key));
                           }
                       }
                       sprintf(writeptr,"%*s%s %s[%d]  =  %s\n", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), i, tmpbuf);
                       flushIfNeeded(FLUSHLIMIT);
                       break;
                   case cStructDescriptor::FT_STRUCT_ARRAY:
                       tmpbuf[0]='\0';
                       sd->getFieldAsString(object, fld, i, tmpbuf, MAXWRITE);
                       sprintf(writeptr,"%*s%s %s[%d]  =  %s ", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), i, tmpbuf);
                       flushIfNeeded(FLUSHLIMIT);

                       sd1 = cStructDescriptor::getDescriptorFor(sd->getFieldStructName(object, fld));
                       if (!sd1)
                       {
                           sprintf(writeptr, (tmpbuf[0] ? "\n" : "{...}\n"));
                           flushIfNeeded(FLUSHLIMIT);
                       }
                       else
                       {
                           sprintf(writeptr,"{\n");
                           flushIfNeeded(FLUSHLIMIT);
                           displayStruct(sd->getFieldStructPointer(object, fld,i), sd1, level+1);
                           sprintf(writeptr,"}\n");
                           flushIfNeeded(FLUSHLIMIT);
                       }
                       break;
                   default:
                       sprintf(writeptr,"%*s%s %s[%d]  =  (unknown type)\n", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), i);
                       flushIfNeeded(FLUSHLIMIT);
               }
           }
       }
   }
}

void TStructPanel::update()
{
   Tcl_Interp *interp = getTkApplication()->getInterp();

   // delete display
   CHK(Tcl_VarEval(interp, widgetname, ".txt delete 1.0 end", NULL));

   if (!object)
   {
       CHK(Tcl_VarEval(interp, widgetname, ".txt insert 1.0 {<none>}", NULL));
       return;
   }

   // get descriptor object
   cStructDescriptor *sd = object->getDescriptor();
   if (!sd)
   {
       CHK(Tcl_VarEval(interp, widgetname, ".txt insert 1.0 {class ", object->className()," {\n"
                               "    <fields cannot be displayed -- no descriptor object registered>\n"
                               "}}\n", NULL));
       return;
   }

   // display object and delete descriptor
   writeptr = buf;
   *writeptr = '\0';

   sprintf(writeptr,"class %s {\n", object->className());
   flushIfNeeded(FLUSHLIMIT);

   displayStruct(object, sd, 1);

   sprintf(writeptr,"}\n");

   // flush the rest
   flushIfNeeded(0);
}

void TStructPanel::writeBack()
{
   // nothing to do here as long as GUI is a textfield
}

int TStructPanel::inspectorCommand(Tcl_Interp *interp, int argc, const char **argv)
{
   //
   // These functions are currently not used. Might be useful for a Tcl-based struct inspector.
   //
   if (argc<1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   cStructDescriptor *sd = object->getDescriptor();

   if (strcmp(argv[0],"count")==0)   // 'opp_inspectorcommand <inspector> fieldcount ...'
   {
      if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      char buf[20];
      sprintf(buf, "%d", sd->getFieldCount(object));
      Tcl_SetResult(interp, buf, TCL_VOLATILE);
      return TCL_OK;
   }

   if (strcmp(argv[0],"type")==0)   // 'opp_inspectorcommand <inspector> fieldtype ...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      int type = sd->getFieldType(object, fld);
      switch(type)
      {
          case cStructDescriptor::FT_BASIC:
              Tcl_SetResult(interp, "basic", TCL_STATIC); break;
          case cStructDescriptor::FT_STRUCT:
              Tcl_SetResult(interp, "struct", TCL_STATIC); break;
          case cStructDescriptor::FT_BASIC_ARRAY:
              Tcl_SetResult(interp, "basic array", TCL_STATIC); break;
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
      Tcl_SetResult(interp, TCLCONST(sd->getFieldName(object, fld)), TCL_VOLATILE);
      return TCL_OK;
   }

   if (strcmp(argv[0],"typename")==0)   // 'opp_inspectorcommand <inspector> fieldtypename <fldid> ...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldTypeString(object, fld)), TCL_VOLATILE);
      return TCL_OK;
   }

   if (strcmp(argv[0],"arraysize")==0)   // 'opp_inspectorcommand <inspector> fieldarraysize <fldid> ...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      char buf[20];
      sprintf(buf, "%d", sd->getArraySize(object, fld));
      Tcl_SetResult(interp, buf, TCL_VOLATILE);
      return TCL_OK;
   }

   if (strcmp(argv[0],"value")==0)   // 'opp_inspectorcommand <inspector> fieldvalue <fldid> ?index?...'
   {
      if (argc!=2 && argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      int i=0;
      if (argc==4)
         i = atoi(argv[2]);
      if (!sd->getFieldAsString(object, fld, i, tmpbuf, MAXWRITE))
      {
         Tcl_SetResult(interp, "error in getFieldAsString()", TCL_STATIC);
         return TCL_ERROR;
      }
      Tcl_SetResult(interp, buf, TCL_VOLATILE);
      return TCL_OK;
   }

   if (strcmp(argv[0],"enumname")==0)   // 'opp_inspectorcommand <inspector> fieldenumname <fldid> ?index?...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldEnumName(object, fld)), TCL_VOLATILE);
      return TCL_OK;
   }

   if (strcmp(argv[0],"structname")==0)   // 'opp_inspectorcommand <inspector> fieldenumname <fldid> ?index?...'
   {
      if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[1]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldStructName(object, fld)), TCL_VOLATILE);
      return TCL_OK;
   }
   return TCL_ERROR;
}

