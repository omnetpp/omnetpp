package org.omnetpp.figures.layout;

import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.common.displaymodel.PointF;

public interface ILayoutAlgorithm {
    /**
     * Sets the default preferred edge length
     */
    void setDefaultEdgeLength(int edgeLength);

    /**
     * Sets the size of the background (optional)
     */
    void setSize(double width, double height, double border);

    /**
     * Add node that can be moved.
     */
    void addMovableNode(int nodeId, double width, double height);

    /**
     * Add fixed node
     */
    void addFixedNode(int nodeId, double x, double y, double width, double height);

    /**
     * May be called after addAnchoredNode.
     */
    void setAnchorPosition(String anchor, double x, double y);

    /**
     * Add node that is anchored to a freely movable anchor point. Nodes anchored
     * to the same anchor point can only move together. Anchor points are
     * identified by name, and they need not be pre-declared (they are registered
     * on demand.) Usage: module vectors in ring, matrix, etc. layout.
     *
     * offx, offy: offset to anchor point
     */
    void addAnchoredNode(int nodeId, String anchor, double offx, double offy, double width, double height);

    /**
     * Add connection (graph edge)
     */
    void addEdge(int srcNodeId, int destNodeId, double len);

    /**
     * Add connection (graph edge) to enclosing (parent) module
     */
    void addEdgeToBorder(int srcNodeId, double len);

    /**
     * Set rng seed
     */
    void setSeed(int seed);

    /**
     * The current seed of the random number generator
     */
    int getSeed();

    /**
     * Run the layouting algorithm.
     */
    void execute();

    /**
     * Extract the results after layouting
     */
    PointF getNodePosition(int nodeId);

    /**
     * Returns the position of the given anchor after layouting
     */
    PrecisionPoint getAnchorPosition(String anchor);

//    public void setEnvironment(GraphLayouterEnvironment environment);

}