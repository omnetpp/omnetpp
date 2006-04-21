package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.ConnectionNodeEx;

/**
 * Base class for different bend point related commands like
 * CreateBendpointCommand, MoveBendpointCommand, DeleteBendpointCommand
 * @author rhornig
 *
 */
public class BendpointCommand extends Command {

    protected int index;
    protected Point location;
    protected ConnectionNodeEx conn;
    private Dimension d1, d2;

    protected Dimension getFirstRelativeDimension() {
        return d1;
    }

    protected Dimension getSecondRelativeDimension() {
        return d2;
    }

    protected int getIndex() {
        return index;
    }

    protected Point getLocation() {
        return location;
    }

    protected ConnectionNodeEx getWire() {
        return conn;
    }

    @Override
    public void redo() {
        execute();
    }

    public void setRelativeDimensions(Dimension dim1, Dimension dim2) {
        d1 = dim1;
        d2 = dim2;
    }

    public void setIndex(int i) {
        index = i;
    }

    public void setLocation(Point p) {
        location = p;
    }

    public void setWire(ConnectionNodeEx w) {
        conn = w;
    }

}
