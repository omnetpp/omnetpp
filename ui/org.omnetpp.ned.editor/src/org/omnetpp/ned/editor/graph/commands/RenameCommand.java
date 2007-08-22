package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * Rename any element which has a name attribute
 *
 * @author rhornig
 */
public class RenameCommand extends Command {
    private IHasName target;
    private String name, oldName;

    /**
     * Rename a node which supports the name attribute
     * @param element The element to be renamed
     * @param newName The new name of the element
     */
    public RenameCommand(IHasName element, String newName) {
        super();
        Assert.isNotNull(element);
        target = element;
        oldName = target.getName();
        name = newName;
        setLabel("Rename "+oldName);
    }

    @Override
    public boolean canExecute() {
        return StringUtils.isNotEmpty(name);
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

