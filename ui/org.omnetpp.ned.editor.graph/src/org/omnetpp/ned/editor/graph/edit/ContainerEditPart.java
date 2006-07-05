package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.IDisplayStringChangeListener;
import org.omnetpp.ned2.model.IDisplayStringProvider;
import org.omnetpp.ned2.model.INEDChangeListener;
import org.omnetpp.ned2.model.NEDElement;

/**
 * Provides support for Container EditParts.
 */
abstract public class ContainerEditPart 
   extends AbstractGraphicalEditPart implements INEDChangeListener, IDisplayStringChangeListener  {

    @Override
    public void activate() {
        if (isActive()) return;
        super.activate();
        // register as listener of the model object
        getNEDModel().addListener(this);
        // register to the given node's display string as a listener
        if (getNEDModel() instanceof IDisplayStringProvider)
        	((IDisplayStringProvider)getNEDModel()).getDisplayString().setChangeListener(this);
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
        // unregister from the model's display string
        if (getNEDModel() instanceof IDisplayStringProvider)
        	((IDisplayStringProvider)getNEDModel()).getDisplayString().setChangeListener(null);
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

    public void propertyChanged(Prop changedProp) {
		// by default refresh all visuals if the display string has changed
        refreshVisuals();
	}

	public void attributeChanged(NEDElement node, String attr) {
		// refresh only if a node attribute changed. Child changes are discarded
		// FIXME chack wheter this works correctly if we display the vector size too
		if (node == getModel()) 
			refreshVisuals();
	}
    
	public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
		// TODO maybe addChild would be a better idea (faster)
//		System.out.println("childInserted on "+this+": (node="+node+", where="+where+", child="+child+")");
		refreshChildren();
	}

	public void childRemoved(NEDElement node, NEDElement child) {
		// TODO maybe removeChild would be a better idea (faster)
//		System.out.println("childRemoved on "+this+": (node="+node+", child="+child+")");
		refreshChildren();
	}
}
