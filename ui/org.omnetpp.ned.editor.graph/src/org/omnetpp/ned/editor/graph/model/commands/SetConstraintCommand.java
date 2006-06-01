package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.INamedGraphNode;

/**
 * Change the size and location of the element
 * @author rhornig
 *
 */
public class SetConstraintCommand extends Command {

    private Point newPos;
    private Dimension newSize;
    private Point oldPos;
    private Dimension oldSize;
    private INamedGraphNode module;

    public SetConstraintCommand(INamedGraphNode newModule) {
    	super();
        module = newModule;
    }

    @Override
    public String getLabel() {
        if (oldSize.equals(newSize)) 
            return "Move " + module.getName();
        return "Resize " + module.getName();
    }

    @Override
    public void execute() {
        oldSize = module.getDisplayString().getSize();
        oldPos = module.getDisplayString().getLocation();
        redo();
    }

    @Override
    public void redo() {
        module.getDisplayString().setConstraint(newPos, newSize);
    }

    @Override
    public void undo() {
        module.getDisplayString().setConstraint(oldPos, oldSize);
    }

    public void setConstrant(Rectangle r) {
        setLocation(r.getLocation());
        setSize(r.getSize());
    }

    public void setLocation(Point p) {
        newPos = p;
    }

    public void setSize(Dimension p) {
        newSize = p;
    }
}