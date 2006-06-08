package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.Iterator;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.omnetpp.common.color.ColorFactory;

/**
 * Handles feedback figures during move and resize.
 * 
 */
public class ResizeFeedbackEditPolicy extends ResizableEditPolicy {

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

    protected IFigure getCustomFeedbackFigure(GraphicalEditPart part, Object modelPart) {
        IFigure figure;

        figure = new RectangleFigure();
        ((RectangleFigure) figure).setFill(false);
        figure.setForegroundColor(ColorFactory.highlight);

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

    @Override
    protected void addSelectionHandles() {
        super.addSelectionHandles();
        getLayer(LayerConstants.HANDLE_LAYER).setForegroundColor(ColorFactory.highlight);
    }

}