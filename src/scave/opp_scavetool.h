//=========================================================================
//  OPP_SCAVETOOL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SCAVETOOL_H
#define __OMNETPP_SCAVE_SCAVETOOL_H

#include <string>
#include "scavedefs.h"
#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

class ScaveTool
{
protected:
    void loadFiles(ResultFileManager& manager, const std::vector<std::string>& fileNames, bool indexingAllowed, bool allowNonmatching, bool verbose);
    std::string rebuildCommandLine(int argc, char **argv);
    int resolveResultTypeFilter(const std::string& filter);

    void helpCommand(int argc, char **argv);
    void printHelpPage(const std::string& page);
    void queryCommand(int argc, char **argv);
    void exportCommand(int argc, char **argv);
    void indexCommand(int argc, char **argv);
public:
    int main(int argc, char **argv);

};

}  // namespace scave
}  // namespace omnetpp

#endif
