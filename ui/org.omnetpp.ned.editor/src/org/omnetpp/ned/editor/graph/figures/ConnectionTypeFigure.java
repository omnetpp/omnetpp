/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.NedTypeFigure;
import org.omnetpp.ned.editor.graph.misc.IDirectEditSupport;
import org.omnetpp.ned.editor.graph.misc.LabelCellEditorLocator;

/**
 * Figure for a channel or channel interface NED type.
 *
 * @author andras
 */
public class ConnectionTypeFigure extends NedTypeFigure implements IDirectEditSupport {
    protected ConnectionFigure connectionFigure = new ConnectionFigure();
    protected String tmpName;

    public ConnectionTypeFigure() {
        setLayoutManager(new XYLayout());
        add(nameFigure); 
        add(connectionFigure);
        add(problemMarkerFigure, new Rectangle(-1, 0, 16, 16));
        connectionFigure.setArrowHeadEnabled(true);
    }
    
    @Override
    protected boolean useLocalCoordinates() {
        return true;
    }

    /**
	 * Adjusts the figure properties using a displayString object
	 */
	public void setDisplayString(IDisplayString dps) {
	    int connFigureWidth = 60;
	    
	    // we currently ignore the display strings of interfaces 
	    int lineWidth = isInterface() ? 1 : dps.getAsInt(IDisplayString.Prop.CONNECTION_WIDTH, 1);
        Color lineColor = isInterface() ? ColorFactory.GREY65 : ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.CONNECTION_COLOR));
        String lineStyle = isInterface() ? "da" : dps.getAsString(IDisplayString.Prop.CONNECTION_STYLE);

        int height = Math.max(10, 4*lineWidth);
        connectionFigure.setPoints(new PointList(new int[] {0, 3+height/2, connFigureWidth-2*lineWidth/3, 3+height/2}));
        connectionFigure.setStyle(lineColor, lineWidth, lineStyle);

        setConstraint(connectionFigure, new Rectangle(0,0,connFigureWidth, height));
        setConstraint(nameFigure, new Rectangle(connFigureWidth+6, 2, -1, height));
        
        invalidateTree();
	}
	
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
