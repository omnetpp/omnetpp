/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.ListenerList;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.model.ModelObject;

public class CommandStack {

    public static boolean debug = true;

    private String name;

    private List<ICommand> commands = new ArrayList<ICommand>();
    private int numExecutedCommands = 0;
    private int savedPosition = 0; // the value of numExecutedCommands when the last save was done, or -1
    private ICommand mostRecentCommand; // the last command which was executed, undone, or redone
    private ListenerList<CommandStackListener> listeners = new ListenerList<CommandStackListener>();

    public CommandStack(String name) {
        this.name = name;
    }

    private void log(String msg, ICommand command) {
        if (debug)
            Debug.println("CommandStack " + name + " " + msg + " command: " + command.getLabel());
    }

    public void execute(ICommand command) {
        log("executing", command);

        // we add the command before executing it, so that the command stack
        // is in the final state when the model changes happen
        addExecuted(command);
        command.execute();
    }

    public void addExecuted(ICommand command) {
        log("adding executed", command);

        while (numExecutedCommands < commands.size())
            commands.remove(commands.size() - 1);

        if (savedPosition > commands.size())
            savedPosition = -1;

        commands.add(command);
        mostRecentCommand = command;
        numExecutedCommands = commands.size();

        listeners.forEach((l) -> { l.commandStackChanged(this); });
    }

    public void undo() {
        if (canUndo()) {
            ICommand command = commands.get(numExecutedCommands - 1);

            log("undoing", command);

            mostRecentCommand = command;
            numExecutedCommands--;
            listeners.forEach((l) -> { l.commandStackChanged(this); });

            // this should be the last, so that the command stack
            // is in the final state when the model changes happen
            command.undo();
        }
    }

    public void redo() {
        if (canRedo()) {
            ICommand command = commands.get(numExecutedCommands);

            log("redoing executed", command);

            mostRecentCommand = command;
            numExecutedCommands++;
            listeners.forEach((l) -> { l.commandStackChanged(this); });

            // this should be the last, so that the command stack
            // is in the final state when the model changes happen
            command.redo();
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

    public ICommand getCommandToUndo() {
        return (numExecutedCommands > 0) ? commands.get(numExecutedCommands - 1) : null;
    }

    public ICommand getCommandToRedo() {
        return (numExecutedCommands < commands.size()) ? commands.get(numExecutedCommands) : null;
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
