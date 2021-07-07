//==========================================================================
//  MSGCOMPILEROPTIONS.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_MSGCOMPILEROPTIONS_H
#define __OMNETPP_NEDXML_MSGCOMPILEROPTIONS_H

#include <string>
#include <vector>
#include "nedxmldefs.h"

namespace omnetpp {
namespace nedxml {

/**
 * @brief Options for MsgCompiler.
 *
 * @ingroup CppGenerator
 */
struct MsgCompilerOptions
{
    std::vector<std::string> importPath;
    std::string exportDef;
    bool generateClasses = true;
    bool generateDescriptors = true;
    bool generateSettersInDescriptors = true;
};

}  // namespace nedxml
}  // namespace omnetpp


#endif


