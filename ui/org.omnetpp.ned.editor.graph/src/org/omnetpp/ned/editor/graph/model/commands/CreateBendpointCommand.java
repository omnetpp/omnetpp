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

import org.omnetpp.ned.editor.graph.model.WireBendpointModel;

/**
 * Creates a new bendpoint on an existing wire
 * @author rhornig
 *
 */
public class CreateBendpointCommand extends BendpointCommand {

    public void execute() {
        WireBendpointModel wbp = new WireBendpointModel();
        wbp.setRelativeDimensions(getFirstRelativeDimension(), getSecondRelativeDimension());
        getWire().insertBendpoint(getIndex(), wbp);
        super.execute();
    }

    public void undo() {
        super.undo();
        getWire().removeBendpoint(getIndex());
    }

}
