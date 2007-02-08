//==========================================================================
//  GRAPHLAYOUT.H - part of
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

#ifndef __GRAPHLAYOUT_H
#define __GRAPHLAYOUT_H

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
#include <map>
#include <string>

#include "layoutdefs.h"

#include "inttypes.h"
#include "lcgrandom.h"

#include "forcedirectedembedding.h"
#include "forcedirectedparameters.h"
#include "graphcomponent.h"

// currently it works fine without boxing the graph
//#define USE_CONTRACTING_BOX

class cModule;

class LAYOUT_API GraphLayouterEnvironment
{
    public:
        virtual ~GraphLayouterEnvironment() {}
        virtual bool inspected() = 0;

        virtual bool getBoolParameter(const char *tagName, int index, bool defaultValue) = 0;
        virtual long getLongParameter(const char *tagName, int index, long defaultValue) = 0;
        virtual double getDoubleParameter(const char *tagName, int index, double defaultValue) = 0;

        virtual void clearGraphics() = 0;
        virtual void showGraphics(const char *text) = 0;
        virtual void drawText(double x, double y, const char *text, const char *tags, const char *color) = 0;
        virtual void drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color) = 0;
        virtual void drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color) = 0;
};

/**
 * Abstract base class for graph layouting algorithms.
 */
class LAYOUT_API GraphLayouter
{
  protected:
    LCGRandom lcgRandom;

    GraphLayouterEnvironment *environment;

    int width, height, border;
    enum {Free, Confine, Scale} sizingMode;

    double privRand01() { return lcgRandom.next01(); }

  public:
    /**
     * Ctor, dtor
     */
    //@{
    GraphLayouter();
    virtual ~GraphLayouter() {}
    //@}

    /**
     * Add node that can be moved.
     */
    virtual void addMovableNode(cModule *mod, int width, int height) = 0;

    /**
     * Add fixed node. (x,y) denotes the center of the node.
     */
    virtual void addFixedNode(cModule *mod, int x, int y, int width, int height) = 0;

    /**
     * Add node that is anchored to a freely movable anchor point. Nodes anchored
     * to the same anchor point can only move together. Anchor points are
     * identified by name, and they need not be predeclared (they are registered
     * on demand.) Usage: module vectors in ring, matrix, etc. layout.
     *
     * offx, offy: offset of the node center to the anchor point
     */
    virtual void addAnchoredNode(cModule *mod, const char *anchorname, int offx, int offy, int width, int height) = 0;

    /**
     * Add connection (graph edge). len is the preferred length (0==unspecified)
     */
    virtual void addEdge(cModule *from, cModule *to, int len=0) = 0;

    /**
     * Add connection (graph edge) to enclosing (parent) module. len is the
     * preferred length (0==unspecified)
     */
    virtual void addEdgeToBorder(cModule *from, int len=0) = 0;

    /**
     * Set parameters
     */
    //@{
    virtual void setEnvironment(GraphLayouterEnvironment *environment) {this->environment = environment;}
    void setSeed(int32 seed) { lcgRandom.setSeed(seed);}
    void setConfineToArea(int width, int height, int border); // TBD currently ignored
    void setScaleToArea(int width, int height, int border);
    //@}

    /**
     * The layouting algorithm.
     */
    virtual void execute() = 0;

    /**
     * Extracting the results. The returned position is the center of the module.
     */
    virtual void getNodePosition(cModule *mod, int& x, int& y) = 0;
};

/**
 * Implementation of the Spring Embedder algorithm.
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

class LAYOUT_API ForceDirectedGraphLayouter : public GraphLayouter
{
  protected:
    /**
     * Time to wait after drawing the actual state of the layouting process.
     */
    double debugWaitTime;

    /**
     * True means all forces will be shown in debug window.
     */
    bool showForces;

    /**
     * True means summa force will be shown in debug window.
     */
    bool showSummaForce;

    /**
     * True means there is at least one movable node.
     */
    bool hasMovableNode;

    /**
     * True means there is at least one fixed node.
     */
    bool hasFixedNode;

    /**
     * True means there is at least one anchored node.
     */
    bool hasAnchoredNode;

    /**
     * Use pre embedding to create an initial layout before calling the force directed embedding.
     */
    bool preEmbedding;

    /**
     * Use force directed embedding.
     */
    bool forceDirectedEmbedding;

    /**
     * Use 3d coordinates and return the base plane projection coordinates.
     * Add springs connected to the base plane.
     * 0 means don't use 3d coordinates. Higher value means bigger initial 3d coordinates.
     */
    double threeDFactor;

    /**
     * Various measures calculated before the actual layout.
     */
    double expectedEmbeddingSize;
    double expectedEdgeLength;
    bool pointLikeDistance;

    // border bodies added only if there are edges connected to the border
    WallBody *topBorder;
    WallBody *bottomBorder;
    WallBody *leftBorder;
    WallBody *rightBorder;

    GraphComponent graphComponent;
    ForceDirectedEmbedding embedding;

    std::map<std::string, Variable *> anchorNameToVariableMap;
    std::map<cModule *, IBody *> moduleToBodyMap;

  public:
    /**
     * Ctor, dtor
     */
    //@{
    ForceDirectedGraphLayouter();
    virtual ~ForceDirectedGraphLayouter() {}
    //@}

    void setParameters();

    /**
     * Add node that can be moved.
     */
    virtual void addMovableNode(cModule *mod, int width, int height);

    /**
     * Add fixed node
     */
    virtual void addFixedNode(cModule *mod, int x, int y, int width, int height);

    /**
     * Add node that is anchored to a freely movable anchor point. Nodes anchored
     * to the same anchor point can only move together. Anchor points are
     * identified by name, and they need not be predeclared (they are registered
     * on demand.) Usage: module vectors in ring, matrix, etc. layout.
     *
     * offx, offy: offset to anchor point
     */
    virtual void addAnchoredNode(cModule *mod, const char *anchorname, int offx, int offy, int width, int height);

    /**
     * Add connection (graph edge)
     */
    virtual void addEdge(cModule *from, cModule *to, int len=0);

    /**
     * Add connection (graph edge) to enclosing (parent) module
     */
    virtual void addEdgeToBorder(cModule *from, int len=0);

    /**
     * The layouting algorithm.
     */
    virtual void execute();

    /**
     * Extracting the results
     */
    virtual void getNodePosition(cModule *mod, int& x, int& y);

  protected:
    void addBody(cModule *mod, IBody *body);
    IBody *findBody(cModule *mod);
    Variable *ensureAnchorVariable(const char *anchorname);

    /**
     * Adds electric repulsions between bodies. Bodies being part of different connected
     * subcomponents will have a finite repulsion range determined by default spring repose length.
     */
    void addElectricRepulsions();

    /**
     * Adds springs generating attraction forces.
     */
    void addBasePlaneSprings();

    /**
     * Calculate various expected embedding measures such as width, height, edge length.
     */
    void calculateExpectedMeasures();

    /**
     * Assigns positions to coordinates not yet assigned by some other means.
     * The size of the random range is determined by the total size of the bodies
     * and the default spring repose length.
     */
    void setRandomPositions();

    /**
     * Executes pre embedding.
     */
    void setInitialPositions();

    /**
     * Adds border bodies to the force directed embedding.
     * Adds springs between left-right and top-bottom walls and electric repulsions to other bodies.
     */
    void ensureBorders();
    void addBorderForceProviders();

    /**
     * Assigns positions to border bodies so that all other bodies are within.
     */
    void setBorderPositions();

    /**
     * Scale coordinates so that average edge length will be the default.
     */
    void scale();

    /**
     * Translates coordinates so that the smallest x and y value will be equal to border size.
     */
    void translate();

    void debugDraw();
};

#endif
