/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Border;
import org.eclipse.draw2d.ConnectionLayer;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LayeredPane;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.TextUtilities;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.anchors.IAnchorBounds;
import org.omnetpp.figures.layout.CompoundModuleLayout;
import org.omnetpp.figures.misc.ILayerSupport;
import org.omnetpp.figures.misc.ISelectableFigure;
import org.omnetpp.figures.misc.ISelectionHandleBounds;
import org.omnetpp.figures.routers.CompoundModuleConnectionRouter;

/**
 * A figure representing a compound module (only the client area, no title or inner type container).
 *
 * @author rhornig
 */
// TODO layouting could be further optimized. If the compound module size is not defined the layouter
// always recalculates the preferred size of the submoduleLayer whenever invalidate() is called because
// invalidate() internally calls layout.invalidate() too.
// maybe we should remove layout.invalidate() from the invalidate() method and call it ONLY if some
// property has changed that requires the recalculation (e.g. submodule added/removed submodule size changed)
// TODO implement ruler
// TODO zoom info: maybe display unit too, e.g. like "1m=3.14px" -- but then in Qtenv too!
public class CompoundModuleFigure extends LayeredPane implements IAnchorBounds, ISelectionHandleBounds, ILayerSupport, ISelectableFigure {
    public static final Color ERROR_BACKGROUND_COLOR = ColorFactory.RED;
    public static final Color ERROR_BORDER_COLOR = ColorFactory.RED4;
    protected static final int ERROR_BORDER_WIDTH = 2;
    public static final int BORDER_SNAP_WIDTH = 3;

    protected Image backgroundImage;
    protected String backgroundImageArrangement = "fix";
    protected Dimension backgroundSize;
    protected int gridTickDistance; // maximum distance between two grid lines (in pixels)
    protected int gridNoOfMinorTics; // number of minor ticks between two major ones
    protected Color gridColor;
    protected Color moduleBackgroundColor = ERROR_BACKGROUND_COLOR;
    protected Color moduleBorderColor = ERROR_BORDER_COLOR;
    protected int moduleBorderWidth;
    protected String unit = "m";

    protected BackgroundLayer backgroundLayer; // compound module background: images, colors, grid, etc.
    protected Layer backDecorationLayer; // submodule background decoration (range indicators, etc), non-extensible
    protected SubmoduleLayer submoduleLayer; // layer to display submodules - size is automatically calculated from children sizes and positions
    protected Layer frontDecorationLayer; // foreground decorations: text messages, decorator icons, etc.
    protected ConnectionLayer connectionLayer; // connections inside a compound module
    protected Layer messageLayer; // layer for message animation effects - not used in the editor

    protected CompoundModuleLayout layouter;

    private float lastScale = -1;
    private float lastIconScale = -1;
    private int lastCumulativeHashCode;
    private int seed = 0;
    private boolean isSelected;

    /**
     * Background layer to provide background coloring, images and grid drawing
     */
    class BackgroundLayer extends Layer {
        @Override
        protected void paintFigure(Graphics graphics) {
            graphics.pushState();

            // draw outer non playground area
            graphics.setBackgroundColor(ColorFactory.DARK_GREY);
            graphics.fillRectangle(getClientArea());

            // get the size of the viewport (which is actually the module size)
            Rectangle viewportRect = new Rectangle(new Point(0,0), this.getSize());

            // draw a solid background
            graphics.setBackgroundColor(moduleBackgroundColor);
            graphics.fillRectangle(viewportRect);

            // draw and clip background image
            graphics.clipRect(viewportRect);
            if (backgroundImage != null) {
                Rectangle imageRect = new Rectangle(backgroundImage.getBounds());
                if (backgroundImageArrangement.toLowerCase().startsWith("t"))
                    for (int y = viewportRect.y; y<viewportRect.bottom(); y += imageRect.height)
                        for (int x = viewportRect.x; x<viewportRect.right(); x += imageRect.width)
                            graphics.drawImage(backgroundImage, x, y);
                else if (backgroundImageArrangement.toLowerCase().startsWith("s"))
                    graphics.drawImage(backgroundImage, imageRect, viewportRect);
                else if (backgroundImageArrangement.toLowerCase().startsWith("c")) {
                    Point centerPoint = viewportRect.getCenter().translate(-imageRect.width/2, -imageRect.height/2);
                    graphics.drawImage(backgroundImage, centerPoint);
                } else
                    graphics.drawImage(backgroundImage, viewportRect.getLocation());
            }

            // draw the grid
            if (gridTickDistance > 0 && gridColor != null) {

                graphics.setForegroundColor(gridColor);
                float minorTickDistance = 0;
                if (gridNoOfMinorTics > 1)
                    minorTickDistance = (float)gridTickDistance / gridNoOfMinorTics;

                // horizontal grid
                for (int y = viewportRect.y; y < viewportRect.bottom(); y += gridTickDistance) {
                    graphics.setLineStyle(SWT.LINE_SOLID);
                    graphics.drawLine(viewportRect.x, y, viewportRect.right(), y);
                    // minor ticks
                    graphics.setLineStyle(SWT.LINE_DOT);
                    for (float my = y;  my < y+gridTickDistance && my < viewportRect.bottom() && minorTickDistance > 1; my+=minorTickDistance)
                        graphics.drawLine(viewportRect.x, (int)my, viewportRect.right(), (int)my);
                }

                // vertical grid
                for (int x = viewportRect.x; x < viewportRect.right(); x += gridTickDistance) {
                    graphics.setLineStyle(SWT.LINE_SOLID);
                    graphics.drawLine(x, viewportRect.y, x, viewportRect.bottom());
                    // minor ticks
                    graphics.setLineStyle(SWT.LINE_DOT);
                    for (float mx = x;  mx < x+gridTickDistance && mx < viewportRect.right() && minorTickDistance > 1; mx+=minorTickDistance)
                        graphics.drawLine((int)mx, viewportRect.y, (int)mx, viewportRect.bottom());
                }
            }

            // restore the graphics state
            graphics.popState();
        }
    }

    /**
     * Main layer used to display submodules
     */
    public class SubmoduleLayer extends Layer {
        @Override
        public Dimension getPreferredSize(int wHint, int hHint) {
            return layouter.getPreferredSize(this, backgroundSize.width, backgroundSize.height);
        }

        @Override
        public Dimension getMinimumSize(int wHint, int hHint) {
            return new Dimension(8,8);
        }
    }

    public CompoundModuleFigure() {
        super();

        setLayoutManager(new StackLayout());

        add(backgroundLayer = new BackgroundLayer());
        add(backDecorationLayer = new Layer());
        add(submoduleLayer = new SubmoduleLayer());
        add(frontDecorationLayer = new Layer());
        add(connectionLayer = new ConnectionLayer());
        add(messageLayer = new Layer());

        // add a compound module border
        setBorder(new CompoundModuleLineBorder());

        // Set up the layouter. Preferred sizes should be set to 0 so the main container
        // can follow the size of the submoduleLayer which uses the layouter to calculate the
        // preferred size.
        submoduleLayer.setLayoutManager(layouter = new CompoundModuleLayout(this));
        messageLayer.setPreferredSize(0, 0);
        connectionLayer.setPreferredSize(0, 0);
        frontDecorationLayer.setPreferredSize(0, 0);
        backDecorationLayer.setPreferredSize(0, 0);
        backgroundLayer.setPreferredSize(0, 0);


        // set the connection routing
//        FanRouter fr = new FanRouter();
//        fr.setSeparation(10);
//        CompoundModuleShortestPathConnectionRouter spcr =
//          new CompoundModuleShortestPathConnectionRouter(submoduleLayer);
//        spcr.setSpacing(10);
//        fr.setNextRouter(spcr);
// use this for fan router
//        setConnectionRouter(fr);
// use this for shortest path router
//        setConnectionRouter(spcr);

        // simple straight connection router
        connectionLayer.setConnectionRouter(new CompoundModuleConnectionRouter());
    }

    @Override
    protected boolean useLocalCoordinates() {
        return true;
    }

    @Override
    public boolean isSelected() {
        return isSelected;
    }

    @Override
    public void setSelected(boolean isSelected) {
        if (isSelected == this.isSelected)
            return;
        this.isSelected = isSelected;
        repaint();
    }

    @Override
    public Rectangle getHandleBounds() {
        Rectangle result = getBounds().getCopy();
        return result;
    }

    @Override
    public Rectangle getAnchorBounds() {
        Rectangle box = getClientArea().shrink(BORDER_SNAP_WIDTH, BORDER_SNAP_WIDTH);
        translateToParent(box);
        return box;
    }

    @Override
    public Dimension getMaximumSize() {
        return super.getPreferredSize();
    }

    @Override
    public void setMaximumSize(Dimension d) {
        throw new RuntimeException("maximum size is automatic, it cannot be set");
    }

    /**
     * Adjusts compound module background parameters
     */
    protected void setBackground(Image image, String arrangement, Color backgroundColor, Color borderColor, int borderWidth) {
        moduleBackgroundColor = backgroundColor==null ? ERROR_BACKGROUND_COLOR : backgroundColor;
        moduleBorderColor = borderColor==null ? ERROR_BORDER_COLOR : borderColor;
        moduleBorderWidth = borderWidth < 0 ? ERROR_BORDER_WIDTH : borderWidth;

        // the global background is the same as the border color
        setBackgroundColor(moduleBorderColor);

        // background image
        backgroundImage = image;
        backgroundImageArrangement = arrangement != null ? arrangement : "";

        getBorder().setColor(moduleBorderColor);
        getBorder().setWidth(moduleBorderWidth);
    }

    /**
     * Adjusts grid parameters.
     */
    protected void setGrid(int tickDistance, int noOfTics, Color gridColor) {
        this.gridTickDistance = tickDistance;
        this.gridNoOfMinorTics = noOfTics;
        this.gridColor = gridColor;
    }

    protected void setDistanceUnit(String unit) {
        this.unit = unit;
    }

    /**
     * Adjusts the figure properties using a displayString object
     */
    public void setDisplayString(IDisplayString dps, float scale, float iconScale, IProject project) {
        // Optimization: do not change anything if the display string has not changed
        int cumulativeHashCode = dps.cumulativeHashCode();
        if (lastScale == scale && lastIconScale == iconScale && lastCumulativeHashCode != 0 && cumulativeHashCode == lastCumulativeHashCode)
            return;

        this.lastCumulativeHashCode = cumulativeHashCode;
        this.lastScale = scale;
        this.lastIconScale = iconScale;

        // background color and image
        Image backgroundImage = ImageFactory.of(project).getImage(dps.getAsString(IDisplayString.Prop.MODULE_IMAGE), null, null, 0);
        String imageArrangementStr = dps.getAsString(IDisplayString.Prop.MODULE_IMAGE_ARRANGEMENT);
        setBackground(
                backgroundImage,
                imageArrangementStr,
                ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.MODULE_FILL_COLOR)),
                ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.MODULE_BORDER_COLOR)),
                dps.getAsInt(IDisplayString.Prop.MODULE_BORDER_WIDTH, -1));

        // distance unit
        setDistanceUnit(dps.getAsString(IDisplayString.Prop.MODULE_UNIT));

        // grid support
        setGrid(
                unitToPixel(dps.getAsDistance(IDisplayString.Prop.MODULE_GRID_DISTANCE), scale),
                dps.getAsInt(IDisplayString.Prop.MODULE_GRID_SUBDIVISION, -1),
                ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.MODULE_GRID_COLOR)));

        // finally set the location and size using the models helper methods
        // if the size is specified in the display string we should set it as preferred size
        // otherwise getPreferredSize should return the size calculated from the children
        // we call the resizing last time because other parameters like the icon size or the border width
        // can affect the size of bounding box
        backgroundSize = dps.getCompoundSize().toPixels(scale);

        layouter.setScale(scale);

        int newSeed = dps.getAsInt(IDisplayString.Prop.MODULE_LAYOUT_SEED, 1);

        // if the seed changed, we explicitly have to force a re-layout
        if (seed != newSeed) {
            seed = newSeed;
            layouter.setSeed(seed);
            layouter.requestFullLayout();
            // a full new layout must be executed before any repainting occurs otherwise
            // figures without centerLocation cannot be rendered
            invalidate();
            layouter.layout(submoduleLayer);
        }

        layouter.invalidate();
        repaint();
    }

    @Override
    public void paint(Graphics graphics) {
        super.paint(graphics);

        if (lastScale != 1.0f)
            paintZoomInfo(graphics);

        if (isSelected) {
            Rectangle r = getHandleBounds();
            graphics.setForegroundColor(ColorFactory.RED);
            graphics.drawRectangle(r.x, r.y, r.width - 1, r.height - 1);
        }
    }

    protected void paintZoomInfo(Graphics graphics) {
        String zoomInfo = String.format("%.2fx", lastScale);
        Dimension textSize = TextUtilities.INSTANCE.getTextExtents(zoomInfo, getFont());
        Rectangle r = getHandleBounds();
        graphics.drawText(zoomInfo, r.right() - textSize.width-5, r.bottom()-textSize.height-5);
    }

    public Dimension getBackgroundSize() {
        return backgroundSize;
    }

    public Layer getBackgroundLayer() {
        return backgroundLayer;
    }

    public Layer getBackgroundDecorationLayer() {
        return backDecorationLayer;
    }

    public Layer getConnectionLayer() {
        return connectionLayer;
    }

    public Layer getSubmoduleLayer() {
        return submoduleLayer;
    }

    public Layer getForegroundDecorationLayer() {
        return frontDecorationLayer;
    }

    public Layer getMessageLayer() {
        return messageLayer;
    }

    @Override
    public CompoundModuleLineBorder getBorder() {
        return (CompoundModuleLineBorder)super.getBorder();
    }

    @Override
    public void setBorder(Border border) {
        Assert.isTrue(border instanceof CompoundModuleLineBorder, "Only CompoundModuleBorder is supported");
        super.setBorder(border);
    }

    static int unitToPixel(float f, float scale) {
        return (int)(f * scale);
    }
}
