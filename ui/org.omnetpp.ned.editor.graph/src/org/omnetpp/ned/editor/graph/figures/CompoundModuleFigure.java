package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.ColorConstants;
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
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.handles.HandleBounds;
import org.omnetpp.ned.editor.graph.figures.properties.LayerSupport;
import org.omnetpp.ned.editor.graph.misc.FreeformDesktopLayout;

public class CompoundModuleFigure extends NedFigure implements LayerSupport, HandleBounds {

    private Layer pane;
    private LayeredPane layeredPane;

    public CompoundModuleFigure() {
        super();
        
        setBorder(new CompoundModuleBorder());
        setLayoutManager(new StackLayout());

        // create scroller and viewport to manage the scrollbars and scrolling
        ScrollPane scrollpane = new ScrollPane();
        // add the maint layer to the scroller pane
        // create the main and the decoration layers that will be added into the viewportPane
        pane = new FreeformLayer();
        pane.setLayoutManager(new FreeformDesktopLayout());

        layeredPane = new FreeformLayeredPane();
        layeredPane.setLayoutManager(new StackLayout());
        layeredPane.addLayerAfter(pane, LayerSupport.DEFAULT_LAYER, null);
        layeredPane.addLayerBefore(new FreeformLayer(), LayerSupport.BACK_DECORATION_LAYER, LayerSupport.DEFAULT_LAYER);
        layeredPane.addLayerAfter(new FreeformLayer(), LayerSupport.FRONT_DECORATION_LAYER, LayerSupport.DEFAULT_LAYER);
        
        scrollpane.setViewport(new FreeformViewport());
        scrollpane.setContents(layeredPane);
        
        add(scrollpane);

        // this effectively creates the following hierechy:
        // -- ScrollPane (+FreeformViewport)
        // -- viewportPane
        // ---- FreeformLayeredPane (viewportContent)
        // ------ backgroundLayer
        // ------ pane
        // ------ foregroundLayer
        
        
        createConnectionAnchors();
        setBackgroundColor(ColorConstants.lightGray);
        
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
        return getBounds().getCropped(new Insets(CompoundModuleBorder.HEADER_HEIGHT, 0, 0, 0));
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

    /**
     * @see org.eclipse.draw2d.Figure#paintFigure(Graphics)
     */
    @Override
    protected void paintFigure(Graphics graphics) {
        Rectangle rect = getBounds().getCopy();
        rect.crop(new Insets(CompoundModuleBorder.HEADER_HEIGHT, 0, 0, 0));
        graphics.fillRectangle(rect);
    }

    @Override
    public String toString() {
        return "ModuleFigure"; //$NON-NLS-1$
    }

    @Override
    public void validate() {
        if (isValid()) return;
        layoutConnectionAnchors();
        super.validate();
    }

    public Rectangle getIconBounds() {
        // for the moment we return the whole client area of the figure
        return getBounds();
    }

    public Layer getLayer(Object layerId) {
        return layeredPane.getLayer(layerId);
    }
    
    

}