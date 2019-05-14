package org.omnetpp.scave.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Inputs extends ModelObject {
    protected List<InputFile> inputs = new ArrayList<InputFile>();

    public List<InputFile> getInputs() {
        return Collections.unmodifiableList(inputs);
    }

    public void setInputs(List<InputFile> inputs) {
        for (InputFile i : this.inputs)
            i.parent = null;
        this.inputs = inputs;
        for (InputFile i : this.inputs)
            i.parent = this;
        notifyListeners();
    }

    public void addInput(InputFile input) {
        addInput(input, inputs.size());
    }

    public void addInput(InputFile input, int index) {
        input.parent = this;
        inputs.add(index, input);
        notifyListeners();
    }

    public void removeInput(InputFile input) {
        input.parent = null;
        inputs.remove(input);
        notifyListeners();
    }

    @Override
    protected Inputs clone() throws CloneNotSupportedException {
        Inputs clone = (Inputs)super.clone();

        clone.inputs = new ArrayList<InputFile>(inputs.size());

        for (int i = 0; i < inputs.size(); ++i)
            clone.inputs.add(inputs.get(i).clone());

        return clone;
    }
}
