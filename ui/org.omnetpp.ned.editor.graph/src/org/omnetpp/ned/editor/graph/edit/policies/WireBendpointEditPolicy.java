/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.geometry.Point;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.BendpointRequest;

import org.omnetpp.ned.editor.graph.model.WireModel;
import org.omnetpp.ned.editor.graph.model.commands.BendpointCommand;
import org.omnetpp.ned.editor.graph.model.commands.CreateBendpointCommand;
import org.omnetpp.ned.editor.graph.model.commands.DeleteBendpointCommand;
import org.omnetpp.ned.editor.graph.model.commands.MoveBendpointCommand;

public class WireBendpointEditPolicy extends org.eclipse.gef.editpolicies.BendpointEditPolicy {

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
        com.setWire((WireModel) request.getSource().getModel());
        com.setIndex(request.getIndex());
        return com;
    }

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
        com.setWire((WireModel) request.getSource().getModel());
        com.setIndex(request.getIndex());
        return com;
    }

    protected Command getDeleteBendpointCommand(BendpointRequest request) {
        BendpointCommand com = new DeleteBendpointCommand();
        Point p = request.getLocation();
        com.setLocation(p);
        com.setWire((WireModel) request.getSource().getModel());
        com.setIndex(request.getIndex());
        return com;
    }

}