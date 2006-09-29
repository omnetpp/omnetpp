package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.ITopLevelElement;
import org.omnetpp.ned2.model.NedFileNodeEx;
import org.omnetpp.ned2.model.SubmoduleNodeEx;

/**
 * @author rhornig
 * Factory to create corresponding controller objects for the modell objects.
 * Only model objects ecplicitly handled here will have a controleer and a visulal
 * counterpart in the editor 
 */
public class NedEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        if (model == null) return null;

        EditPart child = null;

        if (model instanceof NedFileNodeEx) 
        	child = new NedFileEditPart();
        else if (model instanceof CompoundModuleNodeEx)
            child = new CompoundModuleEditPart();
        else if (model instanceof SubmoduleNodeEx)
            child = new SubmoduleEditPart();
        else if (model instanceof ConnectionNodeEx)
            child = new ModuleConnectionEditPart();
        else if (model instanceof ITopLevelElement)
            child = new TopLevelEditPart();
        else {
            System.out.println("Unknown model element: " + model.toString());
            return null;
        }
        child.setModel(model);
        return child;
    }

}
