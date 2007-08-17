package org.omnetpp.figures;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.ConnectionLayer;
import org.eclipse.draw2d.ConnectionRouter;
import org.eclipse.draw2d.FanRouter;
import org.eclipse.draw2d.FreeformLayer;
import org.eclipse.draw2d.FreeformLayeredPane;
import org.eclipse.draw2d.FreeformViewport;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LayeredPane;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.gef.tools.CellEditorLocator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Pattern;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.layout.SpringEmbedderLayout;
import org.omnetpp.figures.misc.IDirectEditSupport;
import org.omnetpp.figures.routers.CompoundModuleConnectionRouter;
import org.omnetpp.figures.routers.CompoundModuleShortestPathConnectionRouter;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class CompoundModuleFigure extends ModuleFigure
				implements ILayerSupport, HandleBounds, IDirectEditSupport {

    private static final int ERROR_BORDER_WIDTH = 2;
    public static final Color ERROR_BACKGROUND_COLOR = ColorFactory.RED;
    public static final Color ERROR_BORDER_COLOR = ColorFactory.RED4;
    private static final int BORDER_SNAP_WIDTH = 3;
    private static final Dimension DEFAULT_SIZE = new Dimension(300, 200);

	private Layer pane;
    private ScrollPane scrollpane;
    private LayeredPane layeredPane;
    private Image backgroundImage;
    private String backgroundImageArrangement = "fix";
    private int gridTickDistance;
    private int gridNoOfMinorTics;
    private Color gridColor;
    private Color moduleBackgroundColor = ERROR_BACKGROUND_COLOR;
    private Color moduleBorderColor = ERROR_BORDER_COLOR;
    private ConnectionLayer connectionLayer;
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

        	// get the size of the viewport (which is actually the module size)
	        Rectangle viewportRect = new Rectangle(new Point(0,0), scrollpane.getSize());

	        // draw outer non playground area
	        Pattern nonplayPattern = new Pattern(null, 0,0,5,5, moduleBackgroundColor, moduleBorderColor);
	        graphics.setBackgroundColor(moduleBackgroundColor);
	        // WARNING only the default SWT graphics implements this method
	        // if we are using ScaledGrahics we should not use it.
	        // TODO look for other effect to show non-play area which is more scale friendly
	        if (graphics instanceof SWTGraphics)
	        	graphics.setBackgroundPattern(nonplayPattern);

	        graphics.fillRectangle(getClientArea());

	        // draw a solid background
	        if (moduleBackgroundColor != null) {
	        	graphics.setBackgroundColor(moduleBackgroundColor);
	        	graphics.fillRectangle(viewportRect);
	        }

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

    public CompoundModuleFigure() {
        super();
        setBorder(new CompoundModuleBorder());
        getCompoundModuleBorder().getTitleBorder().setPadding(5);

        setLayoutManager(new StackLayout());
        // create scroller and viewport to manage the scrollbars and scrolling
		scrollpane = new ScrollPane();
        // add the main layer to the scroller pane
        // create the main and the decoration layers that will be added into the viewportPane
        pane = new FreeformLayer();
        messageLayer = new NonExtendableFreeformLayer();
        connectionLayer = new NEDConnectionLayer();

        layeredPane = new FreeformLayeredPane();
        layeredPane.setLayoutManager(new StackLayout());

        layeredPane.addLayerAfter(new BackgroundLayer(), LayerID.BACKGROUND, null);
        layeredPane.addLayerAfter(new NonExtendableFreeformLayer(), LayerID.BACKGROUND_DECORATION, LayerID.BACKGROUND);
        layeredPane.addLayerAfter(pane, LayerID.DEFAULT, LayerID.BACKGROUND_DECORATION);
        layeredPane.addLayerAfter(new FreeformLayer(), LayerID.FRONT_DECORATION, LayerID.DEFAULT);
        layeredPane.addLayerAfter(connectionLayer, LayerID.CONNECTION, LayerID.FRONT_DECORATION);
        layeredPane.addLayerAfter(messageLayer, LayerID.MESSAGE, LayerID.CONNECTION);

        scrollpane.setViewport(new FreeformViewport());
        scrollpane.setContents(layeredPane);
        add(scrollpane);

        pane.setLayoutManager(layouter = new SpringEmbedderLayout(pane, connectionLayer));

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
        	new CompoundModuleShortestPathConnectionRouter(pane);
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

    public IFigure getContentsPane() {
        return pane;
    }

    /**
     * @see org.eclipse.gef.handles.HandleBounds#getHandleBounds()
     */
    public Rectangle getHandleBounds() {
    	// the selection handle should exclude the outer (title) border
        return getBounds().getCropped(
        		((CompoundModuleBorder)getBorder()).getOuterBorder().getInsets(this));
    }

    /**
     * Returns the bounds where the anchors should be placed
     */
    public Rectangle getAnchorBounds() {
    	Rectangle box = getClientArea();
    	box.setLocation(0, 0);
    	// take into account the compound module scrolling position
    	layeredPane.translateToAbsolute(box);
    	// take into account the NedFile figure scrolling position
    	getParent().translateToRelative(box);
    	// decrease the size a little to the same as the selection border
    	return box.shrink(2*BORDER_SNAP_WIDTH, 2*BORDER_SNAP_WIDTH);

    }

    @Override
    public Dimension getPreferredSize(int w, int h) {
    	// we are not sensitive to the external size hints
        Dimension prefSize = super.getPreferredSize(-1, -1);
        if (pane.getChildren().size() == 0)
            return DEFAULT_SIZE;

        return prefSize;
    }

    /**
     * Helper function to return the current border
     */
    public CompoundModuleBorder getCompoundModuleBorder() {
    	return (CompoundModuleBorder)getBorder();
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
		setBackgroundColor(moduleBorderColor);
		getCompoundModuleBorder().setBorderColor(moduleBorderColor);
		// there is no separate title color
        getCompoundModuleBorder().setTitleBackgroundColor(null);
		getCompoundModuleBorder().setBorderWidth(borderWidth < 0 ? ERROR_BORDER_WIDTH : borderWidth);
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
		getCompoundModuleBorder().setImage(img);
		// TODO support shapes too
		invalidate();
	}

	public void setName(String name) {
		getCompoundModuleBorder().setLabel(name);
		figureName = name;
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
		lastDisplayString = dps;

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
        // we call the resizing last time because other parameters like the icon size or the broder width
        // can affect the size of bounding box
        Dimension newSize = dps.getCompoundSize(null);

        long seed = dps.getAsInt(IDisplayString.Prop.MODULE_LAYOUT_SEED, 1);
        // if the seed changed we explicitly have to force a re-layout
        if (seed != layouter.getSeed()) {
            layouter.setSeed(seed);
            layouter.requestAutoLayout();
        }

        if (newSize.height > 0 || newSize.width > 0) {
            Insets borderInset = getBorder().getInsets(this);
            Dimension newPrefSize = newSize.getCopy().expand(borderInset.getWidth(), borderInset.getHeight());
            setPreferredSize(newPrefSize);
            pane.setPreferredSize(newPrefSize.getCopy());
        }
        else {
            setPreferredSize(null);
            pane.setPreferredSize(null);
        }
        invalidate();
	}

	/**
	 * Returns whether the point is on the border area, where dragging and selection and connection start/end is possible
	 */
	public boolean isOnBorder(int x, int y) {
		Point mouse = new Point(x,y);
		translateToRelative(mouse);
		return getBounds().contains(mouse) &&
			!getClientArea().shrink(2*BORDER_SNAP_WIDTH, 2*BORDER_SNAP_WIDTH).contains(mouse);
	}

	/**
	 * Adds the given submodule child figure to the correct layer
	 */
	public void addSubmoduleFigure(SubmoduleFigure submoduleFig) {
		pane.add(submoduleFig);
	}

	/**
	 * Removes a submodule child figure from the compound module
	 */
	public void removeSubmoduleFigure(SubmoduleFigure submoduleFig) {
		pane.remove(submoduleFig);
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

    // delegate the direct edit support to the TitleBorder
    public CellEditorLocator getDirectEditCellEditorLocator() {
        return getCompoundModuleBorder().getTitleBorder().getDirectEditCellEditorLocator();
    }

    public String getDirectEditText() {
        return getCompoundModuleBorder().getTitleBorder().getDirectEditText();
    }

    public void setDirectEditTextVisible(boolean visible) {
        getCompoundModuleBorder().getTitleBorder().setDirectEditTextVisible(visible);
    }

    /**
     * Display a "problem" image decoration on the submodule.
     * @param severity  any of the IMarker.SEVERITY_xxx constants, or -1 for none
     */
    public void setProblemDecoration(int severity) {
    	Image image = getProblemImageFor(severity);
//TODO implement problem decoration    	
//    	if (image != null)
//    		problemMarkerFigure.setImage(image);
//    	problemMarkerFigure.setVisible(image==null);
//    	invalidate(); //XXX needed?
    }

}
