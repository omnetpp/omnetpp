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

class Vertex {
    public:
        std::vector<Vertex *> neighbours;

	    Pt pt;

	    Rs rs;

	    void *identity;
    	
	    Vertex *spanningTreeParent;
    	
        std::vector<Vertex *> spanningTreeChildren;

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
	    Vertex(Pt pt, Rs rc, void *identity = NULL) {
		    this->pt = pt;
		    this->rs = rc;
		    this->identity = identity;
    	
	        spanningTreeParent = NULL;
            starTreeCenter = Pt::getNil();
	        starTreeCircleCenter = Pt::getNil();
	        starTreeRadius = -1;
	    }
};

class Edge {
    public:
	    Vertex *source;

	    Vertex *target;
	
	    void *identity;

    public:
	    Edge(Vertex *source, Vertex *target, void *identity = NULL) {
		    this->source = source;
		    this->target = target;
		    this->identity = identity;
	    }
};

/**
 * A coherent component of a graph.
 */
class GraphComponent {
    private:
        std::vector<Vertex *> vertices;

        std::vector<Edge *> edges;
	
    public:
	    Vertex *spanningTreeRoot;

    public:
	    GraphComponent() {
            spanningTreeRoot = NULL;
	    }

        ~GraphComponent() {
            for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++)
                delete *it;

            for (std::vector<Edge *>::iterator it = edges.begin(); it != edges.end(); it++)
                delete *it;
        }
    	
	    bool isEmpty() {
		    return vertices.size() == 0;
	    }

	    int addVertex(Vertex *vertex) {
		    vertices.push_back(vertex);
		    return vertices.size() - 1;
	    }

	    int addEdge(Edge *edge) {
		    edges.push_back(edge);
            edge->source->neighbours.push_back(edge->target);
            edge->target->neighbours.push_back(edge->source);
		    return edges.size() - 1;
	    }

	    int getVertexCount() {
		    return vertices.size();
	    }

	    int indexOfVertex(Vertex *vertex) {
            std::vector<Vertex *>::iterator it = find(vertices.begin(), vertices.end(), vertex);

            if (it == vertices.end())
                return -1;
            else
                return it - vertices.begin();
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

	    void calculateSpanningTree() {
		    Vertex *rootVertex = NULL;

            for (std::vector<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); it++) {
                Vertex *vertex = *it;

                if (rootVertex == NULL || rootVertex->neighbours.size() < vertex->neighbours.size())
                    rootVertex = vertex;
            }

		    if (vertices.size() != 0)
			    calculateSpanningTree(rootVertex);
	    }

	    void calculateSpanningTree(Vertex *rootVertex) {
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
    		
		    while (vertices.size() != 0) {
			    Vertex *vertex = vertices[0];
                vertices.pop_front();

                for (std::vector<Vertex *>::iterator it = vertex->neighbours.begin(); it != vertex->neighbours.end(); it++) {
                    Vertex *neighbour = *it;

                    if (!neighbour->color) {
					    addToSpanningTreeParent(vertex, neighbour);
    					
					    // breadth search
					    vertices.push_back(neighbour);
				    }
                }
		    }
	    }

    private:
	    void addToSpanningTreeParent(Vertex *parentVertex, Vertex *vertex) {
            vertex->color = 1;
		    vertex->spanningTreeParent = parentVertex;
    		
		    if (parentVertex)
			    parentVertex->spanningTreeChildren.push_back(vertex);
	    }
};

#endif
