package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.eclipse.swt.graphics.Image;

import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.NedTypeFigure;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
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
 * @author rhornig
 */
public class NedTypeEditPart extends NedEditPart {

    @Override
    public void activate() {
        super.activate();
        renameValidator = new TypeNameValidator((IHasName)getNEDModel());
    }

    /**
     * Returns the model associated with this as a INEDElement.
     */
    @Override
	protected IFigure createFigure() {
		return new NedTypeFigure();
	}

    public NedTypeFigure getNedTypeFigure() {
        return (NedTypeFigure)getFigure();
    }

	@Override
	protected void refreshVisuals() {
        super.refreshVisuals();
        // define the properties that determine the visual appearance
    	if (getModel() instanceof IHasName) {
    		// set module name and vector size
    		String nameToDisplay = ((IHasName)getModel()).getName();

            if (getModel() instanceof ChannelInterfaceElement)
                nameToDisplay = nameToDisplay +" (channel interface)";
            else if (getModel() instanceof ChannelElement)
                nameToDisplay = "Channel: " + nameToDisplay;
            else if (getModel() instanceof ModuleInterfaceElement)
                nameToDisplay = nameToDisplay +" (module interface)";

            getNedTypeFigure().setText(nameToDisplay);
    	}

    	// parse a display string, so it's easier to get values from it.
    	// for other visual properties
    	if (getModel() instanceof IHasDisplayString) {
    		DisplayString dps = ((IHasDisplayString)getModel()).getDisplayString();

    		getNedTypeFigure().setDisplayString(dps);

            // set icon names for the channel and module/channel interface figures
            // we use special icons for these types
            Image image = null;
            if (getModel() instanceof ChannelInterfaceElement)
                image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE);
            else if (getModel() instanceof ChannelElement)
                image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);

            if (image != null)
                getNedTypeFigure().setIcon(image);
    	}

    	// indicate the error
        getNedTypeFigure().setProblemDecoration(getNEDModel().getMaxProblemSeverity());

	}

    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.NedEditPart#getTypeNameForDblClickOpen()
     * open the first base component for double click
     */
    @Override
    protected INEDElement getNEDElementToOpen() {
        if (getModel() instanceof INedTypeElement)
            return ((INedTypeElement)getNEDModel()).getFirstExtendsRef();

        return null;
    }
}
