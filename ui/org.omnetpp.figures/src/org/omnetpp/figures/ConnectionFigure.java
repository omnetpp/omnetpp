/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.ConnectionLocator;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.MidpointLocator;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.misc.ConnectionLabelLocator;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.figures.misc.ISelectableFigure;

/**
 * A figure representing a connection between two modules
 *
 * @author rhornig
 */
public class ConnectionFigure extends PolylineConnection
                              implements ITooltipTextProvider, IProblemDecorationSupport, ISelectableFigure {
    protected int localLineStyle = Graphics.LINE_SOLID;
    protected int localLineWidth = 1;
    protected Color localLineColor = null;
    protected Label textFigure = new Label() {
        // KLUDGE: we don't want to allow selecting a connection by clicking on its visible label
        // NOTE: overriding findFigureAt does not work
        @Override
        public boolean containsPoint(int x, int y) {
            return false;
        };
    };
    protected ConnectionLabelLocator labelLocator = new ConnectionLabelLocator(this);
    protected TooltipFigure tooltipFigure;
    protected boolean isArrowHeadEnabled;
    protected IFigure centerDecoration;
    private int oldCumulativeHashCode;
    protected ITooltipTextProvider problemMarkerTextProvider;
    protected ImageFigure problemMarkerFigure = new ImageFigure(); // FIXME create it on demand
    private boolean isSelected;

    public boolean isSelected() {
        return isSelected;
    }

    public void setSelected(boolean isSelected) {
        if (isSelected == this.isSelected)
            return;
        else {
            this.isSelected = isSelected;
            repaint();
        }
    }

    @Override
    public void addNotify() {
        super.addNotify();
        add(problemMarkerFigure, new MidpointLocator(this, 0));
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
        if (this.isArrowHeadEnabled != arrowHeadEnabled) {
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
    }

    public void setMidpointDecoration(IFigure decoration) {
        if (centerDecoration == decoration)
            return;
        if (centerDecoration != null)
            remove(centerDecoration);
        centerDecoration = decoration;
        if (centerDecoration != null)
            add(centerDecoration, new MidpointLocator(this, 0), 0); // insert as first so it will be displayed UNDER the error marker
    }

    public void setStyle(Color color, int width, String style) {
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
            ((PolygonDecoration)getTargetDecoration()).setScale(5+getLineWidth(), 2+getLineWidth());
            ((PolygonDecoration)getTargetDecoration()).setLineWidth(getLineWidth());
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

    public void setInfoText(String text, String alignment, Color color) {

        if (textFigure == null)
            return;
        boolean isEmpty = StringUtils.isEmpty(text);
        textFigure.setVisible(!isEmpty);
        // we add an extra space, because label ends might be clipped because of antialiasing
        textFigure.setText(isEmpty ? "" : text + " ");
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
     */
    public void setDisplayString(IDisplayString dps) {
        // OPTIMIZATION: do not change anything if the display string has not changed
        int newCumulativeHashCode = dps.cumulativeHashCode();
        if (oldCumulativeHashCode != 0 && newCumulativeHashCode == oldCumulativeHashCode)
            return;

        this.oldCumulativeHashCode = newCumulativeHashCode;

        setStyle(ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.CONNECTION_COLOR)),
                dps.getAsInt(IDisplayString.Prop.CONNECTION_WIDTH, 1),
                dps.getAsString(IDisplayString.Prop.CONNECTION_STYLE));
        // tooltip support
        setTooltipText(dps.getAsString(IDisplayString.Prop.TOOLTIP));
        // additional text support
        setInfoText(dps.getAsString(IDisplayString.Prop.TEXT),
                dps.getAsString(IDisplayString.Prop.TEXT_POS),
                ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.TEXT_COLOR)));

        invalidate();
    }

    @Override
    public void paint(Graphics graphics) {
        graphics.pushState();
        // zero width connections do not appear (but we display them almost invisible for convenience
        if (getLineWidth() < 1)
            graphics.setAlpha(16);

        graphics.setLineCap(SWT.CAP_FLAT);
        if (isSelected)
            graphics.setForegroundColor(ColorFactory.RED);
        else
            graphics.setForegroundColor(getForegroundColor());
        super.paint(graphics);
        graphics.popState();
    }

    @Override
    protected void outlineShape(Graphics g) {
        // override the line width so it will grow/shrink with the scale factor
        g.setLineWidthFloat(getLineWidthFloat()*(float)g.getAbsoluteScale());
        super.outlineShape(g);
    }

    @Override
    protected boolean useLocalCoordinates() {
        return false;  // locators are working correctly ONLY in non-local coordinate systems
    }

    public String getTooltipText(int x, int y) {
        // if there is a problem marker and an associated tooltip text provider
        // and the cursor is over the marker, delegate to the problem marker text provider
        if (problemMarkerTextProvider != null && problemMarkerFigure != null) {
            Rectangle markerBounds = problemMarkerFigure.getBounds().getCopy();
            translateToParent(markerBounds);
            translateToAbsolute(markerBounds);
            if (markerBounds.contains(x, y)) {
                String text = problemMarkerTextProvider.getTooltipText(x, y);
                if (text != null)
                    return text;
            }
        }
        return null;
    }

    public void setProblemDecoration(int maxSeverity, ITooltipTextProvider textProvider) {
        Image image = FigureUtils.getProblemImageFor(maxSeverity);
        if (image != null)
            problemMarkerFigure.setImage(image);
        problemMarkerFigure.setVisible(image != null);

        problemMarkerTextProvider = textProvider;
        repaint();
    }

}
