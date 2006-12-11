package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.gef.CompoundSnapToHelper;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.SnapToHelper;
import org.omnetpp.figures.NedFileFigure;
import org.omnetpp.ned.editor.graph.edit.policies.NedFileLayoutEditPolicy;
import org.omnetpp.ned.model.ex.NedFileNodeEx;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.resources.NEDResources;
import org.omnetpp.resources.NEDResourcesPlugin;

/**
 * Holds all other NedEditParts under this. It is activated by
 * LogicEditorPart, to hold the entire model. It is sort of a blank board where
 * all other EditParts get added.
 */
public class NedFileEditPart extends BaseEditPart  {

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
//        installEditPolicy(EditPolicy.COMPONENT_ROLE, new RootComponentEditPolicy());
        // install a layout edit policy, this one provides also the creation commands
        installEditPolicy(EditPolicy.LAYOUT_ROLE, new NedFileLayoutEditPolicy());
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

    @Override
    public void modelChanged(NEDModelEvent event) {
        if (lastEventSerial >= event.getSerial())
            return;
        else // process the even and remeber this serial
            lastEventSerial = event.getSerial();

        // logging etc.
        super.modelChanged(event);
        
        NEDResourcesPlugin.getNEDResources().modelChanged(event);

        // invalidate the type info cache and redraw the children
        if (NEDResources.inheritanceMayHaveChanged(event))
            totalRefresh();
    }
    
    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.BaseEditPart#getTypeNameForDblClickOpen()
     * do not open anything on double click 
     */
    @Override
    protected String getTypeNameForDblClickOpen() {
        return null;
    }
}    
