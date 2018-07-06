/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;


import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.DragTracker;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editparts.AbstractGraphicalEditPart;
import org.eclipse.gef.tools.DirectEditManager;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.graphics.Cursor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.IProblemDecorationSupport;
import org.omnetpp.figures.ITooltipTextProvider;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.graph.misc.IDirectEditSupport;
import org.omnetpp.ned.editor.graph.misc.NedSharedCursors;
import org.omnetpp.ned.editor.graph.misc.RenameDirectEditManager;
import org.omnetpp.ned.editor.graph.parts.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.parts.policies.NedDirectEditPolicy;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INedModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Provides support for deleting and renaming the components (using direct edit).
 * Sets the error markers and messages as tooltips. Disables all commands if
 * the edit part is read only.
 *
 * @author rhornig
 */
abstract public class NedEditPart extends AbstractGraphicalEditPart implements IReadOnlySupport, INedModelProvider
{
    private boolean editable = true;
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

    public DragTracker getDragTracker(Request request) {
        // this is required only to override the cursors for the tool because the default cursors
        // in GEF have the image data and mask data swapped (a bug in the GEF code)
        // see: https://bugs.eclipse.org/bugs/show_bug.cgi?id=467983
        DragTracker tracker = new org.eclipse.gef.tools.DragEditPartsTracker(this) {
            @Override
            protected Cursor getDefaultCursor() {
                if (isCloneActive())
                    return NedSharedCursors.CURSOR_TREE_ADD;
                return NedSharedCursors.CURSOR_TREE_MOVE;
            }
        };

        return tracker;
    }

    /**
     * Returns the model associated with this as a INedElement.
     */
    @Override
    public INedElement getModel() {
        return (INedElement)super.getModel();
    }

    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();

        if (getFigure() instanceof IProblemDecorationSupport) {
            ITooltipTextProvider textProvider = new ITooltipTextProvider() {
                public String getTooltipText(int x, int y) {
                    String message = "";
                    if (getModel().getMaxProblemSeverity() >= IMarker.SEVERITY_INFO) {
                        IMarker[] markers = NedResourcesPlugin.getNedResources().getMarkersForElement(getModel(), true, 11);
                        int i = 0;
                        for (IMarker marker : markers) {
                            message += marker.getAttribute(IMarker.MESSAGE , "")+"\n";
                            // we allow 10 markers maximum in a single message
                            if (++i > 10) {
                                message += "and some more...\n";
                                break;
                            }
                        }
                    }
                    return StringUtils.strip(message);
                }
            };

            ((IProblemDecorationSupport)getFigure()).setProblemDecoration(getModel().getMaxProblemSeverity(), textProvider);
        }
    }

    @Override
    public void refresh() {
        Assert.isTrue((getModel() instanceof NedFileElementEx) || getModel().getParent() != null,
        "NedElement must be inside a model (must have parent)");
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
        if (RequestConstants.REQ_DIRECT_EDIT.equals(req.getType()) && isEditable())
            performDirectEdit();

        // let's open or activate a new editor if someone has double clicked the component
        if (RequestConstants.REQ_OPEN.equals(req.getType())) {
            INedElement elementToOpen = getNedTypeElementToOpen();
            if (elementToOpen != null)
                NedResourcesPlugin.openNedElementInEditor(elementToOpen);
        }
    }

    /**
     * Performs a direct edit operation on the part
     */
    protected void performDirectEdit() {
        if (manager == null && (getFigure() instanceof IDirectEditSupport)
                && getModel() instanceof IHasName && isEditable()) {
            IDirectEditSupport deSupport = (IDirectEditSupport)getFigure();
            manager = new RenameDirectEditManager(this, TextCellEditor.class, renameValidator, deSupport);
        }
        if (manager != null)
            manager.show();
    }

    /**
     * Returns the type name that must be opened if the user presses F3 (open type/super type)
     */
    public abstract INedTypeElement getNedTypeElementToOpen();


    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class key) {
        // TODO Auto-generated method stub
        return null;
    }
}
