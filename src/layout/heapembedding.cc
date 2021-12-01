//=========================================================================
//  HEAPEMBEDDING.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "heapembedding.h"

namespace omnetpp {
namespace layout {

using omnetpp::common::POSITIVE_INFINITY;

HeapEmbedding::HeapEmbedding(GraphComponent *graphComponent, double vertexSpacing) :
    vertexSpacing(vertexSpacing), graphComponent(graphComponent)
{
}

void HeapEmbedding::embed()
{
    std::vector<Rc> rcs;  // rectangles of vertices already positioned
    std::vector<Pt> pts;  // candidate points where vertices may be positioned
    pts.push_back(Pt::getZero());

    // delete all vertex positions
    for (int i = 0; i < graphComponent->getVertexCount(); i++)
        graphComponent->getVertex(i)->rc.pt = Pt::getNil();

    // go through vertices in spanning tree order
    for (auto vertex : graphComponent->spanningTreeVertices) {
        Rs rs = vertex->rc.rs;

        // find the best minimizing the distance cost function
        double bestDistance = POSITIVE_INFINITY;
        Rc bestRc;

        // for all candidate points
        for (auto pt : pts) {
            // align vertex to candidate points with its various points
            for (int k = 0; k < 8; k++) {
                Rc candidateRc;
                Pt ptCopy(pt);

                switch (k) {
                    case 0:
                        // candidate point is top left
                        candidateRc = Rc(ptCopy.subtract(0, 0, 0), rs);
                        break;
                    case 1:
                        // candidate point is top center
                        candidateRc = Rc(ptCopy.subtract(rs.width / 2, 0, 0), rs);
                        break;
                    case 2:
                        // candidate point is top right
                        candidateRc = Rc(ptCopy.subtract(rs.width, 0, 0), rs);
                        break;
                    case 3:
                        // candidate point is center left
                        candidateRc = Rc(ptCopy.subtract(0, rs.height / 2, 0), rs);
                        break;
                    case 4:
                        // candidate point is center right
                        candidateRc = Rc(ptCopy.subtract(rs.width, rs.height / 2, 0), rs);
                        break;
                    case 5:
                        // candidate point is bottom left
                        candidateRc = Rc(ptCopy.subtract(0, rs.height, 0), rs);
                        break;
                    case 6:
                        // candidate point is bottom center
                        candidateRc = Rc(ptCopy.subtract(rs.width / 2, rs.height, 0), rs);
                        break;
                    case 7:
                        // candidate point is bottom right
                        candidateRc = Rc(ptCopy.subtract(rs.width, rs.height, 0), rs);
                        break;
                }

                // find an already positioned vertex which would intersect the candidate rectangle
                bool intersects = false;
                for (auto rc : rcs) {
                    if (candidateRc.basePlaneProjectionIntersects(rc, true)) {
                        intersects = true;
                        break;
                    }
                }
                // if found one then this is a wrong candidate
                if (intersects)
                    continue;

                // calculate the distance sum to neighbours already positioned
                double distance = 0;
                for (auto neighbour : vertex->neighbours) {
                    if (!neighbour->rc.pt.isNil())
                        distance += candidateRc.getCenterCenter().getDistance(neighbour->rc.getCenterCenter());
                }

                // if better than the current best
                if (distance < bestDistance) {
                    bestRc = candidateRc;
                    bestDistance = distance;
                }
            }
        }

        // store position and rectangle
        vertex->rc.pt = bestRc.pt;

        // grow rectangle
        bestRc.pt.x -= vertexSpacing;
        bestRc.pt.y -= vertexSpacing;
        bestRc.rs.width += 2 * vertexSpacing;
        bestRc.rs.height += 2 * vertexSpacing;

        // delete candidate points covered by best rc
        for (int j = 0; j < (int)pts.size(); j++) {
            Pt pt = pts[j];

            if (bestRc.basePlaneProjectionContains(pt, true))
                pts.erase(pts.begin() + j--);
        }

        // push new candidates
        pushPtUnlessRcsContains(pts, rcs, bestRc.getCenterTop());
        pushPtUnlessRcsContains(pts, rcs, bestRc.getCenterBottom());
        pushPtUnlessRcsContains(pts, rcs, bestRc.getLeftCenter());
        pushPtUnlessRcsContains(pts, rcs, bestRc.getRightCenter());

        rcs.push_back(bestRc);
    }
}

void HeapEmbedding::pushPtUnlessRcsContains(std::vector<Pt>& pts, const std::vector<Rc>& rcs, const Pt& pt)
{
    for (auto rc : rcs) {
        if (rc.basePlaneProjectionContains(pt, true))
            return;
    }

    pts.push_back(pt);
}

}  // namespace layout
}  // namespace omnetpp

