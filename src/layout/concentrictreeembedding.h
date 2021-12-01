//=========================================================================
//  CONCENTRICTREEEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_CONCENTRICTREEEMBEDDING_H
#define __OMNETPP_LAYOUT_CONCENTRICTREEEMBEDDING_H

#include "vector"
#include "geometry.h"
#include "graphcomponent.h"

namespace omnetpp {
namespace layout {

/**
 *
 */
class LAYOUT_API ConcentricTreeEmbedding
{
    public:
        /**
         * Minimum distance between vertices
         */
        double vertexSpacing;

    private:
        /**
         * A connected graph component which must have a spanning tree.
         */
        GraphComponent *graphComponent;

        std::vector<double> levelPositions;

    public:
        ConcentricTreeEmbedding(GraphComponent *graphComponent, double vertexSpacing);
        void embed();

    private:
        void calculateCenterRecursive(Vertex *vertex, int level);
};

}  // namespace layout
}  // namespace omnetpp


#endif
