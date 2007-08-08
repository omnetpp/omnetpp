package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.CompoundSnapToHelper;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.SnapToHelper;
import org.eclipse.swt.widgets.Display;

import org.omnetpp.figures.NedFileFigure;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.edit.policies.NedFileLayoutEditPolicy;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileNodeEx;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.notification.NEDModelEvent;

/**
 * Holds all other NedEditParts under this. It is activated by
 * LogicEditorPart, to hold the entire model. It is sort of a blank board where
 * all other EditParts get added.
 *
 * @author rhornig
 */
public class NedFileEditPart extends BaseEditPart  implements INEDChangeListener {

    /**
     * Installs EditPolicies specific to this.
     */
    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();

        // NedFile cannot be connectoed to anything via connections
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

    @Override
    public void activate() {
        if (isActive()) return;
        super.activate();
        // attache to the resources plugin so we will be notified about any change in the model
        NEDResourcesPlugin.getNEDResources().getNEDModelChangeListenerList().add(this);
    }

    @Override
    public void deactivate() {
        if (!isActive()) return;
        NEDResourcesPlugin.getNEDResources().getNEDModelChangeListenerList().remove(this);
        super.deactivate();
    }
    /**
     * Returns a Figure to represent this.
     *
     * @return Figure.
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
    protected List getModelChildren() {
    	return ((NedFileNodeEx)getNEDModel()).getTopLevelElements();
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.BaseEditPart#getTypeNameForDblClickOpen()
     * do not open anything on double click
     */
    @Override
    protected INEDElement getNEDElementToOpen() {
        return null;
    }

    public void modelChanged(NEDModelEvent event) {
        // we do a full refresh in response of a change
        // if we are in a background thread, refresh later when UI thread is active
        if (Display.getCurrent() == null)
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    totalRefresh();
                }
            });
        else // refresh in the current UI thread
            totalRefresh();
    }
}
