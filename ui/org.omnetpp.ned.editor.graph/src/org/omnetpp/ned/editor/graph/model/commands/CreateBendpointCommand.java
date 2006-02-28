package org.omnetpp.ned.editor.graph.model.commands;

import org.omnetpp.ned.editor.graph.model.old.WireBendpointModel;

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
