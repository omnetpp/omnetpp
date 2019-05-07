package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;

public class SetChartPropertyCommand implements ICommand {

    private Property property;
    private String oldValue;
    private String newValue;

    public SetChartPropertyCommand(Property property, String newValue) {
        this.property = property;
        this.newValue = newValue;
    }

    @Override
    public void execute() {
        oldValue = property.getValue();
        property.setValue(newValue);
    }

    @Override
    public void undo() {
        property.setValue(oldValue);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Set chart property";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(property);
    }

}
