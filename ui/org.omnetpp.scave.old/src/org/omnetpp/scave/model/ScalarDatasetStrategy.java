/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.swt.widgets.Composite;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.renderer.category.BarRenderer;
import org.jfree.chart.renderer.category.CategoryItemRenderer;
import org.jfree.data.category.CategoryDataset;
import org.jfree.data.category.DefaultCategoryDataset;

import org.omnetpp.scave.charting.ChartHelper;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.panel.FilterPanel;
import org.omnetpp.scave.panel.FilterPanelComposite;
import org.omnetpp.scave.panel.ScalarPanelComposite;
import org.omnetpp.scave.panel.ScalarTableContentSorter;
import org.omnetpp.scave.panel.ScalarTableLabelProvider;
import org.omnetpp.scave.panel.TableContentSorter;
import org.omnetpp.scave.plugin.ScavePlugin;

public class ScalarDatasetStrategy implements IDatasetStrategy {

    public FilterPanel createFilterPanel(Composite parent, int style) {
        return new FilterPanel(parent, style, this);
    }

    public FilterPanelComposite createFilterPanelComposite(Composite parent, int style) {
            return new ScalarPanelComposite(parent, style);
    }

    public ITableLabelProvider createFilterPanelLabelProvider() {
        return new ScalarTableLabelProvider();
    }

    public IDList getAll() {
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        return resultFileManager.getAllScalars();
    }

    public File loadResultFile(String fileName) {
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        return resultFileManager.loadScalarFile(fileName);
    }

    public boolean isCompatible(IDList idList) {
        return idList.areAllScalars();
    }

    public TableContentSorter createTableSorter() {
        return new ScalarTableContentSorter();
    }

    public JFreeChart createEmptyChart() {
        // create dataset
        DefaultCategoryDataset categorydataset = new DefaultCategoryDataset();

        // create chart
        JFreeChart jfreechart = ChartFactory.createBarChart3D(
                "Title", "Category", "Value",
                categorydataset, PlotOrientation.VERTICAL,
                true, true, false);
        CategoryPlot categoryplot = jfreechart.getCategoryPlot();
        CategoryItemRenderer categoryitemrenderer = categoryplot.getRenderer();
        categoryitemrenderer.setItemLabelsVisible(true);
        BarRenderer barrenderer = (BarRenderer)categoryitemrenderer;
        barrenderer.setMaximumBarWidth(0.05D);
        return jfreechart;
    }

    public void updateDataset(JFreeChart chart, IDList idlist) {
        CategoryDataset ds = ChartHelper.createChartWithRunsOnXAxis(idlist);
        chart.getCategoryPlot().setDataset(ds);
    }
}
