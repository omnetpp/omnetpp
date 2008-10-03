package org.omnetpp.ned.editor.graph.parts.outline;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;

/**
 * Creates edit parts for the NEDElements used in outline view
 *
 * @author rhornig
 */
public class NedTreeEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        return new NedTreeEditPart(model);
    }

}