package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * Rename any element which has a name attribute
 * @author rhornig
 */
public class RenameCommand extends Command {
    private IHasName target;
    private String name, oldName;

    /**
     * Rename a node which supports the name attribute
     * @param node The node to be renamed
     * @param newName The new name of the node
     */
    public RenameCommand(INEDElement node, String newName) {
        super("Rename");
        // if the element supports naming set the target. otherwise leave it as NULL
        // and the command will be disabled
        if (node instanceof IHasName) {
            target = (IHasName)node;
            oldName = target.getName();
            name = newName;
            setLabel("Rename "+oldName);
        }
    }

    @Override
    public boolean canExecute() {
        return target!=null && name!=null && !"".equals(name);
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        target.setName(name);
    }

    @Override
    public void undo() {
        target.setName(oldName);
    }

}

