//=========================================================================
//  GLOBALS.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/globals.h"
#include "omnetpp/ccomponenttype.h"

namespace omnetpp {
namespace internal {

cGlobalRegistrationList componentTypes("component types");
cGlobalRegistrationList nedFunctions("NED functions");
cGlobalRegistrationList classes("classes");
cGlobalRegistrationList enums("enums");
cGlobalRegistrationList classDescriptors("class descriptors");
cGlobalRegistrationList configOptions("supported configuration options");
cGlobalRegistrationList resultFilters("resultFilters");
cGlobalRegistrationList resultRecorders("resultRecorders");
cGlobalRegistrationList messagePrinters("messagePrinters");
std::map<std::string,std::string> figureTypes;
std::vector<EmbeddedNedFile> embeddedNedFiles;

EXECUTE_ON_SHUTDOWN(
        componentTypes.clear();
        nedFunctions.clear();
        classes.clear();
        enums.clear();
        classDescriptors.clear();
        configOptions.clear();
        resultFilters.clear();
        resultRecorders.clear();
        messagePrinters.clear();
        figureTypes.clear();
        embeddedNedFiles.clear();
);

}  // namespace internal
}  // namespace omnetpp
