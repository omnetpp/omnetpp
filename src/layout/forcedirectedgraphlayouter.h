//==========================================================================
//  FORCEDIRECTEDGRAPHLAYOUTER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_FORCEDIRECTEDGRAPHLAYOUTER_H
#define __OMNETPP_LAYOUT_FORCEDIRECTEDGRAPHLAYOUTER_H

#include <vector>
#include <map>

#include "graphlayouter.h"
#include "forcedirectedembedding.h"
#include "forcedirectedparameters.h"
#include "graphcomponent.h"

namespace omnetpp {
namespace layout {


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
    bool hasMovableNode = false;

    /**
     * True means there is at least one fixed node.
     */
    bool hasFixedNode = false;

    /**
     * True means there is at least one anchored node.
     */
    bool hasAnchoredNode = false;

    /**
     * True means there is at least one edge to the border.
     */
    bool hasEdgeToBorder = false;

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
     * 0 means do not use 3d coordinates. Higher value means bigger initial 3d coordinates.
     */
    double threeDFactor;
    double threeDCoefficient;

    /**
     * Various measures calculated before the actual layout.
     */
    double expectedEmbeddingWidth = -1;
    double expectedEmbeddingHeight = -1;
    double expectedEdgeLength = -1;
    bool slippery = false;
    bool pointLikeDistance = true;

    // border bodies will be added if there are either
    // fixed nodes or edges connected to the border
    // or the width or the height of the bounding box is specified
    WallBody *topBorder = nullptr;
    WallBody *bottomBorder = nullptr;
    WallBody *leftBorder = nullptr;
    WallBody *rightBorder = nullptr;

    GraphComponent graphComponent;
    ForceDirectedEmbedding embedding;

    std::map<std::string, Variable *> anchorNameToVariableMap;
    std::map<int, IBody *> moduleToBodyMap;

  public:
    /**
     * Ctor, dtor
     */
    //@{
    ForceDirectedGraphLayouter() {}
    virtual ~ForceDirectedGraphLayouter();
    //@}

    void setParameters();

    /**
     * Add node that can be moved.
     */
    virtual void addMovableNode(int nodeId, double width, double height) override;

    /**
     * Add fixed node
     */
    virtual void addFixedNode(int nodeId, double x, double y, double width, double height) override;

    /**
     * Add node that is anchored to a freely movable anchor point. Nodes anchored
     * to the same anchor point can only move together. Anchor points are
     * identified by name, and they need not be predeclared (they are registered
     * on demand.) Usage: module vectors in ring, matrix, etc. layout.
     *
     * offx, offy: offset to anchor point
     */
    virtual void addAnchoredNode(int nodeId, const char *anchorname, double offx, double offy, double width, double height) override;

    /**
     * Add connection (graph edge)
     */
    virtual void addEdge(int srcNodeId, int destNodeId, double preferredLength=0) override;

    /**
     * Add connection (graph edge) to enclosing (parent) module
     */
    virtual void addEdgeToBorder(int srcNodeId, double preferredLength=0) override;

    /**
     * The layouting algorithm.
     */
    virtual void execute() override;

    /**
     * Extracting the results
     */
    virtual void getNodePosition(int nodeId, double& x, double& y) override;

  protected:
    void addBody(int nodeId, IBody *body);
    IBody *findBody(int nodeId);
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
     * Executes pre embedding, it always takes into consideration the bounding box.
     */
    void executePreEmbedding();

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
     * Returns the bounding box of the embedding.
     */
    Rc getBoundingBox();

    /**
     * Scale coordinates.
     */
    void scale(Pt pt);

    /**
     * Translates coordinates.
     */
    void translate(Pt pt);

    /**
     * Draws the current state of the layout to a window.
     */
    void debugDraw();
};

}  // namespace layout
}  // namespace omnetpp


#endif


