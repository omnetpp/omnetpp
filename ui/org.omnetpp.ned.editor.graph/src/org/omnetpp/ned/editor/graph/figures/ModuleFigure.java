/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
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
import org.omnetpp.ned.editor.graph.model.CompoundModuleModel;

public class ModuleFigure extends NedFigure implements LayerSupport, HandleBounds {

    private Layer pane;
    private LayeredPane layeredPane;

    public ModuleFigure() {
        super();
        
        setBorder(new ModuleBorder());
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
            addSourceConnectionAnchor(out, CompoundModuleModel.TERMINALS_OUT[i]);
            addTargetConnectionAnchor(out, CompoundModuleModel.TERMINALS_IN[i]);

        }
    }

    public IFigure getContentsPane() {
        return pane;
    }

    /**
     * @see org.eclipse.gef.handles.HandleBounds#getHandleBounds()
     */
    public Rectangle getHandleBounds() {
        return getBounds().getCropped(new Insets(2, 0, 2, 0));
    }

    protected PinnableNoncentralChopboxAnchor getInputConnectionAnchor(int i) {
        return (PinnableNoncentralChopboxAnchor) getConnectionAnchor(CompoundModuleModel.TERMINALS_IN[i]);
    }

    protected PinnableNoncentralChopboxAnchor getOutputConnectionAnchor(int i) {
        return (PinnableNoncentralChopboxAnchor) getConnectionAnchor(CompoundModuleModel.TERMINALS_OUT[i]);
    }

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
    protected void paintFigure(Graphics graphics) {
        Rectangle rect = getBounds().getCopy();
        rect.crop(new Insets(2, 0, 2, 0));
        graphics.fillRectangle(rect);
    }

    public String toString() {
        return "ModuleFigure"; //$NON-NLS-1$
    }

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