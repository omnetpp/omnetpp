package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringProvider;
import org.omnetpp.figures.TopLevelFigure;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.INEDChangeListener;
import org.omnetpp.ned2.model.interfaces.INamed;

/**
 * @author rhornig
 * Controller object for toplevel elements in the ned file like:
 * SimpleModule, Channel, CHannelInterface and interface.
 * (NOTE: compound module has it's own controller)
 */
public class TopLevelEditPart extends AbstractGraphicalEditPart 
								  implements INEDChangeListener {

    @Override
    public void activate() {
        if (isActive()) return;
        super.activate();
        // register as listener of the model object
        getNEDModel().addListener(this);
    }

    /**
     * Makes the EditPart insensible to changes in the model by removing itself
     * from the model's list of listeners.
     */
    @Override
    public void deactivate() {
        if (!isActive()) return;
        super.deactivate();
        getNEDModel().removeListener(this);
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

	public void attributeChanged(NEDElement node, String attr) {
		System.out.println("attrChange for: "+this+" (node="+node+"attr="+attr+")");
		if (node == getModel()) 
			refreshVisuals();
	}

	public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
	}

	public void childRemoved(NEDElement node, NEDElement child) {
	}

	@Override
	protected void refreshVisuals() {
		super.refreshVisuals();
        // define the properties that determine the visual appearence
    	
    	if (getModel() instanceof INamed) {
    		// set module name and vector size
    		String nameToDisplay = ((INamed)getModel()).getName();
    		((TopLevelFigure)getFigure()).setText(nameToDisplay);
    	}

    	// parse a dispaly string, so it's easier to get values from it.
    	// for other visula properties
    	if (getModel() instanceof IDisplayStringProvider) {
    		DisplayString dps = ((IDisplayStringProvider)getModel()).getDisplayString();

    		// TODO compute the inherited display string
//    		if (node.getFirstExtendsChild() != null) {
//    			DisplayString fallbackDps = NEDResourcesPlugin.getNEDResources()
//    			.getEffectiveDisplayString(node.getFirstExtendsChild().getName());
//    			dps.setDefaults(fallbackDps);
//    		}

    		((TopLevelFigure)getFigure()).setDisplayString(dps);
    	}
        
	}
}
