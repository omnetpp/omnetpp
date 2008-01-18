//=========================================================================
//  HEAPEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __HEAPEMBEDDING_H_
#define __HEAPEMBEDDING_H_

#include "vector"
#include "geometry.h"
#include "graphcomponent.h"

NAMESPACE_BEGIN

/**
 * This is a planar graph embedding for a connected graph component with spanning tree.
 *
 * Vertices are positioned in the component's spanning tree traversal order.
 * A list of availabe positions are kept during the embedding and vertices are positioned
 * to the best position with one of their side center point. The best position is the one
 * which minimizes the length of the edges to already positioned neighbours.
 * When a vertex is positioned its side center points are added to the list of availabe points.
 *
 * The resulting embedding will not have overlapping vertices.
 */
class HeapEmbedding
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

    public:
        HeapEmbedding(GraphComponent *graphComponent, double vertexSpacing);
        void embed();

    private:
        void pushPtUnlessRcsContains(std::vector<Pt>& pts, const std::vector<Rc>& circles, const Pt& pt);
};

NAMESPACE_END


#endif


