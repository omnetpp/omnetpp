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
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayBackgroundSupport;
import org.omnetpp.ned.editor.graph.figures.properties.DisplayTitleSupport;
import org.omnetpp.ned.editor.graph.figures.properties.LayerSupport;
import org.omnetpp.ned.editor.graph.misc.FreeformDesktopLayout;

public class CompoundModuleFigure extends ModuleFigure 
				implements DisplayBackgroundSupport, DisplayTitleSupport,
				LayerSupport, HandleBounds {

    private Layer pane;
    private LayeredPane layeredPane;
    private Image backgroundImage;
    private ImageArrangement backgroundImageArr = ImageArrangement.Scretch;

    // background layer to provide bacground coloring, images and grid drawing
    class BackgroundLayer extends FreeformLayer {

    	
		@Override
		protected void paintFigure(Graphics graphics) {
	        Rectangle clientRect = getClientArea();
	        // draw background image
	        if (backgroundImage != null) {
	            Rectangle imageRect = new Rectangle(backgroundImage.getBounds());
	            if (backgroundImageArr.equals(ImageArrangement.Scretch))
	            	graphics.drawImage(backgroundImage, imageRect, clientRect);
		        // TODO implement centered and tiled image arrangement

	        }
		}
    }
    
    public CompoundModuleFigure() {
        super();
        setBorder(new CompoundModuleBorder());
        getCompoundModuleBorder().getTitleBorder().setPadding(5);
        
        setLayoutManager(new StackLayout());
        // create scroller and viewport to manage the scrollbars and scrolling
        ScrollPane scrollpane = new ScrollPane();
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
        Dimension prefSize = super.getPreferredSize(w, h);
        Dimension defaultSize = new Dimension(100, 100);
        prefSize.union(defaultSize);
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

	public void setBackgorund(Image img, ImageArrangement arrange, Color backgroundColor, Color borderColor, int borderWidth) {
		getCompoundModuleBorder().setColor(borderColor);
		getCompoundModuleBorder().setWidth(borderWidth);
		setBackgroundColor(backgroundColor);
		backgroundImage = img;
		backgroundImageArr = arrange;
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

}