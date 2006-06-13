package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.swt.SWT;
import org.omnetpp.ned.editor.graph.figures.properties.LayerSupport;

/**
 * Supports adding / removing conections to a node. It is a generic 'Module' (anything that can have connections)
 * @author rhornig
 *
 */
public class ModuleFigure extends Figure {

    /**
     * Returns a layer from any ancestor that supports multiple layers for decorations
     * @param id Layer id
     * @return The layer with teh given id from any ancestor that inpmelements tha LayerSupport IF
     */
    // TODO implement in a way, that if a layer is not found, it should search furter in the ancestor list
    // and return null only if eached the root figure
    public Layer getAncestorLayer(Object id) {
        IFigure figureIter = getParent();
        // look for a parent who is an instance of LayerSupport and get the layer from it
        while (!(figureIter == null || (figureIter instanceof LayerSupport)))
            figureIter = figureIter.getParent();
        if(figureIter instanceof LayerSupport) return ((LayerSupport)figureIter).getLayer(id); 
        return null;
    }
    
    /* (non-Javadoc)
     * @see org.eclipse.draw2d.IFigure#paint(org.eclipse.draw2d.Graphics)
     * Enable antialiasing for all derived figures
     */
    @Override
    public void paint(Graphics graphics) {
        graphics.setAntialias(SWT.ON);
        super.paint(graphics);
    }

    static public void debugPrintFigureHierarchy(Figure f, String indent) {
    	System.out.println(indent+f.getClass().getSimpleName()+"@"+f.hashCode()+"  "+f.getBounds()+" opaque="+f.isOpaque());
    	for (Object child : f.getChildren()) {
    		debugPrintFigureHierarchy((Figure)child, indent+"    ");
    	}
    }
}