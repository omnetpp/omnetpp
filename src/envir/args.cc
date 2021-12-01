//==========================================================================
//  ARGS.CC - part of
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

#include <cstring>
#include <cstdio>
#include "common/opp_ctype.h"
#include "common/stlutil.h"
#include "args.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

void ArgList::parse(int argc, char *argv[], const char *spec)
{
    this->argc = argc;
    this->argv = argv;
    this->spec = spec;

    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--") == 0) {
            // end of options
            i++;
            break;
        }
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            // long option -- treat it liberally for now
            const char *eqpos = strchr(argv[i], '=');
            if (eqpos)
                longOpts[std::string(argv[i]+2, eqpos-argv[i]-2)] = eqpos+1;
            else if (i == argc-1)
                longOpts[argv[i]+2] = "";
            else {
                longOpts[argv[i]+2] = argv[i+1];
                i++;
            }
        }
        else if (argv[i][0] == '-') {
            // short option
            char c = argv[i][1];
            if (!isValidOption(c))
                throw opp_runtime_error("Invalid command-line option %s, try -h for help", argv[i]);

            std::vector<std::string>& v = shortOpts[c];  // get or create map element
            if (hasArg(c)) {
                if (argv[i][2])
                    v.push_back(argv[i]+2);
                else if (i < argc-1)
                    v.push_back(argv[++i]);
                else if (!hasOptionalArg(c))
                    throw opp_runtime_error("command-line option %s is missing required argument", argv[i]);
            }
            else {
                if (argv[i][2])
                    throw opp_runtime_error("command-line option -%c expects no argument", argv[i][1]);
            }
        }
        else {
            // non-option arg before '--'
            params.push_back(argv[i]);
        }
    }

    // store the rest of the args (after '--')
    for ( ; i < argc; i++) {
        params.push_back(argv[i]);
    }
}

bool ArgList::isValidOption(char c) const
{
    return strchr(spec.c_str(), c) != nullptr;
}

bool ArgList::hasArg(char c) const
{
    const char *p = strchr(spec.c_str(), c);
    return p && (*(p+1) == ':' || *(p+1) == '?');
}

bool ArgList::hasOptionalArg(char c) const
{
    const char *p = strchr(spec.c_str(), c);
    return p && *(p+1) == '?';
}

std::map<std::string, std::string> ArgList::getLongOptions() const
{
    return longOpts;
}

bool ArgList::optionGiven(char c) const
{
    return containsKey(shortOpts, c);
}

bool ArgList::longOptionGiven(const char *opt) const
{
    return containsKey(longOpts, std::string(opt));
}

const char *ArgList::optionValue(char c, int k) const
{
    if (!containsKey(shortOpts, c))
        return nullptr;
    const std::vector<std::string>& v = shortOpts.at(c);
    return k == -1 ? v.back().c_str() : k < (int)v.size() ? v.at(k).c_str() : nullptr;
}

std::vector<std::string> ArgList::optionValues(char c) const
{
    if (!containsKey(shortOpts, c))
        return std::vector<std::string>();
    return shortOpts.at(c);
}

const char *ArgList::argument(int k) const
{
    return k < (int)params.size() ? params.at(k).c_str() : nullptr;
}

}  // namespace envir
}  // namespace omnetpp

