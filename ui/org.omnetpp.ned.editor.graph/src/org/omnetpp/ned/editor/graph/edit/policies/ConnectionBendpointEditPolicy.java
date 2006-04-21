package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.BendpointRequest;
import org.omnetpp.ned.editor.graph.model.commands.BendpointCommand;
import org.omnetpp.ned.editor.graph.model.commands.CreateBendpointCommand;
import org.omnetpp.ned.editor.graph.model.commands.DeleteBendpointCommand;
import org.omnetpp.ned.editor.graph.model.commands.MoveBendpointCommand;
import org.omnetpp.ned2.model.ConnectionNodeEx;

public class ConnectionBendpointEditPolicy extends org.eclipse.gef.editpolicies.BendpointEditPolicy {

    @Override
    protected Command getCreateBendpointCommand(BendpointRequest request) {
        CreateBendpointCommand com = new CreateBendpointCommand();
        Point p = request.getLocation();
        Connection conn = getConnection();

        conn.translateToRelative(p);

        com.setLocation(p);
        Point ref1 = getConnection().getSourceAnchor().getReferencePoint();
        Point ref2 = getConnection().getTargetAnchor().getReferencePoint();

        conn.translateToRelative(ref1);
        conn.translateToRelative(ref2);

        com.setRelativeDimensions(p.getDifference(ref1), p.getDifference(ref2));
        com.setWire((ConnectionNodeEx) request.getSource().getModel());
        com.setIndex(request.getIndex());
        return com;
    }

    @Override
    protected Command getMoveBendpointCommand(BendpointRequest request) {
        MoveBendpointCommand com = new MoveBendpointCommand();
        Point p = request.getLocation();
        Connection conn = getConnection();

        conn.translateToRelative(p);

        com.setLocation(p);

        Point ref1 = getConnection().getSourceAnchor().getReferencePoint();
        Point ref2 = getConnection().getTargetAnchor().getReferencePoint();

        conn.translateToRelative(ref1);
        conn.translateToRelative(ref2);

        com.setRelativeDimensions(p.getDifference(ref1), p.getDifference(ref2));
        com.setWire((ConnectionNodeEx) request.getSource().getModel());
        com.setIndex(request.getIndex());
        return com;
    }

    @Override
    protected Command getDeleteBendpointCommand(BendpointRequest request) {
        BendpointCommand com = new DeleteBendpointCommand();
        Point p = request.getLocation();
        com.setLocation(p);
        com.setWire((ConnectionNodeEx) request.getSource().getModel());
        com.setIndex(request.getIndex());
        return com;
    }

}