//==========================================================================
//  PATMATCH.H - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//         pattern matching stuff
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "defs.h"
#include "envdefs.h"

ENVIR_API bool transform_pattern(const char *from, short *topattern);
ENVIR_API bool stringmatch(const short *pattern, const char *line);

