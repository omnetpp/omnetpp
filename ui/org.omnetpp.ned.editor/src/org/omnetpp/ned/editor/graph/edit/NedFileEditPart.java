package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.CompoundSnapToHelper;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.SnapToHelper;
import org.omnetpp.figures.NedFileFigure;
import org.omnetpp.ned.editor.graph.edit.policies.NedFileLayoutEditPolicy;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Represents the NED file in the graphical editor.
 *
 * @author rhornig
 */
public class NedFileEditPart extends NedEditPart {

    /**
     * Installs EditPolicies specific to this.
     */
    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();

        // NedFile cannot be connected to anything via connections
        installEditPolicy(EditPolicy.NODE_ROLE, null);
        //
        installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, null);
        // nedfile cannot be selected
        installEditPolicy(EditPolicy.SELECTION_FEEDBACK_ROLE, null);
        // this is a root edit part, so it cannot be deleted
        installEditPolicy(EditPolicy.COMPONENT_ROLE, null);
        // install a layout edit policy, this one provides also the creation commands
        installEditPolicy(EditPolicy.LAYOUT_ROLE, new NedFileLayoutEditPolicy());
        // no direct editing is possible on the toplevel model element
        installEditPolicy(EditPolicy.DIRECT_EDIT_ROLE, null);
    }

    /**
     * Creates and returns a new figure to represent this editpart.
     */
    @Override
    protected IFigure createFigure() {
        return new NedFileFigure();
    }

    /**
     * @see org.eclipse.core.runtime.IAdaptable#getAdapter(java.lang.Class)
     */
    @SuppressWarnings("unchecked")
	@Override
    public Object getAdapter(Class adapter) {
        if (adapter == SnapToHelper.class) {
            List snapStrategies = new ArrayList();
            Boolean val = (Boolean) getViewer().getProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED);
            if (val != null && val.booleanValue()) snapStrategies.add(new SnapToGeometry(this));

            if (snapStrategies.size() == 0) return null;
            if (snapStrategies.size() == 1) return snapStrategies.get(0);

            SnapToHelper ss[] = new SnapToHelper[snapStrategies.size()];
            for (int i = 0; i < snapStrategies.size(); i++)
                ss[i] = (SnapToHelper) snapStrategies.get(i);
            return new CompoundSnapToHelper(ss);
        }
        return super.getAdapter(adapter);
    }

    @Override
    protected void refreshVisuals() {
    	super.refreshVisuals();
    	((NedFileFigure)getFigure()).setProblemMessage("Gebasz van!");
    }
    
    @Override
    protected List<INedTypeElement> getModelChildren() {
    	return ((NedFileElementEx)getNEDModel()).getTopLevelTypeNodes();
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.NedEditPart#getTypeNameForDblClickOpen()
     * do not open anything on double click
     */
    @Override
    protected INEDElement getNEDElementToOpen() {
        return null;
    }
}
