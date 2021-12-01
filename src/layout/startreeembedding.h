//=========================================================================
//  STARTREEEMBEDDING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_STARTREEEMBEDDING_H
#define __OMNETPP_LAYOUT_STARTREEEMBEDDING_H

#include "vector"
#include "geometry.h"
#include "graphcomponent.h"

namespace omnetpp {
namespace layout {

/**
 * This is a planar graph embedding for a connected graph component with spanning tree.
 *
 * Vertices are positioned recursively accoring to the spanning tree as follows.
 * The parent vertex is positioned in the center and its sub trees are positioned around it.
 * The parent vertex and each subtree is modelled with a circle and the circles
 * are positioned on the plane by putting one circle next to two already positioned ones.
 * The best circle position for a child subtree circle is the one which minimizes the distance
 * from the parent circle. When all child subtrees are positioned the parent tree is wrapped
 * by the smallest radius circle.
 *
 * The second pass rotates the subtree circles in place to move the weight point of the subtree
 * opposite to the parent.
 *
 * The resulting embedding will not have overlapping vertices.
 */
class StarTreeEmbedding
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
        StarTreeEmbedding(GraphComponent *graphComponent, double vertexSpacing);
        void embed();

    private:
        /**
         * Calculates center coordinates relative to parents.
         */
        void calculateCenter();
        void calculateCenterRecursive(Vertex *vertex);

        /**
         * Rotate the child subtrees to have the weight point opposite to the parent.
         */
        void rotateCenter();
        void rotateCenterRecursive(Vertex *vertex);

        /**
         * Calculates absolute positions.
         */
        void calculatePosition();
        void calculatePositionRecursive(Vertex *vertex, Pt pt);
};

}  // namespace layout
}  // namespace omnetpp


#endif
