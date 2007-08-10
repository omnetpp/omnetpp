package org.omnetpp.figures.misc;

import org.eclipse.gef.tools.CellEditorLocator;

/**
 * Figures that support direct editing their name should implement this interface
 * @author rhornig
 */
public interface IDirectEditSupport {

    /**
     * Returns the text that should be direct edited
     */
    String getDirectEditText();

    /**
     * Sets the text that is displayed under the cell editor. Note that this changes
     * ONLY the figure, so the underlying model is NOT changed by this method. Model should be changed
     * by a command.
     */
    void setDirectEditTextVisible(boolean visible);

    /**
     * Returns an editor locator for placing the cell editor
     */
    CellEditorLocator getDirectEditCellEditorLocator();
}
