package org.omnetpp.ned.editor.graph.edit;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;

import org.omnetpp.ned.editor.graph.edit.policies.NedFileLayoutEditPolicy;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Inner types in a compound module are displayed inside a TypesEditpart. It relates directly
 * to TypesElement
 * @author rhornig
 */
public class TypesEditPart extends AbstractGraphicalEditPart implements IModelProvider {

    @Override
    protected IFigure createFigure() {
        return ((CompoundModuleEditPart)getParent()).getCompoundModuleFigure().getInnerTypeContainer();
    }

    @Override
    public void addNotify() {
        Assert.isTrue(getParent() instanceof CompoundModuleEditPart, "Types editpart must be the child of a CompoundModuleEditPart");
        super.addNotify();
    }

    @Override
    protected void createEditPolicies() {
        // install a layout edit policy, this one provides also the creation commands
        installEditPolicy(EditPolicy.LAYOUT_ROLE, new NedFileLayoutEditPolicy());
    }

    public INEDElement getNEDModel() {
        return (INEDElement)getModel();
    }

    @Override
    protected List<INedTypeElement> getModelChildren() {
        return ((CompoundModuleElementEx)getNEDModel().getParent()).getOwnInnerTypes();
    }


    @Override
    public void refresh() {
        super.refresh();
        // refresh all children too
        for (Object child : getChildren())
            ((AbstractGraphicalEditPart)child).refresh();
    }

}
