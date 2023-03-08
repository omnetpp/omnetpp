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

class cConfiguration;
class cProperties;
class opp_string_map;

namespace envir {

typedef std::map<std::string, std::string> StringMap;
typedef std::vector<std::pair<std::string, std::string>> OrderedKeyValueList;

class ENVIR_API ResultFileUtils
{
  private:
    cConfiguration *cfg;
  public:
    ResultFileUtils(cConfiguration *cfg=nullptr) : cfg(cfg) {}
    void setConfiguration(cConfiguration *cfg) {this->cfg = cfg;}
    std::string getRunId();
    StringMap getRunAttributes();
    StringMap getIterationVariables();
    OrderedKeyValueList getSelectedConfigEntries();
    StringMap convertProperties(const cProperties *properties);
    StringMap convertMap(const opp_string_map *m);
    std::string augmentFileName(const std::string& fname);
};

}  // namespace envir
}  // namespace omnetpp

#endif

