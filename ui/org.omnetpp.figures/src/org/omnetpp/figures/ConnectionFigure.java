package org.omnetpp.figures;

import org.eclipse.draw2d.ConnectionLocator;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.misc.ConnectionLabelLocator;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class ConnectionFigure extends PolylineConnection {
	protected int localLineStyle = Graphics.LINE_SOLID;
	protected int localLineWidth = 1;
	protected Color localLineColor = null;
    protected Label textFigure = new Label();
    protected ConnectionLabelLocator labelLocator = new ConnectionLabelLocator(this);
    protected TooltipFigure tooltipFigure;
	private IDisplayString lastDisplayString;
	protected boolean isArrowHeadEnabled;

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

	public void setArrowHeadEnabled(boolean arrowHeadEnabled) {
	    this.isArrowHeadEnabled = arrowHeadEnabled;
        if (arrowHeadEnabled) {
        	if (getTargetDecoration() == null) {
        		PolygonDecoration arrow = new PolygonDecoration();
        		arrow.setTemplate(PolygonDecoration.TRIANGLE_TIP);
        		setTargetDecoration(arrow);
        	}
        }
		if (getTargetDecoration() != null)
			getTargetDecoration().setVisible(arrowHeadEnabled);

    }

    protected void setStyle(Color color, int width, String style) {
    	style = style == null ? "" : style;
    	if (style.toLowerCase().startsWith("da"))
    		setLineStyle(localLineStyle = Graphics.LINE_DASH);
    	else if (style.toLowerCase().startsWith("d"))
    		setLineStyle(localLineStyle = Graphics.LINE_DOT);
    	else
    		setLineStyle(localLineStyle = Graphics.LINE_SOLID);

    	setLineWidth(localLineWidth = width);
    	setForegroundColor(localLineColor = color);
    	// arrow scaling proportional with the line width
		if (getTargetDecoration() != null) {
            ((PolygonDecoration)getTargetDecoration()).setScale(5+lineWidth, 2+lineWidth);
		    ((PolygonDecoration)getTargetDecoration()).setLineWidth(lineWidth);
		}
    }

    public void setTooltipText(String tttext) {
        if (tttext == null || "".equals(tttext)) {
            setToolTip(null);
            tooltipFigure = null;
        }
        else {
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
        // we add an extra space, because label ends might be clipped because of antialiasing
        textFigure.setText(text+" ");
        textFigure.setForegroundColor(color);
        // set position
        if (alignment != null) {
            if (alignment.startsWith("l")) {
                labelLocator.setAlignment(ConnectionLocator.SOURCE);
            }
            else if (alignment.startsWith("r")) {
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

        setStyle(ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.CONNECTION_COL)),
				dps.getAsInt(IDisplayString.Prop.CONNECTION_WIDTH, 1),
				dps.getAsString(IDisplayString.Prop.CONNECTION_STYLE));
        // tooltip support
        setTooltipText(dps.getAsString(IDisplayString.Prop.TOOLTIP));
        // additional text support
        setInfoText(dps.getAsString(IDisplayString.Prop.TEXT),
                dps.getAsString(IDisplayString.Prop.TEXTPOS),
                ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.TEXTCOLOR)));

        invalidate();
	}

	@Override
	public void paint(Graphics graphics) {
	    graphics.pushState();
	    // zero width connections do not appear (but we display them almost invisible for convenience
        if (getLineWidth() < 1)
            graphics.setAlpha(16);

        graphics.setLineCap(SWT.CAP_FLAT);
	    super.paint(graphics);
	    graphics.popState();
	}

    /**
     * Returns the most recently set display string
     */
	public IDisplayString getLastDisplayString() {
    	return lastDisplayString;
    }

}
