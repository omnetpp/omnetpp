package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.TopLevelFigure;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasAncestors;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;

/**
 * Controller object for toplevel elements in the ned file like:
 * SimpleModule, Channel, CHannelInterface and interface.
 * (NOTE: compound module has it's own controller)
 *
 * @author rhornig
 */
public class TopLevelEditPart extends BaseEditPart {

    /**
     * Returns the model associated with this as a INEDElement.
     */
    @Override
	protected IFigure createFigure() {
		return new TopLevelFigure();
	}

    public TopLevelFigure getTopLevelFigure() {
        return (TopLevelFigure)getFigure();
    }

	@Override
	protected void refreshVisuals() {
        super.refreshVisuals();
        // define the properties that determine the visual appearance
    	if (getModel() instanceof IHasName) {
    		// set module name and vector size
    		String nameToDisplay = ((IHasName)getModel()).getName();

            if (getModel() instanceof ChannelInterfaceNode)
                nameToDisplay = "Channel interface: " + nameToDisplay;
            else if (getModel() instanceof ChannelNode)
                nameToDisplay = "Channel: " + nameToDisplay;
            else if (getModel() instanceof ModuleInterfaceNode)
                nameToDisplay = "Interface: " + nameToDisplay;

            getTopLevelFigure().setText(nameToDisplay);
    	}

    	// parse a display string, so it's easier to get values from it.
    	// for other visual properties
    	if (getModel() instanceof IHasDisplayString) {
    		DisplayString dps = ((IHasDisplayString)getModel()).getEffectiveDisplayString();

    		getTopLevelFigure().setDisplayString(dps);

            // set icon names for the channel and module/channel interface figures
            // we use special icons for these types
            Image image = null;
            if (getModel() instanceof ChannelInterfaceNode)
                image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNELINTERFACE);
            else if (getModel() instanceof ChannelNode)
                image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_CHANNEL);
            else if (getModel() instanceof ModuleInterfaceNode)
                image = ImageFactory.getImage(ImageFactory.MODEL_IMAGE_INTERFACE);

            if (image != null)
                getTopLevelFigure().setIcon(image);
    	}
    	// indicate the error
        ((TopLevelFigure)getFigure()).setErrorDecoration(getNEDModel().hasErrorMarkers());

	}

    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.BaseEditPart#getTypeNameForDblClickOpen()
     * open the first base component for double click
     */
    @Override
    protected INEDElement getNEDElementToOpen() {
        if (getModel() instanceof IHasAncestors)
            return ((IHasAncestors)getNEDModel()).getFirstExtendsRef();

        return null;
    }
}
