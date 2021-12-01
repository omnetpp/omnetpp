//=========================================================================
//  STARTREEEMBEDDING.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "startreeembedding.h"

namespace omnetpp {
namespace layout {

using omnetpp::common::POSITIVE_INFINITY;

StarTreeEmbedding::StarTreeEmbedding(GraphComponent *graphComponent, double vertexSpacing) :
    vertexSpacing(vertexSpacing), graphComponent(graphComponent)
{
}

void StarTreeEmbedding::embed()
{
    calculateCenter();
    rotateCenter();
    calculatePosition();
}

void StarTreeEmbedding::calculateCenter()
{
    calculateCenterRecursive(graphComponent->spanningTreeRoot);
}

void StarTreeEmbedding::calculateCenterRecursive(Vertex *vertex)
{
    if (vertex->spanningTreeChildren.empty()) {
        // position a vertex without children
        vertex->starTreeRadius = vertex->rc.rs.getDiagonalLength() / 2;
        vertex->starTreeCenter = Pt::getZero();
        vertex->starTreeCircleCenter = Pt::getZero();
    }
    else {
        vertex->starTreeCenter = Pt::getZero();

        for (auto & it : vertex->spanningTreeChildren)
            calculateCenterRecursive(it);

        std::vector<Pt> pts;
        std::vector<Cc> circles;

        for (std::vector<Vertex *>::iterator it = vertex->spanningTreeChildren.begin(); it != vertex->spanningTreeChildren.end(); ++it) {
            Vertex *vertexChild = *it;
            pts.clear();
            circles.clear();

            circles.push_back(Cc(Pt::getZero(), vertex->rc.rs.getDiagonalLength() / 2 + vertexSpacing + vertexChild->starTreeRadius));

            for (auto vertexPositioned : vertex->spanningTreeChildren) {
                if (vertexPositioned == vertexChild)
                    break;

                circles.push_back(Cc(vertexPositioned->starTreeCenter.copy().add(vertexPositioned->starTreeCircleCenter), vertexPositioned->starTreeRadius + vertexSpacing + vertexChild->starTreeRadius));
            }

            // Find point candidates
            for (int i = 0; i < (int)circles.size(); i++)
                for (int j = i + 1; j < (int)circles.size(); j++) {
                    Cc circle1 = circles[i];
                    Cc circle2 = circles[j];

                    std::vector<Pt> intersectingPts = circle1.basePlaneProjectionIntersect(circle2);

                    for (auto & intersectingPt : intersectingPts)
                        pts.push_back(intersectingPt);
                }

            // Default points
            if (circles.size() == 1) {
                Cc circleSelf = circles[0];
                pts.push_back(circleSelf.getCenterTop());
                pts.push_back(circleSelf.getCenterBottom());
                pts.push_back(circleSelf.getLeftCenter());
                pts.push_back(circleSelf.getRightCenter());
            }

            for (int j = 0; j < (int)pts.size(); j++) {
                for (auto cc : circles) {
                    Pt pt = pts[j];
                    if (pt.getDistance(cc.origin) < cc.radius - 1) {
                        pts.erase(pts.begin() + j--);
                        break;
                    }
                }
            }

            // Optimize cost function
            double costMin = POSITIVE_INFINITY;

            for (auto pt : pts) {
                double cost = vertex->starTreeCenter.getDistance(pt);

                if (cost < costMin) {
                    costMin = cost;
                    vertexChild->starTreeCenter = pt.copy().subtract(vertexChild->starTreeCircleCenter);
                }
            }
        }

        // Find minimum covering circle
        circles.clear();
        circles.push_back(Cc(Pt::getZero(), vertex->rc.rs.getDiagonalLength() / 2));

        for (auto vertexChild : vertex->spanningTreeChildren) {
            circles.push_back(Cc(vertexChild->starTreeCenter.copy().add(vertexChild->starTreeCircleCenter), vertexChild->starTreeRadius));
        }

        Cc circleEncolsing = Cc::getBasePlaneProjectionEnclosingCircle(circles);

        vertex->starTreeRadius = circleEncolsing.radius;
        vertex->starTreeCircleCenter = circleEncolsing.origin;
    }
}

void StarTreeEmbedding::rotateCenter()
{
    rotateCenterRecursive(graphComponent->spanningTreeRoot);
}

void StarTreeEmbedding::rotateCenterRecursive(Vertex *vertex)
{
    if (!vertex->spanningTreeChildren.empty()) {
        if (vertex->spanningTreeParent) {
            Pt pt = Pt::getZero();
            double angle = (vertex->starTreeCenter.copy().add(vertex->starTreeCircleCenter)).getBasePlaneProjectionAngle();

            // Find weight point
            Pt weightPoint = Pt::getZero();
            double area = 0;

            for (std::vector<Vertex *>::iterator it = vertex->spanningTreeChildren.begin(); it != vertex->spanningTreeChildren.end(); ++it) {
                Vertex *vertexChild = *it;
                area += vertexChild->rc.rs.getArea();
                weightPoint.add(vertexChild->starTreeCenter.copy().add(vertex->starTreeCircleCenter).multiply(vertexChild->rc.rs.getArea()));
            }

            weightPoint.divide(area);
            double angleWeight = weightPoint.getBasePlaneProjectionAngle();
            double angleRotate = angle - angleWeight;

            if (!isNaN(angleRotate)) {
                // Rotate children around circle center
                for (std::vector<Vertex *>::iterator it = vertex->spanningTreeChildren.begin(); it != vertex->spanningTreeChildren.end(); ++it) {
                    Vertex *vertexChild = *it;
                    pt.assign(vertexChild->starTreeCenter).add(vertexChild->starTreeCircleCenter).add(vertex->starTreeCircleCenter);
                    pt.basePlaneRotate(angleRotate);
                    vertexChild->starTreeCenter.assign(pt).subtract(vertexChild->starTreeCircleCenter);
                }

                // Rotate parent around circle center
                pt.assign(vertex->starTreeCircleCenter).reverse();
                pt.basePlaneRotate(angleRotate);
                vertex->starTreeCenter.add(vertex->starTreeCircleCenter).add(pt);
                vertex->starTreeCircleCenter.assign(pt).reverse();

                // Correct children position
                for (std::vector<Vertex *>::iterator it = vertex->spanningTreeChildren.begin(); it != vertex->spanningTreeChildren.end(); ++it) {
                    Vertex *vertexChild = *it;
                    vertexChild->starTreeCenter.subtract(vertex->starTreeCircleCenter);
                }
            }
        }

        for (auto & it : vertex->spanningTreeChildren)
            rotateCenterRecursive(it);
    }
}

void StarTreeEmbedding::calculatePosition()
{
    calculatePositionRecursive(graphComponent->spanningTreeRoot, Pt::getZero());
}

void StarTreeEmbedding::calculatePositionRecursive(Vertex *vertex, Pt pt)
{
    vertex->rc.pt = Pt(pt.x - vertex->rc.rs.width / 2, pt.y  - vertex->rc.rs.height / 2, pt.z);

    if (vertex->spanningTreeParent)
        vertex->starTreeCircleCenter.add(pt);

    for (auto child : vertex->spanningTreeChildren) {
        calculatePositionRecursive(child, pt.copy().add(child->starTreeCenter));
    }
}

}  // namespace layout
}  // namespace omnetpp

