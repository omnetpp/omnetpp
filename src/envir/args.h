//==========================================================================
//  ARGS.H - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//    Command line argument handling
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ARGS_H
#define __ARGS_H

#include "envdefs.h"

void argInit(int argc, char *argv[]);     /* args passed to main() */

int argGiven(char c);              /* returns 0 or 1 */
char *argValue(char c, int k=0);   /* returns NULL or ptr to arg string */

#endif
