//=========================================================================
//  GRAPHCOMPONENT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <float.h>
#include "graphcomponent.h"

USING_NAMESPACE

Vertex::Vertex(Pt pt, Rs rs, void *identity) {
    this->rc = Rc(pt, rs);
    this->identity = identity;

    color = 0;
    connectedSubComponent = NULL;
    spanningTreeParent = NULL;
    starTreeCenter = Pt::getNil();
    starTreeCircleCenter = Pt::getNil();
    starTreeRadius = -1;
}

Edge::Edge(Vertex *source, Vertex *target, void *identity) {
    this->source = source;
    this->target = target;
    this->identity = identity;

    color = 0;
    connectedSubComponent = NULL;
}

GraphComponent::GraphComponent() {
    owner = true;
    spanningTreeRoot = NULL;
}

GraphComponent::~GraphComponent() {
    if (owner) {
        for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++)
            delete *it;

        for (std::vector<Edge *>::iterator it = edges.begin(); it != edges.end(); it++)
            delete *it;

        for (std::vector<GraphComponent *>::iterator it = connectedSubComponents.begin(); it != connectedSubComponents.end(); it++)
            delete *it;
    }
}

int GraphComponent::addVertex(Vertex *vertex) {
    vertices.push_back(vertex);
    return vertices.size() - 1;
}

int GraphComponent::addEdge(Edge *edge) {
    edges.push_back(edge);
    edge->source->edges.push_back(edge);
    edge->target->edges.push_back(edge);
    edge->source->neighbours.push_back(edge->target);
    edge->target->neighbours.push_back(edge->source);
    return edges.size() - 1;
}

int GraphComponent::indexOfVertex(Vertex *vertex) {
    std::vector<Vertex *>::iterator it = find(vertices.begin(), vertices.end(), vertex);

    if (it == vertices.end())
        return -1;
    else
        return it - vertices.begin();
}

Vertex *GraphComponent::findVertex(void *identity)
{
    for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++) {
        Vertex *vertex = *it;

        if (vertex->identity == identity)
            return vertex;
    }

    return NULL;
}

Rc GraphComponent::getBoundingRectangle() {
    double top = DBL_MAX, bottom = DBL_MIN;
    double left = DBL_MAX, right = DBL_MIN;

    for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++) {
        Vertex *vertex = *it;

        Pt pt = vertex->rc.pt;
        Rs rs = vertex->rc.rs;

        top = std::min(top, pt.y);
        bottom = std::max(bottom, pt.y + rs.height);
        left = std::min(left, pt.x);
        right = std::max(right, pt.x + rs.width);
    }

    return Rc(left, top, 0, right - left, bottom - top);
}

void GraphComponent::calculateSpanningTree() {
    Vertex *rootVertex = NULL;
    spanningTreeVertices.clear();

    for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++) {
        Vertex *vertex = *it;

        if (rootVertex == NULL || rootVertex->neighbours.size() < vertex->neighbours.size())
            rootVertex = vertex;
    }

    if (vertices.size() != 0)
        calculateSpanningTree(rootVertex);
}

void GraphComponent::calculateSpanningTree(Vertex *rootVertex) {
    spanningTreeRoot = rootVertex;

    for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++) {
        Vertex *vertex = *it;
        vertex->spanningTreeChildren.clear();
        vertex->spanningTreeParent = NULL;
        vertex->color = 0;
    }

    addToSpanningTreeParent(NULL, rootVertex);
    std::deque<Vertex *> vertices;
    vertices.push_back(rootVertex);
    spanningTreeVertices.push_back(rootVertex);

    while (vertices.size() != 0) {
        Vertex *vertex = vertices[0];
        vertices.pop_front();

        for (std::vector<Vertex *>::iterator it = vertex->neighbours.begin(); it != vertex->neighbours.end(); it++) {
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

void GraphComponent::addToSpanningTreeParent(Vertex *parentVertex, Vertex *vertex) {
    vertex->color = 1;
    vertex->spanningTreeParent = parentVertex;
    
    if (parentVertex)
        parentVertex->spanningTreeChildren.push_back(vertex);
}

void GraphComponent::calculateConnectedSubComponents() {
    connectedSubComponents.clear();

    for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++) {
        Vertex *vertex = *it;
        vertex->color = 0;
    }

    for (std::vector<Edge *>::iterator it = edges.begin(); it != edges.end(); it++) {
        Edge *edge = *it;
        edge->color = 0;
    }

    int color = 1;

    for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++) {
        Vertex *vertex = *it;

        if (!vertex->color) {
            GraphComponent *childComponent = new GraphComponent();
            childComponent->owner = false;
            connectedSubComponents.push_back(childComponent);

            colorizeConnectedSubComponent(childComponent, vertex, color++);
        }
    }
}

void GraphComponent::colorizeConnectedSubComponent(GraphComponent *childComponent, Vertex *vertex, int color) {
    vertex->color = color;
    vertex->connectedSubComponent = childComponent;
    childComponent->vertices.push_back(vertex);

    for (std::vector<Edge *>::iterator it = vertex->edges.begin(); it != vertex->edges.end(); it++) {
        Edge *edge = *it;

        if (!edge->color) {
            edge->color = color;
            edge->connectedSubComponent = childComponent;
            childComponent->edges.push_back(edge);
        }
    }

    for (std::vector<Vertex *>::iterator it = vertex->neighbours.begin(); it != vertex->neighbours.end(); it++) {
        Vertex *neighbour = *it;

        if (!neighbour->color)
            colorizeConnectedSubComponent(childComponent, neighbour, color);
    }
}
