/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.common.displaymodel.DimensionF;
import org.omnetpp.common.displaymodel.PointF;
import org.omnetpp.common.displaymodel.RectangleF;
import org.omnetpp.ned.model.interfaces.IConnectableElement;

/**
 * Change the size and location of the submodule
 *
 * @author rhornig
 */
public class SetConstraintCommand extends Command {

    String oldDisplayString;
    String newDisplayString;
    private RectangleF oldBounds;
    private PointF newPos;
    private DimensionF newSize;
    private boolean pinOperation = false;
    private final IConnectableElement module;

    public SetConstraintCommand(IConnectableElement newModule, RectangleF oldBounds) {
        super();
        this.oldBounds = oldBounds;
        module = newModule;
    }

    @Override
    public void execute() {
        oldDisplayString = module.getDisplayString().toString();
        // get the current (old) value of the size. if the size is not specified change it to -1
        boolean sizeChanged = (newSize != null) && (oldBounds != null) && !newSize.equals(oldBounds.getSize());
        boolean posChanged = (newPos != null) && (oldBounds != null) && !newPos.equals(oldBounds.getLocation());

        if (pinOperation) {
            module.getDisplayString().setLocation(newPos);
            setLabel((newPos == null ? "Unpin " :"Pin ") + module.getName());
        }
        else {
            // move or resize operation
            if (sizeChanged && posChanged) {
                module.getDisplayString().setLocation(newPos);
                module.getDisplayString().setSize(newSize);
                setLabel("Resize " + module.getName());
            }
            else if (sizeChanged) {
                module.getDisplayString().setSize(newSize);
                setLabel("Resize " + module.getName());
            }
            else if (posChanged) {
                module.getDisplayString().setLocation(newPos);
                setLabel("Move " + module.getName());
            }
        }
        
        // store for later redo operation
        newDisplayString = module.getDisplayString().toString();
    }

    @Override
    public void redo() {
        module.getDisplayString().set(newDisplayString);
    }

    @Override
    public void undo() {
        module.getDisplayString().set(oldDisplayString);
    }

    /**
     * Sets both the location and size
     */
    public void setConstraint(RectangleF bounds) {
        setNewLocation(bounds.getLocation());
        setNewSize(bounds.getSize());
    }

    /**
     * Sets the new location (should be used for move operation)
     */
    public void setNewLocation(PointF p) {
        pinOperation = false;
        newPos = p;
    }

    /**
     * Sets the new size of the module (for resize operation only)
     */
    public void setNewSize(DimensionF p) {
        pinOperation = false;
        newSize = p;
    }

    /**
     * Sets the pinned location of module (or null if unpin is requested)
     * Should be used ONLY for pin/unpin operation
     */
    public void setPinLocation(PointF p) {
        pinOperation = true;
        newPos = p;
    }
}