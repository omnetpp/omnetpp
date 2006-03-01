package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.INedNode;

/**
 * Change the size and location of the element
 * @author rhornig
 *
 */
public class SetConstraintCommand extends org.eclipse.gef.commands.Command {

    private Point newPos;
    private Dimension newSize;
    private Point oldPos;
    private Dimension oldSize;
    private INedNode part;

    public void execute() {
        oldSize = part.getSize();
        oldPos = part.getTransientLocation();
        redo();
    }

    public String getLabel() {
        if (oldSize.equals(newSize)) return MessageFactory.SetLocationCommand_Label_Location;
        return MessageFactory.SetLocationCommand_Label_Resize;
    }

    public void redo() {
        part.setSize(newSize);
        part.setTransientLocation(newPos);
    }

    public void setLocation(Rectangle r) {
      setLocation(r.getLocation());
        setSize(r.getSize());
    }

    public void setLocation(Point p) {
        newPos = p;
    }

    public void setPart(INedNode part) {
        this.part = part;
    }

    public void setSize(Dimension p) {
        newSize = p;
    }

    public void undo() {
        part.setSize(oldSize);
        part.setTransientLocation(oldPos);
    }

}