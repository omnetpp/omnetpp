//==========================================================================
//  ARGS.CC - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//    Command line argument handling
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "args.h"


ArgList::ArgList(int ac, char *av[])
{
    argc = ac;
    argv = av;
}

bool ArgList::argGiven(char c)
{
    int i;
    for(i=1; i<argc; i++)
        if (argv[i][0]=='-' && toupper(argv[i][1])==toupper(c))
            return true;
    return false;
}

char *ArgList::argValue(char c, int k)
{
    int i;
    for(i=1; i<argc; i++)
       if (argv[i][0]=='-' && toupper(argv[i][1])==toupper(c))
           if (k-- == 0) break;
    if (i==argc)
       return NULL; // no such switch

    if (argv[i][2])
    {
        return argv[i]+2;  // like: -fOUTPUTFILE
    }
    else
    {
        if (i==argc-1 || argv[i+1][0]=='-')
            return NULL;
        return argv[i+1];  // like: -f OUTPUTFILE
    }
}

