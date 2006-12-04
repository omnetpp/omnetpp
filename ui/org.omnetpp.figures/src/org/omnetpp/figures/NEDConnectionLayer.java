package org.omnetpp.figures;

import org.eclipse.draw2d.ConnectionLayer;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * @author rhornig
 * A special layer used in compound module, for holding the connecions. This
 * layer overrides the getFreeFormExtent method, do the size of this layer will not be
 * used durnig the calculation of compound module size.
 */
public class NEDConnectionLayer extends ConnectionLayer {
    
    /* (non-Javadoc)
     * @see org.eclipse.draw2d.FreeformLayer#getFreeformExtent()
     * This method is overridden so the size of a NEDConnecionLayer is not counted during 
     * compound module size calculation. This prevents showing the scrollbars if we move 
     * a connection endpoint out of the compound module
     */
    @Override
    public Rectangle getFreeformExtent() {
        // the size of the figure is always smaller than it's parent (it's a point in the middle)
        return getParent().getBounds().getCopy().scale(0);
    }

}
