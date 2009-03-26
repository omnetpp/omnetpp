/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.gef.tools.CellEditorLocator;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.editor.graph.misc.IDirectEditSupport;
import org.omnetpp.ned.editor.graph.misc.LabelCellEditorLocator;

/**
 * EDITING specific parts of the figure. Direct editing etc.
 */
public class SubmoduleFigureEx extends SubmoduleFigure implements
		IDirectEditSupport {

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
}
