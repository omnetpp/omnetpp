package org.omnetpp.ned.editor.graph.edit.outline;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.omnetpp.ned.editor.graph.model.NedFileNodeEx;

public class NedTreeEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        if (model instanceof NedFileNodeEx) return new NedFileTreeEditPart(model);
        return new NedTreeEditPart(model);
    }

}