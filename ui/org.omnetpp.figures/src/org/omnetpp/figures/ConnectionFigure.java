package org.omnetpp.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;

public class ConnectionFigure extends PolylineConnection {
	protected int localLineStyle = Graphics.LINE_SOLID;
	protected int localLineWidth = 1;
	protected Color localLineColor = null;
    protected PolygonDecoration arrow = null;
    
    public Color getLocalLineColor() {
		return localLineColor;
	}

	public int getLocalLineStyle() {
		return localLineStyle;
	}

	public int getLocalLineWidth() {
		return localLineWidth;
	}

	public void setArrowEnabled(boolean arrowEnabled) {

        if (arrowEnabled) {
        	if (arrow == null) {
        		arrow = new PolygonDecoration();
        		arrow.setTemplate(PolygonDecoration.TRIANGLE_TIP);
        		setTargetDecoration(arrow);
        	}
        }
		if (arrow != null)
			arrow.setVisible(arrowEnabled);

    }
    
    public void setStyle(Color color, int width, String style, String segments) {
    	style = (style == null) ? "" : style;
    	if (style.toLowerCase().startsWith("da"))
    		setLineStyle(localLineStyle = Graphics.LINE_DASH);
    	else if (style.toLowerCase().startsWith("d"))
    		setLineStyle(localLineStyle = Graphics.LINE_DOT);
    	else
    		setLineStyle(localLineStyle = Graphics.LINE_SOLID);
    	
    	// special handling: connection is visible only if width is greater than 0
    	setVisible(width > 0);
    	setLineWidth(localLineWidth = width);
    	setForegroundColor(localLineColor = color);
    	// arrow scaling proportional with the line width
		if (arrow != null) 
			arrow.setScale(5+lineWidth, 2+lineWidth);
    }

	/**
	 * Adjusts the figure properties using a displayString object
	 * @param dps The display string object containing the properties
	 */
	public void setDisplayString(IDisplayString dps) {
        setStyle(ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.CONNECTION_COL)), 
				dps.getAsIntDef(DisplayString.Prop.CONNECTION_WIDTH, 1), 
				dps.getAsStringDef(DisplayString.Prop.CONNECTION_STYLE), 
				dps.getAsStringDef(DisplayString.Prop.CONNECTION_SEGMENTS));
	}
}
