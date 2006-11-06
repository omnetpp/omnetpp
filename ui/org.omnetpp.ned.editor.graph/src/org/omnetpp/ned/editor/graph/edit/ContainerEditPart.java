package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.ex.ConnectionNodeEx;
import org.omnetpp.ned2.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned2.model.interfaces.INEDChangeListener;

/**
 * Provides support for Container EditParts.
 */
abstract public class ContainerEditPart 
   extends AbstractGraphicalEditPart implements INEDChangeListener {

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
    }

    /**
     * Returns the model associated with this as a NEDElement.
     * 
     * @return The model of this as a NedElement.
     */
    protected NEDElement getNEDModel() {
        return (NEDElement) getModel();
    }
    
    /**
     * Refreshes all visuals for ALL children
     */
    protected void refreshChildrenVisuals() {
    	for(Object child : getChildren())
    		((ContainerEditPart)child).refreshVisuals();
    }

    /**
     * Refreshes all connections (source and destination) attached to the submodule children
     */
    protected void refreshChildrenConnections() {
        for(Object child : getChildren()) {
            ((ContainerEditPart)child).refreshSourceConnections();
            ((ContainerEditPart)child).refreshTargetConnections();
        }
    }

    public void attributeChanged(NEDElement node, String attr) {
		System.out.println("attrChange for: "+this+" (node="+node+"attr="+attr+")");
	}
    
	public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
		System.out.println("childInserted for: "+this+": (node="+node+", where="+where+", child="+child+")");
	}

	public void childRemoved(NEDElement node, NEDElement child) {
		System.out.println("childRemoved for: "+this+": (node="+node+", child="+child+")");
	}
}
