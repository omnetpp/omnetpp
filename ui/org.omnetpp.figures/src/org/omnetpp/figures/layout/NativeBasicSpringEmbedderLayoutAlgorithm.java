package org.omnetpp.figures.layout;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.layout.engine.BasicSpringEmbedderLayout;
import org.omnetpp.layout.engine.GraphLayouterEnvironment;

public class NativeBasicSpringEmbedderLayoutAlgorithm extends BasicSpringEmbedderLayout implements ILayoutAlgorithm {
    
    public NativeBasicSpringEmbedderLayoutAlgorithm(GraphLayouterEnvironment environment) {
        Assert.isNotNull(environment);
        setEnvironment(environment);
    }

    public PrecisionPoint getNodePosition(int mod) {
        return new PrecisionPoint(super.getNodePositionX(mod), super.getNodePositionY(mod));
    }

    public PrecisionPoint getAnchorPosition(String anchor) {
        throw new RuntimeException("not implemented");
    }

    public void setAnchorPosition(String anchor, double x, double y) {
        throw new RuntimeException("not implemented");
    }
}
