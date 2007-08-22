package org.omnetpp.ned.editor.graph.edit;

import java.util.Arrays;
import java.util.Set;

import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.tools.DirectEditManager;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.ui.views.properties.IPropertySource;

import org.omnetpp.figures.misc.IDirectEditSupport;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedDirectEditPolicy;
import org.omnetpp.ned.editor.graph.misc.RenameDirectEditManager;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDElementConstants;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
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
     * Validator used for checking top level type names. NOTE: Inner types should be validated differently.
     */
    class TypeNameValidator implements ICellEditorValidator {
        public String isValid(Object newText) {
            if (!NEDElementUtilEx.isValidIdentifier((String)newText))
                return "Invalid identifier. Names must begin with a letter or underscore, and may contain letters," +
                " digits or underscore.";
            Set<String> usedNames = NEDResourcesPlugin.getNEDResources().getReservedComponentNames();
            if (usedNames.contains(newText))
                return "Name is already in use. There is already a type with the same name.";
            if (Arrays.asList(NEDElementConstants.RESERVED_NED_KEYWORDS).contains(newText))
                return "Name is a reserved keyword.";

            return null;
        }
    };

    /**
     * Validator used for checking submodule names.
     */
    class SubmoduleNameValidator implements ICellEditorValidator {
        CompoundModuleElementEx compoundModuleModel;

        public SubmoduleNameValidator(CompoundModuleElementEx compoundModuleModel) {
            this.compoundModuleModel = compoundModuleModel;
        }

        public String isValid(Object newText) {
            if (!NEDElementUtilEx.isValidIdentifier((String)newText))
                return "Invalid identifier. Names must begin with a letter or underscore, and may contain letters," +
                		" digits or underscore.";
            if (compoundModuleModel.getNEDTypeInfo().getMembers().containsKey(newText))
                return "Name is already in use. Submodule name must not be the same as an existing" +
                		" submodule, gate, parameter or inner type name.";
            if (Arrays.asList(NEDElementConstants.RESERVED_NED_KEYWORDS).contains(newText))
                return "Name is a reserved keyword.";

            return null;
        }
    };

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
                && getNEDModel() instanceof IHasName) {
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
