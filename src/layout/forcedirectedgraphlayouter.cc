//==========================================================================
//  FORCEDIRECTEDGRAPHLAYOUTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <ctime>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <sstream>

#include "common/commonutil.h"
#include "common/stlutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "forcedirectedgraphlayouter.h"
#include "startreeembedding.h"
#include "heapembedding.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace layout {

ForceDirectedGraphLayouter::~ForceDirectedGraphLayouter()
{
    const auto& bodies = embedding.getBodies();
    for (auto b : {topBorder, bottomBorder, leftBorder, rightBorder})
        if (!contains(bodies, (IBody*)b))
            delete b; // the embedding itself deletes the bodies in it
}

void ForceDirectedGraphLayouter::setParameters()
{
    // random seed
    embedding.parameters = embedding.getParameters(lcgRandom.getSeed());

    // timing parameters
    embedding.parameters.minTimeStep = environment->getDoubleParameter("mits", 0, embedding.parameters.minTimeStep);
    embedding.parameters.maxTimeStep = environment->getDoubleParameter("mats", 0, embedding.parameters.maxTimeStep);
    embedding.parameters.minAccelerationError = environment->getDoubleParameter("miae", 0, embedding.parameters.minAccelerationError);
    embedding.parameters.maxAccelerationError = environment->getDoubleParameter("maae", 0, embedding.parameters.maxAccelerationError);

    // various algorithm parameters
    embedding.parameters.defaultSlippery = environment->getBoolParameter("sp", 0, slippery);
    embedding.parameters.defaultPointLikeDistance = environment->getBoolParameter("pld", 0, pointLikeDistance);
    embedding.parameters.defaultSpringCoefficient = environment->getDoubleParameter("sc", 0, privUniform(0.1, 1));
    embedding.parameters.defaultSpringReposeLength = environment->getDoubleParameter("srl", 0, privUniform(expectedEdgeLength / 2, expectedEdgeLength));
    embedding.parameters.electricRepulsionCoefficient = environment->getDoubleParameter("erc", 0, privUniform(10000, 100000));
    embedding.parameters.frictionCoefficient = environment->getDoubleParameter("fc", 0, privUniform(1, 5));
    embedding.parameters.velocityRelaxLimit = environment->getDoubleParameter("vrl", 0, 0.2);
    embedding.parameters.maxCalculationTime = environment->getDoubleParameter("mct", 0, privUniform(1000, 20000));

    // 3d
    threeDFactor = environment->getDoubleParameter("3df", 0, privRand01() < 0.5 ? 0 : privUniform(0, 1));
    threeDCoefficient = environment->getDoubleParameter("3dc", 0, privUniform(0, 10));

    // which embedding to use
    preEmbedding = environment->getBoolParameter("pe", 0, privRand01() < 0.5);
    forceDirectedEmbedding = environment->getBoolParameter("fde", 0, true);

    // debug parameters
    debugWaitTime = environment->getDoubleParameter("dwt", 0, 0);
    showForces = environment->getBoolParameter("sf", 0, false);
    showSummaForce = environment->getBoolParameter("ssf", 0, false);
}

void ForceDirectedGraphLayouter::addBody(int nodeId, IBody *body)
{
    embedding.addBody(body);
    moduleToBodyMap[nodeId] = body;
}

IBody *ForceDirectedGraphLayouter::findBody(int nodeId)
{
    std::map<int, IBody *>::iterator it = moduleToBodyMap.find(nodeId);

    if (it != moduleToBodyMap.end())
        return it->second;
    else
        return nullptr;
}

Variable *ForceDirectedGraphLayouter::ensureAnchorVariable(const char *anchorName)
{
    std::map<std::string, Variable *>::iterator it = anchorNameToVariableMap.find(anchorName);

    if (it != anchorNameToVariableMap.end())
        return it->second;
    else
        return anchorNameToVariableMap[anchorName] = new Variable(Pt::getNil());
}

void ForceDirectedGraphLayouter::calculateExpectedMeasures()
{
    const std::vector<IBody *>& bodies = embedding.getBodies();

    // body sizes
    int count = 0;
    double maxBodyLength = 0;

    // expected measures
    double expectedEmbeddingSize = 0;
    double averageBodyLength = 0;

    for (auto body : bodies) {
        if (!dynamic_cast<WallBody *>(body)) {
            double length = body->getSize().getDiagonalLength();
            count++;
            maxBodyLength = std::max(maxBodyLength, length);
            averageBodyLength += length;
            expectedEmbeddingSize += body->getSize().getArea();
        }
    }

    // use pointLikeDistance if there is no too big bodies compared to the others
    // this will make the embedding faster
    Assert(!isNaN(averageBodyLength));
    averageBodyLength /= count;
    pointLikeDistance = maxBodyLength < 2 * averageBodyLength;
    // FIXME: fix the following to be able to nicely layout big and small nodes in the same embedding
    // this means the node rectangles will be used properly when calculating the distance and vector between them
    // slippery = pointLikeDistance && privRand01() < 0.5;

    // minimum length plus averageBodyLength
    double minEdgeLength = 20 + 2400 / (20 + bodies.size());
    expectedEdgeLength = pointLikeDistance ? minEdgeLength + averageBodyLength : minEdgeLength;

    // calculate expected embedding size
    expectedEmbeddingSize += 2 * pow(expectedEdgeLength, 2) * bodies.size();
    expectedEmbeddingSize = sqrt(expectedEmbeddingSize);
    Assert(!isNaN(expectedEmbeddingSize));
    expectedEmbeddingWidth = width ? width : expectedEmbeddingSize + 2 * border;
    expectedEmbeddingHeight = height ? height : expectedEmbeddingSize + 2 * border;
}

void ForceDirectedGraphLayouter::setRandomPositions()
{
    // assign values to not yet assigned coordinates
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (auto variable : variables) {
        Pt pt(variable->getPosition());

        if (isNaN(pt.x))
            pt.x = privUniform(border, expectedEmbeddingWidth - border);

        if (isNaN(pt.y))
            pt.y = privUniform(border, expectedEmbeddingHeight - border);

        if (isNaN(pt.z))
            pt.z = (privRand01() - 0.5) * sqrt(expectedEmbeddingWidth * expectedEmbeddingHeight) * threeDFactor;

        variable->assignPosition(pt);
    }
}

void ForceDirectedGraphLayouter::ensureBorders()
{
    // top and bottom borders are handled together
    if (!topBorder && (height || hasFixedNode || hasEdgeToBorder)) {
        topBorder = new WallBody(true, NaN);
        bottomBorder = new WallBody(true, NaN);
    }
    // left and right borders are handled together
    if (!leftBorder && (width || hasFixedNode || hasEdgeToBorder)) {
        leftBorder = new WallBody(false, NaN);
        rightBorder = new WallBody(false, NaN);
    }
}

void ForceDirectedGraphLayouter::addBorderForceProviders()
{
    // add electric repulsion to all bodies
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (auto body : bodies) {
        if (topBorder) {
            embedding.addForceProvider(new VerticalElectricRepulsion(topBorder, body));
            embedding.addForceProvider(new VerticalElectricRepulsion(bottomBorder, body));
        }
        if (leftBorder) {
            embedding.addForceProvider(new HorizontalElectricRepulsion(leftBorder, body));
            embedding.addForceProvider(new HorizontalElectricRepulsion(rightBorder, body));
        }
    }
    if (topBorder) {
        // the positions for non constrained variables are not yet set
        topBorder->setVariable(hasFixedNode || height ? new PointConstrainedVariable(Pt(NaN, 0, NaN)) : new Variable(Pt::getNil()));
        bottomBorder->setVariable(height ? new PointConstrainedVariable(Pt(NaN, height, NaN)) : new Variable(Pt::getNil()));
        embedding.addBody(topBorder);
        embedding.addBody(bottomBorder);
        if (!height)
            embedding.addForceProvider(new VerticalSpring(topBorder, bottomBorder, -1, expectedEmbeddingHeight));
    }
    if (leftBorder) {
        // the positions for non constrained variables are not yet set
        leftBorder->setVariable(hasFixedNode || width ? new PointConstrainedVariable(Pt(0, NaN, NaN)) : new Variable(Pt::getNil()));
        rightBorder->setVariable(width ? new PointConstrainedVariable(Pt(width, NaN, NaN)) : new Variable(Pt::getNil()));
        embedding.addBody(leftBorder);
        embedding.addBody(rightBorder);
        if (!width)
            embedding.addForceProvider(new HorizonalSpring(leftBorder, rightBorder, -1, expectedEmbeddingWidth));
    }
}

void ForceDirectedGraphLayouter::setBorderPositions()
{
    double distance = 100;
    double top = DBL_MAX, bottom = DBL_MIN;
    double left = DBL_MAX, right = DBL_MIN;

    if (!width || !height) {
        const std::vector<IBody *>& bodies = embedding.getBodies();
        for (auto body : bodies) {
            if (!dynamic_cast<WallBody *>(body)) {
                if (!height) {
                    top = std::min(top, body->getTop());
                    bottom = std::max(bottom, body->getBottom());
                }
                if (!width) {
                    left = std::min(left, body->getLeft());
                    right = std::max(right, body->getRight());
                }
            }
        }
    }

    // ensure all bodies are within the box specified by the borders
    if (topBorder) {
        topBorder->setPosition(hasFixedNode || height ? 0 : top - distance);
        bottomBorder->setPosition(height ? height : bottom + distance);
    }
    if (leftBorder) {
        leftBorder->setPosition(hasFixedNode || width ? 0 : left - distance);
        rightBorder->setPosition(width ? width : right + distance);
    }
}

void ForceDirectedGraphLayouter::addElectricRepulsions()
{
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++)
        for (int j = i + 1; j < (int)bodies.size(); j++) {
            IBody *body1 = bodies[i];
            IBody *body2 = bodies[j];
            Variable *variable1 = body1->getVariable();
            Variable *variable2 = body2->getVariable();

            // ignore wall bodies
            if (!dynamic_cast<WallBody *>(body1) &&
                !dynamic_cast<WallBody *>(body2) &&
                variable1 != variable2)
            {
                Vertex *vertex1 = graphComponent.findVertex(variable1);
                Vertex *vertex2 = graphComponent.findVertex(variable2);

                Assert(vertex1);
                Assert(vertex2);

                // check if the two are in the same connected component
                if (vertex1->connectedSubComponent != vertex2->connectedSubComponent)
                    embedding.addForceProvider(new ElectricRepulsion(body1, body2, expectedEdgeLength / 2, expectedEdgeLength));
                else
                    embedding.addForceProvider(new ElectricRepulsion(body1, body2));
            }
        }
}

void ForceDirectedGraphLayouter::addBasePlaneSprings()
{
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (auto body : bodies) {
        if (!dynamic_cast<WallBody *>(body))
            embedding.addForceProvider(new BasePlaneSpring(body, threeDCoefficient, 0));
    }
}

Rc ForceDirectedGraphLayouter::getBoundingBox()
{
    double top = DBL_MAX, bottom = DBL_MIN;
    double left = DBL_MAX, right = DBL_MIN;

    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (auto body : bodies) {
        if (!dynamic_cast<WallBody *>(body)) {
            top = std::min(top, body->getTop());
            bottom = std::max(bottom, body->getBottom());
            left = std::min(left, body->getLeft());
            right = std::max(right, body->getRight());
        }
    }

    return Rc(Pt(left, top, 0), Rs(right - left, bottom - top));
}

void ForceDirectedGraphLayouter::scale(Pt pt)
{
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (auto variable : variables)
        variable->assignPosition(Pt(variable->getPosition()).multiply(pt));
}

void ForceDirectedGraphLayouter::translate(Pt pt)
{
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (auto variable : variables) {
        variable->assignPosition(Pt(variable->getPosition()).add(pt));
    }
}

void ForceDirectedGraphLayouter::addMovableNode(int nodeId, double width, double height)
{
    hasMovableNode = true;

    // this is a free variable and body
    Variable *variable = new Variable(Pt::getNil());
    addBody(nodeId, new Body(variable, Rs(width, height)));

    graphComponent.addVertex(new Vertex(Pt::getNil(), Rs(width, height), variable));
}

void ForceDirectedGraphLayouter::addFixedNode(int nodeId, double x, double y, double width, double height)
{
    hasFixedNode = true;
    ensureBorders();

    // a fix node is a constrained variable which can only move on the z axes
    Variable *variable = new PointConstrainedVariable(Pt(x, y, NaN));
    IBody *body = new Body(variable, Rs(width, height));
    addBody(nodeId, body);

    graphComponent.addVertex(new Vertex(Pt(x - width / 2, y - height / 2, NaN), Rs(width, height), variable));
}

void ForceDirectedGraphLayouter::addAnchoredNode(int nodeId, const char *anchorname, double offx, double offy, double width, double height)
{
    hasAnchoredNode = true;

    // an anchored node is a relatively (to a variable, namely the anchor) positioned body
    Variable *variable = ensureAnchorVariable(anchorname);
    addBody(nodeId, new RelativelyPositionedBody(variable, Pt(offx, offy, 0), Rs(width, height)));

    // update vertex size
    Vertex *vertex = graphComponent.findVertex(variable);
    if (!vertex)
        graphComponent.addVertex(new Vertex(Pt::getNil(), Rs(offx + width / 2, offy + height / 2), variable));
    else {
        Rs rs = vertex->rc.rs;
        vertex->rc.rs = Rs(std::max(rs.width, (double)(offx + width / 2)), std::max(rs.height, (double)(offy + height / 2)));
    }
}

void ForceDirectedGraphLayouter::addEdge(int srcNodeId, int destNodeId, double len)
{
    // an edge is a spring
    // the -1 edge length will be replaced with expectedEdgeLength
    Spring *spring = new Spring(findBody(srcNodeId), findBody(destNodeId), -1, len ? len : -1);
    embedding.addForceProvider(spring);

    // and also an edge in the graph
    graphComponent.addEdge(new Edge(graphComponent.findVertex(spring->getBody1()->getVariable()),
                    graphComponent.findVertex(spring->getBody2()->getVariable())));
}

void ForceDirectedGraphLayouter::addEdgeToBorder(int srcNodeId, double len)
{
    hasEdgeToBorder = true;
    ensureBorders();
    IBody *body = findBody(srcNodeId);

    // add a force provider which uses four springs each connected to a wall
    // the least expanded spring will provided an attraction force
    std::vector<AbstractSpring *> springs;
    springs.push_back(new VerticalSpring(topBorder, body, -1, !len ? -1 : len));
    springs.push_back(new VerticalSpring(bottomBorder, body, -1, !len ? -1 : len));
    springs.push_back(new HorizonalSpring(leftBorder, body, -1, !len ? -1 : len));
    springs.push_back(new HorizonalSpring(rightBorder, body, -1, !len ? -1 : len));
    embedding.addForceProvider(new LeastExpandedSpring(springs));
}

void ForceDirectedGraphLayouter::executePreEmbedding()
{
    GraphComponent childrenComponentsStar;
    Vertex *childrenComponentsStarRoot = nullptr;

    // first pre embed all connected components, one by one
    for (auto childComponent : graphComponent.connectedSubComponents) {
        childComponent->calculateSpanningTree();

        // use tree embedding if connected component is a tree
        if (childComponent->getVertexCount() == childComponent->getEdgeCount() + 1 || privRand01() < 0.5) {
            StarTreeEmbedding starTreeEmbedding(childComponent, expectedEdgeLength);
            starTreeEmbedding.embed();
        }
        else {
            HeapEmbedding heapEmbedding(childComponent, expectedEdgeLength);
            heapEmbedding.embed();
        }

        // add a new vertex to component graph for each embedded sub component
        Vertex *childComponentVertex = new Vertex(Pt::getNil(), childComponent->getBoundingRectangle().rs, nullptr);
        if (!childrenComponentsStarRoot)
            childrenComponentsStarRoot = childComponentVertex;

        childrenComponentsStar.addVertex(childComponentVertex);
        childrenComponentsStar.addEdge(new Edge(childrenComponentsStarRoot, childComponentVertex));
    }

    // embed component graph
    childrenComponentsStar.calculateSpanningTree();
    HeapEmbedding heapEmbedding(&childrenComponentsStar, expectedEdgeLength);
    heapEmbedding.embed();

    // position all vertices on the plane so that they do not overlap
    // adding coordinates in the connected component and in the component graph
    for (int i = 0; i < (int)graphComponent.connectedSubComponents.size(); i++) {
        Vertex *childComponentVertex = childrenComponentsStar.getVertex(i);
        GraphComponent *childComponent = graphComponent.connectedSubComponents[i];

        for (int i = 0; i < childComponent->getVertexCount(); i++) {
            Vertex *vertex = childComponent->getVertex(i);
            Variable *variable = (Variable *)vertex->identity;
            Pt pt = vertex->rc.getCenterCenter();
            pt.add(childComponentVertex->rc.pt);
            pt.z = NaN;
            variable->assignPosition(pt);
        }
    }

    // ensure all vertices are within the bounding box if it is specified
    Rc rc = getBoundingBox();
    // ensure the top left corner of the bounding box is in the origin
    translate(Pt(-rc.pt.x, -rc.pt.y, 0));

    double scalex = width / rc.rs.width;
    double scaley = height / rc.rs.height;

    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (auto body : bodies) {
        if (!dynamic_cast<WallBody *>(body)) {
            scalex = std::min(scalex, (width - border - body->getSize().width) / body->getLeft());
            scaley = std::min(scaley, (height - border - body->getSize().height) / body->getTop());
        }
    }

    scale(Pt(width > 0 && scalex > 0 ? scalex : 1,
             height > 0 && scaley > 0 ? scaley : 1,
             NaN));
    // ensure all vertices have positive coordinates
    rc = getBoundingBox();
    translate(Pt(-rc.pt.x + border, -rc.pt.y + border, 0));

}

void ForceDirectedGraphLayouter::execute()
{
    if (hasMovableNode || hasAnchoredNode) {
        calculateExpectedMeasures();
        setParameters();

        // always calculate graph components
        graphComponent.calculateConnectedSubComponents();

        // execute pre-embedding if requested
        if (preEmbedding) {
            executePreEmbedding();

            // draw initial pre embedding result
            if (environment->inspected())
                debugDraw();
        }

        // execute force directed embedding if requested
        if (forceDirectedEmbedding) {
            if (width || height)
                ensureBorders();
            if (topBorder || leftBorder)
                addBorderForceProviders();

            addElectricRepulsions();

            if (threeDFactor > 0)
                addBasePlaneSprings();

            embedding.addForceProvider(new Drag());

            // assign random values to missing positions
            setRandomPositions();

            if (topBorder || leftBorder)
                setBorderPositions();

            // embedding.debug = 3;
            embedding.inspected = true;
            embedding.reinitialize();

            while (!embedding.getFinished() && environment->okToProceed()) {
                embedding.embed();

                if (environment->inspected())
                    debugDraw();
            }

            // ensure all vertices have positive coordinates even when there were no borders
            Rc rc = getBoundingBox();
            translate(Pt(hasFixedNode || width ? 0 : -rc.pt.x + border, hasFixedNode || height ? 0 : -rc.pt.y + border, 0));
        }

        // set random positions if no embedding was used
        if (!preEmbedding && !forceDirectedEmbedding)
            setRandomPositions();
    }
}

void ForceDirectedGraphLayouter::getNodePosition(int nodeId, double& x, double& y)
{
    IBody *body = findBody(nodeId);
    const Pt& pt = body->getPosition();
    x = pt.x;
    y = pt.y;
}

void ForceDirectedGraphLayouter::debugDraw()
{
    environment->clearGraphics();

    // draw bodies
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (auto body : bodies) {
        Pt pt(body->getPosition());
        Rs rs = body->getSize();
        pt.subtract(Pt(rs.width / 2, rs.height / 2, 0));

        // wall bodies are long lines
        if (dynamic_cast<WallBody *>(body)) {
            if (rs.width == POSITIVE_INFINITY) {
                pt.x = leftBorder ? leftBorder->getPosition().x : -1000;
                rs.width = leftBorder ? rightBorder->getPosition().x - leftBorder->getPosition().x : 2000;
            }

            if (rs.height == POSITIVE_INFINITY) {
                pt.y = topBorder ? topBorder->getPosition().y : -1000;
                rs.height = topBorder ? bottomBorder->getPosition().y - topBorder->getPosition().y : 2000;
            }
        }

        environment->drawRectangle(pt.x, pt.y, pt.x + rs.width, pt.y + rs.height, "{node bbox}", "black");
        char text[100];
        snprintf(text, sizeof(text), "%g", pt.z);
        environment->drawText(pt.x, pt.y, text, "{node bbox}", "black");
    }

    // draw springs
    const std::vector<IForceProvider *>& forceProviders = embedding.getForceProviders();
    for (auto forceProvider : forceProviders) {
        Spring *spring = dynamic_cast<Spring *>(forceProvider);
        if (spring) {
            const Pt& pt1 = spring->getBody1()->getPosition();
            const Pt& pt2 = spring->getBody2()->getPosition();
            environment->drawLine(pt1.x, pt1.y, pt2.x, pt2.y, "{edge bbox}", "black");
        }
    }

    // draw forces
    double forceScale = 10;
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (auto variable : variables) {
        const Pt& pt1 = variable->getPosition();

        if (showForces) {
            std::vector<Pt> forces = variable->getForces();

            for (auto force : forces) {
                Pt pt2(pt1);
                force.multiply(forceScale);
                pt2.add(force);
                environment->drawLine(pt1.x, pt1.y, pt2.x, pt2.y, "force", "red");
            }
        }

        if (showSummaForce) {
            Pt pt2(pt1);
            Pt force(variable->getForce());
            force.multiply(forceScale);
            pt2.add(force);
            environment->drawLine(pt1.x, pt1.y, pt2.x, pt2.y, "force", "blue");
        }
    }

    // write force directed embedding internal state info
    std::stringstream info;
    embedding.writeDebugInformation(info);
    environment->showGraphics(info.str().c_str());

    usleep((int)(debugWaitTime*1000));
}

}  // namespace layout
}  // namespace omnetpp

