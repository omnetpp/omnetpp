package org.omnetpp.ned.editor.graph.edit.outline;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class NedTreeEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        return new NedTreeEditPart(model);
    }

}