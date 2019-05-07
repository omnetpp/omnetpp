package org.omnetpp.scave.model;

public class Analysis extends AnalysisObject {
    protected Inputs inputs = new Inputs();
    protected Charts charts = new Charts(); // TODO rename to items

    public Analysis() {
        inputs.parent = this;
        charts.parent = this;
    }

    public Inputs getInputs() {
        return inputs;
    }

    public void setInputs(Inputs inputs) {
        this.inputs.parent = null;
        this.inputs = inputs;
        this.inputs.parent = this;

        notifyListeners();
    }

    public Charts getCharts() {
        return charts;
    }

    public void setCharts(Charts charts) {
        this.charts.parent = null;
        this.charts = charts;
        this.charts.parent = this;
        notifyListeners();
    }

    @Override
    public Analysis clone() throws CloneNotSupportedException {
        Analysis clone = (Analysis)super.clone();
        clone.inputs = inputs.clone();
        clone.charts = charts.clone();
        return clone;
    }
}

