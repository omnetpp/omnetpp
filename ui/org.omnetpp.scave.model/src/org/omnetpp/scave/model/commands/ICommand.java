package org.omnetpp.scave.model.commands;

import java.util.Collection;

import org.omnetpp.scave.model.ModelObject;

public interface ICommand {
    public void execute();
    public void undo();
    public void redo();
    public String getLabel();
    public Collection<ModelObject> getAffectedObjects();
}
