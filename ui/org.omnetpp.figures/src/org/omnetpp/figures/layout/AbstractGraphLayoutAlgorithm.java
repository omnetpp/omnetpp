package org.omnetpp.figures.layout;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Point;

/**
 * Abstract base class for graph layouting algorithms.
 *
 * @author andras
 */
public abstract class AbstractGraphLayoutAlgorithm
{
	protected static boolean debug = false;
 
	protected static boolean firstTime = true;
	
	protected int defaultEdgeLen;

    // params set by setConfineToArea()
    protected int width;
    protected int height;
    protected int border;

    protected static final int SIZINGMODE_FREE = 1;
    protected static final int SIZINGMODE_CONFINE = 2;
    protected static final int SIZINGMODE_SCALE = 3;

    protected int sizingMode;

    protected int rndseed;

    protected double privRand01() {
        // standard LCG32 RNG; see cLCG32 in the simulation kernel
        final int GLRAND_MAX = 0x7ffffffe;
        final int a=16807, q=127773, r=2836;
        rndseed = a * (rndseed % q) - r * (rndseed / q);  // Note: parens are important, don't remove!
        if (rndseed <= 0) rndseed += GLRAND_MAX + 1;
        return rndseed / (double)(GLRAND_MAX + 1);
    }

    private void rngSelfTest()
    {
    	rndseed = 1;
        for (int i=0; i<10000; i++)
            privRand01();
        Assert.isTrue(rndseed == 1043618065);
        // System.out.println("Layouter: internal RNG tested and OK");
    }    

    /**
     * Constructor
     */
    public AbstractGraphLayoutAlgorithm() {
    	// test the RNG once per session -- do not remove this code!
    	if (firstTime) {
    		firstTime = false;
    		rngSelfTest(); 
    	}
    	
        // go on with constructor
    	rndseed = 1;
        defaultEdgeLen = 40;
        width = height = border = 0;
        sizingMode = SIZINGMODE_FREE;
    }

    /**
     * Add node that can be moved.
     */
    public abstract void addMovableNode(Object mod, int x, int y, int width, int height);

    /**
     * Add fixed node
     */
    public abstract void addFixedNode(Object mod, int x, int y, int width, int height);

    /**
     * May be called after addAnchoredNode.
     */
    public abstract void setAnchorPosition(String anchor, int x, int y);

    /**
     * Add node that is anchored to a freely movable anchor point. Nodes anchored
     * to the same anchor point can only move together. Anchor points are
     * identified by name, and they need not be pre-declared (they are registered
     * on demand.) Usage: module vectors in ring, matrix, etc. layout.
     *
     * offx, offy: offset to anchor point
     */
    public abstract void addAnchoredNode(Object mod, String anchor, int offx, int offy, int width, int height);

    /**
     * Add connection (graph edge)
     */
    public abstract void addEdge(Object from, Object to, int len);

    /**
     * Add connection (graph edge) to enclosing (parent) module
     */
    public abstract void addEdgeToBorder(Object from, int len);

    /**
     * Set rng seed
     */
    void setSeed(int seed) {
        rndseed = seed;
    }

    /**
     * Set preferred graph edge length
     */
    void setDefaultEdgeLength(int len) {
        defaultEdgeLen = len;
    }

    /**
     * Nodes may be confined to a rectangle; currently unsupported.
     */
    void setConfineToArea(int width, int height, int border) {
        this.width = width;
        this.height = height;
        this.border = border;
        sizingMode = SIZINGMODE_CONFINE;
    }

    /**
     * After layouting, scale graph to fit into the given rectangle
     */
    void setScaleToArea(int width, int height, int border) {
        this.width = width;
        this.height = height;
        this.border = border;
        sizingMode = SIZINGMODE_SCALE;
    }

    /**
     * Run the layouting algorithm.
     */
    public abstract void execute();

    /**
     * Extract the results after layouting
     */
    public abstract Point getNodePosition(Object mod);

    public abstract Point getAnchorPosition(String anchor);
}

