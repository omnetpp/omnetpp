package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.NodeEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.requests.DropRequest;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedNodeEditPolicy;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDElementUtil;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.interfaces.IConnectable;
import org.omnetpp.ned2.model.interfaces.INamedGraphNode;
import org.omnetpp.ned2.model.pojo.NEDElementTags;

/**
 * Base abstract controller for NedModel and NedFigures. Provides support for 
 * connection handling and common display attributes.
 */
abstract public class ModuleEditPart extends ContainerEditPart implements NodeEditPart {

	@Override
	protected void createEditPolicies() {
		super.createEditPolicies();
		installEditPolicy(EditPolicy.COMPONENT_ROLE, new NedComponentEditPolicy());
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
	 * Returns a conn anchor registered for the given gate
	 * @param gate
	 * @return
	 */
	public abstract GateAnchor getConnectionAnchor(String gate);

	public ConnectionAnchor getSourceConnectionAnchor(ConnectionEditPart connEditPart) {
		ConnectionNodeEx conn = (ConnectionNodeEx) connEditPart.getModel();
		return getConnectionAnchor(conn.getSrcGateWithIndex());
	}

	public ConnectionAnchor getSourceConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest) request).getLocation());
		return getConnectionAnchorAt(pt);
	}

	public ConnectionAnchor getTargetConnectionAnchor(ConnectionEditPart connEditPart) {
		ConnectionNodeEx conn = (ConnectionNodeEx) connEditPart.getModel();
		return getConnectionAnchor(conn.getDestGateWithIndex());
	}

	public ConnectionAnchor getTargetConnectionAnchor(Request request) {
		Point pt = new Point(((DropRequest) request).getLocation());
		return getConnectionAnchorAt(pt);
	}

    /**
     * @return The scale factor of the module
     */
    public abstract float getScale();
    
    /**
     * @return The compound module itself or the compound module controller 
     *  wich contains this conroller
     */
    public abstract CompoundModuleEditPart getCompoundModulePart();

}
