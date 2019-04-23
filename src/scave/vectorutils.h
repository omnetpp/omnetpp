//=========================================================================
//  VECTORUTILS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_VECTORUTILS_H
#define __OMNETPP_SCAVE_VECTORUTILS_H

#include "scavedefs.h"
#include "common/stlutil.h"
#include "common/stringutil.h"

namespace omnetpp {
namespace scave {

class XYArray;
class ResultFileManager;
class IDList;

/**
 * Read the VectorResult items in the IDList into the XYArrays.
 * TODO: ProgressMonitor support.
 */
SCAVE_API std::vector<XYArray*> readVectorsIntoArrays(ResultFileManager *manager, const IDList& idlist, bool includeEventNumbers, const std::vector<std::string>& filters = {});

} // namespace scave
}  // namespace omnetpp

#endif


