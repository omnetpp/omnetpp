package org.omnetpp.scave.model.commands;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.model.ModelObject;

public class CompoundCommand implements ICommand {

    boolean executed = false;
    String label;
    private List<ICommand> commands;

    public CompoundCommand(String label) {
        this.label = label;
        this.commands = new ArrayList<ICommand>();
    }

    public CompoundCommand(String label, List<ICommand> commands) {
        this.label = label;
        this.commands = commands;
    }

    public void append(ICommand command) {
        Assert.isTrue(!executed);
        commands.add(command);
    }

    @Override
    public void execute() {
        executed = true;
        for (ICommand c : commands)
            c.execute();
    }

    @Override
    public void undo() {
        for (int i = commands.size()-1; i >= 0; --i)
            commands.get(i).undo();
    }

    @Override
    public void redo() {
        for (ICommand c : commands)
            c.redo();
    }

    @Override
    public String getLabel() {
        return label;
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        Set<ModelObject> objs = new HashSet<ModelObject>();
        for (ICommand c : commands)
            objs.addAll(c.getAffectedObjects());
        return objs;
    }

}
