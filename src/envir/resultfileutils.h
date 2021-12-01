//==========================================================================
//  RESULTFILEUTILS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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

class cProperties;
class opp_string_map;

namespace envir {

typedef std::map<std::string, std::string> StringMap;
typedef std::vector<std::pair<std::string, std::string>> OrderedKeyValueList;

class ResultFileUtils {
  public:
    static std::string getRunId();
    static StringMap getRunAttributes();
    static StringMap getIterationVariables();
    static OrderedKeyValueList getSelectedConfigEntries();
    static StringMap convertProperties(const cProperties *properties);
    static StringMap convertMap(const opp_string_map *m);
};

}  // namespace envir
}  // namespace omnetpp

#endif

