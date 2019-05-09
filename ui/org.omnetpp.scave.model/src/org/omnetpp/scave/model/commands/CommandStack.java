package org.omnetpp.scave.model.commands;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.ListenerList;
import org.omnetpp.scave.model.ModelObject;

public class CommandStack {
    private List<ICommand> commands = new ArrayList<ICommand>();
    private int numExecutedCommands = 0;
    private int savedPosition = 0; // the value of numExecutedCommands when the last save was done, or -1
    private ICommand mostRecentCommand; // the last command which was executed, undone, or redone
    private ListenerList<CommandStackListener> listeners = new ListenerList<CommandStackListener>();

    public void execute(ICommand command) {
        command.execute();
        while (numExecutedCommands < commands.size())
            commands.remove(commands.size() - 1);

        if (savedPosition > commands.size())
            savedPosition = -1;

        commands.add(command);
        mostRecentCommand = command;
        numExecutedCommands = commands.size();

        listeners.forEach((l) -> { l.commandStackChanged(); });
    }

    public void undo() {
        if (canUndo()) {
            ICommand command = commands.get(numExecutedCommands - 1);
            command.undo();
            mostRecentCommand = command;
            numExecutedCommands--;

            listeners.forEach((l) -> { l.commandStackChanged(); });
        }
    }

    public void redo() {
        if (canRedo()) {
            ICommand command = commands.get(numExecutedCommands);
            command.redo();
            mostRecentCommand = command;
            numExecutedCommands++;

            listeners.forEach((l) -> { l.commandStackChanged(); });
        }
    }

    public boolean canUndo() {
        return numExecutedCommands > 0;
    }

    public boolean canRedo() {
        return numExecutedCommands < commands.size();
    }

    public ICommand getMostRecentCommand() {
        return mostRecentCommand;
    }

    public boolean wasObjectAffected(ModelObject object) {
        for (int i = 0; i < numExecutedCommands; ++i)
            if (commands.get(i).getAffectedObjects().contains(object))
                return true;
        return false;
    }

    public void addListener(CommandStackListener listener) {
        listeners.add(listener);
    }

    public void removeListener(CommandStackListener listener) {
        listeners.remove(listener);
    }

    public boolean isSaveNeeded() {
        return numExecutedCommands != savedPosition;
    }

    public void saved() {
        savedPosition = numExecutedCommands;
    }
}
