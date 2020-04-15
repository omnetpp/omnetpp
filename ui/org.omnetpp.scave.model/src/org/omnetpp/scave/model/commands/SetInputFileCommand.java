package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ModelObject;

public class SetInputFileCommand implements ICommand {

    private InputFile inputFile;
    String oldName;
    String newName;

    public SetInputFileCommand(InputFile inputFile, String newName) {
        this.inputFile = inputFile;
        this.newName = newName;
    }

    @Override
    public void execute() {
        oldName = inputFile.getName();
        inputFile.setName(newName);
    }

    @Override
    public void undo() {
        inputFile.setName(oldName);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Set input file";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(inputFile);
    }

}
