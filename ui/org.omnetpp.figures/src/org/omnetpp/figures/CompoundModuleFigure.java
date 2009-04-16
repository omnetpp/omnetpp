/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.ConnectionLayer;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LayeredPane;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.layout.SpringEmbedderLayout;
import org.omnetpp.figures.misc.ILayerSupport;
import org.omnetpp.figures.routers.CompoundModuleConnectionRouter;

/**
 * A figure representing a compound module (only the client area. No border, title or inner type container).
 * image (if any)
 *
 * @author rhornig
 */
// TODO layouting could be further optimized. If the compound module size is not defined the layouter
// always recalculates the preferred size of the submoduleLayer whenever invalidate() is called because
// invalidate() internally calls layout.invalidate() too.
// mabe we should remove layout.invalidate() from the invalidate() method and call it ONLY if some
// property has changed that requires the recalculation (e.g. submodule added/removed submodule size changed)
public class CompoundModuleFigure extends NedFigure
				implements ILayerSupport {

    public static final Color ERROR_BACKGROUND_COLOR = ColorFactory.RED;
    public static final Color ERROR_BORDER_COLOR = ColorFactory.RED4;
    protected static final int ERROR_BORDER_WIDTH = 2;
    protected static final int BORDER_SNAP_WIDTH = 3;

    protected LayeredPane mainContainer;
    protected Image backgroundImage;
    protected String backgroundImageArrangement = "fix";
    protected Dimension backgroundSize;
    protected int gridTickDistance;
    protected int gridNoOfMinorTics;
    protected Color gridColor;
    protected Color moduleBackgroundColor = ERROR_BACKGROUND_COLOR;
    protected Color moduleBorderColor = ERROR_BORDER_COLOR;
    protected BackgroundLayer backgroundLayer;
    protected Layer backDecorationLayer;
    protected SubmoduleLayer submoduleLayer;
    protected Layer frontDecorationLayer;
    protected ConnectionLayer connectionLayer;
    protected Layer messageLayer;
    protected SpringEmbedderLayout layouter;

    // TODO implement ruler
    protected float scale = 1.0f;
    protected String unit = "px";

    // background layer to provide background coloring, images and grid drawing
    class BackgroundLayer extends Layer {
		@Override
		protected void paintFigure(Graphics graphics) {
			graphics.pushState();

            // draw outer non playground area
	        graphics.setBackgroundColor(ColorFactory.DARK_GREY);
	        graphics.fillRectangle(getClientArea());

	        // get the size of the viewport (which is actually the module size)
	        Rectangle viewportRect = new Rectangle(new Point(0,0), mainContainer.getSize());
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

	        // =============================================================================
	        // draw the grid
	        if (gridTickDistance > 0) {

	        	graphics.setForegroundColor(gridColor);
	        	double minorTickDistance = 0;
	        	if (gridNoOfMinorTics > 1)
	        		minorTickDistance = (double)gridTickDistance / gridNoOfMinorTics;

	        	// horizontal grid
	        	for (int y = viewportRect.y; y<viewportRect.bottom(); y += gridTickDistance) {
	        		graphics.setLineStyle(SWT.LINE_SOLID);
	        		graphics.drawLine(viewportRect.x, y, viewportRect.right(), y);
	        		// minor ticks
	        		graphics.setLineStyle(SWT.LINE_DOT);
	        		for (double my = y;  my < y+gridTickDistance && my < viewportRect.bottom() && minorTickDistance > 1; my+=minorTickDistance)
		        		graphics.drawLine(viewportRect.x, (int)my, viewportRect.right(), (int)my);
	        	}
	        	// vertical grid
	        	for (int x = viewportRect.x; x<viewportRect.right(); x += gridTickDistance) {
	        		graphics.setLineStyle(SWT.LINE_SOLID);
	        		graphics.drawLine(x, viewportRect.y, x, viewportRect.bottom());
	        		// minor ticks
	        		graphics.setLineStyle(SWT.LINE_DOT);
	        		for (double mx = x;  mx < x+gridTickDistance && mx < viewportRect.right() && minorTickDistance > 1; mx+=minorTickDistance)
		        		graphics.drawLine((int)mx, viewportRect.y, (int)mx,viewportRect.bottom());
	        	}
	        }
	        // restore the graphics state
      	graphics.popState();
		}

    }

    // main layer used to display submodules
    class SubmoduleLayer extends Layer {
        
        @Override
        public void add(IFigure child, Object constraint, int index) {
            // request an auto-layout whenever an unpinned submodule is added (added from the text editor) 
            if (child instanceof SubmoduleFigure && !((SubmoduleFigure)child).isPinVisible())
                layouter.requestFullLayout();
            super.add(child, constraint, index);
        }
        
        public CompoundModuleFigure getCompoundModuleFigure() {
        	return CompoundModuleFigure.this;
        }

        @Override
        public Dimension getPreferredSize(int wHint, int hHint) {
			return layouter.getPreferredSize(this, backgroundSize.width, backgroundSize.height);
		}
        
        @Override
        public Dimension getMinimumSize(int wHint, int hHint) {
			return getPreferredSize(wHint, hHint);
        }
    }

    public CompoundModuleFigure() {
        super();

        // set up the name and error marker figure
        ToolbarLayout tb = new ToolbarLayout();
        tb.setSpacing(2);
        tb.setStretchMinorAxis(false);
        setLayoutManager(tb);
        // contains all layers used inside a compound modules submodule area
        mainContainer = new LayeredPane() {
        	@Override
        	protected boolean useLocalCoordinates() {
        		return true;
        	}
        };
        mainContainer.setLayoutManager(new StackLayout());
        mainContainer.addLayerAfter(messageLayer = new Layer(), null, null);
        mainContainer.addLayerAfter(connectionLayer = new ConnectionLayer(), null, null);
        mainContainer.addLayerAfter(frontDecorationLayer = new Layer(), null, null);
        mainContainer.addLayerAfter(submoduleLayer = new SubmoduleLayer(), null, null);
        mainContainer.addLayerAfter(backDecorationLayer = new Layer(), null, null);
        mainContainer.addLayerAfter(backgroundLayer = new BackgroundLayer(), null, null);

        // set up the layouter. Preferred sizes should be set to 0 so the mainContainer
        // can follow the size of the submoduleLayer which uses the layouter to calculate the 
        // preferred size
        submoduleLayer.setLayoutManager(layouter = new SpringEmbedderLayout(this));
        messageLayer.setPreferredSize(0, 0);
        connectionLayer.setPreferredSize(0, 0);
        frontDecorationLayer.setPreferredSize(0, 0);
        backDecorationLayer.setPreferredSize(0, 0);
        backgroundLayer.setPreferredSize(0, 0);
        
        add(mainContainer);

        // this effectively creates the following hierarchy:
        // ---- LayeredPane
        // ------ backgroundLayer (compound module background. images, colors, grid etc)
        // ------ backgroundDecorationLayer (submodule background decoration (range indicator etc). non extendable
        // ------ submoduleLayer (plain layer used to display submodules - size is automatically calculated from child size and positions)
        // ------ foregroundDecorationLayer (text messages, decorator icons etc)
        // ------ connection (connections inside a compound module)
        // ------ message layer (used to display message animation effects)

        // set the connection routing
//        FanRouter fr = new FanRouter();
//        fr.setSeparation(10);
//        CompoundModuleShortestPathConnectionRouter spcr =
//        	new CompoundModuleShortestPathConnectionRouter(submoduleLayer);
//        spcr.setSpacing(10);
//        fr.setNextRouter(spcr);
// use this for fan router
//        setConnectionRouter(fr);
// use this for shortest path router
//        setConnectionRouter(spcr);

        // simple straight connection router
        connectionLayer.setConnectionRouter(new CompoundModuleConnectionRouter());
    }

    /**
     * @see org.eclipse.gef.handles.HandleBounds#getHandleBounds()
     */
    public Rectangle getHandleBounds() {
        return mainContainer.getBounds();
    }

    /**
     * Returns the bounds where the anchors should be placed in parent coordinate system.
     */
    public Rectangle getAnchorBounds() {
    	Rectangle box = mainContainer.getClientArea().shrink(2*BORDER_SNAP_WIDTH, 2*BORDER_SNAP_WIDTH);
    	mainContainer.translateToParent(box);
    	translateToParent(box);
    	return box;

    }

	/**
	 * Adjusts compound module background parameters
	 * @param img Background image
	 * @param arrange
	 * @param backgroundColor
	 * @param borderColor
	 * @param borderWidth
	 */
	protected void setBackground(Image img, String arrange, Color backgroundColor, Color borderColor, int borderWidth) {
		moduleBackgroundColor = backgroundColor==null ? ERROR_BACKGROUND_COLOR : backgroundColor;
		moduleBorderColor = borderColor==null ? ERROR_BORDER_COLOR : borderColor;

		// the global background is the same as the border color
		// XXX do we need to set this?
		mainContainer.setBackgroundColor(moduleBorderColor);
		// background image
		backgroundImage = img;
		backgroundImageArrangement = arrange != null ? arrange : "";
		invalidate();
	}

	/**
	 * Adjusts grid parameters.
	 * @param tickDistance Maximum distance between two ticks measured in pixels
	 * @param noOfTics Number of minor ticks between two major one
	 * @param gridColor Grid color
	 */
	protected void setGrid(int tickDistance, int noOfTics, Color gridColor) {
		this.gridTickDistance = tickDistance;
		this.gridNoOfMinorTics = noOfTics;
		this.gridColor = gridColor;
		invalidate();
	}

	/**
	 * Scaling and unit support.
	 * @param scale scale value (a value of 18 means: 1 unit = 18 pixels)
	 * @param unit the unit of the dimension
	 */
	protected void setScale(float scale, String unit) {
		this.scale = scale;
		this.unit = unit;
		invalidate();
	}

	/**
	 * Adjusts the figure properties using a displayString object
	 * @param dps The display string object containing the properties
	 */
	@Override
    public void setDisplayString(IDisplayString dps) {
        // background color / image
        Image imgback = ImageFactory.getImage(
        		dps.getAsString(IDisplayString.Prop.MODULE_IMAGE), null, null, 0);

        // decode the image arrangement
        String imageArrangementStr = dps.getAsString(IDisplayString.Prop.MODULE_IMAGEARRANGEMENT);

        // set the background
        setBackground(
        		imgback,
        		imageArrangementStr,
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.MODULE_FILLCOL)),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.MODULE_BORDERCOL)),
        		dps.getAsInt(IDisplayString.Prop.MODULE_BORDERWIDTH, -1));

        // scaling support
        setScale(
                dps.getAsFloat(IDisplayString.Prop.MODULE_SCALE, 1),
                dps.getAsString(IDisplayString.Prop.MODULE_UNIT));

        // grid support
        setGrid(
        		dps.unit2pixel(dps.getAsInt(IDisplayString.Prop.MODULE_TICKDISTANCE, -1), null),
        		dps.getAsInt(IDisplayString.Prop.MODULE_TICKNUMBER, -1),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.MODULE_GRIDCOL)));

        // finally set the location and size using the models helper methods
        // if the size is specified in the display string we should set it as preferred size
        // otherwise getPreferredSize should return the size calculated from the children
        // we call the resizing last time because other parameters like the icon size or the border width
        // can affect the size of bounding box
        backgroundSize = dps.getCompoundSize(null);

        long seed = dps.getAsInt(IDisplayString.Prop.MODULE_LAYOUT_SEED, 1);
        // if the seed changed we explicitly have to force a re-layout
        if (seed != layouter.getSeed()) {
            layouter.setSeed(seed);
            layouter.requestFullLayout();
        }

        invalidate();
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

}
