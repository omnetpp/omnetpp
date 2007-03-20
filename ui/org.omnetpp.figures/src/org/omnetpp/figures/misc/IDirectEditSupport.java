package org.omnetpp.figures.misc;

import org.eclipse.gef.tools.CellEditorLocator;

/**
 * @author rhornig
 * Figures that support direct editing their name should implement this interface
 */
public interface IDirectEditSupport {
    
    /**
     * @return The text that should be direct edited
     */
    String getDirectEditText();
    
    /**
     * @return An editor locator to place the cell editor at the correct place 
     */
    CellEditorLocator getDirectEditCellEditorLocator();
}
