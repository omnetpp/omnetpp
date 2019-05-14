package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ModelObject;

public class RemoveInputFileCommand implements ICommand {

    private int index;
    private ModelObject parent;
    private InputFile inputFile;

    public RemoveInputFileCommand(InputFile inputFile) {
        this.inputFile = inputFile;
        this.parent = inputFile.getParent();
    }

    @Override
    public void execute() {
        ModelObject parent = inputFile.getParent();

        if (parent instanceof Inputs) {
            index = ((Inputs)parent).getInputs().indexOf(inputFile);
        }

        if (parent instanceof Inputs)
            ((Inputs)parent).removeInput(inputFile);
        else {
            // TODO
        }
    }

    @Override
    public void undo() {
        if (parent instanceof Inputs)
            ((Inputs)parent).addInput(inputFile, index);

    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Remove inputFile";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(parent, inputFile);
    }

}
