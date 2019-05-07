package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.InputFile;

public class AddInputFileCommand implements ICommand {

    private Analysis analysis;
    private InputFile inputFile;

    public AddInputFileCommand(Analysis analysis, InputFile inputFile) {
        this.analysis = analysis;
        this.inputFile = inputFile;
    }

    @Override
    public void execute() {
        analysis.getInputs().addInput(inputFile);
    }

    @Override
    public void undo() {
        analysis.getInputs().removeInput(inputFile);

    }

    @Override
    public void redo() {
        execute();

    }

    @Override
    public String getLabel() {
        return "Add input file";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(analysis.getInputs(), inputFile);
    }

}
