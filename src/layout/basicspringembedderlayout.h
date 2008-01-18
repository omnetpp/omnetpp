//==========================================================================
//  BASICSPRINGEMBEDDERLAYOUT.H - part of
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

#ifndef __BASICSPRINGEMBEDDERLAYOUT_H
#define __BASICSPRINGEMBEDDERLAYOUT_H

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
#include <string>

#include "graphlayouter.h"

NAMESPACE_BEGIN

// currently it works fine without boxing the graph
//#define USE_CONTRACTING_BOX


/**
 * Implementation of the Spring Embedder algorithm. This class is the layouter
 * used in OMNeT++ 3.x.
 *
 * Simplifications:
 *  - ignores node size (this is visible when item is very long, e.g.
 *    500 x 10 pixels)
 *  - ignores connections to the parent module (nodes which are connected
 *    to the parent may end up in the middle of the layout)
 *  - it is also problematic if the graph is not connected -- disjoint
 *    parts tend to drift far away
 */
class LAYOUT_API BasicSpringEmbedderLayout : public GraphLayouter
{
  protected:
    struct Anchor
    {
        std::string name; // anchor name
        double x, y;      // position
        int refcount;     // how many nodes are anchored to it
        double dx, dy;    // internal: movement at each step (NOT preserved across iterations!)
    };

    struct Node
    {
        cModule *key;      // node "handle"
        bool fixed;        // allowed to move?
        Anchor *anchor;    // not NULL for achored nodes
        double x, y;       // position (of the center of the shape)
        int offx, offy;    // anchored nodes: offset to anchor point (and x,y are calculated)
        int sx, sy;        // half width/height

        double dx, dy;     // internal: movement at each step
        int color;         // internal: connected nodes share the same color
    };

    struct Edge
    {
        Node *from;
        Node *to;
        int len;
    };

    // AnchorList and NodeList must be vectors because iteration on std::map is very slow
    typedef std::vector<Anchor*> AnchorList;
    typedef std::vector<Node*> NodeList;
    typedef std::vector<Edge> EdgeList;

    AnchorList anchors;
    NodeList nodes;
    EdgeList edges;

#ifdef USE_CONTRACTING_BOX
    struct Box
    {
        bool fixed;      // allowed to move?
        double x1, y1;   // coordinates
        double x2, y2;

        double dx1, dy1; // movement
        double dx2, dy2;
    };

    // contracting bounding box
    Box box;
#endif

    int defaultEdgeLen;

    bool haveFixedNode;
    bool haveAnchoredNode;
    bool allNodesAreFixed;

    double minx, miny, maxx, maxy;

    int maxIterations;
    double repulsiveForce;
    double attractionForce;

#ifdef USE_CONTRACTING_BOX
  public:
    double boxContractionForce;
    double boxRepulsiveForce;
    double boxRepForceRatio;
#endif

  protected:
    // utility
    Node *findNode(cModule *mod);

    // mark connected nodes with same color (needed by relax())
    virtual void doColoring();

    // main algorithm (modified spring embedder)
    virtual double relax();

    // for debugging: draw whole thing in a window
    void debugDraw(int step);

  public:
    /**
     * Ctor, dtor
     */
    //@{
    BasicSpringEmbedderLayout();
    virtual ~BasicSpringEmbedderLayout();
    //@}

    /** Redefined GraphLayouter methods */
    //@{
    virtual void setEnvironment(GraphLayouterEnvironment *environment);
    void addMovableNode(cModule *mod, int width, int height);
    void addFixedNode(cModule *mod, int x, int y, int width, int height);
    void addAnchoredNode(cModule *mod, const char *anchorname, int offx, int offy, int width, int height);
    void addEdge(cModule *from, cModule *to, int len=0);
    void addEdgeToBorder(cModule *from, int len=0);
    virtual void execute();
    void getNodePosition(cModule *mod, int& x, int& y);
    //@}
};

NAMESPACE_END


#endif



