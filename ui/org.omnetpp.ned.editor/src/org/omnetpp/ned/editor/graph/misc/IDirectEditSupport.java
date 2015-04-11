/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.tools.CellEditorLocator;

/**
 * Figures that support direct editing their name should implement this interface
 *
 * @author rhornig
 */
public interface IDirectEditSupport {

    /**
     * Returns the text that should be direct edited
     */
    String getName();

    /**
     * Sets whether the label under the cell editor should be visible. When the cell editor is active
     * we should hide the original label otherwise it would look ugly during edit operation if the
     * new text is shorted than the old.
     */
    void showLabelUnderCellEditor(boolean isVisible);

    /**
     * Returns an editor locator for placing the cell editor
     */
    CellEditorLocator getDirectEditCellEditorLocator();
}
