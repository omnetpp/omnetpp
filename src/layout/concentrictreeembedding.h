//=========================================================================
//  CONCENTRICTREEEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CONCENTRICTREEEMBEDDING_H_
#define __CONCENTRICTREEEMBEDDING_H_

#include "vector"
#include "geometry.h"
#include "graphcomponent.h"

NAMESPACE_BEGIN

/**
 *
 */
class ConcentricTreeEmbedding
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

NAMESPACE_END


#endif
