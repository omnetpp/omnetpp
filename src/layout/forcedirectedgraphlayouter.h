//==========================================================================
//  FORCEDIRECTEDGRAPHLAYOUTER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FORCEDIRECTEDGRAPHLAYOUTER_H
#define __FORCEDIRECTEDGRAPHLAYOUTER_H

#include <vector>
#include <map>

#include "graphlayouter.h"
#include "forcedirectedembedding.h"
#include "forcedirectedparameters.h"
#include "graphcomponent.h"

NAMESPACE_BEGIN


/**
 * XXX
 */
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

NAMESPACE_END


#endif


