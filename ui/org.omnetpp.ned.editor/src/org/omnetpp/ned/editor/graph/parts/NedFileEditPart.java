/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;
import org.omnetpp.ned.editor.graph.figures.NedFileFigure;
import org.omnetpp.ned.editor.graph.parts.policies.NedTypeContainerLayoutEditPolicy;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Represents the NED file in the graphical editor.
 *
 * @author rhornig
 */
public class NedFileEditPart extends NedEditPart {

    /**
     * Installs EditPolicies specific to this.
     */
    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();

        // this is a root edit part, so it cannot be deleted
        installEditPolicy(EditPolicy.COMPONENT_ROLE, null);
        // install a layout edit policy, this one provides also the creation commands
        // for the children (i.e. for top level types)
        installEditPolicy(EditPolicy.LAYOUT_ROLE, new NedTypeContainerLayoutEditPolicy());
        // no direct editing is possible on the toplevel model element
        installEditPolicy(EditPolicy.DIRECT_EDIT_ROLE, null);
    }

    @Override
    public boolean isEditable() {
        NedFileElementEx nedFileElement = getModel();
        return super.isEditable() && !nedFileElement.isReadOnly() && !nedFileElement.hasSyntaxError();
    }

    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();
        getFigure().setProblemMessage(!getModel().hasSyntaxError() ? null : "Syntax error, the graphical editor content may be outdated and invalid. Please fix the syntax errors first.");
        getFigure().setPackageName(getModel().getPackage());
    }

    @Override
    protected List<INedTypeElement> getModelChildren() {
        return getModel().getTopLevelTypeNodes();
    }

    @Override
    public INedTypeElement getNedTypeElementToOpen() {
        return null;
    }

    @Override
    public NedFileElementEx getModel() {
        return (NedFileElementEx)super.getModel();
    }

    @Override
    protected IFigure createFigure() {
        return new NedFileFigure();
    }

    @Override
    public NedFileFigure getFigure() {
        return (NedFileFigure)super.getFigure();
    }

}
