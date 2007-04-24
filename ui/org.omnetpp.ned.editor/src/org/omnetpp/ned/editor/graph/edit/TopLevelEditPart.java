package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.figures.TopLevelFigure;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.IHasAncestors;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;

/**
 * @author rhornig
 * Controller object for toplevel elements in the ned file like:
 * SimpleModule, Channel, CHannelInterface and interface.
 * (NOTE: compound module has it's own controller)
 */
public class TopLevelEditPart extends BaseEditPart {

    /**
     * Returns the model associated with this as a NEDElement.
     * 
     * @return The model of this as a NedElement.
     */
    @Override
	protected IFigure createFigure() {
		return new TopLevelFigure();
	}

	@Override
	protected void refreshVisuals() {
        super.refreshVisuals();
        // define the properties that determine the visual appearence
    	if (getModel() instanceof IHasName) {
    		// set module name and vector size
    		String nameToDisplay = ((IHasName)getModel()).getName();
            
            if (getModel() instanceof ChannelInterfaceNode)
                nameToDisplay = "Channel interface: " + nameToDisplay;
            else if (getModel() instanceof ChannelNode)
                nameToDisplay = "Channel: " + nameToDisplay;
            else if (getModel() instanceof ModuleInterfaceNode)
                nameToDisplay = "Interface: " + nameToDisplay;

            ((TopLevelFigure)getFigure()).setText(nameToDisplay);
    	}

    	// parse a dispaly string, so it's easier to get values from it.
    	// for other visual properties
    	if (getModel() instanceof IHasDisplayString) {
    		DisplayString dps = ((IHasDisplayString)getModel()).getEffectiveDisplayString();
            
    		((TopLevelFigure)getFigure()).setDisplayString(dps);

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
                ((TopLevelFigure)getFigure()).setIcon(image);
            
    	}
        
	}
    
    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.BaseEditPart#getTypeNameForDblClickOpen()
     * open the first base component for double click
     */
    @Override
    protected NEDElement getNEDElementToOpen() {
        if (getModel() instanceof IHasAncestors)
            return ((IHasAncestors)getNEDModel()).getFirstExtendsRef();
        
        return null;
    }
}
