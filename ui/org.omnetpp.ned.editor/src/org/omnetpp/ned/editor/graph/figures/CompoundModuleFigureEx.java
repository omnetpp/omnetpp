/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.gef.tools.CellEditorLocator;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.CompoundModuleTypeFigure;
import org.omnetpp.ned.editor.graph.misc.IDirectEditSupport;
import org.omnetpp.ned.editor.graph.misc.LabelCellEditorLocator;

/**
 * EDITING specific parts of the figure. Selection rectangle, direct editing etc.
 */
public class CompoundModuleFigureEx extends CompoundModuleTypeFigure implements
		HandleBounds, IDirectEditSupport {

    protected String tmpName;

    // Direct edit support
    public CellEditorLocator getDirectEditCellEditorLocator() {
        return new LabelCellEditorLocator(nameFigure);
    }

    public void showLabelUnderCellEditor(boolean visible) {
        // HACK to hide the text part only of the label
        if (!visible) {
            tmpName = nameFigure.getText();
            nameFigure.setText("");
        }
        else {
            if (StringUtils.isEmpty(nameFigure.getText()))
                nameFigure.setText(tmpName);
        }
        invalidate();
        validate();
    }

    /**
     * Returns whether the point is on the border area, where dragging and selection and connection start/end is possible.
	 * Coordinates are viewport relative.
     */
    public boolean isOnBorder(int x, int y) {
    	// translate the mouse coordinates to root figure relative
        Point mouse = new Point(x,y);
        translateToRelative(mouse);

        // translate the client area coordinates to root figure relative
        Rectangle clientArea = mainContainer.getClientArea().shrink(2*BORDER_SNAP_WIDTH, 2*BORDER_SNAP_WIDTH);
        mainContainer.translateToParent(clientArea);
        translateToParent(clientArea);
        return getBounds().contains(mouse) && !clientArea.contains(mouse);
    }

}
