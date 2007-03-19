package org.omnetpp.figures;

import org.eclipse.draw2d.Label;

/**
 * @author rhornig
 * Figures that support direct editing their name should implement this interface
 */
public interface IDirectEditSupport {
    
    /**
     * @return The label figure describing the figure name
     */
    Label getLabel();

}
