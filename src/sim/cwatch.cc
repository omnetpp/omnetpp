//=========================================================================
//
//  CWATCH.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//
//   Member functions of
//    cWatch : shell for a char/int/long/float/double/char*/cObject* variable
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // memcmp, memcpy, memset
#include "cwatch.h"

// no Register_Class( cWatch ) -- makes no sense

//==========================================================================
//=== cWatch - member functions

cWatch::cWatch(const cWatch& vs) : cObject()
{
      setName(vs.name());
      operator=(vs);
}

void cWatch::info(char *buf)
{
      // sprintf(buf,"(%s) ", className());
      // printTo( buf + strlen(buf) );

      printTo( buf );
}

void cWatch::writeContents(ostream& os)
{
      char buf[128];
      printTo( buf );
      os << "  " << buf << "\n";
}

void cWatch::printTo(char *buf)
{
      switch (type)
      {
         case 'c':  sprintf(buf, "char %s = '%c' (%d,0x%x)", name(),
                                 *(char *)ptr, *(char *)ptr, *(char *)ptr );
                    break;
         case 'b':  sprintf(buf, "bool %s = %s", name(),
                                 (*(bool *)ptr ? "true" : "false"));
                    break;
         case 'i':  sprintf(buf, "int %s = %d (%uU, 0x%x)", name(),
                                 *(int *)ptr, *(int *)ptr, *(int *)ptr );
                    break;
         case 'l':  sprintf(buf, "long %s = %ldL (%luLU, 0x%lx)", name(),
                                 *(long *)ptr, *(long *)ptr, *(long *)ptr );
                    break;
         case 'd':  sprintf(buf, "double %s = %f", name(),
                                 *(double *)ptr );
                    break;
         case 's':  if (*(char **)ptr==NULL)
                       sprintf(buf, "char *%s = NULL", name());
                    else
                       // FIXME following is dangerous, but usually doesn't crash if the pointer has once been initialized...
                       // it would be better to have a checkbox in the inspector to allow/disallow pointer dereferencing!
                       sprintf(buf, "char *%s = \"%.40s\" (%p)", name(), *(char **)ptr, *(char **)ptr);
                    break;
         case 'o':  if (*(cObject **)ptr==NULL)
                       sprintf(buf, "cObject *%s = NULL", name());
                    else
                       // cannot call name(), className() or info() -- may crash if this is not a valid object!
                       sprintf(buf, "cObject *%s = %p (may or may not be a valid object pointer)",
                               name(), *(cObject **)ptr );
                    break;
         default:   sprintf(buf, "unknown %s (type '%c')", name(), type);
      }
}

