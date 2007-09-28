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

#include <string.h>
#include <stdio.h>
#include "args.h"
#include "opp_ctype.h"
#include "exception.h"


ArgList::ArgList(int ac, char *av[], const char *sp)
{
    argc = ac;
    argv = av;
    spec = sp;
}

void ArgList::checkArgs()
{
    const char *dummy;
    getOpt(0, 0, dummy, true);
}

bool ArgList::isValidOption(char c)
{
    return strchr(spec.c_str(), c) != NULL;
}

bool ArgList::hasArg(char c)
{
    const char *p = strchr(spec.c_str(), c);
    return p && *(p+1)==':';
}

bool ArgList::getOpt(char c, int k, const char *&value, bool validate)
{
    value = NULL;

    // loop through the options
    int i;
    bool inoptions = true;
    for (i=1; i<argc; i++)
    {
        // break if we reached the end of the options, hitting either a
        // non-option argument (no leading '-') or '--'.
        const char *argstr = argv[i];
        if (argstr[0]!='-' || !argstr[1])
            {inoptions=false; break;}  // end of options
        if (strcmp(argstr, "--")==0)
            {inoptions=false; ++i; break;}  // end of options

        // check if this option is a valid one, and if this is the one we're searching for
        if (validate && !isValidOption(argstr[1]))
            throw opp_runtime_error("invalid command-line option %s, try -h for help", argstr);
        if (c && argstr[1]==c)
            if (k-- == 0)
                break; // found the kth 'c' option

        // skip value of this option in argv if it has one
        if (hasArg(argstr[1]) && !argstr[2])
            i++;
        if (validate && i>=argc)
            throw opp_runtime_error("command-line option -%c is missing required argument", argstr[1]);

    }
    if (i>=argc)
        return false; // no such option or argument

    // finished scanning options; if we looked for one, we can return it
    if (c)
    {
        if (!inoptions)
            return false; // option not found

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
    return getOpt(c, 0, dummy, false);
}

const char *ArgList::optionValue(char c, int k)
{
    const char *value;
    getOpt(c, k, value, false);
    //printf("DBG: -%c option #%d = `%s'\n", c, k, value);
    return value;
}

const char *ArgList::argument(int k)
{
    const char *value;
    getOpt(0, k, value, false);
    //printf("DBG: arg #%d = `%s'\n", k, value);
    return value;
}


