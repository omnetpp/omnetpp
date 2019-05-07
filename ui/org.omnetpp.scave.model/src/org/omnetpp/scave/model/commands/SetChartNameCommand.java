package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.AnalysisObject;

public class SetChartNameCommand implements ICommand {

    private AnalysisItem item;
    private String oldName;
    private String newName;

    public SetChartNameCommand(AnalysisItem item, String newValue) {
        this.item = item;
        this.newName = newValue;
    }

    @Override
    public void execute() {
        oldName = item.getName();
        item.setName(newName);
    }

    @Override
    public void undo() {
        item.setName(oldName);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Set chart name";
    }

    @Override
    public Collection<AnalysisObject> getAffectedObjects() {
        return Arrays.asList(item);
    }

}
