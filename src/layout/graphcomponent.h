//=========================================================================
//  GRAPHCOMPONENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_GRAPHCOMPONENT_H
#define __OMNETPP_LAYOUT_GRAPHCOMPONENT_H

#include <algorithm>
#include <vector>
#include <deque>
#include "geometry.h"

namespace omnetpp {
namespace layout {

class Edge;
class GraphComponent;

/**
 * A vertex in a graph component.
 */
class Vertex {
    public:
        /**
         * All neighbours of this vertex. Updated during building the graph.
         */
        std::vector<Vertex *> neighbours;

        /**
         * All incoming and outgoing edges. Updated during building the graph.
         */
        std::vector<Edge *> edges;

        /**
         * Position and size assigned to this vertex. Embedding algorithms will set
         * the position here.
         */
        Rc rc;

        /**
         * Used to look up the vertex. Not used by any graph algorithm.
         */
        void *identity;

        /**
         * The parent of this vertex in the spanning tree. Filled by calculateSpanningTree in
         * the owner graphComponent.
         */
        Vertex *spanningTreeParent;

        /**
         * Children vertices of this vertex in the spanning tree. Filled by calculateSpanningTree in
         * the owner graphComponent.
         */
        std::vector<Vertex *> spanningTreeChildren;

        /**
         * The connected graph component this vertex is part of. Filled by calculateConnectedSubComponents
         * in the owner graphComponent.
         */
        GraphComponent *connectedSubComponent;

        /**
         * Used to colorize the vertex in graph algorithms.
         */
        int color;

        /**
         * Center coordinate relative to parent.
         */
        Pt starTreeCenter;

        /**
         * Subtree enclosing circle center.
         */
        Pt starTreeCircleCenter;

        /**
         * Subtree enclosing circle radius.
         */
        double starTreeRadius;

    public:
        Vertex(Pt pt, Rs rc, void *identity = nullptr);
};

/**
 * An edge between two vertices in a graph component.
 */
class Edge {
    public:
        /**
         * One of the vertices where the edge ends.
         */
        Vertex *source;

        /**
         * One of the vertices where the edge ends.
         */
        Vertex *target;

        /**
         * Used to look up the edge. Not used by any graph algorithm.
         */
        void *identity;

        /**
         * The connected graph component this edge is part of. Filled by calculateConnectedSubComponents
         * in the owner graphComponent.
         */
        GraphComponent *connectedSubComponent;

        /**
         * Used to colorize the edge in graph algorithms.
         */
        int color;

    public:
        Edge(Vertex *source, Vertex *target, void *identity = nullptr);
};

/**
 * A graph or a connected component of a graph.
 */
class GraphComponent {
    private:
        /**
         * True means this graphComponent owns the vertices and edges so it will
         * delete them in the descructor.
         */
        bool owner;

        /**
         * A list of vertices present in this component.
         */
        std::vector<Vertex *> vertices;

        /**
         * A list of edges present in this component.
         */
        std::vector<Edge *> edges;

    public:
        /**
         * The root of the spanning tree. Filled by calculateSpanningTree.
         */
        Vertex *spanningTreeRoot;

        /**
         * Contains all vertices in the order of the spanning tree traversal.
         */
        std::vector<Vertex *> spanningTreeVertices;

        /**
         * A list of connected sub graph components. These components do not own the
         * vertices and edges present in this component but still refer to them.
         */
        std::vector<GraphComponent *> connectedSubComponents;

    public:
        GraphComponent();
        ~GraphComponent();

        int addVertex(Vertex *vertex);
        int addEdge(Edge *edge);
        int indexOfVertex(Vertex *vertex);
        Vertex *findVertex(void *identity);
        Rc getBoundingRectangle();

        /**
         * Calculates the spanning tree using breadth search starting from the
         * vertex having the highest rank.
         */
        void calculateSpanningTree();
        void calculateSpanningTree(Vertex *rootVertex);

        /**
         * Calculates the list of connected sub graph components by colorizing vertices and edges.
         */
        void calculateConnectedSubComponents();

        bool isEmpty() {
            return vertices.empty();
        }

        int getVertexCount() {
            return vertices.size();
        }

        std::vector<Vertex *>& getVertices() {
            return vertices;
        }

        Vertex *getVertex(int index) {
            return vertices[index];
        }

        int getEdgeCount() {
            return edges.size();
        }

        std::vector<Edge *> getEdges() {
            return edges;
        }

        Edge *getEdge(int index) {
            return edges[index];
        }

    private:
        void addToSpanningTreeParent(Vertex *parentVertex, Vertex *vertex);
        void colorizeConnectedSubComponent(GraphComponent *childComponent, Vertex *vertex, int color);
};

} // namespace layout
}  // namespace omnetpp


#endif
