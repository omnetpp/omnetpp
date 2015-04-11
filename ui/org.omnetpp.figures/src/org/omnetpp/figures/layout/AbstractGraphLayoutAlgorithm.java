/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;

import org.eclipse.core.runtime.Assert;

/**
 * Abstract base class for graph layouting algorithms.
 *
 * @author andras
 */
public abstract class AbstractGraphLayoutAlgorithm implements ILayoutAlgorithm
{
    protected static boolean debug = false;

    protected static boolean firstTime = true;

    protected double defaultEdgeLen;

    // params set by setConfineToArea()
    protected double width;
    protected double height;
    protected double border;

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
        // Debug.println("Layouter: internal RNG tested and OK");
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

    public void setSeed(int seed) {
        rndseed = seed;
    }

    public int getSeed() {
        return rndseed;
    }

    /**
     * Set preferred graph edge length
     */
    public void setDefaultEdgeLength(int len) {
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
    void setScaleToArea(double width, double height, double border) {
        this.width = width;
        this.height = height;
        this.border = border;
        sizingMode = SIZINGMODE_SCALE;
    }
}

