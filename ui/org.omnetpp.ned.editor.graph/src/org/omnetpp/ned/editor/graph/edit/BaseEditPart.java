package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Label;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.tools.DirectEditManager;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.figures.IDirectEditSupport;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedDirectEditPolicy;
import org.omnetpp.ned.editor.graph.misc.ISelectionSupport;
import org.omnetpp.ned.editor.graph.misc.RenameDirectEditManager;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;

/**
 * Provides support for Container EditParts.
 */
abstract public class BaseEditPart
                           extends AbstractGraphicalEditPart
                           implements INEDChangeListener, IReadOnlySupport,
                                      IPropertySourceSupport {

    protected long lastEventSerial;
    private boolean editable = true;
    private IPropertySource propertySource;
    protected DirectEditManager manager;

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
        installEditPolicy(EditPolicy.DIRECT_EDIT_ROLE, new NedDirectEditPolicy());
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
        if (RequestConstants.REQ_DIRECT_EDIT.equals(req.getType()))
            performDirectEdit();
        // let's open or activate a new editor if somone has double clicked the component
        if (RequestConstants.REQ_OPEN.equals(req.getType())) {
            openEditor(getNEDModel(), getTypeNameForDblClickOpen());
        }
    }

    /**
     * Performs a direct edit operation on the part
     */
    protected void performDirectEdit() {
        if (manager == null && (getFigure() instanceof IDirectEditSupport)) {
            Label l = ((IDirectEditSupport)getFigure()).getLabel();
            manager = new RenameDirectEditManager(this, TextCellEditor.class,
                              new RenameDirectEditManager.LabelCellEditorLocator(l), l);
        }
        if (manager != null)
            manager.show();
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
                        .openEditor(fileEditorInput, GraphicalNedEditor.MULTIPAGE_NEDEDITOR_ID, true);

                // select the component so it will be visible in the opened editor
                if (editor instanceof ISelectionSupport)
                    ((ISelectionSupport)editor).selectComponent(typeInfo.getName());

            } catch (PartInitException e) {
                // should not happen
                e.printStackTrace();  //FIXME log it or something. Just "print" is not OK!
                Assert.isTrue(false);
            }
        }
    }

    public IPropertySource getPropertySource() {
        return propertySource;
    }

    public void setPropertySource(IPropertySource propertySource) {
        this.propertySource = propertySource;
    }
    /**
     * @return Should return the type name that must be opened if the user double clicks the module
     */
    protected abstract String getTypeNameForDblClickOpen();
}
