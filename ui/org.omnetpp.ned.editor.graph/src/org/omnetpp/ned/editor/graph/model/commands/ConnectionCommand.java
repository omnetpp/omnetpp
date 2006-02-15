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

import java.util.Iterator;
import java.util.Vector;

import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.NedNode;
import org.omnetpp.ned.editor.graph.model.Wire;

/**
 * (Re)assigns a wire object to source/target submodule gates
 * @author rhornig
 *
 */
public class ConnectionCommand extends Command {

    protected NedNode oldSource;
    protected String oldSourceTerminal;
    protected NedNode oldTarget;
    protected String oldTargetTerminal;
    protected NedNode source;
    protected String sourceGate;
    protected NedNode target;
    protected String targetGate;
    protected Wire wire;

    public ConnectionCommand() {
        super(MessageFactory.ConnectionCommand_Label);
    }

    
    /**
     * Input output gate config consistency can be checked here
     */
    public boolean canExecute() {
        if (target != null) {
            Vector conns = target.getConnections();
            Iterator i = conns.iterator();
            while (i.hasNext()) {
                Wire conn = (Wire) i.next();
                if (targetGate != null && conn.getTargetGate() != null)
                    if (conn.getTargetGate().equals(targetGate) && conn.getTarget().equals(target))
                        return false;
            }
        }
        return true;
    }

    public void execute() {
        if (source != null) {
            wire.detachSource();
            wire.setSource(source);
            wire.setSourceGate(sourceGate);
            wire.attachSource();
        }
        if (target != null) {
            wire.detachTarget();
            wire.setTarget(target);
            wire.setTargetGate(targetGate);
            wire.attachTarget();
        }
        if (source == null && target == null) {
            wire.detachSource();
            wire.detachTarget();
            wire.setTarget(null);
            wire.setSource(null);
            wire.setTargetGate(null);
            wire.setSourceGate(null);
        }
    }

    public String getLabel() {
        return MessageFactory.ConnectionCommand_Description;
    }

    public NedNode getSource() {
        return source;
    }

    public java.lang.String getSourceGate() {
        return sourceGate;
    }

    public NedNode getTarget() {
        return target;
    }

    public String getTargetGate() {
        return targetGate;
    }

    public Wire getWire() {
        return wire;
    }

    public void redo() {
        execute();
    }

    public void setSource(NedNode newSource) {
        source = newSource;
    }

    public void setSourceGate(String newSourceGate) {
        sourceGate = newSourceGate;
    }

    public void setTarget(NedNode newTarget) {
        target = newTarget;
    }

    public void setTargetGate(String newTargetGate) {
        targetGate = newTargetGate;
    }

    public void setWire(Wire w) {
        wire = w;
        oldSource = w.getSource();
        oldTarget = w.getTarget();
        oldSourceTerminal = w.getSourceGate();
        oldTargetTerminal = w.getTargetGate();
    }

    public void undo() {
        source = wire.getSource();
        target = wire.getTarget();
        sourceGate = wire.getSourceGate();
        targetGate = wire.getTargetGate();

        wire.detachSource();
        wire.detachTarget();

        wire.setSource(oldSource);
        wire.setTarget(oldTarget);
        wire.setSourceGate(oldSourceTerminal);
        wire.setTargetGate(oldTargetTerminal);

        wire.attachSource();
        wire.attachTarget();
    }

}
