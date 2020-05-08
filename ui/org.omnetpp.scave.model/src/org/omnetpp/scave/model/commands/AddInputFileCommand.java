/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ModelObject;

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
