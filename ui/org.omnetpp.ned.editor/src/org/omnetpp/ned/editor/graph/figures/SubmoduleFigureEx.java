/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.editor.graph.misc.IDirectEditSupport;

/**
 * EDITING specific parts of the figure. Direct editing etc.
 */
public class SubmoduleFigureEx extends SubmoduleFigure implements HandleBounds,
		IDirectEditSupport {

    protected String tmpName;

    // support locator for finding the name of the figure allowing the direct edit
    // cell editor above the name figure 
    class SubmoduleFigureNameCellEditorLocator implements CellEditorLocator {
		@Override
		public void relocate(CellEditor celleditor) {
	        Text text = (Text)celleditor.getControl();
	        // FIXME rewrite to handle scaling/zoom correctly
//	        Font scaledFont = label.getFont();
//	        FontData data = scaledFont.getFontData()[0];
//	        Dimension fontSize = new Dimension(0, data.getHeight());
//	        label.translateToAbsolute(fontSize);
//	        data.setHeight(fontSize.height);
//	        scaledFont = new Font(null, data);
//	        text.setFont(scaledFont);

//	        Point editorSize = text.computeSize(-1, -1);
//	        Rectangle labelBounds = label.getTextBounds();
//	        label.translateToAbsolute(labelBounds);
//	        // if the label is center aligned we mimic that with the cell editor too
//	        if (label.getTextAlignment() == PositionConstants.CENTER)
//	            text.setBounds(labelBounds.x + (labelBounds.width - editorSize.x)/2 , labelBounds.y, editorSize.x, editorSize.y + 1);
//	        else
//	            text.setBounds(labelBounds.x, labelBounds.y, editorSize.x, editorSize.y + 1);
	        
	        Rectangle newBounds = getBounds();
	        text.setBounds(newBounds.x, newBounds.y, newBounds.width, newBounds.height);
		}
    	
    }
    
    // Direct edit support
    public CellEditorLocator getDirectEditCellEditorLocator() {
        return new SubmoduleFigureNameCellEditorLocator();
    }

    public void showLabelUnderCellEditor(boolean visible) {
    	// FIXME hide/unhide the name text under the cell editor
//        // HACK to hide the text part only of the label
//        if (!visible) {
//            tmpName = nameFigure.getText();
//            nameFigure.setText("");
//        }
//        else {
//            if (StringUtils.isEmpty(nameFigure.getText()))
//                nameFigure.setText(tmpName);
//        }
//        invalidate();
//        validate();
    }
}
