package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.NedFileNodeEx;
import org.omnetpp.ned2.model.SubmoduleNodeEx;

public class NedEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        if (model == null) return null;

        EditPart child = null;

        if (model instanceof NedFileNodeEx) 
        	child = new NedFileDiagramEditPart();
        else if (model instanceof CompoundModuleNodeEx)
            child = new CompoundModuleEditPart();
        else if (model instanceof SubmoduleNodeEx)
            child = new SubmoduleEditPart();
        else if (model instanceof ConnectionNodeEx)
            child = new ConnectionEditPart();
        else
                System.out.println("Unknown model element: " + model.toString());
        child.setModel(model);
        return child;
    }

}
