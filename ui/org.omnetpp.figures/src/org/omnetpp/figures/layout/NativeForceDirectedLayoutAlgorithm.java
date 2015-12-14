package org.omnetpp.figures.layout;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.common.displaymodel.PointF;
import org.omnetpp.layout.engine.ForceDirectedGraphLayouter;
import org.omnetpp.layout.engine.GraphLayouterEnvironment;

public class NativeForceDirectedLayoutAlgorithm extends ForceDirectedGraphLayouter implements ILayoutAlgorithm {

    public NativeForceDirectedLayoutAlgorithm(GraphLayouterEnvironment environment) {
        Assert.isNotNull(environment);
        setEnvironment(environment);
    }

    public PointF getNodePosition(int mod) {
        return new PointF((float)super.getNodePositionX(mod), (float)super.getNodePositionY(mod));
    }

    public PrecisionPoint getAnchorPosition(String anchor) {
        throw new RuntimeException("not implemented");
    }

    public void setAnchorPosition(String anchor, double x, double y) {
        throw new RuntimeException("not implemented");
    }

    public void setDefaultEdgeLength(int edgeLength) {
        // TODO Auto-generated method stub

    }

    public void setMaxIterations(int iterations) {
        // TODO Auto-generated method stub

    }

    public void setRepulsiveForce(double force) {
        // TODO Auto-generated method stub

    }

    public void setAttractionForce(double force) {
        // TODO Auto-generated method stub

    }
}
