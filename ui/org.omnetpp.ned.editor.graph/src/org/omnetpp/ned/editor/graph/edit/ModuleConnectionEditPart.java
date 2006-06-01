package org.omnetpp.ned.editor.graph.edit;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ManhattanConnectionRouter;
import org.eclipse.draw2d.RelativeBendpoint;
import org.eclipse.draw2d.RoutingAnimator;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractConnectionEditPart;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.ned.editor.graph.edit.policies.ConnectionBendpointEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.ConnectionEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.ConnectionEndpointEditPolicy;
import org.omnetpp.ned.editor.graph.figures.ConnectionFigure;
import org.omnetpp.ned2.model.ConnectionDisplayString;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.DisplayString;
import org.omnetpp.ned2.model.IDisplayStringChangeListener;
import org.omnetpp.ned2.model.INEDChangeListener;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDElementUtil;
import org.omnetpp.ned2.model.SubmoduleDisplayString;
import org.omnetpp.ned2.model.WireBendpointModel;
import org.omnetpp.ned2.model.DisplayString.Prop;

/**
 * Implements a Connection Editpart to represnt a Wire like connection.
 * 
 */
public class ModuleConnectionEditPart extends AbstractConnectionEditPart implements INEDChangeListener, IDisplayStringChangeListener {

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
        super.activateFigure();
        /*
         * Once the figure has been added to the ConnectionLayer, start
         * listening for its router to change.
         */
        // getFigure().addPropertyChangeListener(Connection.PROPERTY_CONNECTION_ROUTER, this);
    }

    @Override
    public void deactivateFigure() {
        // getFigure().removePropertyChangeListener(Connection.PROPERTY_CONNECTION_ROUTER, this);
        super.deactivateFigure();
    }

    /**
     * Adds extra EditPolicies as required.
     */
    @Override
    protected void createEditPolicies() {
        installEditPolicy(EditPolicy.CONNECTION_ENDPOINTS_ROLE, new ConnectionEndpointEditPolicy());
        // Note that the Connection is already added to the diagram and knows
        // its Router.
        refreshBendpointEditPolicy();
        installEditPolicy(EditPolicy.CONNECTION_ROLE, new ConnectionEditPolicy());
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
     * Updates the bendpoints, based on the model.
     */
    protected void refreshBendpoints() {
        if (getConnectionFigure().getConnectionRouter() instanceof ManhattanConnectionRouter) return;
        List modelConstraint = getConnectionModel().getBendpoints();
        List figureConstraint = new ArrayList();
        for (int i = 0; i < modelConstraint.size(); i++) {
            WireBendpointModel wbp = (WireBendpointModel) modelConstraint.get(i);
            RelativeBendpoint rbp = new RelativeBendpoint(getConnectionFigure());
            rbp.setRelativeDimensions(wbp.getFirstRelativeDimension(), wbp.getSecondRelativeDimension());
            rbp.setWeight((i + 1) / ((float) modelConstraint.size() + 1));
            figureConstraint.add(rbp);
        }
        getConnectionFigure().setRoutingConstraint(figureConstraint);
    }

    private void refreshBendpointEditPolicy() {
        if (getConnectionFigure().getConnectionRouter() instanceof ManhattanConnectionRouter)
            installEditPolicy(EditPolicy.CONNECTION_BENDPOINTS_ROLE, null);
        else
            installEditPolicy(EditPolicy.CONNECTION_BENDPOINTS_ROLE, new ConnectionBendpointEditPolicy());
    }

    /**
     * Refreshes the visual aspects of this, based upon the model (Wire). It
     * changes the wire color depending on the state of Wire.
     * 
     */
    @Override
    protected void refreshVisuals() {
        ConnectionDisplayString dps = (ConnectionDisplayString)getConnectionModel().getDisplayString();

    	refreshBendpoints();
        // XXX do we need this here?
        // refreshBendpointEditPolicy();
        ConnectionFigure cfig = (ConnectionFigure)getFigure();  
        
        cfig.setStyle(ColorFactory.asColor(dps.getAsStringDef(DisplayString.Prop.CONNECTION_COL)), 
        				dps.getAsIntDef(DisplayString.Prop.CONNECTION_WIDTH, 1), 
        				dps.getAsStringDef(DisplayString.Prop.CONNECTION_STYLE), 
        				dps.getAsStringDef(DisplayString.Prop.CONNECTION_SEGMENTS));
        
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
        refreshBendpoints();
        refreshBendpointEditPolicy();
	}
}
