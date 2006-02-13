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

import org.eclipse.draw2d.Bendpoint;

/**
 * Removes a bendpoint from a wire
 * @author rhornig
 *
 */
public class DeleteBendpointCommand extends BendpointCommand {

    private Bendpoint bendpoint;

    public void execute() {
        bendpoint = (Bendpoint) getWire().getBendpoints().get(getIndex());
        getWire().removeBendpoint(getIndex());
        super.execute();
    }

    public void undo() {
        super.undo();
        getWire().insertBendpoint(getIndex(), bendpoint);
    }

}
