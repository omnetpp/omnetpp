//==========================================================================
//  PATMATCH.H - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//         pattern matching stuff
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "defs.h"

bool transform_pattern(char *from, short *to);
bool stringmatch(short *pat, char *line);

