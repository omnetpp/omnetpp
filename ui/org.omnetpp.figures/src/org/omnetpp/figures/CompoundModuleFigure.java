package org.omnetpp.figures;

import org.eclipse.draw2d.*;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Pattern;

import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.layout.SpringEmbedderLayout;
import org.omnetpp.figures.misc.ILayerSupport;
import org.omnetpp.figures.routers.CompoundModuleConnectionRouter;
import org.omnetpp.figures.routers.CompoundModuleShortestPathConnectionRouter;

/**
 * A figure representing a compound module. Also displays a caption with the name and the default
 * image (if any)
 *
 * @author rhornig
 */
public class CompoundModuleFigure extends NedFigure
				implements ILayerSupport, HandleBounds {

    private static final int ERROR_BORDER_WIDTH = 2;
    public static final Color ERROR_BACKGROUND_COLOR = ColorFactory.RED;
    public static final Color ERROR_BORDER_COLOR = ColorFactory.RED4;
    private static final int BORDER_SNAP_WIDTH = 3;

    private Figure innerTypeContainer;
    private ScrollPane mainContainer;
    private LayeredPane layeredPane;
    private Image backgroundImage;
    private String backgroundImageArrangement = "fix";
    private Dimension backgroundSize;
    private int gridTickDistance;
    private int gridNoOfMinorTics;
    private Color gridColor;
    private Color moduleBackgroundColor = ERROR_BACKGROUND_COLOR;
    private Color moduleBorderColor = ERROR_BORDER_COLOR;
    private ConnectionLayer connectionLayer;
    private Layer submoduleLayer;
    private FreeformLayer messageLayer;
    private SpringEmbedderLayout layouter;

    // TODO implement ruler
    @SuppressWarnings("unused")
    private float scale = 1.0f;
    @SuppressWarnings("unused")
    private String unit = "px";

    // background layer to provide background coloring, images and grid drawing
    class BackgroundLayer extends FreeformLayer {
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
    class SubmoduleLayer extends FreeformLayer {
        // we get the submodule size from the submodule layouter directly
        @Override
        public Rectangle getFreeformExtent() {
            invalidate();
            Rectangle rect = super.getFreeformExtent();
            rect.setSize(layouter.getPreferredSize(this, -1, -1));
            return rect;
        }
        
        @Override
        public void add(IFigure child, Object constraint, int index) {
            // request an auto-layout whenever an unpinned submodule is added (added from the text editor) 
            if (child instanceof SubmoduleFigure && !((SubmoduleFigure)child).isPinned())
                layouter.requestAutoLayout();
            super.add(child, constraint, index);
        }
    }

    /**
     * A special layer used in compound module, for holding the connections. This
     * layer overrides the getFreeFormExtent method, so the size of this layer will not be
     * used during the calculation of compound module size.
     */
    class NedConnectionLayer extends ConnectionLayer {

        /* (non-Javadoc)
         * @see org.eclipse.draw2d.FreeformLayer#getFreeformExtent()
         * This method is overridden so the size of a NEDConnecionLayer is not counted during
         * compound module size calculation. This prevents showing the scrollbars if we move
         * a connection end-point out of the compound module
         */
        @Override
        public Rectangle getFreeformExtent() {
            // the size of the figure is always smaller than it's parent (it's a point in the middle)
            return getParent().getBounds().getCopy().scale(0);
        }

        @Override
        public void paint(Graphics graphics) {
            graphics.pushState();
            // set antialiasing on content and child/derived figures
            if (NedFileFigure.antialias != SWT.DEFAULT)
                graphics.setAntialias(NedFileFigure.antialias);
            super.paint(graphics);
            graphics.popState();
        }
    }

    /**
     * This layer overrides the getFreeFormExtent method, so the size of this layer will not be
     * used during the calculation of compound module size.
     */
    class NonExtendableFreeformLayer extends FreeformLayer {
        @Override
        public Rectangle getFreeformExtent() {
            // the size of the figure is always smaller than it's parent (it's a point in the middle of the parent figure)
            return getParent().getBounds().getCopy().scale(0);
        }
    }
    
    /**
     * Used at the left inner types compartment.
     */
    class InnerTypesBorder extends MarginBorder {
        InnerTypesBorder(int t, int l, int b, int r) {
            super(t, l, b, r);
        }

        @Override
        public void paint(IFigure f, Graphics g, Insets i) {
            Rectangle r = getPaintRectangle(f, i);
            g.setForegroundColor(ColorConstants.buttonDarker);
            int x = r.x + insets.left/4;
            g.drawLine(x, r.y, x, r.bottom());
        }
    }

    public CompoundModuleFigure() {
        super();

        // set up the name and error marker figure
        ToolbarLayout tb = new ToolbarLayout();
        tb.setSpacing(2);
        tb.setStretchMinorAxis(false);
        setLayoutManager(tb);
        // position the error marker above the problemMarker figure
        Layer nameHelperLayer = new Layer();
        nameHelperLayer.setLayoutManager(new XYLayout());
        nameHelperLayer.add(nameFigure, new Rectangle(0,0,-1,-1));
        nameHelperLayer.add(problemMarkerFigure, new Rectangle(-1,0,16,16));
        add(nameHelperLayer);

        // create the container for the inner types
        innerTypeContainer = new Figure();
        ToolbarLayout typesLayout = new ToolbarLayout();
        typesLayout.setStretchMinorAxis(false);
        typesLayout.setSpacing(5);
        innerTypeContainer.setBorder(new InnerTypesBorder(0, 20, 0, 0 ));
        innerTypeContainer.setLayoutManager(typesLayout);
        add(innerTypeContainer);

        // contains all layers used inside a compound modules submodule area
        layeredPane = new FreeformLayeredPane();
        layeredPane.setLayoutManager(new StackLayout());
        layeredPane.addLayerAfter(new BackgroundLayer(), LayerID.BACKGROUND, null);
        layeredPane.addLayerAfter(new NonExtendableFreeformLayer(), LayerID.BACKGROUND_DECORATION, LayerID.BACKGROUND);
        layeredPane.addLayerAfter(submoduleLayer = new SubmoduleLayer(), LayerID.DEFAULT, LayerID.BACKGROUND_DECORATION);
        layeredPane.addLayerAfter(new NonExtendableFreeformLayer(), LayerID.FRONT_DECORATION, LayerID.DEFAULT);
        layeredPane.addLayerAfter(connectionLayer = new NedConnectionLayer(), LayerID.CONNECTION, LayerID.FRONT_DECORATION);
        layeredPane.addLayerAfter(messageLayer = new NonExtendableFreeformLayer(), LayerID.MESSAGE, LayerID.CONNECTION);

        submoduleLayer.setLayoutManager(layouter = new SpringEmbedderLayout(this));

        // the main container area (where submodules are displayed) of the compModule
        // create scroller and viewport to manage the scrollbars and scrolling
        mainContainer = new ScrollPane();
        mainContainer.setScrollBarVisibility(ScrollPane.NEVER);
        mainContainer.setViewport(new FreeformViewport());
        mainContainer.setContents(layeredPane);
        mainContainer.setBorder(new CompoundModuleLineBorder());
        add(mainContainer);

        // this effectively creates the following hierarchy:
        // -- ScrollPane (+FreeformViewport)
        // -- viewportPane
        // ---- FreeformLayeredPane (viewportContent)
        // ------ backgroundLayer (compound module background. images, colors, grid etc)
        // ------ backgroundDecorationLayer (submodule background decoration (range indicator etc). non extendable
        // ------ pane (pain layer used to display submodules - size is automatically calculated from child size and positions)
        // ------ foregroundDecorationLayer (text messages, decorator icons etc)
        // ------ connection (connections inside a compound module)
        // ------ message layer (used to display message animation effects)

        // set the connection routing
        FanRouter fr = new FanRouter();
        fr.setSeparation(10);
        CompoundModuleShortestPathConnectionRouter spcr =
        	new CompoundModuleShortestPathConnectionRouter(submoduleLayer);
        spcr.setSpacing(10);
        fr.setNextRouter(spcr);
// use this for fan router
//        setConnectionRouter(fr);
// use this for shortest path router
//        setConnectionRouter(spcr);
// simple straight connection router
        setConnectionRouter(new CompoundModuleConnectionRouter());
    }

    public void setConnectionRouter(ConnectionRouter router) {
		connectionLayer.setConnectionRouter(router);
	}

    public IFigure getSubmoduleContainer() {
        // this is the figure which is used to add submodule children by the editpart
        return submoduleLayer;
    }

    public IFigure getInnerTypeContainer() {
        // this is the figure which is used to add inner types
        return innerTypeContainer;
    }

    public IFigure getConnectionContainer() {
        return connectionLayer;
    }
    /**
     * @see org.eclipse.gef.handles.HandleBounds#getHandleBounds()
     */
    public Rectangle getHandleBounds() {
        return mainContainer.getBounds();
    }

    /**
     * Returns the bounds where the anchors should be placed
     */
    public Rectangle getAnchorBounds() {
    	Rectangle box = mainContainer.getClientArea();
    	box.setLocation(0, 0);
    	// take into account the compound module scrolling position
    	layeredPane.translateToAbsolute(box);
    	// take into account the NedFile figure scrolling position
    	getParent().translateToRelative(box);
    	// decrease the size a little to the same as the selection border
    	return box.shrink(2*BORDER_SNAP_WIDTH, 2*BORDER_SNAP_WIDTH);

    }

    /**
     * Returns whether the point is on the border area, where dragging and selection and connection start/end is possible
     */
    public boolean isOnBorder(int x, int y) {
        Point mouse = new Point(x,y);
        translateToRelative(mouse);
        return getBounds().contains(mouse) &&
            !mainContainer.getClientArea().shrink(2*BORDER_SNAP_WIDTH, 2*BORDER_SNAP_WIDTH).contains(mouse);
    }

    @Override
    public Dimension getPreferredSize(int w, int h) {
    	// IMPORTANT: we do not care about external size hints so we do not pass the downwards
        return super.getPreferredSize(-1, -1);
    }

    /**
     * Helper function to return the current border
     */
    public CompoundModuleLineBorder getCompoundModuleBorder() {
    	return (CompoundModuleLineBorder)mainContainer.getBorder();
    }

    public Layer getLayer(LayerID layerId) {
        return layeredPane.getLayer(layerId);
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
		mainContainer.setBackgroundColor(moduleBorderColor);
		getCompoundModuleBorder().setColor(moduleBorderColor);
		getCompoundModuleBorder().setWidth(borderWidth < 0 ? ERROR_BORDER_WIDTH : borderWidth);
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

	protected void setDefaultShape(Image img, String shape, int shapeWidth, int shapeHeight, Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
        if (img == null)
            img = ImageFactory.getImage(ImageFactory.DEFAULT_KEY);
		nameFigure.setIcon(img);
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
		// setup the figure's properties
        // set the icon showing the default representation in the titlebar
        Image img = ImageFactory.getImage(
        		dps.getAsString(IDisplayString.Prop.IMAGE),
        		dps.getAsString(IDisplayString.Prop.IMAGESIZE),
        		ColorFactory.asRGB(dps.getAsString(IDisplayString.Prop.IMAGECOLOR)),
        		dps.getAsInt(IDisplayString.Prop.IMAGECOLORPERCENTAGE,0));
        setDefaultShape(img,
        		dps.getAsString(IDisplayString.Prop.SHAPE),
        		dps.getAsInt(IDisplayString.Prop.WIDTH, -1),
        		dps.getAsInt(IDisplayString.Prop.HEIGHT, -1),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.FILLCOL)),
        		ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.BORDERCOL)),
        		dps.getAsInt(IDisplayString.Prop.BORDERWIDTH, -1));

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
            layouter.requestAutoLayout();
        }

        // size is manually set
        if (backgroundSize.height > 0 || backgroundSize.width > 0) {
            Insets borderInset = mainContainer.getBorder().getInsets(this);
            Dimension newPrefSize = backgroundSize.getCopy().expand(borderInset.getWidth(), borderInset.getHeight());
            mainContainer.setPreferredSize(newPrefSize);
        } 
        else { // size should be calculated by the layouter
            mainContainer.setPreferredSize(null);
        }
        invalidate();
	}

	/**
	 * Adds the given submodule child figure to the correct layer
	 */
	public void addSubmoduleFigure(SubmoduleFigure submoduleFig) {
		submoduleLayer.add(submoduleFig);
	}

	/**
	 * Removes a submodule child figure from the compound module
	 */
	public void removeSubmoduleFigure(SubmoduleFigure submoduleFig) {
		submoduleLayer.remove(submoduleFig);
	}

	/**
	 * Adds a connection figure to the connection layer of the compound module
	 */
	public void addConnectionFigure(Connection conn) {
		connectionLayer.add(conn);
	}

	/**
	 * Removes a connection child figure from the compound module
	 */
	public void removeConnectionFigure(Connection conn) {
		connectionLayer.remove(conn);
	}

	/**
	 * Adds a message decoration figure above connection layer of the compound module
	 */
	public void addMessageFigure(IFigure messageFigure) {
		messageLayer.add(messageFigure);
	}

	/**
	 * Removes a message decoration child figure from the compound module
	 */
	public void removeMessageFigure(IFigure messageFigure) {
		messageLayer.remove(messageFigure);
	}

    public Dimension getBackgroundSize() {
        return backgroundSize;
    }

}
