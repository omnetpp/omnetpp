/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.dialogs;

import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.CategoryAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.title.TextTitle;
import org.omnetpp.scave.charting.ChartSWTWrapper;

public class TitlePreferencePage extends PreferencePage {

    private ChartSWTWrapper swtChart;
    private TitlePreferencePanel panel;

    public TitlePreferencePage(String title, ChartSWTWrapper swtChart) {
        super(title);
        this.swtChart = swtChart;
    }

    @Override
    protected Control createContents(Composite parent) {
        panel = new TitlePreferencePanel(parent, SWT.NONE);
        fillControls();
        return panel;
    }

    @Override
    protected void performApply() {
        applyChanges();
        super.performApply();
    }

    @Override
    public boolean performOk() {
        applyChanges();
        return super.performOk();
    }

    protected void fillControls()   {
        JFreeChart chart = swtChart.getChart();
        if (chart != null) {
            TextTitle title = chart.getTitle();
            Plot plot = chart.getPlot();

            if (title != null)
                panel.setTitleText(title.getText());
            if (plot != null && plot instanceof CategoryPlot) {
                CategoryPlot categoryPlot = (CategoryPlot)plot;
                CategoryAxis xAxis = categoryPlot.getDomainAxis();
                ValueAxis yAxis = categoryPlot.getRangeAxis();
                if (xAxis != null)
                    panel.setXAxisText(xAxis.getLabel());
                if (yAxis != null)
                    panel.setYAxisText(yAxis.getLabel());
            }
        }
    }

    protected void applyChanges() {
        JFreeChart chart = swtChart.getChart();

        if (chart != null) {
            TextTitle title = chart.getTitle();
            Plot plot = chart.getPlot();

            if (title != null)
                title.setText(panel.getTitleText());
            if (plot != null && plot instanceof CategoryPlot) {
                CategoryPlot categoryPlot = (CategoryPlot)plot;
                CategoryAxis xAxis = categoryPlot.getDomainAxis();
                ValueAxis yAxis = categoryPlot.getRangeAxis();
                if (xAxis != null)
                    xAxis.setLabel(panel.getXAxisText());
                if (yAxis != null)
                    yAxis.setLabel(panel.getYAxisText());
            }
        }
    }


}
