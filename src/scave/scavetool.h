//=========================================================================
//  SCAVETOOL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

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
    void printUsage();
    void loadFiles(ResultFileManager& manager, const std::vector<std::string>& fileNames, bool verbose);
    std::string rebuildCommandLine(int argc, char **argv);
    void vectorCommand(int argc, char **argv);
    void parseScalarFunction(const std::string& functionCall,  /*out*/ std::string& name,  /*out*/ std::vector<std::string>& params);
    void scalarCommand(int argc, char **argv);
    void listCommand(int argc, char **argv);
    void infoCommand(int argc, char **argv);
    void indexCommand(int argc, char **argv);
public:
    int main(int argc, char **argv);

};

}  // namespace scave
}  // namespace omnetpp

#endif
