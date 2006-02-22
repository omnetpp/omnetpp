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
package org.omnetpp.ned.editor.graph.model.commands;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.eclipse.draw2d.geometry.Rectangle;

import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.*;
import org.omnetpp.ned.editor.graph.model.old.CompoundModuleModel;
import org.omnetpp.ned.editor.graph.model.old.ContainerModel;
import org.omnetpp.ned.editor.graph.model.old.NedNodeModel;
import org.omnetpp.ned.editor.graph.model.old.SubmoduleModel;
import org.omnetpp.ned.editor.graph.model.old.WireBendpointModel;
import org.omnetpp.ned.editor.graph.model.old.WireModel;

/**
 * Clones a set of parts (copy operation)
 * @author rhornig
 *
 */
public class CloneCommand extends Command {

    private List parts, newTopLevelParts, newConnections;
    private ContainerModel parent;
    private Map bounds, indices, connectionPartMap;

    public CloneCommand() {
        super(MessageFactory.CloneCommand_Label);
        parts = new LinkedList();
    }

    public void addPart(NedNodeModel part, Rectangle newBounds) {
        parts.add(part);
        if (bounds == null) {
            bounds = new HashMap();
        }
        bounds.put(part, newBounds);
    }

    public void addPart(NedNodeModel part, int index) {
        parts.add(part);
        if (indices == null) {
            indices = new HashMap();
        }
        indices.put(part, new Integer(index));
    }

    protected void clonePart(NedNodeModel oldPart, ContainerModel newParent, Rectangle newBounds,
            List newConnections, Map connectionPartMap, int index) {
        NedNodeModel newPart = null;

        if (oldPart instanceof SubmoduleModel) {
            newPart = new SubmoduleModel();
        } else if (oldPart instanceof CompoundModuleModel) {
            newPart = new CompoundModuleModel();
        } 

        if (oldPart instanceof ContainerModel) {
            Iterator i = ((ContainerModel) oldPart).getChildren().iterator();
            while (i.hasNext()) {
                // for children they will not need new bounds
                clonePart((NedNodeModel) i.next(), (ContainerModel) newPart, null, newConnections,
                        connectionPartMap, -1);
            }
        }

        Iterator i = oldPart.getTargetConnections().iterator();
        while (i.hasNext()) {
            WireModel connection = (WireModel) i.next();
            WireModel newConnection = new WireModel();
            newConnection.setTarget(newPart);
            newConnection.setTargetGate(connection.getTargetGate());
            newConnection.setSourceGate(connection.getSourceGate());
            newConnection.setSource(connection.getSource());

            Iterator b = connection.getBendpoints().iterator();
            Vector newBendPoints = new Vector();

            while (b.hasNext()) {
                WireBendpointModel bendPoint = (WireBendpointModel) b.next();
                WireBendpointModel newBendPoint = new WireBendpointModel();
                newBendPoint.setRelativeDimensions(bendPoint.getFirstRelativeDimension(), bendPoint
                        .getSecondRelativeDimension());
                newBendPoint.setWeight(bendPoint.getWeight());
                newBendPoints.add(newBendPoint);
            }

            newConnection.setBendpoints(newBendPoints);
            newConnections.add(newConnection);
        }

        if (index < 0) {
            newParent.addChild(newPart);
        } else {
            newParent.addChild(newPart, index);
        }

        newPart.setSize(oldPart.getSize());

        if (newBounds != null) {
            newPart.setLocation(newBounds.getTopLeft());
        } else {
            newPart.setLocation(oldPart.getLocation());
        }

        // keep track of the new parts so we can delete them in undo
        // keep track of the oldpart -> newpart map so that we can properly
        // attach
        // all connections.
        if (newParent == parent) newTopLevelParts.add(newPart);
        connectionPartMap.put(oldPart, newPart);
    }

    public void execute() {
        connectionPartMap = new HashMap();
        newConnections = new LinkedList();
        newTopLevelParts = new LinkedList();

        Iterator i = parts.iterator();

        NedNodeModel part = null;
        while (i.hasNext()) {
            part = (NedNodeModel) i.next();
            if (bounds != null && bounds.containsKey(part)) {
                clonePart(part, parent, (Rectangle) bounds.get(part), newConnections, connectionPartMap, -1);
            } else if (indices != null && indices.containsKey(part)) {
                clonePart(part, parent, null, newConnections, connectionPartMap,
                        ((Integer) indices.get(part)).intValue());
            } else {
                clonePart(part, parent, null, newConnections, connectionPartMap, -1);
            }
        }

        // go through and set the source of each connection to the proper
        // source.
        Iterator c = newConnections.iterator();

        while (c.hasNext()) {
            WireModel conn = (WireModel) c.next();
            NedNodeModel source = conn.getSource();
            if (connectionPartMap.containsKey(source)) {
                conn.setSource((NedNodeModel) connectionPartMap.get(source));
                conn.attachSource();
                conn.attachTarget();
            }
        }

    }

    public void setParent(ContainerModel parent) {
        this.parent = parent;
    }

    public void redo() {
        for (Iterator iter = newTopLevelParts.iterator(); iter.hasNext();)
            parent.addChild((NedNodeModel) iter.next());
        for (Iterator iter = newConnections.iterator(); iter.hasNext();) {
            WireModel conn = (WireModel) iter.next();
            NedNodeModel source = conn.getSource();
            if (connectionPartMap.containsKey(source)) {
                conn.setSource((NedNodeModel) connectionPartMap.get(source));
                conn.attachSource();
                conn.attachTarget();
            }
        }
    }

    public void undo() {
        for (Iterator iter = newTopLevelParts.iterator(); iter.hasNext();)
            parent.removeChild((NedNodeModel) iter.next());
    }

}