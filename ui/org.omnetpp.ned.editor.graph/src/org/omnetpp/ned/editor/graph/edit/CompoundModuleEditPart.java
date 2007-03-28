package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.AutoexposeHelper;
import org.eclipse.gef.CompoundSnapToHelper;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.ExposeHelper;
import org.eclipse.gef.MouseWheelHelper;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.SnapToHelper;
import org.eclipse.gef.editparts.ViewportAutoexposeHelper;
import org.eclipse.gef.editparts.ViewportExposeHelper;
import org.eclipse.gef.editparts.ViewportMouseWheelHelper;
import org.eclipse.gef.editpolicies.SnapFeedbackPolicy;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.CompoundModuleGateAnchor;
import org.omnetpp.figures.misc.GateAnchor;
import org.omnetpp.ned.editor.graph.edit.policies.CompoundModuleLayoutEditPolicy;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.notification.NEDAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.notification.NEDStructuralChangeEvent;

public class CompoundModuleEditPart extends ModuleEditPart {

    // stores  the connection model - connection controller mapping for the compound module
    private Map<Object, ConnectionEditPart> modelToConnectionPartsRegistry = new HashMap<Object, ConnectionEditPart>();

    protected CompoundModuleGateAnchor gateAnchor;

    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.LAYOUT_ROLE, 
                          new CompoundModuleLayoutEditPolicy((XYLayout) getContentPane().getLayoutManager()));
        installEditPolicy("Snap Feedback", new SnapFeedbackPolicy()); //$NON-NLS-1$
    }

    /**
     * Creates a new Module Figure and returns it.
     * @return Figure representing the module.
     */
    @Override
    protected IFigure createFigure() {
        IFigure fig = new CompoundModuleFigure();
        gateAnchor = new CompoundModuleGateAnchor(fig);
        return fig;
    }

    /**
     * Returns the Figure of this as a ModuleFigure.
     * @return ModuleFigure of this.
     */
    protected CompoundModuleFigure getCompoundModuleFigure() {
        return (CompoundModuleFigure) getFigure();
    }


    /**
     * @return Helper function to return the model object with correct type
     */
    public CompoundModuleNodeEx getCompoundModuleModel() {
        return (CompoundModuleNodeEx)getModel();
    }

    @Override
    public IFigure getContentPane() {
        return getCompoundModuleFigure().getContentsPane();
    }

    @Override
    public Object getAdapter(Class key) {
        if (key == AutoexposeHelper.class) return new ViewportAutoexposeHelper(this);
        if (key == ExposeHelper.class) return new ViewportExposeHelper(this);
        if (key == MouseWheelHelper.class) return new ViewportMouseWheelHelper(this);
        // snap to grig/guide adaptor
        if (key == SnapToHelper.class) {
            List<SnapToGeometry> snapStrategies = new ArrayList<SnapToGeometry>();
            Boolean val = (Boolean) getViewer().getProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED);
            if (val != null && val.booleanValue()) snapStrategies.add(new SnapToGeometry(this));

            if (snapStrategies.size() == 0) return null;
            if (snapStrategies.size() == 1) return snapStrategies.get(0);

            SnapToHelper ss[] = new SnapToHelper[snapStrategies.size()];
            for (int i = 0; i < snapStrategies.size(); i++)
                ss[i] = (SnapToHelper) snapStrategies.get(i);
            return new CompoundSnapToHelper(ss);
        }
        
        return super.getAdapter(key);
    }

    @Override
    protected List getModelChildren() {
        // return all submodule including inherited ones
    	return getCompoundModuleModel().getSubmodules();
    }
    
    /**
     * Returns a list of connections for which this is the srcModule.
     * 
     * @return List of connections.
     */
    @Override
    protected List getModelSourceConnections() {
        return getCompoundModuleModel().getSrcConnections();
    }

    /**
     * Returns a list of connections for which this is the destModule.
     * 
     * @return List of connections.
     */
    @Override
    protected List getModelTargetConnections() {
        return getCompoundModuleModel().getDestConnections();
    }

    @Override
    public void modelChanged(NEDModelEvent event) {
        // skip the event processing if te last serial is greater or equal. only newer
        // events should be processed. this prevent the processing of the same event multiple times
        // prevents circular notification
        if (lastEventSerial >= event.getSerial())
            return;
        else // process the even and remeber this serial
            lastEventSerial = event.getSerial();

        super.modelChanged(event);

        // forward the event to the type info component
        INEDTypeInfo typeInfo = getNEDModel().getContainerNEDTypeInfo();
        if (typeInfo != null)
            typeInfo.modelChanged(event);

        // check for attribute changes
        if (event instanceof NEDAttributeChangeEvent) {
            totalRefresh();
        }

        // check for structural changes
        if (event instanceof NEDStructuralChangeEvent) {
            NEDStructuralChangeEvent structEvent = (NEDStructuralChangeEvent) event;

            // refresh children if a submodule was added/removed
            if (structEvent.getChild() instanceof SubmoduleNodeEx)
                refreshChildren();
            
            // refresh connections if a connection was inserted/deleted
            if (structEvent.getChild() instanceof ConnectionNodeEx) {
                totalRefresh();
            }
        }
        
    }
    
    /**
     * Updates the visual aspect of this compound module
     */
    @Override
    protected void refreshVisuals() {
        // define the properties that determine the visual appearence
        getCompoundModuleFigure().setName(getCompoundModuleModel().getName());
    	getCompoundModuleFigure().setDisplayString(getCompoundModuleModel().getEffectiveDisplayString());
    }

	/**
	 * Returns whether the compound module is selectable (mouse is over the bordering area)
	 * for the slection tool based on the current mouse target coordinates.
	 * @param x
	 * @param y
	 * @return
	 */
	public boolean isOnBorder(int x, int y) {
		return getCompoundModuleFigure().isOnBorder(x, y);
	}
	
	/**
	 * Compute the source connection anchor to be assigned based on the current mouse 
	 * location and available gates. 
	 * @param p current mouse coordinates
	 * @return The selected connection anchor
	 */
	public ConnectionAnchor getConnectionAnchorAt(Point p) {
//		return new CompoundModuleGateAnchor(getFigure());
        return gateAnchor;
	}

	/**
	 * Returns a conn anchor registered for the given gate
	 * @param gate
	 * @return
	 */
	public GateAnchor getConnectionAnchor(String gate) {
//		return new CompoundModuleGateAnchor(getFigure());
        return gateAnchor;
	}
    
    /**
     * @return The current scaling factor of the compound module
     */
    public float getScale() {
        return ((CompoundModuleNodeEx)getModel()).getDisplayString().getScale();
    }

    @Override
    public CompoundModuleEditPart getCompoundModulePart() {
        return this;
    }

    /**
     * @return The MAP that contains the connection model - controller associations 
     */
    public Map<Object, ConnectionEditPart> getModelToConnectionPartsRegistry() {
        return modelToConnectionPartsRegistry;
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.BaseEditPart#getTypeNameForDblClickOpen()
     * open the first base component for double click
     */
    @Override
    protected String getTypeNameForDblClickOpen() {
        return getCompoundModuleModel().getFirstExtends();
    }
}
