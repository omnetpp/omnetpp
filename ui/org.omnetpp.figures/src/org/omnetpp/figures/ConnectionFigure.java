package org.omnetpp.figures;

import org.eclipse.draw2d.ConnectionLocator;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.swt.graphics.Color;

import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.misc.ConnectionLabelLocator;

public class ConnectionFigure extends PolylineConnection {
	protected int localLineStyle = Graphics.LINE_SOLID;
	protected int localLineWidth = 1;
	protected Color localLineColor = null;
    protected Label textFigure = new Label();
    protected ConnectionLabelLocator labelLocator = new ConnectionLabelLocator(this);
    protected TooltipFigure tooltipFigure;
	private IDisplayString lastDisplayString;

	@Override
    public void addNotify() {
        super.addNotify();
        add(textFigure, labelLocator);
    }
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
        	if (getTargetDecoration() == null) {
        		PolygonDecoration arrow = new PolygonDecoration();
        		arrow.setTemplate(PolygonDecoration.TRIANGLE_TIP);
        		setTargetDecoration(arrow);
        	}
        }
		if (getTargetDecoration() != null)
			getTargetDecoration().setVisible(arrowEnabled);

    }

    protected void setStyle(Color color, int width, String style) {
    	style = style == null ? "" : style;
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
		if (getTargetDecoration() != null)
			((PolygonDecoration)getTargetDecoration()).setScale(5+lineWidth, 2+lineWidth);
    }

    public void setTooltipText(String tttext) {
        if(tttext == null || "".equals(tttext)) {
            setToolTip(null);
            tooltipFigure = null;
        } else {
            tooltipFigure = new TooltipFigure();
            setToolTip(tooltipFigure);
            tooltipFigure.setText(tttext);
            invalidate();
        }
    }

    protected void setInfoText(String text, String alignment, Color color) {

        if (textFigure == null)
            return;
        textFigure.setVisible(text != null && !"".equals(text));
        textFigure.setText(text);
        textFigure.setForegroundColor(color);
        // set position
        if (alignment != null) {
            if (alignment.startsWith("l")) {
                labelLocator.setAlignment(ConnectionLocator.SOURCE);
            } else if (alignment.startsWith("r")) {
                labelLocator.setAlignment(ConnectionLocator.TARGET);
            } else
                labelLocator.setAlignment(ConnectionLocator.MIDDLE);
        }
        revalidate();
    }

    /**
	 * Adjusts the figure properties using a displayString object
	 * @param dps The display string object containing the properties
	 */
	public void setDisplayString(IDisplayString dps) {
		lastDisplayString = dps;

        setStyle(ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.CONNECTION_COL)),
				dps.getAsIntDef(DisplayString.Prop.CONNECTION_WIDTH, 1),
				dps.getAsStringDef(DisplayString.Prop.CONNECTION_STYLE));
        // tooltip support
        setTooltipText(dps.getAsStringDef(IDisplayString.Prop.TOOLTIP));
        // additional text support
        setInfoText(dps.getAsStringDef(IDisplayString.Prop.TEXT),
                dps.getAsStringDef(IDisplayString.Prop.TEXTPOS),
                ColorFactory.asColor(dps.getAsStringDef(IDisplayString.Prop.TEXTCOLOR)));

        invalidate();
	}
    /**
     * Returns the lastly set display string
     * @return
     */
	public IDisplayString getLastDisplayString() {
    	return lastDisplayString;
    }

}
