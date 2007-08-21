package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Factory to create corresponding controller objects for the modell objects.
 * Only model objects ecplicitly handled here will have a controleer and a visulal
 * counterpart in the editor
 *
 * @author rhornig
 */
public class NedEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        if (model == null) return null;

        EditPart child = null;

        if (model instanceof NedFileElementEx)
        	child = new NedFileEditPart();
        else if (model instanceof CompoundModuleElementEx)
            child = new CompoundModuleEditPart();
        else if (model instanceof SubmoduleElementEx)
            child = new SubmoduleEditPart();
        else if (model instanceof ConnectionElementEx)
            child = new ModuleConnectionEditPart();
        else if (model instanceof INedTypeElement)
            child = new NedTypeEditPart();
        else {
            Assert.isTrue(false,"Unknown model element: "+model.getClass().getName());
            return null;
        }
        child.setModel(model);
        return child;
    }

}
