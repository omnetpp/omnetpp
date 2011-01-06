/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import org.eclipse.draw2d.IFigure;
import org.omnetpp.figures.NedTypeFigure;
import org.omnetpp.ned.editor.graph.figures.NedTypeFigureEx;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ChannelElement;
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
        renameValidator = new TypeNameValidator(getNedTypeModel());
    }

    @Override
	protected IFigure createFigure() {
		return new NedTypeFigureEx();
	}

    public NedTypeFigure getNedTypeFigure() {
        return (NedTypeFigure)getFigure();
    }

    public INedTypeElement getNedTypeModel() {
        return (INedTypeElement) getNedModel();
    }

	@Override
	protected void refreshVisuals() {
        super.refreshVisuals();

        // set name
        String nameToDisplay = ((IHasName)getModel()).getName();
        if (getModel() instanceof ChannelInterfaceElement)
            nameToDisplay += " (channel interface)";
        else if (getModel() instanceof ChannelElement)
            nameToDisplay += " (channel)";
        else if (getModel() instanceof ModuleInterfaceElement)
            nameToDisplay += " (module interface)";

        getNedTypeFigure().setName(nameToDisplay);

    	// update visual properties
        DisplayString displayString = getNedTypeModel().getDisplayString();
        getNedTypeFigure().setDisplayString(displayString);
	}

	
    @Override
    protected INedElement getNedElementToOpen() {
        return getNedTypeModel().getSuperType(); 
    }
}
