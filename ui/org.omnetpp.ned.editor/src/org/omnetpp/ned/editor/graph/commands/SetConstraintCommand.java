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

    String oldDisplayString;
    String newDisplayString;
    private Point newPos;
    private Dimension newSize;
    private boolean pinOperation = false;
    private final INamedGraphNode module;
    private float scale = 1.0f;

    public SetConstraintCommand(INamedGraphNode newModule, float scale) {
    	super();
        this.scale = scale;
        module = newModule;
    }

    @Override
    public void execute() {
        oldDisplayString = module.getDisplayString().toString();
        boolean sizeChanged = (newSize != null) && !newSize.equals(module.getDisplayString().getSize(scale));
        boolean posChanged = (newPos != null) && !newPos.equals(module.getDisplayString().getLocation(scale));
        if (pinOperation) {
            module.getDisplayString().setLocation(newPos, scale);
            setLabel((newPos == null ? "Unpin " :"Pin ") + module.getName());
        } else {
            // move or resize operation
            if (sizeChanged && posChanged) {
                module.getDisplayString().setConstraint(newPos, newSize, scale);
                setLabel("Resize " + module.getName());
            } else if (sizeChanged) {
                module.getDisplayString().setSize(newSize, scale);
                setLabel("Resize " + module.getName());
            } else if (posChanged) {
                module.getDisplayString().setLocation(newPos, scale);
                setLabel("Move " + module.getName());
            }
        }
        // store for later redo operation
        newDisplayString = module.getDisplayString().toString();
        System.out.println("displaystring set: "+newDisplayString);
    }

    @Override
    public void redo() {
        module.getDisplayString().set(newDisplayString);
        System.out.println("displaystring set: "+newDisplayString);
    }

    @Override
    public void undo() {
        module.getDisplayString().set(oldDisplayString);
        System.out.println("displaystring set: "+oldDisplayString);
    }

    /**
     * Sets both the location and size
     */
    public void setConstraint(Rectangle r) {
        setNewLocation(r.getLocation());
        setNewSize(r.getSize());
    }

    /**
     * Sets the new location (should be used for move operation)
     */
    public void setNewLocation(Point p) {
        pinOperation = false;
        newPos = p;
    }

    /**
     * Sets the new size of the module (for resize operation only)
     */
    public void setNewSize(Dimension p) {
        pinOperation = false;
        newSize = p;
    }

    /**
     * Sets the pinned location of module (or null if unpin is requested)
     * Should be used ONLY for pin/unpin operation
     */
    public void setPinLocation(Point p) {
        pinOperation = true;
        newPos = p;
    }
}