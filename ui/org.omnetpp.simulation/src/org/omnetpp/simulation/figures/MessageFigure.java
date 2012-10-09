package org.omnetpp.simulation.figures;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FigureUtilities;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;

/**
 *
 * @author Andras
 */
public class MessageFigure extends Figure {
    private static final Image DEFAULT_IMAGE = SimulationPlugin.getCachedImage(SimulationUIConstants.IMG_OBJ_MESSAGE);

    private Image image;
    private String label;
    private Point centerLocation;
    private int imageWidth = -1, imageHeight = -1;
    private int labelWidth = -1, labelHeight = -1;

    public MessageFigure() {
        this(DEFAULT_IMAGE, null);
    }

    public MessageFigure(String label) {
        this(DEFAULT_IMAGE, label);
    }

    public MessageFigure(Image image, String label) {
        setImage(image);
        setLabel(label);
    }

    public Image getImage() {
        return image;
    }

    public void setImage(Image image) {
        Assert.isNotNull(image, "image cannot be null");
        this.image = image;
        imageWidth = imageHeight = -1;

        if (getParent() != null) {
            revalidate();
            repaint();
        }
    }

    public String getLabel() {
        return label;
    }

    public void setLabel(String label) {
        this.label = label;
        labelWidth = labelHeight = -1;

        if (getParent() != null) {
            revalidate();
            repaint();
        }
    }

    public Point getCenterLocation() {
        return centerLocation;
    }

    public void setCenterLocation(Point centerLocation) {
        if (getParent() != null)
            erase();

        this.centerLocation = centerLocation;

        if (getParent() != null) {
            fireFigureMoved();
            repaint();
        }
    }

    @Override
    public void setBounds(Rectangle rect) {
        throw new UnsupportedOperationException(); // call setCenterLocation instead
    }

    @Override
    public void setLocation(Point p) {
        throw new UnsupportedOperationException(); // call setCenterLocation instead
    }

    @Override
    public void setSize(int w, int h) {
        throw new UnsupportedOperationException();
    }

    @Override
    public void setPreferredSize(Dimension size) {
        throw new UnsupportedOperationException();
    }

    @Override
    public Dimension getPreferredSize(int hint, int hint2) {
        throw new UnsupportedOperationException();
    }

    @Override
    public Rectangle getBounds() {
        if (imageWidth == -1)
            computeImageSize();
        if (labelWidth == -1)
            computeLabelSize();

        Rectangle r = new Rectangle();
        int width = Math.max(imageWidth, labelWidth);
        r.setLocation(centerLocation.x - width/2, centerLocation.y - imageHeight/2);
        r.setSize(width, imageHeight + labelHeight);
        return r;
    }

    public void paint(Graphics graphics) {
        Assert.isNotNull(centerLocation, "centerLocation must be set before painting");
        Assert.isNotNull(image, "image cannot be null");

        if (imageWidth == -1)
            computeImageSize();
        graphics.drawImage(image, centerLocation.x - imageWidth/2, centerLocation.y - imageHeight/2);
        if (label !=null) {
            if (labelWidth == -1)
                computeLabelSize();
            Color oldColor = graphics.getForegroundColor();
            graphics.setForegroundColor(ColorFactory.BLACK);
            graphics.drawText(label, centerLocation.x - labelWidth/2, centerLocation.y + (imageHeight+1)/2);
            graphics.setForegroundColor(oldColor);
        }
    }

    private void computeImageSize() {
        org.eclipse.swt.graphics.Rectangle bounds = image.getBounds();
        imageHeight = bounds.height;
        imageWidth = bounds.width;
    }

    private void computeLabelSize() {
        Dimension textExtents = FigureUtilities.getTextExtents(label, getFont());
        labelWidth = textExtents.width;
        labelHeight = textExtents.height;
    }


}
