//==========================================================================
//  GLOBALS.H - part of
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

#ifndef __OMNETPP_GLOBALS_H
#define __OMNETPP_GLOBALS_H

#include <map>
#include <string>
#include "onstartup.h"
#include "cregistrationlist.h"
#include "cobjectfactory.h"

namespace omnetpp {
namespace internal {

// Global registration lists
SIM_API extern cGlobalRegistrationList componentTypes;  ///< List of all component types (cComponentType)
SIM_API extern cGlobalRegistrationList nedFunctions;    ///< List if all NED functions (cNedFunction and cNedMathFunction)
SIM_API extern cGlobalRegistrationList classes;         ///< List of all classes that can be instantiated using createOne(); see cObjectFactory and Register_Class() macro
SIM_API extern cGlobalRegistrationList enums;           ///< List of all enum objects (cEnum)
SIM_API extern cGlobalRegistrationList classDescriptors;///< List of all class descriptors (cClassDescriptor)
SIM_API extern cGlobalRegistrationList configOptions;   ///< List of supported configuration options (cConfigOption)
SIM_API extern cGlobalRegistrationList resultFilters;   ///< List of result filters (cResultFilter)
SIM_API extern cGlobalRegistrationList resultRecorders; ///< List of result recorders (cResultRecorder)
SIM_API extern cGlobalRegistrationList messagePrinters; ///< List of message printers (cMessagePrinter)
SIM_API extern std::map<std::string,std::string> figureTypes; ///< Maps figure type names to implementation C++ class names (index into "classes")

// Internal: list in which objects are accumulated if there is no simple module in context.
// @see cOwnedObject::setOwningContext() and cSimulation::setContextModule())
SIM_API extern cSoftOwner globalOwningContext;

// Internal: Support for embedding NED files as string constants
struct EmbeddedNedFile
{
    EmbeddedNedFile(std::string fileName, std::string nedText, std::string garblephrase="") :
        fileName(fileName), nedText(nedText), garblephrase(garblephrase) {}
    std::string fileName;
    std::string nedText;
    std::string garblephrase;
};

SIM_API extern std::vector<EmbeddedNedFile> embeddedNedFiles;

}  // namespace internal
}  // namespace omnetpp


#endif

