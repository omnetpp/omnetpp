//=========================================================================
//  ICHUNK.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ICHUNK_H_
#define __ICHUNK_H_

#include "omnetpp/platdep/platmisc.h"
#include "eventlogdefs.h"

namespace omnetpp {
namespace eventlog {

/**
 * Manages one part of the eventlog file that consists of a sequence of chunks.
 */
class EVENTLOG_API IChunk
{
    public:
        IChunk() {}
        virtual ~IChunk() {}

        /**
         * Returns the begin offset of the chunk in the eventlog file.
         */
        virtual file_offset_t getBeginOffset() = 0;
        /**
         * Returns the end offset of the chunk in the eventlog file.
         */
        virtual file_offset_t getEndOffset() = 0;

        /**
         * Returns the corresponding event number.
         */
        virtual eventnumber_t getEventNumber() = 0;
        /**
         * Returns the corresponding simulation time.
         */
        virtual simtime_t getSimulationTime() = 0;
};

}  // namespace eventlog
}  // namespace omnetpp

#endif
