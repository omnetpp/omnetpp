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
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class CompoundModuleEditPart extends ModuleEditPart {
	
    // stores  the connection model - connection controller mapping for the compound module
    private final Map<Object, ConnectionEditPart> modelToConnectionPartsRegistry = new HashMap<Object, ConnectionEditPart>();

    protected CompoundModuleGateAnchor gateAnchor;

    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.LAYOUT_ROLE,
                          new CompoundModuleLayoutEditPolicy((XYLayout) getContentPane().getLayoutManager()));
        installEditPolicy("Snap Feedback", new SnapFeedbackPolicy());
    }

    /**
     * Creates and returns a new module figure
     */
    @Override
    protected IFigure createFigure() {
        IFigure fig = new CompoundModuleFigure();
        gateAnchor = new CompoundModuleGateAnchor(fig);
        return fig;
    }

    /**
     * Convenience method to return the figure object with the correct type
     */
    protected CompoundModuleFigure getCompoundModuleFigure() {
        return (CompoundModuleFigure) getFigure();
    }

    /**
     * Convenience method to return the model object with the correct type
     */
    public CompoundModuleNodeEx getCompoundModuleModel() {
        return (CompoundModuleNodeEx)getModel();
    }

    @Override
    public IFigure getContentPane() {
        return getCompoundModuleFigure().getContentsPane();
    }

    @Override
    @SuppressWarnings("unchecked")
    public Object getAdapter(Class key) {
        if (key == AutoexposeHelper.class) return new ViewportAutoexposeHelper(this);
        if (key == ExposeHelper.class) return new ViewportExposeHelper(this);
        if (key == MouseWheelHelper.class) return new ViewportMouseWheelHelper(this);
        // snap to grid/guide adaptor
        if (key == SnapToHelper.class) {
            List<SnapToGeometry> snapStrategies = new ArrayList<SnapToGeometry>();
            Boolean val = (Boolean) getViewer().getProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED);
            if (val != null && val.booleanValue()) snapStrategies.add(new SnapToGeometry(this));

            if (snapStrategies.size() == 0) return null;
            if (snapStrategies.size() == 1) return snapStrategies.get(0);

            SnapToHelper ss[] = new SnapToHelper[snapStrategies.size()];
            for (int i = 0; i < snapStrategies.size(); i++)
                ss[i] = snapStrategies.get(i);
            return new CompoundSnapToHelper(ss);
        }

        return super.getAdapter(key);
    }

    @Override
    protected List<SubmoduleNodeEx> getModelChildren() {
        // return all submodule including inherited ones
    	return getCompoundModuleModel().getSubmodules();
    }

    /**
     * Returns a list of connections for which this is the srcModule.
     */
    @Override
    protected List<ConnectionNodeEx> getModelSourceConnections() {
        return getCompoundModuleModel().getSrcConnections();
    }

    /**
     * Returns a list of connections for which this is the destModule.
     */
    @Override
    protected List<ConnectionNodeEx> getModelTargetConnections() {
        return getCompoundModuleModel().getDestConnections();
    }

    /**
     * Updates the visual aspect of this compound module
     */
    @Override
    protected void refreshVisuals() {
        // define the properties that determine the visual appearance
        getCompoundModuleFigure().setName(getCompoundModuleModel().getName());
    	getCompoundModuleFigure().setDisplayString(getCompoundModuleModel().getDisplayString());
        // mark if the model is invalid
        getCompoundModuleFigure().setProblemDecoration(getCompoundModuleModel().getMaxProblemSeverity());
    }

	/**
	 * Returns whether the compound module is selectable (mouse is over the bordering area)
	 * for the selection tool based on the current mouse target coordinates.
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
	@Override
    public ConnectionAnchor getConnectionAnchorAt(Point p) {
        return gateAnchor;
	}

	/**
	 * Returns a connection anchor registered for the given gate
	 */
	@Override
    public GateAnchor getConnectionAnchor(String gate) {
        return gateAnchor;
	}

    /**
     * Returns the current scaling factor of the compound module
     */
    @Override
    public float getScale() {
        return ((CompoundModuleNodeEx)getModel()).getDisplayString().getScale();
    }

    @Override
    public CompoundModuleEditPart getCompoundModulePart() {
        return this;
    }

    /**
     * Returns the MAP that contains the connection model - controller associations
     */
    public Map<Object, ConnectionEditPart> getModelToConnectionPartsRegistry() {
        return modelToConnectionPartsRegistry;
    }

    /* (non-Javadoc)
     * @see org.omnetpp.ned.editor.graph.edit.NedEditPart#getTypeNameForDblClickOpen()
     * open the first base component for double click
     */
    @Override
    protected INEDElement getNEDElementToOpen() {
        return getCompoundModuleModel().getFirstExtendsRef();
    }
}
