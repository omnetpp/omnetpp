package org.omnetpp.ned.editor.graph.parts.policies;

import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.Request;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;

/**
 * Handles feedback figures during move and resize and enables / disables
 * drag and resize.
 *
 * @author rhornig
 */
public class NedResizeEditPolicy extends ResizableEditPolicy {


    /**
     * Creates and returns the figure used for feedback.
     */
    @Override
    protected IFigure createDragSourceFeedbackFigure() {
        IFigure figure = createFigure((GraphicalEditPart) getHost(), null);

        figure.setBounds(getInitialFeedbackBounds());
        addFeedback(figure);
        return figure;
    }

    @SuppressWarnings("unchecked")
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

        for (GraphicalEditPart childEP : (List<GraphicalEditPart>)part.getChildren())
            createFigure(childEP, child);

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
     * change the color of all selection handles (read only parts will be different color)
     */
    @Override
    protected List<?> createSelectionHandles() {
        List<?> handles = super.createSelectionHandles();
        Color color = EditPartUtil.isEditable(getHost()) ?
                GraphicalNedEditor.HIGHLIGHT_COLOR : GraphicalNedEditor.LOWLIGHT_COLOR;
        // set the color for all handles
        for (Object handle : handles)
            ((IFigure)handle).setForegroundColor(color);

        return handles;
    }

    @Override
    public boolean understandsRequest(Request request) {
        // XXX this is a MEGA HACK.  During the initiation of cloning (CTRL+drag) the DragEditPartsTracker does not check
        // the state of CTRL key, so it creates a MOVE request (instead of clone), however a non-movable submodule
        // (for example an inherited submodule which should not be moved) will report that he does not understand
        // a move request, so the currently selected edit part will be filtered out from targetEditParts (it check it by calling the
        // understandsRequest method on the policy) of the request. The cloning of the submodule will not work because of this,
        // because the understandsRequest for the MOVE request will return false ( it would return isDragAllowed (which is false in case of an
        // inherited submodule)
        // we are pretending here that we support the MOVE command (so the selected part in the targetEditParts of the
        // CLONE request will not be filtered). Of course we still not return an executable command for the MOVE request (so
        // move is still not possible)
        // if this bug will be fixed we could remove this method
        if (REQ_MOVE.equals(request.getType()))
            return true;
        return super.understandsRequest(request);
    }
}
