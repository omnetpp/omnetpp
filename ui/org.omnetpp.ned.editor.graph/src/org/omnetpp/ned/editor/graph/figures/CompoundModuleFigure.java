package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.FreeformLayer;
import org.eclipse.draw2d.FreeformLayeredPane;
import org.eclipse.draw2d.FreeformViewport;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.LayeredPane;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Pattern;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayBackgroundSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayTitleSupport;
import org.omnetpp.ned.editor.graph.figures.properties.LayerSupport;
import org.omnetpp.ned.editor.graph.misc.FreeformDesktopLayout;

public class CompoundModuleFigure extends ModuleFigure 
				implements DisplayBackgroundSupport, DisplayTitleSupport,
				LayerSupport, HandleBounds {

    private static final int DEFAULT_BORDER_WIDTH = 2;
    private static Dimension DEFAULT_SIZE = new Dimension(200, 100);
	private Layer pane;
    private ScrollPane scrollpane;
    private LayeredPane layeredPane;
    private Image backgroundImage;
    private String backgroundImageArr = "fix";
    private int gridTickDistance;
    private int gridNoOfMinorTics;
    private Color gridColor;
    private Color moduleBackgroundColor = ColorFactory.defaultBackground;
    private Color moduleBorderColor = ColorFactory.defaultBorder;
    private float scale = 1.0f;
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
	        graphics.setBackgroundPattern(nonplayPattern);
	        graphics.fillRectangle(getClientArea());

	        // draw a solid background
	        if (moduleBackgroundColor != null) {
	        	graphics.setBackgroundColor(moduleBackgroundColor);
	        	graphics.fillRectangle(viewportRect);
	        }
	        
	        // draw background image
	        // clip the background image
	        graphics.clipRect(viewportRect);
	        if (backgroundImage != null) {
	            Rectangle imageRect = new Rectangle(backgroundImage.getBounds());
	            if (backgroundImageArr.toLowerCase().startsWith("t"))
					for(int y = viewportRect.y; y<viewportRect.bottom(); y += imageRect.height)
						for(int x = viewportRect.x; x<viewportRect.right(); x += imageRect.width)
							graphics.drawImage(backgroundImage, x, y);
	            else if (backgroundImageArr.toLowerCase().startsWith("s"))
	            	graphics.drawImage(backgroundImage, imageRect, viewportRect);
	            else if (backgroundImageArr.toLowerCase().startsWith("c")) {
	            	Point centerPoint = viewportRect.getCenter().translate(-imageRect.width/2, -imageRect.height/2);
	            	graphics.drawImage(backgroundImage, centerPoint);
	            } else
	            	graphics.drawImage(backgroundImage, viewportRect.getLocation());
	        }

	        // =============================================================================
	        // draw the grid
	        if(gridTickDistance > 1) {

	        	graphics.setForegroundColor(gridColor);
	        	double minorTickDistance = 0;
	        	if (gridNoOfMinorTics > 1)
	        		minorTickDistance = (double)gridTickDistance / gridNoOfMinorTics;
	        	
	        	// horizontal grid
	        	for(int y = viewportRect.y; y<viewportRect.bottom(); y += gridTickDistance) {
	        		graphics.setLineStyle(SWT.LINE_SOLID);
	        		graphics.drawLine(viewportRect.x, y, viewportRect.right(), y);
	        		// minor ticks
	        		graphics.setLineStyle(SWT.LINE_DOT);
	        		for(double my = y;  my < y+gridTickDistance && my < viewportRect.bottom() && minorTickDistance > 1; my+=minorTickDistance)
		        		graphics.drawLine(viewportRect.x, (int)my, viewportRect.right(), (int)my);
	        	}
	        	// vertical grid
	        	for(int x = viewportRect.x; x<viewportRect.right(); x += gridTickDistance) {
	        		graphics.setLineStyle(SWT.LINE_SOLID);
	        		graphics.drawLine(x, viewportRect.y, x, viewportRect.bottom());
	        		// minor ticks
	        		graphics.setLineStyle(SWT.LINE_DOT);
	        		for(double mx = x;  mx < x+gridTickDistance && mx < viewportRect.right() && minorTickDistance > 1; mx+=minorTickDistance)
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
        // add the maint layer to the scroller pane
        // create the main and the decoration layers that will be added into the viewportPane
        pane = new FreeformLayer();
        pane.setLayoutManager(new FreeformDesktopLayout());

        layeredPane = new FreeformLayeredPane();
        layeredPane.setLayoutManager(new StackLayout());
        layeredPane.addLayerAfter(pane, LayerSupport.LayerID.Default, null);
        layeredPane.addLayerBefore(new BackgroundLayer(), LayerID.Background, LayerID.Default);
        layeredPane.addLayerBefore(new FreeformLayer(), LayerID.BackgroundDecoration, LayerID.Default);
        layeredPane.addLayerAfter(new FreeformLayer(), LayerID.FrontDecoration, LayerID.Default);
        
        scrollpane.setViewport(new FreeformViewport());
        scrollpane.setContents(layeredPane);
        add(scrollpane);

        // this effectively creates the following hierechy:
        // -- ScrollPane (+FreeformViewport)
        // -- viewportPane
        // ---- FreeformLayeredPane (viewportContent)
        // ------ backgroundLayer
        // ------ backgroundDecorationLayer
        // ------ pane
        // ------ foregroundDecorationLayer
        
    	createConnectionAnchors();
    }

    protected void createConnectionAnchors() {
        PinnableNoncentralChopboxAnchor in, out;
        for (int i = 0; i < 8; i++) {
            in = new PinnableNoncentralChopboxAnchor(this);
            out = new PinnableNoncentralChopboxAnchor(this);
            in.setPinnedDown(true);
            out.setPinnedDown(true);
            if (i > 3) {
                in.setPinnedLocationY(1.0);
                out.setPinnedLocationY(0.0);
            } else {
                in.setPinnedLocationY(0.0);
                out.setPinnedLocationY(1.0);
            }
            addSourceConnectionAnchor(out, "OUT"+i);
            addTargetConnectionAnchor(out, "IN"+i);

        }
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

    protected PinnableNoncentralChopboxAnchor getInputConnectionAnchor(int i) {
        return (PinnableNoncentralChopboxAnchor) getConnectionAnchor("IN"+i);
    }

    protected PinnableNoncentralChopboxAnchor getOutputConnectionAnchor(int i) {
        return (PinnableNoncentralChopboxAnchor) getConnectionAnchor("OUT"+i);
    }

    @Override
    public Dimension getPreferredSize(int w, int h) {
    	// we are not sensitve to the external size hints 
        Dimension prefSize = super.getPreferredSize(-1, -1);
        prefSize.union(DEFAULT_SIZE);
        return prefSize;
    }
    
    protected void layoutConnectionAnchors() {
        for (int i = 0; i < 4; i++) {
            getOutputConnectionAnchor(i + 4).setPinnedLocationX(0.125 + i / 4.0);
            getInputConnectionAnchor(i).setPinnedLocationX(0.125 + i / 4.0);
            getOutputConnectionAnchor(i).setPinnedLocationX(0.125 + i / 4.0);
            getInputConnectionAnchor(i + 4).setPinnedLocationX(0.125 + i / 4.0);
        }
    }

    @Override
    public void validate() {
        if (isValid()) return;
        layoutConnectionAnchors();
        super.validate();
    }

    /**
     * Helper function to return the current border 
     * @return
     */
    protected CompoundModuleBorder getCompoundModuleBorder() {
    	return (CompoundModuleBorder)getBorder();
    }
    
    public Layer getLayer(Object layerId) {
        return layeredPane.getLayer(layerId);
    }

	public void setBackgorund(Image img, String arrange, Color backgroundColor, Color borderColor, int borderWidth) {
		moduleBackgroundColor = (backgroundColor==null) ? ColorFactory.defaultBackground : backgroundColor;
		moduleBorderColor = (borderColor==null) ? ColorFactory.defaultBorder : borderColor;
		
		// the global background is the same as the border color
		setBackgroundColor(moduleBorderColor);
		getCompoundModuleBorder().setBorderColor(moduleBorderColor);
		getCompoundModuleBorder().setTitleBackgroundColor(moduleBackgroundColor);
		getCompoundModuleBorder().setBorderWidth(borderWidth < 0 ? DEFAULT_BORDER_WIDTH : borderWidth);
		// background image
		backgroundImage = img;
		backgroundImageArr = arrange != null ? arrange : "";
		invalidate();
	}

	public void setGrid(int tickDistance, int noOfTics, Color gridColor) {
		this.gridTickDistance = tickDistance;
		this.gridNoOfMinorTics = noOfTics;
		this.gridColor = gridColor;
		invalidate();
	}

	public void setDefaultShape(Image img, String shape, int shapeWidth, int shapeHeight, Color shapeFillColor, Color shapeBorderColor, int shapeBorderWidth) {
		getCompoundModuleBorder().setImage(img);
		// TODO support shapes too
		invalidate();
	}

	public void setName(String name) {
		getCompoundModuleBorder().setLabel(name);
		invalidate();
	}

	public void setScale(float scale, String unit) {
		this.scale = scale;
		this.unit = unit;
		invalidate();
	}
}