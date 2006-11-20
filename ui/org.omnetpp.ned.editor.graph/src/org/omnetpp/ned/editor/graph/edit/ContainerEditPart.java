package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.notification.INEDChangeListener;
import org.omnetpp.ned2.model.notification.NEDModelEvent;

/**
 * Provides support for Container EditParts.
 */
abstract public class ContainerEditPart 
   extends AbstractGraphicalEditPart implements INEDChangeListener {

    protected long lastEventSerial;
    
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
     * Refreshes all visuals and connection models for ALL children (delegates to the children)
     */
    protected void refreshChildrenVisuals() {
    	for(Object child : getChildren())
    		((AbstractGraphicalEditPart)child).refresh();
    }
    
    
    protected void refreshChildrenConnections() {
        for(Object child : getChildren()) {
            for(Object conn : ((AbstractGraphicalEditPart)child).getSourceConnections())
                ((AbstractGraphicalEditPart)conn).refresh();
            
            for(Object conn : ((AbstractGraphicalEditPart)child).getTargetConnections())
                ((AbstractGraphicalEditPart)conn).refresh();
        }
    }
    
    /**
     * Refreshes everything in this controller. Visual appearence, children and connection list
     * and children and connection appearence too.
     */
    protected void totalRefresh() {
        // refresh ourselves
        refresh();
        // delegate to all children and refresh all their appearence
        for(Object child : getChildren())
            if (child instanceof ContainerEditPart)
                ((ContainerEditPart)child).totalRefresh();
            else
                ((AbstractGraphicalEditPart)child).refresh();
        
        // refresh connections
        refreshChildrenConnections();
    }

    public void modelChanged(NEDModelEvent event) {
        String nameString = getNEDModel().getAttribute("name");
        if (nameString == null) 
            nameString = "";
        System.out.println("NOTIFY ON: "+getModel().getClass().getSimpleName()+" "+nameString+" "+event);
    }
}
