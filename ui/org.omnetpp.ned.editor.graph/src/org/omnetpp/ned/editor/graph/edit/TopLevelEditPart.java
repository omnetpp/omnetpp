package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.figures.TopLevelFigure;
import org.omnetpp.ned.model.interfaces.IHasAncestors;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;

/**
 * @author rhornig
 * Controller object for toplevel elements in the ned file like:
 * SimpleModule, Channel, CHannelInterface and interface.
 * (NOTE: compound module has it's own controller)
 */
public class TopLevelEditPart extends BaseEditPart 
								  implements INEDChangeListener {
    /**
     * Installs the desired EditPolicies for this.
     */
//    @Override
//    protected void createEditPolicies() {
//		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
//    }

    /**
     * Returns the model associated with this as a NEDElement.
     * 
     * @return The model of this as a NedElement.
     */
    @Override
	protected IFigure createFigure() {
		return new TopLevelFigure();
	}

    public void modelChanged(NEDModelEvent event) {
        // skip the event processing if te last serial is greater or equal. only newer
        // events should be processed. this prevent the processing of the same event multiple times
        if (lastEventSerial >= event.getSerial())
            return;
        else // process the even and remeber this serial
            lastEventSerial = event.getSerial();

        // forward the event to the type info component
        INEDTypeInfo typeInfo = getNEDModel().getContainerNEDTypeInfo();
        if (typeInfo != null)
            typeInfo.modelChanged(event);
        
        refreshVisuals();
        super.modelChanged(event);
    }

	@Override
	protected void refreshVisuals() {
        super.refreshVisuals();
        // define the properties that determine the visual appearence
    	if (getModel() instanceof IHasName) {
    		// set module name and vector size
    		String nameToDisplay = ((IHasName)getModel()).getName();
    		((TopLevelFigure)getFigure()).setText(nameToDisplay);
    	}

    	// parse a dispaly string, so it's easier to get values from it.
    	// for other visula properties
    	if (getModel() instanceof IHasDisplayString) {
    		DisplayString dps = ((IHasDisplayString)getModel()).getEffectiveDisplayString();

    		((TopLevelFigure)getFigure()).setDisplayString(dps);
    	}
        
	}
    
    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.BaseEditPart#getTypeNameForDblClickOpen()
     * open the first base component for double click
     */
    @Override
    protected String getTypeNameForDblClickOpen() {
        if (getModel() instanceof IHasAncestors)
            return ((IHasAncestors)getNEDModel()).getFirstExtends();
        
        return null;
    }
}
