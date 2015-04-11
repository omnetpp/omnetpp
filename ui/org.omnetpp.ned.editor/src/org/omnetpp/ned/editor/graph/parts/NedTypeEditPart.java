/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import org.eclipse.core.resources.IProject;
import org.eclipse.draw2d.IFigure;
import org.omnetpp.ned.editor.graph.figures.ConnectionTypeFigure;
import org.omnetpp.ned.editor.graph.figures.ModuleTypeFigure;
import org.omnetpp.ned.editor.graph.figures.NedTypeFigure;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.interfaces.IChannelKindTypeElement;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;

/**
 * Controller object for toplevel elements in the NED file, like:
 * simple module, module interface, channel, channel interface,
 * EXCEPT compound modules. Compound modules are handled specially,
 * and they rather share a base class with submodules (this makes
 * connection handling easier).
 *
 * @author rhornig, andras (cleanup)
 */
public class NedTypeEditPart extends NedEditPart {

    @Override
    public void activate() {
        super.activate();
        renameValidator = new TypeNameValidator(getModel());
    }

    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();

        // set name
        String nameToDisplay = getModel().getName();
        if (getModel() instanceof ChannelInterfaceElement)
            nameToDisplay += " (interface)";
        else if (getModel() instanceof ModuleInterfaceElement)
            nameToDisplay += " (interface)";

        getFigure().setName(nameToDisplay);

        // update visual properties
        getFigure().setInterface(getModel() instanceof IInterfaceTypeElement);
        getFigure().setInnerType(getModel().getEnclosingTypeElement() != null);
        DisplayString displayString = getModel().getDisplayString();
        IProject project = getModel().getNedTypeInfo().getProject();
        getFigure().setDisplayString(displayString, project);
    }

    @Override
    public INedTypeElement getNedTypeElementToOpen() {
        return getModel().getSuperType();
    }

    @Override
    protected IFigure createFigure() {
        if (getModel() instanceof IChannelKindTypeElement)
            return new ConnectionTypeFigure();
        else
            return new ModuleTypeFigure();
    }

    @Override
    public NedTypeFigure getFigure() {
        return (NedTypeFigure)super.getFigure();
    }

    @Override
    public INedTypeElement getModel() {
        return (INedTypeElement)super.getModel();
    }

}
