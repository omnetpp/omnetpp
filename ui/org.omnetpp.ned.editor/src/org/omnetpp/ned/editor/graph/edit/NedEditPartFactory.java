package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;

import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Factory to create corresponding controller objects for the model objects.
 * Only model objects explicitly handled here will have a controller and a visual
 * counterpart in the editor
 *
 * @author rhornig
 */
public class NedEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        EditPart child = null;

        if (model == null)
            return null;

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
        else if (model instanceof TypesElement)
            child = new TypesEditPart();
        else
            throw new IllegalArgumentException("Unknown model element: "+model);

        child.setModel(model);
        return child;
    }

}
