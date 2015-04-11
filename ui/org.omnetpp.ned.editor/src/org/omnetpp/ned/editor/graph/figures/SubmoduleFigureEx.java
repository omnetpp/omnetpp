/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.editor.graph.misc.IDirectEditSupport;

/**
 * Editing-specific parts of the figure. Direct editing, etc.
 */
public class SubmoduleFigureEx extends SubmoduleFigure implements HandleBounds, IDirectEditSupport {

    protected String tmpName;

    public SubmoduleFigureEx() {
    }

    // support locator for finding the name of the figure allowing the direct edit
    // cell editor above the name figure
    class SubmoduleFigureNameCellEditorLocator implements CellEditorLocator {

        public void relocate(CellEditor celleditor) {
            Text text = (Text)celleditor.getControl();

            // adjust the font to the scaling
            FontData data = getFont().getFontData()[0];
            Dimension fontSize = new Dimension(0, data.getHeight());
            translateToAbsolute(fontSize);
            data.setHeight(fontSize.height);
            Font scaledFont = new Font(null, data);
            text.setFont(scaledFont);

            // position the center of the editor to the name bounds center
            Point editorSize = text.computeSize(-1, -1);
            Rectangle labelBounds = getNameBounds();
            translateToAbsolute(labelBounds);
            text.setBounds(labelBounds.x + (labelBounds.width - editorSize.x)/2 , labelBounds.y-1, editorSize.x, editorSize.y + 1);
        }

    }

    // Direct edit support
    public CellEditorLocator getDirectEditCellEditorLocator() {
        return new SubmoduleFigureNameCellEditorLocator();
    }

    public void showLabelUnderCellEditor(boolean visible) {
        setNameVisible(visible);
    }
}
