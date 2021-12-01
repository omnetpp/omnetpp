//==========================================================================
//  ARGS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Command line argument handling
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_ARGS_H
#define __OMNETPP_ENVIR_ARGS_H

#include <string>
#include <vector>
#include <map>
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

/**
 * Somewhat similar to getopt(), this class stores and parses command-line
 * arguments.
 */
class ENVIR_API ArgList
{
  private:
    int argc = 0;
    char **argv = nullptr;
    std::string spec;

    std::map<char,std::vector<std::string> > shortOpts;
    std::map<std::string,std::string> longOpts;
    std::vector<std::string> params;

  private:
    // looks up spec for the given option
    bool isValidOption(char c) const;

    // looks up spec for the given option, and returns true if it has an arg
    bool hasArg(char c) const;

    // looks up spec for the given option, and returns true if it has an optional arg
    bool hasOptionalArg(char c) const;

    // if char c == 0, returns kth argument not kth option
    bool getOpt(char c, int k, const char *&value, bool validate) const;

    // fills in shortOpts, longOpts and params from argc+argv
    void parse();

  public:
    /**
     * Constructor
     */
    ArgList() {}

    /**
     * Takes argc, argv, and a getopt()-like specification
     * of single-letter options. The spec argument is a string that
     * specifies the option characters that are valid for this program.
     * An option character in this string can be followed by a colon (`:')
     * to indicate that it takes a required argument. If the option
     * character is followed by a question mark ('?'), it means that
     * the argument is optional, that is, it should not be an error to specify
     * the option as the last argument (i.e. followed by nothing).
     * '?' is probably ONLY useful with -h.
     *
     * Throws an exception if there are unrecognized options
     */
    void parse(int argc, char *argv[], const char *spec);

    /**
     * Returns true if the given option is present on the command line.
     */
    bool optionGiven(char c) const;

    /**
     * Returns the value of the given option, or nullptr if not
     * found. If there are more than one occurrences of the given
     * option, k specifies which one to return. The default one
     * is to return the last one (k=-1).
     */
    const char *optionValue(char c, int k=-1) const;

    /**
     * Returns the values specified for given option.
     */
    std::vector<std::string> optionValues(char c) const;

    /**
     * Returns long options (those that begin with '--'), as key-value pairs;
     * keys do not contain the leading '--'.
     */
    std::map<std::string,std::string> getLongOptions() const;

    /**
     * Returns true if the given long option (specified without the leading '--')
     * is present on the command line.
     */
    bool longOptionGiven(const char *opt) const;

    /**
     * Returns the kth non-option argument. Returns nullptr if k is out of range.
     */
    const char *argument(int k) const;

    /**
     * Returns the original argc.
     */
    int getArgCount() const  {return argc;}

    /**
     * Returns the original argv. It should not be modified.
     */
    char **getArgVector() const  {return argv;}
};

}  // namespace envir
}  // namespace omnetpp

#endif


