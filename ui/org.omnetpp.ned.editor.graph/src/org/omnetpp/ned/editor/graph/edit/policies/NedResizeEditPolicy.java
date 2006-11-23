package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.Iterator;
import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;

/**
 * Handles feedback figures during move and resize and enables / disables 
 * drag and resize.
 */
public class NedResizeEditPolicy extends ResizableEditPolicy {

    /**
     * Creates the figure used for feedback.
     * 
     * @return the new feedback figure
     */
    @Override
    protected IFigure createDragSourceFeedbackFigure() {
        IFigure figure = createFigure((GraphicalEditPart) getHost(), null);

        figure.setBounds(getInitialFeedbackBounds());
        addFeedback(figure);
        return figure;
    }

    protected IFigure createFigure(GraphicalEditPart part, IFigure parent) {
        IFigure child = getCustomFeedbackFigure(part, part.getModel());
        
        if (parent != null) parent.add(child);

        Rectangle childBounds = part.getFigure().getBounds().getCopy();

        IFigure walker = part.getFigure().getParent();
        while (walker != ((GraphicalEditPart) part.getParent()).getFigure()) {
            walker.translateToParent(childBounds);
            walker = walker.getParent();
        }
        child.setBounds(childBounds);

        Iterator i = part.getChildren().iterator();
        while (i.hasNext())
            createFigure((GraphicalEditPart) i.next(), child);

        return child;
    }

    protected IFigure getCustomFeedbackFigure(GraphicalEditPart part, Object model) {
        RectangleFigure figure;

        figure = new RectangleFigure();
        figure.setFill(false);
        figure.setLineStyle(SWT.LINE_DOT);
        return figure;
    }

    /**
     * Returns the layer used for displaying feedback.
     * 
     * @return the feedback layer
     */
    @Override
    protected IFigure getFeedbackLayer() {
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }

    /**
     * @see org.eclipse.gef.editpolicies.NonResizableEditPolicy#getInitialFeedbackBounds()
     */
    @Override
    protected Rectangle getInitialFeedbackBounds() {
        return getHostFigure().getBounds();
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.editpolicies.ResizableEditPolicy#createSelectionHandles()
     * change the color of all selection handles
     */
    @Override
    protected List createSelectionHandles() {
        List handles = super.createSelectionHandles();
        Color color = isDragAllowed() ? ColorFactory.highlight : ColorFactory.lowlight;
        // set the color for all handles
        for(Object handle : handles) 
            ((IFigure)handle).setForegroundColor(color);
        
        return handles;
    }
    
}