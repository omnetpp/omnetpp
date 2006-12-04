package org.omnetpp.figures;

import org.eclipse.draw2d.FreeformLayer;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * @author rhornig
 * A special freform layer that always report 0 size as extent so it will not be used
 * during the calculation of the size of parent freeformpane
 */
public class NonExtendableFreeformLayer extends FreeformLayer {

    @Override
    public Rectangle getFreeformExtent() {
        // the size of the figure is always smaller than it's parent (it's a point in the middle of the parent figure)
        return getParent().getBounds().getCopy().scale(0);
    }
}
