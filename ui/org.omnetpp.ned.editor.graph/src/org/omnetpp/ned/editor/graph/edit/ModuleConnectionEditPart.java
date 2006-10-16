package org.omnetpp.ned.editor.graph.edit;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.RoutingAnimator;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractConnectionEditPart;
import org.omnetpp.common.displaymodel.IDisplayStringChangeListener;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.ned.editor.graph.edit.policies.NedConnectionEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedConnectionEndpointEditPolicy;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.INEDChangeListener;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDElementUtil;

/**
 * Implements a Connection Editpart to represnt a Wire like connection.
 * 
 */
public class ModuleConnectionEditPart extends AbstractConnectionEditPart implements INEDChangeListener, IDisplayStringChangeListener {

	private EditPart sourceEditPart; 
	private EditPart targetEditPart;

	@Override
    public void activate() {
        if (isActive()) return;
        super.activate();
        // register as listener of the model object
        getConnectionModel().addListener(this);
        // register to the given node's display string as a listener
        getConnectionModel().getDisplayString().setChangeListener(this);
    }

    @Override
    public void deactivate() {
        if (!isActive()) return;
        // deregister as listener of the model object
        getConnectionModel().removeListener(this);
        // unregister from the model's display string
        getConnectionModel().getDisplayString().setChangeListener(null);
        super.deactivate();
    }

    @Override
    public void activateFigure() {
    	// add the connection to the compound module's connection layer instead of the global one
    	((CompoundModuleEditPart)getParent()).getCompoundModuleFigure()
    			.addConnectionFigure(getConnectionFigure());
    }

    @Override
    public void deactivateFigure() {
    	// remove the connection figure from the parent
    	getFigure().getParent().remove(getFigure());
    	getConnectionFigure().setSourceAnchor(null);
    	getConnectionFigure().setTargetAnchor(null);
    }

    /**
     * @see org.eclipse.gef.ConnectionEditPart#getSource()
     */
    @Override
    public EditPart getSource() {
    	return sourceEditPart;
    }

    /**
     * @see org.eclipse.gef.ConnectionEditPart#getTarget()
     */
    @Override
    public EditPart getTarget() {
    	return targetEditPart;
    }

    /**
     * Sets the source EditPart of this connection. Overrides the original implementation
     * to add the connection as the child of a compound module
     *
     * @param editPart  EditPart which is the source.
     */
    @Override
    public void setSource(EditPart editPart) {
    	if (sourceEditPart == editPart)
    		return;
    	
    	sourceEditPart = editPart;
    	if (sourceEditPart != null) {
    		// attach the connection edit part to the compound module as a parent
        	if (sourceEditPart instanceof CompoundModuleEditPart)
        		setParent(sourceEditPart);
        	else if (sourceEditPart instanceof SubmoduleEditPart)
        		setParent(sourceEditPart.getParent());
    	} else if (targetEditPart == null)
    		setParent(null);
    	if (sourceEditPart != null && targetEditPart != null)
    		refresh();
    }

    /**
     * Sets the target EditPart of this connection. Overrides the original implementation
     * to add the connection as the child of a compound module
     * @param editPart  EditPart which is the target.
     */
    @Override
    public void setTarget(EditPart editPart) {
    	if (targetEditPart == editPart)
    		return;
    	targetEditPart = editPart;
    	if (targetEditPart != null) {
    		// attach the connection edit part to the compound module as a parent
        	if (targetEditPart instanceof CompoundModuleEditPart)
        		setParent(targetEditPart);
        	else if (targetEditPart instanceof SubmoduleEditPart)
        		setParent(targetEditPart.getParent());
    	} else if (sourceEditPart == null)
    		setParent(null);
    	if (sourceEditPart != null && targetEditPart != null)
    		refresh();
    }

    /**
     * Adds extra EditPolicies as required.
     */
    @Override
    protected void createEditPolicies() {
        installEditPolicy(EditPolicy.CONNECTION_ENDPOINTS_ROLE, new NedConnectionEndpointEditPolicy());
        // Note that the Connection is already added to the diagram and knows router
        installEditPolicy(EditPolicy.CONNECTION_ROLE, new NedConnectionEditPolicy());
    }

    /**
     * Returns a newly created Figure to represent the connection.
     * 
     * @return The created Figure.
     */
    @Override
    protected IFigure createFigure() {
        ConnectionFigure conn = new ConnectionFigure();
        conn.addRoutingListener(RoutingAnimator.getDefault());

        return conn;    
    }

    /**
     * Returns the model associated to this editpart as a ConnectionNodeEx
     * 
     * @return Model of this as <code>Wire</code>
     */
    protected ConnectionNodeEx getConnectionModel() {
        return (ConnectionNodeEx) getModel();
    }

    /**
     * Refreshes the visual aspects of this, based upon the model (Wire). It
     * changes the wire color depending on the state of Wire.
     * 
     */
    @Override
    protected void refreshVisuals() {
        // XXX do we need this here?
        ConnectionFigure cfig = (ConnectionFigure)getConnectionFigure();  
        cfig.setDisplayString(getConnectionModel().getDisplayString());
        cfig.setArrowEnabled(getConnectionModel().getArrowDirection() != NEDElementUtil.NED_ARROWDIR_BIDIR);
    }

	public void attributeChanged(NEDElement node, String attr) {
		if (node == getModel()) 
			refreshVisuals();
	}

	public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
		// DO nothing. we don't care about child addition removal in a connection subtree 
	}

	public void childRemoved(NEDElement node, NEDElement child) {
		// DO nothing. we don't care about child addition removal in a connection subtree 
	}

	public void propertyChanged(Prop changedProp) {
		// connection router changed
		refreshVisuals();
	}
}
