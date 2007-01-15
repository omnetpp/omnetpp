//=========================================================================
//  HEAPEMBEDDING.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "heapembedding.h"

HeapEmbedding::HeapEmbedding(GraphComponent *graphComponent, double nodeSpacing)
{
    this->graphComponent = graphComponent;
    this->nodeSpacing = nodeSpacing;
}

void HeapEmbedding::embed()
{
    std::vector<Cc> circles;
    std::vector<Pt> pts;
    pts.push_back(Pt::getZero());

    for (int i = 0; i < graphComponent->spanningTreeVertices.size(); i++) {
        Vertex *vertex = graphComponent->spanningTreeVertices[i];

        double bestDistance = POSITIVE_INFINITY;
        Pt bestPosition;

        for (int j = 0; j < pts.size(); j++) {
            Pt pt = pts[j];

            double distance = 0;
            for (int k = 0; k < vertex->neighbours.size(); k++) {
                Vertex *neighbour = vertex->neighbours[k];

                if (!neighbour->pt.isNil())
                    distance += pt.getDistance(neighbour->pt);
            }

            if (distance < bestDistance) {
                bestPosition = pt;
                bestDistance = distance;
            }
        }

        Cc cc = Cc(bestPosition, vertex->rs.getDiagonalLength() / 2 + nodeSpacing);

        for (int j = 0; j < pts.size(); j++) {
            Pt pt = pts[j];

            if (cc.basePlaneProjectionContains(pt, true))
                pts.erase(pts.begin() + j--);
        }

        pushPtUnlessCirclesContains(pts, circles, cc.getCenterTop());
        pushPtUnlessCirclesContains(pts, circles, cc.getCenterBottom());
        pushPtUnlessCirclesContains(pts, circles, cc.getLeftCenter());
        pushPtUnlessCirclesContains(pts, circles, cc.getRightCenter());

        vertex->pt = bestPosition;
        circles.push_back(cc);
    }
}

void HeapEmbedding::pushPtUnlessCirclesContains(std::vector<Pt>& pts, std::vector<Cc>& circles, Pt& pt)
{
    for (int j = 0; j < circles.size(); j++) {
        Cc cc = circles[j];

        if (cc.basePlaneProjectionContains(pt, true))
            return;
    }

    pts.push_back(pt);
}