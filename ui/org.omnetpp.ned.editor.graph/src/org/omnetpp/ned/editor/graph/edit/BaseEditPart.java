package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.misc.ISelectionSupport;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned2.model.notification.INEDChangeListener;
import org.omnetpp.ned2.model.notification.NEDModelEvent;

/**
 * Provides support for Container EditParts.
 */
abstract public class BaseEditPart 
   extends AbstractGraphicalEditPart implements INEDChangeListener, IReadOnlySupport {

    protected long lastEventSerial;
    private boolean editable = true;
    
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
            if (child instanceof BaseEditPart)
                ((BaseEditPart)child).totalRefresh();
            else
                ((AbstractGraphicalEditPart)child).refresh();
        
        // refresh connections
        refreshChildrenConnections();
    }

    public void setEditable(boolean editable) {
        this.editable = editable;
    }
    
    public boolean isEditable() {
        if (!editable)
            return false;
        // otherwise check what about the parent. if parent is read only we should return its state 
        if (getParent() instanceof IReadOnlySupport)
            return ((IReadOnlySupport)getParent()).isEditable();
        // otherwise edit is possible 
        return true;
    }

    public void modelChanged(NEDModelEvent event) {
        String nameString = getNEDModel().getAttribute("name");
        if (nameString == null) 
            nameString = "";
        System.out.println("NOTIFY ON: "+getModel().getClass().getSimpleName()+" "+nameString+" "+event);
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.editparts.AbstractEditPart#performRequest(org.eclipse.gef.Request)
     * Open the base type after double clicking (if any)
     */
    @Override
    public void performRequest(Request req) {
        super.performRequest(req);
        // let's open or activate a new editor if somone has double clicked the component
        if (RequestConstants.REQ_OPEN.equals(req.getType())) {
            openEditor(getNEDModel(), getTypeNameForDblClickOpen());
        }
    }

    /**
     * @param name
     */
    public static void openEditor(NEDElement srcNode, String name) {
        INEDTypeInfo typeInfo = srcNode.getContainerNEDTypeInfo()
                                        .getResolver().getComponent(name);
        if (typeInfo != null) {
            IFile file = typeInfo.getNEDFile();
            IFileEditorInput fileEditorInput = new FileEditorInput(file);

            try {
                IEditorPart editor = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage()
                        .openEditor(fileEditorInput, GraphicalNedEditor.ID, true);

                // select the component so it will be visible in the opened editor
                if (editor instanceof ISelectionSupport)
                    ((ISelectionSupport)editor).selectComponent(typeInfo.getName());

            } catch (PartInitException e) {
                // should not happen
                e.printStackTrace();
                Assert.isTrue(false);
            }
        }
    }
    
    /**
     * @return Should return the type name that must be opened if the user double clicks the module
     */
    protected abstract String getTypeNameForDblClickOpen();
}
