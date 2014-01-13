//==========================================================================
//  ARROW.H - part of
//
//                     OMNeT++/OMNEST
//
//  Contents:
//   connection arrow positioning in module drawing
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ARROW_H
#define __ARROW_H

#include "platmisc.h"   // must precede tk.h otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "platdefs.h"

NAMESPACE_BEGIN

int arrowcoords(Tcl_Interp *interp, int argc, const char **argv);

NAMESPACE_END


#endif

