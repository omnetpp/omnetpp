//==========================================================================
//  RESULTFILEUTILS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_RESULTFILEUTILS_H
#define __OMNETPP_ENVIR_RESULTFILEUTILS_H

#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

typedef std::map<std::string, std::string> StringMap;
typedef std::vector<std::pair<std::string, std::string>> OrderedKeyValueList;

class ResultFileUtils {
  public:
    static std::string getRunId();
    static StringMap getRunAttributes();
    static StringMap getIterationVariables();
    static OrderedKeyValueList getParamAssignments();
    static OrderedKeyValueList getConfigEntries();
    static void writeRunData(FILE *f, const char *fname); //TODO remove this
};

} // namespace envir
}  // namespace omnetpp

#endif

