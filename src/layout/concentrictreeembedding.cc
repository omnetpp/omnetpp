//=========================================================================
//  CONCENTRICTREEEMBEDDING.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "concentrictreeembedding.h"

namespace omnetpp {
namespace layout {

ConcentricTreeEmbedding::ConcentricTreeEmbedding(GraphComponent *graphComponent, double vertexSpacing) :
    vertexSpacing(vertexSpacing), graphComponent(graphComponent)
{
}

void ConcentricTreeEmbedding::embed()
{
    calculateCenterRecursive(graphComponent->spanningTreeRoot, 0);
}

void ConcentricTreeEmbedding::calculateCenterRecursive(Vertex *vertex, int level)
{
    // TODO: finish this quick hack and really embed in concentric circles
    if (levelPositions.size() == level)
        levelPositions.insert(levelPositions.end(), 0);
    double levelPosition = levelPositions[level];
    vertex->rc.pt.x = levelPosition;
    vertex->rc.pt.y = level * 100;
    levelPositions[level] += vertex->rc.rs.width + vertexSpacing;
    for (std::vector<Vertex *>::iterator it = vertex->spanningTreeChildren.begin(); it != vertex->spanningTreeChildren.end(); ++it)
        calculateCenterRecursive(*it, level + 1);
}

}  // namespace layout
}  // namespace omnetpp

