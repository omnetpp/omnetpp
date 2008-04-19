//==========================================================================
//  FORCEDIRECTEDGRAPHLAYOUTER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <time.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <sstream>

#include "commonutil.h"
#include "forcedirectedgraphlayouter.h"
#include "startreeembedding.h"
#include "heapembedding.h"
#include "platmisc.h"

USING_NAMESPACE


ForceDirectedGraphLayouter::ForceDirectedGraphLayouter()
{
    hasFixedNode = false;
    hasMovableNode = false;
    hasAnchoredNode = false;

    topBorder = NULL;
    bottomBorder = NULL;
    leftBorder = NULL;
    rightBorder = NULL;

    expectedEmbeddingSize = -1;
    expectedEdgeLength = -1;
    pointLikeDistance = true;
}

void ForceDirectedGraphLayouter::setParameters()
{
    embedding.parameters = embedding.getParameters(lcgRandom.getSeed());
    embedding.parameters.defaultPointLikeDistance = environment->getBoolParameter("pld", 0, pointLikeDistance);
    embedding.parameters.defaultSpringCoefficient = environment->getDoubleParameter("sc", 0, embedding.parameters.defaultSpringCoefficient);
    embedding.parameters.defaultSpringReposeLength = environment->getDoubleParameter("srl", 0, expectedEdgeLength);
    embedding.parameters.electricRepulsionCoefficient = environment->getDoubleParameter("erc", 0, embedding.parameters.electricRepulsionCoefficient);
    embedding.parameters.frictionCoefficient = environment->getDoubleParameter("fc", 0, embedding.parameters.frictionCoefficient);
    embedding.parameters.minTimeStep = environment->getDoubleParameter("mits", 0, embedding.parameters.minTimeStep);
    embedding.parameters.maxTimeStep = environment->getDoubleParameter("mats", 0, embedding.parameters.maxTimeStep);
    embedding.parameters.minAccelerationError = environment->getDoubleParameter("miae", 0, embedding.parameters.minAccelerationError);
    embedding.parameters.maxAccelerationError = environment->getDoubleParameter("maae", 0, embedding.parameters.maxAccelerationError);
    embedding.parameters.velocityRelaxLimit = environment->getDoubleParameter("vrl", 0, embedding.parameters.velocityRelaxLimit);
    embedding.parameters.maxCalculationTime = environment->getDoubleParameter("mct", 0, embedding.parameters.maxCalculationTime);

    threeDFactor = environment->getDoubleParameter("3d", 0, std::max(0.0, privRand01() - 0.5)); // default: 0.1

    preEmbedding = environment->getBoolParameter("pe", 0, privRand01() > 0.5); // default: true
    forceDirectedEmbedding = environment->getBoolParameter("fde", 0, true);

    // debug parameters
    debugWaitTime = environment->getDoubleParameter("dwt", 0, 0);
    showForces = environment->getBoolParameter("sf", 0, false);
    showSummaForce = environment->getBoolParameter("ssf", 0, false);
}

void ForceDirectedGraphLayouter::addBody(cModule *mod, IBody *body)
{
    embedding.addBody(body);
    moduleToBodyMap[mod] = body;
}

IBody *ForceDirectedGraphLayouter::findBody(cModule *mod)
{
    std::map<cModule *, IBody *>::iterator it = moduleToBodyMap.find(mod);

    if (it != moduleToBodyMap.end())
        return it->second;
    else
        return NULL;
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
    expectedEmbeddingSize = 0;
    double averageBodyLength = 0;

    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body)) {
            double length = body->getSize().getDiagonalLength();
            count++;
            maxBodyLength = std::max(maxBodyLength, length);
            averageBodyLength += length;
            expectedEmbeddingSize += body->getSize().getArea();
        }
    }

    // pointLikeDistance if there is a body longer than 2 times the average size
    Assert(!isNaN(averageBodyLength));
    averageBodyLength /= count;
    pointLikeDistance = maxBodyLength < 2 * averageBodyLength;

    // minimum length plus averageBodyLength
    double minEdgeLength = bodies.size() < 10 ? 100 : bodies.size() < 30 ? 70 : 50;
    expectedEdgeLength = minEdgeLength + averageBodyLength;

    // calculate expected embedding size
    expectedEmbeddingSize += 2 * pow(expectedEdgeLength, 2) * bodies.size();
    expectedEmbeddingSize = sqrt(expectedEmbeddingSize);
    Assert(!isNaN(expectedEmbeddingSize));
}

void ForceDirectedGraphLayouter::setRandomPositions()
{
    // assign values to not yet assigned coordinates
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < (int)variables.size(); i++) {
        Pt pt(variables[i]->getPosition());

        if (isNaN(pt.x))
            pt.x = privRand01() * expectedEmbeddingSize;

        if (isNaN(pt.y))
            pt.y = privRand01() * expectedEmbeddingSize;

        if (isNaN(pt.z))
            pt.z = (privRand01() - 0.5) * expectedEmbeddingSize * threeDFactor;

        variables[i]->assignPosition(pt);
    }
}

void ForceDirectedGraphLayouter::setInitialPositions() {
    GraphComponent childrenComponentsStar;
    Vertex *childrenComponentsStarRoot = NULL;

    // first pre embed all connected components, one by one
    for (std::vector<GraphComponent *>::iterator it = graphComponent.connectedSubComponents.begin(); it != graphComponent.connectedSubComponents.end(); it++) {
        GraphComponent *childComponent = *it;
        childComponent->calculateSpanningTree();

        // use tree embedding if connected component is a tree
        if (childComponent->getEdgeCount() == childComponent->getVertexCount() - 1) {
            StarTreeEmbedding starTreeEmbedding(childComponent, expectedEdgeLength);
            starTreeEmbedding.embed();
        }
        else {
            HeapEmbedding heapEmbedding(childComponent, expectedEdgeLength);
            heapEmbedding.embed();
        }

        // add a new vertex to component graph for each embedded sub component
        Vertex *childComponentVertex = new Vertex(Pt::getNil(), childComponent->getBoundingRectangle().rs, NULL);
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
    // use both connected component pre embedding coordinates and component graph coordinates
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
}

void ForceDirectedGraphLayouter::ensureBorders()
{
    if (!topBorder) {
        topBorder = new WallBody(true, NaN);
        bottomBorder = new WallBody(true, NaN);
        leftBorder = new WallBody(false, NaN);
        rightBorder = new WallBody(false, NaN);
    }
}

void ForceDirectedGraphLayouter::addBorderForceProviders()
{
    // add electric repulsion to all bodies
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];
        embedding.addForceProvider(new VerticalElectricRepulsion(topBorder, body));
        embedding.addForceProvider(new VerticalElectricRepulsion(bottomBorder, body));
        embedding.addForceProvider(new HorizontalElectricRepulsion(leftBorder, body));
        embedding.addForceProvider(new HorizontalElectricRepulsion(rightBorder, body));
    }

    // add springs between walls
    embedding.addBody(topBorder);
    embedding.addBody(bottomBorder);
    embedding.addBody(leftBorder);
    embedding.addBody(rightBorder);
    embedding.addForceProvider(new VerticalSpring(topBorder, bottomBorder, -1, expectedEmbeddingSize));
    embedding.addForceProvider(new HorizonalSpring(leftBorder, rightBorder, -1, expectedEmbeddingSize));
}

void ForceDirectedGraphLayouter::setBorderPositions()
{
    double distance = 100;
    double top = DBL_MAX, bottom = DBL_MIN;
    double left = DBL_MAX, right = DBL_MIN;

    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body)) {
            top = std::min(top, body->getTop());
            bottom = std::max(bottom, body->getBottom());
            left = std::min(left, body->getLeft());
            right = std::max(right, body->getRight());
        }
    }

    // ensure all bodies are within
    topBorder->setPosition(top - distance);
    bottomBorder->setPosition(bottom + distance);
    leftBorder->setPosition(left - distance);
    rightBorder->setPosition(right + distance);
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
    for (int i = 0; i < (int)bodies.size(); i++) {
          IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body))
            embedding.addForceProvider(new BasePlaneSpring(body, 5, 0));
    }
}

void ForceDirectedGraphLayouter::scale()
{
    // find average spring length
    int springCount = 0;
    double averageSpringLength = 0;
    const std::vector<IForceProvider *>& forceProviders = embedding.getForceProviders();
    for (int i = 0; i < (int)forceProviders.size(); i++) {
        Spring *spring = dynamic_cast<Spring *>(forceProviders[i]);

        if (spring) {
            const Pt& pt1 = spring->getBody1()->getPosition();
            const Pt& pt2 = spring->getBody2()->getPosition();
            springCount++;
            averageSpringLength += pt1.getBasePlaneProjectionDistance(pt2);
        }
    }

    double edgeScale = 0;
    if (springCount) {
        averageSpringLength /= springCount;
        Assert(!isNaN(averageSpringLength));
        edgeScale = expectedEdgeLength / averageSpringLength;
    }

    double actualSize = sqrt(embedding.getBoundingRectangle().rs.getArea());
    double areaScale = expectedEmbeddingSize / actualSize;

    double scale = springCount ? (edgeScale + areaScale) / 2 : areaScale;
    scale = std::max(0.2, std::min(5.0, scale));

    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < (int)variables.size(); i++) {
        variables[i]->assignPosition(Pt(variables[i]->getPosition()).multiply(scale));
    }
}

void ForceDirectedGraphLayouter::translate()
{
    double top = DBL_MAX;
    double left = DBL_MAX;

    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];

        if (!dynamic_cast<WallBody *>(body)) {
            top = std::min(top, body->getTop());
            left = std::min(left, body->getLeft());
        }
    }

    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < (int)variables.size(); i++) {
        variables[i]->assignPosition(Pt(variables[i]->getPosition()).subtract(Pt(left - border, top - border, 0)));
    }
}

void ForceDirectedGraphLayouter::addMovableNode(cModule *mod, int width, int height)
{
    hasMovableNode = true;

    // this is a free variable and body
    Variable *variable = new Variable(Pt::getNil());
    addBody(mod, new Body(variable, Rs(width, height)));

    graphComponent.addVertex(new Vertex(Pt::getNil(), Rs(width, height), variable));
}

void ForceDirectedGraphLayouter::addFixedNode(cModule *mod, int x, int y, int width, int height)
{
    hasFixedNode = true;

    // a fix node is a constrained variable which can only move on the z axes
    Variable *variable = new PointConstrainedVariable(Pt(x, y, NaN));
    IBody *body = new Body(variable, Rs(width, height));
    addBody(mod, body);

    graphComponent.addVertex(new Vertex(Pt(x - width / 2, y - height / 2, NaN), Rs(width, height), variable));
}

void ForceDirectedGraphLayouter::addAnchoredNode(cModule *mod, const char *anchorname, int offx, int offy, int width, int height)
{
    hasAnchoredNode = true;

    // an anchored node is a relatively (to a variable, namely the anchor) positioned body
    Variable *variable = ensureAnchorVariable(anchorname);
    addBody(mod, new RelativelyPositionedBody(variable, Pt(offx, offy, 0), Rs(width, height)));

    // update vertex size
    Vertex *vertex = graphComponent.findVertex(variable);
    if (!vertex)
        graphComponent.addVertex(new Vertex(Pt::getNil(), Rs(offx + width / 2, offy + height / 2), variable));
    else {
        Rs rs = vertex->rc.rs;
        vertex->rc.rs = Rs(std::max(rs.width, (double)(offx + width / 2)), std::max(rs.height, (double)(offy + height / 2)));
    }
}

void ForceDirectedGraphLayouter::addEdge(cModule *from, cModule *to, int len)
{
    // an edge is a spring
    Spring *spring = new Spring(findBody(from), findBody(to), -1, !len ? -1 : len);
    embedding.addForceProvider(spring);

    // and also an edge in the graph
    graphComponent.addEdge(new Edge(
            graphComponent.findVertex(spring->getBody1()->getVariable()),
            graphComponent.findVertex(spring->getBody2()->getVariable())));
}

void ForceDirectedGraphLayouter::addEdgeToBorder(cModule *from, int len)
{
    ensureBorders();

    IBody *body = findBody(from);

    // add a force provider which uses for springs each connected to a wall
    // the least expanded spring will provided an attraction force
    std::vector<AbstractSpring *> springs;
    springs.push_back(new VerticalSpring(topBorder, body, -1, !len ? -1 : len));
    springs.push_back(new VerticalSpring(bottomBorder, body, -1, !len ? -1 : len));
    springs.push_back(new HorizonalSpring(leftBorder, body, -1, !len ? -1 : len));
    springs.push_back(new HorizonalSpring(rightBorder, body, -1, !len ? -1 : len));
    embedding.addForceProvider(new LeastExpandedSpring(springs));
}

void ForceDirectedGraphLayouter::execute()
{
    if (hasMovableNode) {
        calculateExpectedMeasures();
        setParameters();

        // always calculate graph components
        graphComponent.calculateConnectedSubComponents();

        // call pre-embedding if requested
        if (preEmbedding) {
            setInitialPositions();

            // draw initial pre embedding result
            if (environment->inspected())
                debugDraw();
        }

        // call force directed embedding if requested
        if (forceDirectedEmbedding) {
            if (topBorder)
                addBorderForceProviders();

            addElectricRepulsions();

            if (threeDFactor > 0)
                addBasePlaneSprings();

            embedding.addForceProvider(new Drag());

            // assign random values to missing positions
            setRandomPositions();

            if (topBorder)
                setBorderPositions();

            //embedding.debug = 3;
            embedding.inspected = environment->inspected();
            embedding.reinitialize();

            if (embedding.inspected) {
                while (!embedding.getFinished()) {
                    embedding.embed();
                    debugDraw();
                }
            }
            else
                embedding.embed();
        }

        // set random positions if no embedding was used
        if (!preEmbedding && !forceDirectedEmbedding)
            setRandomPositions();

        if (!hasFixedNode) {
            if (!hasAnchoredNode)
                scale();
            translate();
        }
    }
}

void ForceDirectedGraphLayouter::getNodePosition(cModule *mod, int& x, int& y)
{
    IBody *body = findBody(mod);
    const Pt& pt = body->getPosition();
    x = (int) pt.x;
    y = (int) pt.y;
}

void ForceDirectedGraphLayouter::debugDraw()
{
    environment->clearGraphics();

    // draw bodies
    const std::vector<IBody *>& bodies = embedding.getBodies();
    for (int i = 0; i < (int)bodies.size(); i++) {
        IBody *body = bodies[i];
        Pt pt(body->getPosition());
        Rs rs = body->getSize();
        pt.subtract(Pt(rs.width / 2, rs.height / 2, 0));

        // wall bodies are long lines
        if (dynamic_cast<WallBody *>(body)) {
            if (rs.width == POSITIVE_INFINITY) {
                pt.x = -1000;
                rs.width = 2000;
            }

            if (rs.height == POSITIVE_INFINITY) {
                pt.y = -1000;
                rs.height = 2000;
            }
        }

        environment->drawRectangle(pt.x , pt.y, pt.x + rs.width, pt.y + rs.height, "{node bbox}", "black");
        char text[100];
        sprintf(text,"%g", pt.z);
        environment->drawText(pt.x, pt.y, text, "{node bbox}", "black");
    }

    // draw springs
    const std::vector<IForceProvider *>& forceProviders = embedding.getForceProviders();
    for (int i = 0; i < (int)forceProviders.size(); i++) {
        Spring *spring = dynamic_cast<Spring *>(forceProviders[i]);

        if (spring) {
            const Pt& pt1 = spring->getBody1()->getPosition();
            const Pt& pt2 = spring->getBody2()->getPosition();
            environment->drawLine(pt1.x, pt1.y, pt2.x, pt2.y, "{edge bbox}", "black");
        }
    }

    // draw forces
    double forceScale = 10;
    const std::vector<Variable *>& variables = embedding.getVariables();
    for (int i = 0; i < (int)variables.size(); i++) {
        Variable *variable = variables[i];
        const Pt& pt1 = variable->getPosition();

        if (showForces) {
            std::vector<Pt> forces = variable->getForces();

            for (int j = 0; j < (int)forces.size(); j++) {
                Pt pt2(pt1);
                Pt force(forces[j]);
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


