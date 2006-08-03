package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.INamedGraphNode;

/**
 * Change the size and location of the submodule
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
        newPos = module.getDisplayString().getLocation();
        newSize = module.getDisplayString().getSize();
    }

    @Override
    public String getLabel() {
    	if (newPos == null )
    		return "Unpin " + module.getName();
        if (newSize != null && !newSize.equals(oldSize)) 
            return "Resize " + module.getName();
        return "Move " + module.getName();
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

    public void setConstraint(Rectangle r) {
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