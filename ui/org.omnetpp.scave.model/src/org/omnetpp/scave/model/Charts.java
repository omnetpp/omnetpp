/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.eclipse.core.runtime.Assert;

public class Charts extends ModelObject {
    protected List<AnalysisItem> charts = new ArrayList<AnalysisItem>();

    public List<AnalysisItem> getCharts() {
        return Collections.unmodifiableList(charts);
    }

    public void setCharts(List<AnalysisItem> charts) {
        for (AnalysisItem i : this.charts)
            i.parent = null;
        this.charts = charts;
        for (AnalysisItem i : this.charts)
            i.parent = this;

        for (int i = 0; i < charts.size(); ++i)
            Assert.isTrue(i == charts.indexOf(charts.get(i)));

        notifyListeners();
    }

    public void addChart(AnalysisItem chart) {
        addChart(chart, charts.size());
    }

    public void addChart(AnalysisItem chart, int index) {
        Assert.isTrue(findById(chart.getId()) == null, "ID not unique");
        chart.parent = this;
        charts.add(index, chart);
        notifyListeners();
    }

    public void removeChart(AnalysisItem chart) {
        chart.parent = null;
        charts.remove(chart);
        notifyListeners();
    }

    public AnalysisItem findById(int id) {
        for (AnalysisItem chart : charts)
            if (chart.getId() == id)
                return chart;
        return null;
    }

    @Override
    protected Charts clone() throws CloneNotSupportedException {
        Charts clone = (Charts)super.clone();

        clone.charts = new ArrayList<AnalysisItem>(charts.size());

        for (int i = 0; i < charts.size(); ++i)
            clone.charts.add(charts.get(i).clone());

        return clone;
    }
}
