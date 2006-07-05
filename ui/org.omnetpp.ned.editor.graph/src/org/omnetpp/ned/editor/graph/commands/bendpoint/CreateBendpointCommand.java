package org.omnetpp.ned.editor.graph.commands.bendpoint;

import org.omnetpp.ned2.model.WireBendpointModel;

/**
 * Currently UNUSED
 * Creates a new bendpoint on an existing wire
 * @author rhornig
 *
 */
public class CreateBendpointCommand extends BendpointCommand {

    @Override
    public void execute() {
        WireBendpointModel wbp = new WireBendpointModel();
        wbp.setRelativeDimensions(getFirstRelativeDimension(), getSecondRelativeDimension());
        getWire().insertBendpoint(getIndex(), wbp);
        super.execute();
    }

    @Override
    public void undo() {
        super.undo();
        getWire().removeBendpoint(getIndex());
    }

}
