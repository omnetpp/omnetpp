//==========================================================================
//  ARGS.CC - part of
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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "args.h"

static int argC;       /* no of args */
static char **argV;    /* argument vector */

void argInit(int argc, char *argv[])
{
   argC = argc;
   argV = argv;
}

int argGiven(char c)
{
   int i;
   for(i=1; i<argC; i++)
     if (argV[i][0]=='-' && toupper(argV[i][1])==toupper(c))
        return 1;
   return 0;
}

char *argValue(char c, int k)
{
   int i;
   for(i=1; i<argC; i++)
     if (argV[i][0]=='-' && toupper(argV[i][1])==toupper(c))
        if (k-- == 0) break;
   if (i==argC) return NULL; // no such switch

   if (argV[i][2])
   {
      return argV[i]+2;  // like: -fOUTPUTFILE
   }
   else
   {
      if (i==argC-1 || argV[i+1][0]=='-') return NULL;
      return argV[i+1];  // like: -f OUTPUTFILE
   }
}

