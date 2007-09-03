package org.omnetpp.ned.editor.graph.edit;


import org.eclipse.core.resources.IMarker;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.tools.DirectEditManager;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.ui.views.properties.IPropertySource;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.NedFigure;
import org.omnetpp.figures.misc.IDirectEditSupport;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedDirectEditPolicy;
import org.omnetpp.ned.editor.graph.misc.RenameDirectEditManager;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IModelProvider;

/**
 * Provides support for Container EditParts.
 *
 * @author rhornig
 */
abstract public class NedEditPart
	extends AbstractGraphicalEditPart
	implements IReadOnlySupport, IPropertySourceSupport, IModelProvider
{
    private boolean editable = true;
    private IPropertySource propertySource;
    protected DirectEditManager manager;
    protected ICellEditorValidator renameValidator;

    /**
     * Installs the desired EditPolicies for this.
     */
    @Override
    protected void createEditPolicies() {
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
        installEditPolicy(EditPolicy.DIRECT_EDIT_ROLE, new NedDirectEditPolicy());
    }

    /**
     * Returns the model associated with this as a INEDElement.
     */
    public INEDElement getNEDModel() {
        return (INEDElement) getModel();
    }

    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();

        // indicate the error
        int maxSeverity = getNEDModel().getMaxProblemSeverity();
        String message = "";
        if (maxSeverity >= IMarker.SEVERITY_INFO) {
            IMarker[] markers = NEDResourcesPlugin.getNEDResources().getMarkersForElement(getNEDModel());
            for (IMarker marker : markers)
                message += marker.getAttribute(IMarker.MESSAGE , "")+"\n";
        }
        if (getFigure() instanceof NedFigure)
            ((NedFigure)getFigure()).setProblemDecoration(maxSeverity, StringUtils.strip(message));
    }

    @Override
    public void refresh() {
    	super.refresh();
    	for (Object child : getChildren())
    		((AbstractGraphicalEditPart)child).refresh();
    	for (Object child : getSourceConnections())
    		((AbstractGraphicalEditPart)child).refresh();
    	for (Object child : getTargetConnections())
    		((AbstractGraphicalEditPart)child).refresh();
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

    /**
     * Validates the received command. If the edit part state does not allow modification, it should
     * return UnexecutableCommand, otherwise it must return the received command.
     * Derived parts can override to refine the validation based on command type and edit part state.
     * By default all commands are disabled if the edit part is not editable.
     */
    protected Command validateCommand(Command command) {
        return isEditable() ? command : UnexecutableCommand.INSTANCE;
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.editparts.AbstractEditPart#getCommand(org.eclipse.gef.Request)
     * Overridden so we have a chance to disable the command if the editor is read only.
     */
    @Override
    public Command getCommand(Request request) {
        return validateCommand(super.getCommand(request));
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
        // let's open or activate a new editor if someone has double clicked the component
        if (RequestConstants.REQ_OPEN.equals(req.getType())) {
            INEDElement elementToOpen= getNEDElementToOpen();
            if (elementToOpen != null)
                NEDResourcesPlugin.openNEDElementInEditor(elementToOpen);
        }
    }

    /**
     * Performs a direct edit operation on the part
     */
    protected void performDirectEdit() {
        if (manager == null && (getFigure() instanceof IDirectEditSupport)
                && getNEDModel() instanceof IHasName && isEditable()) {
            IDirectEditSupport deSupport = (IDirectEditSupport)getFigure();
            manager = new RenameDirectEditManager(this, TextCellEditor.class, renameValidator, deSupport);
        }
        if (manager != null)
            manager.show();
    }

    public IPropertySource getPropertySource() {
        return propertySource;
    }

    public void setPropertySource(IPropertySource propertySource) {
        this.propertySource = propertySource;
    }
    /**
     * Returns the type name that must be opened if the user double clicks the module
     */
    protected abstract INEDElement getNEDElementToOpen();
}
