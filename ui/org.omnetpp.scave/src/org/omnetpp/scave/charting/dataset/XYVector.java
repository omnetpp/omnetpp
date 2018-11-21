package org.omnetpp.scave.charting.dataset;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.XYArray;

/**
 * Java version of libscave's XYArray
 *
 * @author andras
 */
//TODO preciseX; eventNumbers?
public class XYVector {
    public double x[];
    public double y[];

    public XYVector(XYArray array) {
        int length = array.length();
        x = new double[length];
        y = new double[length];
        for (int i = 0; i < length; i++) {
            x[i] = array.getX(i);
            y[i] = array.getY(i);
        }
    }

    public int size() {
        Assert.isTrue(x.length == y.length);
        return x.length;
    }

    public void setSize(int size) {
        Assert.isTrue(x.length == y.length);
        if (size == x.length)
            return;
        double[] newX = new double[size];
        double[] newY = new double[size];
        System.arraycopy(x, 0, newX, 0, Math.min(size, x.length));
        System.arraycopy(y, 0, newY, 0, Math.min(size, y.length));
        x = newX;
        y = newY;
    }

    public void copy(int src, int dest) {
        x[dest] = x[src];
        y[dest] = y[src];
    }

    public XYArray toXYArray() {
        //TODO XYArray has no setters ATM
        return null;
    }


};
