//==========================================================================
//  ARGS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Command line argument handling
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ARGS_H
#define __ARGS_H

#include <string>
#include "envirdefs.h"

NAMESPACE_BEGIN

/**
 * Somewhat similar to getopt(), this class stores and parses command-line
 * arguments.
 */
class ENVIR_API ArgList
{
  private:
    int argc;
    char **argv;
    std::string spec;

  private:
    // looks up spec for the given option
    bool isValidOption(char c);

    // looks up spec for the given option, and returns true if it has an arg
    bool hasArg(char c);

    // looks up spec for the given option, and returns true if it has an optional arg
    bool hasOptionalArg(char c);

    // if char c == 0, returns kth argument not kth option
    bool getOpt(char c, int k, const char *&value, bool validate);

  public:
    /**
     * Constructor takes argc, argv, and a getopt()-like specification
     * of single-letter options. The spec argument is a string that
     * specifies the option characters that are valid for this program.
     * An option character in this string can be followed by a colon (`:')
     * to indicate that it takes a required argument. If the option
     * character is followed by a question mark ('?'), it means that
     * the argument is optional, that is, it not be an error to specify
     * the option as the last argument (ie followed by nothing).
     * '?' is probably ONLY useful with -h.
     */
    ArgList(int argc, char *argv[], const char *spec);

    /**
     * Throws an exception if there are unrecognized options
     */
    void checkArgs();

    /**
     * Returns true if the given option is present on the command line.
     */
    bool optionGiven(char c);

    /**
     * Returns the value of the given option. If there are more than one
     * occurrences, k specifies which one to return. Returns NULL if not
     * found.
     */
    const char *optionValue(char c, int k=0);

    /**
     * Returns the kth non-option argument. Returns NULL if k is out of range.
     */
    const char *argument(int k);

    /**
     * Returns the original argc.
     */
    int argCount()  {return argc;}

    /**
     * Returns the original argv. It should not be modified.
     */
    char **argVector()  {return argv;}
};

NAMESPACE_END


#endif
