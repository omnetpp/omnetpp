//=========================================================================
//  GRAPHCOMPONENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __GRAPHCOMPONENT_H_
#define __GRAPHCOMPONENT_H_

#include <algorithm>
#include <vector>
#include <deque>
#include "geometry.h"

class Edge;
class GraphComponent;

class Vertex {
    public:
        std::vector<Vertex *> neighbours;

        std::vector<Edge *> edges;

        Rc rc;

	    void *identity;
    	
	    Vertex *spanningTreeParent;
    	
        std::vector<Vertex *> spanningTreeChildren;

        GraphComponent *coherentSubComponent;

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
	    Vertex(Pt pt, Rs rc, void *identity = NULL);
};

class Edge {
    public:
	    Vertex *source;

	    Vertex *target;
	
	    void *identity;

        GraphComponent *coherentSubComponent;

        int color;

    public:
	    Edge(Vertex *source, Vertex *target, void *identity = NULL);};

/**
 * A coherent component of a graph.
 */
class GraphComponent {
    private:
        bool owner;

        std::vector<Vertex *> vertices;

        std::vector<Edge *> edges;
	
    public:
	    Vertex *spanningTreeRoot;

        /**
         * Contains all vertices in the order of the spanning tree traversal.
         */
        std::vector<Vertex *> spanningTreeVertices;

        std::vector<GraphComponent *> coherentSubComponents;

    public:
	    GraphComponent();
        ~GraphComponent();    	

	    int addVertex(Vertex *vertex);
	    int addEdge(Edge *edge);
	    int indexOfVertex(Vertex *vertex);
        Vertex *findVertex(void *identity);
        Rs getSize();

	    void calculateSpanningTree();
	    void calculateSpanningTree(Vertex *rootVertex);

        void calculateCoherentSubComponents();

        bool isEmpty() {
		    return vertices.size() == 0;
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
        void colorizeCoherentSubComponent(GraphComponent *childComponent, Vertex *vertex, int color);
};

#endif
