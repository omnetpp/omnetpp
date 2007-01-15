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

#include "graphcomponent.h"

Vertex::Vertex(Pt pt, Rs rc, void *identity) {
    this->pt = pt;
    this->rs = rc;
    this->identity = identity;

    spanningTreeParent = NULL;
    starTreeCenter = Pt::getNil();
    starTreeCircleCenter = Pt::getNil();
    starTreeRadius = -1;
}

Edge::Edge(Vertex *source, Vertex *target, void *identity) {
    this->source = source;
    this->target = target;
    this->identity = identity;
}

GraphComponent::GraphComponent() {
    spanningTreeRoot = NULL;
}

GraphComponent::~GraphComponent() {
    for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++)
        delete *it;

    for (std::vector<Edge *>::iterator it = edges.begin(); it != edges.end(); it++)
        delete *it;
}

int GraphComponent::addVertex(Vertex *vertex) {
    vertices.push_back(vertex);
    return vertices.size() - 1;
}

int GraphComponent::addEdge(Edge *edge) {
    edges.push_back(edge);
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
