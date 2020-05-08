/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

public class Analysis extends ModelObject {
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

