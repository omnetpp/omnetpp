//=========================================================================
//  GRAPHCOMPONENT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cfloat>
#include "graphcomponent.h"

namespace omnetpp {
namespace layout {

Vertex::Vertex(Pt pt, Rs rs, void *identity) : rc(pt, rs), identity(identity)
{
}

Edge::Edge(Vertex *source, Vertex *target, void *identity) : source(source), target(target), identity(identity)
{
}

GraphComponent::~GraphComponent()
{
    if (owner) {
        for (auto & vertex : vertices)
            delete vertex;

        for (auto & edge : edges)
            delete edge;

        for (auto & connectedSubComponent : connectedSubComponents)
            delete connectedSubComponent;
    }
}

int GraphComponent::addVertex(Vertex *vertex)
{
    vertices.push_back(vertex);
    return vertices.size() - 1;
}

int GraphComponent::addEdge(Edge *edge)
{
    edges.push_back(edge);
    edge->source->edges.push_back(edge);
    edge->target->edges.push_back(edge);
    edge->source->neighbours.push_back(edge->target);
    edge->target->neighbours.push_back(edge->source);
    return edges.size() - 1;
}

int GraphComponent::indexOfVertex(Vertex *vertex)
{
    std::vector<Vertex *>::iterator it = find(vertices.begin(), vertices.end(), vertex);

    if (it == vertices.end())
        return -1;
    else
        return it - vertices.begin();
}

Vertex *GraphComponent::findVertex(void *identity)
{
    for (auto vertex : vertices) {
        if (vertex->identity == identity)
            return vertex;
    }

    return nullptr;
}

Rc GraphComponent::getBoundingRectangle()
{
    double top = DBL_MAX, bottom = DBL_MIN;
    double left = DBL_MAX, right = DBL_MIN;

    for (auto vertex : vertices) {
        Pt pt = vertex->rc.pt;
        Rs rs = vertex->rc.rs;

        top = std::min(top, pt.y);
        bottom = std::max(bottom, pt.y + rs.height);
        left = std::min(left, pt.x);
        right = std::max(right, pt.x + rs.width);
    }

    return Rc(left, top, 0, right - left, bottom - top);
}

void GraphComponent::calculateSpanningTree()
{
    Vertex *rootVertex = nullptr;
    spanningTreeVertices.clear();

    for (auto vertex : vertices) {
        if (rootVertex == nullptr || rootVertex->neighbours.size() < vertex->neighbours.size())
            rootVertex = vertex;
    }

    if (!vertices.empty())
        calculateSpanningTree(rootVertex);
}

void GraphComponent::calculateSpanningTree(Vertex *rootVertex)
{
    spanningTreeRoot = rootVertex;

    for (auto vertex : vertices) {
        vertex->spanningTreeChildren.clear();
        vertex->spanningTreeParent = nullptr;
        vertex->color = 0;
    }

    addToSpanningTreeParent(nullptr, rootVertex);
    std::deque<Vertex *> vertices;
    vertices.push_back(rootVertex);
    spanningTreeVertices.push_back(rootVertex);

    while (!vertices.empty()) {
        Vertex *vertex = vertices[0];
        vertices.pop_front();

        for (std::vector<Vertex *>::iterator it = vertex->neighbours.begin(); it != vertex->neighbours.end(); ++it) {
            Vertex *neighbour = *it;

            if (!neighbour->color) {
                addToSpanningTreeParent(vertex, neighbour);

                // breadth search
                vertices.push_back(neighbour);
                spanningTreeVertices.push_back(neighbour);
            }
        }
    }
}

void GraphComponent::addToSpanningTreeParent(Vertex *parentVertex, Vertex *vertex)
{
    vertex->color = 1;
    vertex->spanningTreeParent = parentVertex;

    if (parentVertex)
        parentVertex->spanningTreeChildren.push_back(vertex);
}

void GraphComponent::calculateConnectedSubComponents()
{
    connectedSubComponents.clear();

    for (auto vertex : vertices) {
        vertex->color = 0;
    }

    for (auto edge : edges) {
        edge->color = 0;
    }

    int color = 1;

    for (auto vertex : vertices) {
        if (!vertex->color) {
            GraphComponent *childComponent = new GraphComponent();
            childComponent->owner = false;
            connectedSubComponents.push_back(childComponent);

            colorizeConnectedSubComponent(childComponent, vertex, color++);
        }
    }
}

void GraphComponent::colorizeConnectedSubComponent(GraphComponent *childComponent, Vertex *vertex, int color)
{
    vertex->color = color;
    vertex->connectedSubComponent = childComponent;
    childComponent->vertices.push_back(vertex);

    for (auto edge : vertex->edges) {
        if (!edge->color) {
            edge->color = color;
            edge->connectedSubComponent = childComponent;
            childComponent->edges.push_back(edge);
        }
    }

    for (auto neighbour : vertex->neighbours) {
        if (!neighbour->color)
            colorizeConnectedSubComponent(childComponent, neighbour, color);
    }
}

}  // namespace layout
}  // namespace omnetpp

