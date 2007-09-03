package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.NodeEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.requests.DropRequest;

import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.ned.editor.graph.edit.policies.NedNodeEditPolicy;
import org.omnetpp.ned.model.ex.ConnectionElementEx;

/**
 * Base abstract controller for NedModel and NedFigures. Provides support for
 * connection handling and common display attributes.
 *
 * @author rhornig
 */
abstract public class ModuleEditPart extends NedEditPart implements NodeEditPart {

    @Override
	protected void createEditPolicies() {
		super.createEditPolicies();
		installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, new NedNodeEditPolicy());
	}


	/**
	 * Compute the source connection anchor to be assigned based on the current mouse
	 * location and available gates.
	 * @param p current mouse coordinates
	 * @return The selected connection anchor
	 */
	public abstract ConnectionAnchor getConnectionAnchorAt(Point p);

	/**
	 * Returns a connection anchor registered for the given gate
	 */
	public abstract GateAnchor getConnectionAnchor(String gate);

	public ConnectionAnchor getSourceConnectionAnchor(ConnectionEditPart connEditPart) {
		ConnectionElementEx conn = (ConnectionElementEx) connEditPart.getModel();
		return getConnectionAnchor(conn.getSrcGateWithIndex());
	}

	public ConnectionAnchor getSourceConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest) request).getLocation());
		return getConnectionAnchorAt(pt);
	}

	public ConnectionAnchor getTargetConnectionAnchor(ConnectionEditPart connEditPart) {
		ConnectionElementEx conn = (ConnectionElementEx) connEditPart.getModel();
		return getConnectionAnchor(conn.getDestGateWithIndex());
	}

	public ConnectionAnchor getTargetConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest) request).getLocation());
		return getConnectionAnchorAt(pt);
	}

    /**
     * Returns the scale factor of the module
     */
    public abstract float getScale();

    /**
     * Returns the compound module itself, or the compound module editpart
     * which contains this editpart
     */
    public abstract CompoundModuleEditPart getCompoundModulePart();

    /**
     * Re-implemented because the original implementation has a global (per viewer) MAP
     * to store MODEL - PART associations. This is a problem if we want to display a compound module which
     * inherits some submodules and connections from an other one (that is also displayed in this viewer)
     * In those case the original implementation would not create a new PART for the connection in the
     * derived module but would return the editpart from the base module (which is of course wrong)
     * and leads to very strange bugs.
     */
    @Override
    protected ConnectionEditPart createOrFindConnection(Object model) {
        // get the model - controller cache from the containing compound module
        ConnectionEditPart conx = getCompoundModulePart().getModelToConnectionPartsRegistry().get(model);
        if (conx != null)
            return conx;
        return createConnection(model);
    }

}
