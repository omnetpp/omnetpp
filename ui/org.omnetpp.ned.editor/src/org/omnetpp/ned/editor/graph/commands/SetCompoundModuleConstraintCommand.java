package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;

/**
 * Change the size and location of a compound module (location cannot be changed)
 *
 * @author rhornig
 */
public class SetCompoundModuleConstraintCommand extends Command {

    private Dimension newSize;
    private Dimension oldSize;
    private CompoundModuleElementEx module;

    public SetCompoundModuleConstraintCommand(CompoundModuleElementEx newModule) {
    	super();
        module = newModule;
    }

    @Override
    public String getLabel() {
        return "Resize " + module.getName();
    }

    @Override
    public void execute() {
        oldSize = module.getDisplayString().getCompoundSize(null);
        redo();
    }

    @Override
    public void redo() {
        module.getDisplayString().setCompoundSize(newSize, null);
    }

    @Override
    public void undo() {
        module.getDisplayString().setCompoundSize(oldSize, null);
    }

    public void setSize(Dimension p) {
        newSize = p;
    }
}