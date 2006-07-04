package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.draw2d.Bendpoint;

/**
 * Removes a bendpoint from a wire
 * @author rhornig
 *
 */
public class DeleteBendpointCommand extends BendpointCommand {

    private Bendpoint bendpoint;

    @Override
    public void execute() {
        bendpoint = (Bendpoint) getWire().getBendpoints().get(getIndex());
        getWire().removeBendpoint(getIndex());
        super.execute();
    }

    @Override
    public void undo() {
        super.undo();
        getWire().insertBendpoint(getIndex(), bendpoint);
    }

}
