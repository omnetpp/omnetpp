//==========================================================================
//  STRUCTINSP.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Implementation of
//    cClassDescriptor-based inspector
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
#include "cclassdescriptor.h"

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


void TStructPanel::displayStruct(void *object, cClassDescriptor *sd, int level)
{
   // print everything in a buffer, and periodically display it as the buffer gets full.
   // this is a temporary solution, should be replaced by something more professional!

   int indent = level*4;

   for (int fld=0; fld<sd->getFieldCount(object); fld++)
   {
       bool isarray = sd->getFieldIsArray(object, fld);
       bool iscompound = sd->getFieldIsCompound(object, fld);
       cClassDescriptor *sd1;

       if (!isarray)
       {
           if (!iscompound)
           {
               tmpbuf[0]='\0';
               sd->getFieldAsString(object, fld, 0, tmpbuf, MAXWRITE);
               if (sd->getFieldProperty(object, fld, "enum"))
               {
                   // display enum value as int and as string
                   cEnum* enm = findEnum(sd->getFieldProperty(object, fld, "enum"));
                   if (enm)
                   {
                       int key = atol(tmpbuf);
                       sprintf(tmpbuf, "%d (%s)", key, enm->stringFor(key));
                   }
               }
               sprintf(writeptr,"%*s%s %s  =  %s\n", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), tmpbuf);
               flushIfNeeded(FLUSHLIMIT);
           }
           else
           {
               tmpbuf[0]='\0';
               sd->getFieldAsString(object, fld, 0, tmpbuf, MAXWRITE);
               sprintf(writeptr,"%*s%s %s  =  %s ", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), tmpbuf);
               flushIfNeeded(FLUSHLIMIT);

               sd1 = cClassDescriptor::getDescriptorFor(sd->getFieldStructName(object, fld));
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
           }
       }
       else
       {
           int size = sd->getArraySize(object, fld);
           for (int i=0; i<size; i++)
           {
               if (!iscompound)
               {
                   sd->getFieldAsString(object, fld, i, tmpbuf, MAXWRITE); // FIXME: error handling!
                   if (sd->getFieldProperty(object, fld, "enum"))
                   {
                       // display enum value as int and as string
                       cEnum* enm = findEnum(sd->getFieldProperty(object, fld, "enum"));
                       if (enm)
                       {
                           int key = atol(tmpbuf);
                           sprintf(tmpbuf, "%d (%s)", key, enm->stringFor(key));
                       }
                   }
                   sprintf(writeptr,"%*s%s %s[%d]  =  %s\n", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), i, tmpbuf);
                   flushIfNeeded(FLUSHLIMIT);
               }
               else
               {
                   tmpbuf[0]='\0';
                   sd->getFieldAsString(object, fld, i, tmpbuf, MAXWRITE);
                   sprintf(writeptr,"%*s%s %s[%d]  =  %s ", indent, "", sd->getFieldTypeString(object, fld), sd->getFieldName(object, fld), i, tmpbuf);
                   flushIfNeeded(FLUSHLIMIT);

                   sd1 = cClassDescriptor::getDescriptorFor(sd->getFieldStructName(object, fld));
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
   cClassDescriptor *sd = object->getDescriptor();
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
   Tcl_SetResult(interp, "unsupported", TCL_STATIC); return TCL_ERROR;
}

