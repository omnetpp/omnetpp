/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.omnetpp.ned.editor.graph.figures.CompoundModuleTypeFigure;
import org.omnetpp.ned.editor.graph.parts.policies.NedTypeContainerLayoutEditPolicy;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INedModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Inner types in a compound module are displayed inside a TypesEditpart. It relates directly
 * to TypesElement
 * @author rhornig
 */
public class TypesEditPart extends AbstractGraphicalEditPart implements INedModelProvider {

    @Override
    protected IFigure createFigure() {
        return new CompoundModuleTypeFigure.InnerTypesFigure();
    }

    @Override
    public void addNotify() {
        Assert.isTrue(getParent() instanceof CompoundModuleEditPart, "Types editpart must be the child of a CompoundModuleEditPart");
        super.addNotify();
    }

    @Override
    protected void createEditPolicies() {
        // install a layout edit policy, this one provides also the creation commands
        installEditPolicy(EditPolicy.LAYOUT_ROLE, new NedTypeContainerLayoutEditPolicy());
    }

    public TypesElement getModel() {
        return (TypesElement)super.getModel();
    }

    @Override
    protected List<INedTypeElement> getModelChildren() {
        return ((CompoundModuleElementEx)getModel().getParent()).getOwnInnerTypes();
    }

    @Override
    public void refresh() {
        super.refresh();
        // refresh all children too
        for (Object child : getChildren())
            ((AbstractGraphicalEditPart)child).refresh();
    }

    @Override
    public boolean isSelectable() {
        return false;
    }
}
