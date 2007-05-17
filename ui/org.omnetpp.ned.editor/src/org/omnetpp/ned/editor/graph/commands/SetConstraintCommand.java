package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.interfaces.INamedGraphNode;

/**
 * Change the size and location of the submodule
 *
 * @author rhornig
 */
public class SetConstraintCommand extends Command {

    private Point newPos;
    private Dimension newSize;
    private Point oldPos;
    private Dimension oldSize;
    private final INamedGraphNode module;
    private float scale = 1.0f;
    private boolean pinOperation = false;

    public SetConstraintCommand(INamedGraphNode newModule, float scale) {
    	super();
        this.scale = scale;
        module = newModule;
        newPos = module.getDisplayString().getLocation(scale);
        newSize = module.getDisplayString().getSize(scale);
    }

    @Override
    public String getLabel() {
        if (pinOperation) {
            if (newPos == null )
                return "Unpin " + module.getName();
            else
                return "Pin " + module.getName();
        } else if (newSize != null && !newSize.equals(oldSize))
            return "Resize " + module.getName();
        return "Move " + module.getName();
    }

    @Override
    public void execute() {
        oldSize = module.getDisplayString().getSize(scale);
        oldPos = module.getDisplayString().getLocation(scale);
        redo();
    }

    @Override
    public void redo() {
        module.getDisplayString().setConstraint(newPos, newSize, scale);
    }

    @Override
    public void undo() {
        module.getDisplayString().setConstraint(oldPos, oldSize, scale);
    }

    public void setConstraint(Rectangle r) {
        setNewLocation(r.getLocation());
        setNewSize(r.getSize());
    }

    /**
     * Sets the new location (should be used for move operation)
     * @param p
     */
    public void setNewLocation(Point p) {
        pinOperation = false;
        newPos = p;
    }

    /**
     * Sets the new size of the module (for resize operation only)
     * @param p
     */
    public void setNewSize(Dimension p) {
        pinOperation = false;
        newSize = p;
    }

    /**
     * Sets the pinned location of module (or null if unpin is requested)
     * Should be used ONLY for pin/unpin operation
     * @param p
     */
    public void setPinLocation(Point p) {
        pinOperation = true;
        newPos = p;
    }

    public boolean isPinCommand() {
        return pinOperation && newPos != null;
    }

    public boolean isUnpinCommand() {
        return pinOperation && newPos == null;
    }
}