//==========================================================================
//  ARGS.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//    Command line argument handling
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "args.h"


ArgList::ArgList(int ac, char *av[], const char *sp)
{
    argc = ac;
    argv = av;
    spec = sp;
}

void ArgList::checkArgs()
{
    //FIXME todo
}

bool ArgList::hasArg(char c)
{
    const char *p = strchr(spec.c_str(), c);
    return p && *(p+1)==':';
}

bool ArgList::getOpt(char c, int k, const char *&value)
{
    value = NULL;

    // loop through the options
    int i;
    for (i=1; i<argc; i++)
    {
        const char *argstr = argv[i];
        if (argstr[0]!='-' || !argstr[1])
            break;  // end of options
        if (strcmp(argstr, "--")==0)
            {++i; break;}  // end of options

        if (c && argstr[1] == c)
            if (k-- == 0)
                break; // found the kth 'c' option

        if (hasArg(argstr[1]) && !argstr[2])
            i++;  // arg with value: skip value
    }
    if (i>=argc)
        return false; // no such option or argument

    // finished scanning options; if we looked for one, we can return it
    if (c)
    {
        // store value if it has one
        if (hasArg(c))
        {
            if (argv[i][2])
                value = argv[i]+2;  // -oFOO syntax
            else if (i!=argc-1)
                value = argv[i+1];  // -o FOO syntax
        }
        return true;
    }
    else
    {
        // rest of argv is arguments; return the kth one
        if (i+k >= argc)
            return false;
        value = argv[i+k];
        return true;
    }
}

bool ArgList::optionGiven(char c)
{
    const char *dummy;
    return getOpt(c, 0, dummy);
}

const char *ArgList::optionValue(char c, int k)
{
    const char *value;
    getOpt(c, k, value);
    return value;
}

const char *ArgList::argument(int k)
{
    const char *value;
    getOpt(0, k, value);
    return value;
}


