//==========================================================================
//  ARGS.H - part of
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

#ifndef __ARGS_H
#define __ARGS_H

#include "envdefs.h"

class ENVIR_API ArgList
{
  private:
    int argc;
    char **argv;
  public:
    ArgList(int argc, char *argv[]);

    // functions to handle one-character command-line options
    bool argGiven(char c);
    char *argValue(char c, int k=0);

    int argCount()      {return argc;}
    char **argVector()  {return argv;}
};

#endif
