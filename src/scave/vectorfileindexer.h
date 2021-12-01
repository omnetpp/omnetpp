//=========================================================================
//  VECTORFILEINDEXER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_IVECTORFILEINDEXER_H
#define __OMNETPP_SCAVE_IVECTORFILEINDEXER_H

#include <string>
#include "common/progressmonitor.h"
#include "resultfilemanager.h"
#include "vectorfileindex.h"

namespace omnetpp {
namespace scave {

/**
 * Generate an index file (.vci) for an output vector file (.vec)
 */
class SCAVE_API VectorFileIndexer
{
    using VectorInfo = VectorFileIndex::VectorInfo;
    using Block = VectorFileIndex::Block;

    public:
        typedef omnetpp::common::IProgressMonitor IProgressMonitor;

        void generateIndex(const char *filename, IProgressMonitor *monitor = nullptr);
};

}  // namespace scave
}  // namespace omnetpp


#endif

