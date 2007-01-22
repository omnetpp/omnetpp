package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.NedFileNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;

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
            Assert.isTrue(false,"Unknown model element: "+model.getClass().getName());
            return null;
        }
        child.setModel(model);
        return child;
    }

}
