package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.ui.IEditorPart;

/**
 * @author Eric Bordeau
 */
public class ModulePasteTemplateAction extends PasteTemplateAction {

    /**
     * Constructor for ModulePasteTemplateAction.
     * 
     * @param editor
     */
    public ModulePasteTemplateAction(IEditorPart editor) {
        super(editor);
    }

    /**
     * 
     * @see org.omnetpp.ned.editor.graph.actions.PasteTemplateAction#getPasteLocation(GraphicalEditPart)
     */
    protected Point getPasteLocation(GraphicalEditPart container) {
    	Point result = new Point(10, 10);
    	IFigure fig = container.getContentPane();
    	result.translate(fig.getClientArea(Rectangle.SINGLETON).getLocation());
    	fig.translateToAbsolute(result);
    	return result;
    }
}
