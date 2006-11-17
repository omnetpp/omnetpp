package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.figures.TopLevelFigure;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.IHasName;
import org.omnetpp.ned2.model.notification.INEDChangeListener;
import org.omnetpp.ned2.model.notification.NEDModelEvent;

/**
 * @author rhornig
 * Controller object for toplevel elements in the ned file like:
 * SimpleModule, Channel, CHannelInterface and interface.
 * (NOTE: compound module has it's own controller)
 */
public class TopLevelEditPart extends AbstractGraphicalEditPart 
								  implements INEDChangeListener {

    private long lastEventSerial;

    @Override
    public void activate() {
        if (isActive()) return;
        super.activate();
        // register as listener of the model object
        getNEDModel().getListeners().add(this);
    }

    /**
     * Makes the EditPart insensible to changes in the model by removing itself
     * from the model's list of listeners.
     */
    @Override
    public void deactivate() {
        if (!isActive()) return;
        super.deactivate();
        getNEDModel().getListeners().remove(this);
    }

    /**
     * Installs the desired EditPolicies for this.
     */
    @Override
    protected void createEditPolicies() {
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
    }

    /**
     * Returns the model associated with this as a NEDElement.
     * 
     * @return The model of this as a NedElement.
     */
    protected NEDElement getNEDModel() {
        return (NEDElement) getModel();
    }

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

        String nameString = getNEDModel().getAttribute("name");
        if (nameString == null) 
            nameString = "";
        System.out.println("NOTIFY ON: "+getModel().getClass().getSimpleName()+" "+nameString+" "+event);

        refreshVisuals();
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
    		DisplayString dps = ((IHasDisplayString)getModel()).getDisplayString();

    		((TopLevelFigure)getFigure()).setDisplayString(dps);
    	}
        
	}
}
