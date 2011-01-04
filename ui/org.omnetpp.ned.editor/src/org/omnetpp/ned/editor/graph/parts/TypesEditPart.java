/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.omnetpp.ned.editor.graph.parts.policies.NedTypeContainerLayoutEditPolicy;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INedModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Inner types in a compound module are displayed inside a TypesEditpart. It relates directly
 * to TypesElement
 * @author rhornig
 */
public class TypesEditPart extends AbstractGraphicalEditPart implements INedModelProvider {

    /**
     * Used at the left inner types compartment.
     */
    public static class InnerTypesBorder extends MarginBorder {
        InnerTypesBorder(int t, int l, int b, int r) {
            super(t, l, b, r);
        }

        @Override
        public void paint(IFigure f, Graphics g, Insets i) {
            Rectangle r = getPaintRectangle(f, i);
            g.setForegroundColor(ColorConstants.buttonDarker);
            int x = r.x + insets.left/4;
            g.drawLine(x, r.y, x, r.bottom());
        }
    }

    @Override
    protected IFigure createFigure() {
        // create the container for the inner types
        IFigure innerTypeContainer = new Figure();
        ToolbarLayout typesLayout = new ToolbarLayout();
        typesLayout.setStretchMinorAxis(false);
        typesLayout.setSpacing(5);
        innerTypeContainer.setBorder(new InnerTypesBorder(0, 20, 0, 0 ));
        innerTypeContainer.setLayoutManager(typesLayout);
        return innerTypeContainer;
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

    public INedElement getNedModel() {
        return (INedElement)getModel();
    }

    @Override
    protected List<INedTypeElement> getModelChildren() {
        return ((CompoundModuleElementEx)getNedModel().getParent()).getOwnInnerTypes();
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
