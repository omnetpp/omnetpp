package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.Bendpoint;

import org.omnetpp.ned.editor.graph.model.WireBendpointModel;

/**
 * Moves a bendpoint of a wire to a different location
 * @author rhornig
 *
 */
public class MoveBendpointCommand extends BendpointCommand {

    private Bendpoint oldBendpoint;

    public void execute() {
        WireBendpointModel bp = new WireBendpointModel();
        bp.setRelativeDimensions(getFirstRelativeDimension(), getSecondRelativeDimension());
        setOldBendpoint((Bendpoint) getWire().getBendpoints().get(getIndex()));
        getWire().setBendpoint(getIndex(), bp);
        super.execute();
    }

    protected Bendpoint getOldBendpoint() {
        return oldBendpoint;
    }

    public void setOldBendpoint(Bendpoint bp) {
        oldBendpoint = bp;
    }

    public void undo() {
        super.undo();
        getWire().setBendpoint(getIndex(), getOldBendpoint());
    }

}
