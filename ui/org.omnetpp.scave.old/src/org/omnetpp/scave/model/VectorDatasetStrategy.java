/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import java.awt.Color;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.swt.widgets.Composite;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RectangleInsets;

import org.omnetpp.scave.charting.ChartHelper;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.panel.FilterPanel;
import org.omnetpp.scave.panel.FilterPanelComposite;
import org.omnetpp.scave.panel.TableContentSorter;
import org.omnetpp.scave.panel.VectorPanelComposite;
import org.omnetpp.scave.panel.VectorTableContentSorter;
import org.omnetpp.scave.panel.VectorTableLabelProvider;
import org.omnetpp.scave.plugin.ScavePlugin;


public class VectorDatasetStrategy implements IDatasetStrategy {

    public FilterPanel createFilterPanel(Composite parent, int style) {
        return new FilterPanel(parent, style, this);
    }

    public FilterPanelComposite createFilterPanelComposite(Composite parent, int style) {
        return new VectorPanelComposite(parent, style);
    }

    public ITableLabelProvider createFilterPanelLabelProvider() {
        return new VectorTableLabelProvider();
    }

    public IDList getAll() {
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        return resultFileManager.getAllVectors();
    }

    public File loadResultFile(String fileName) {
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        return resultFileManager.loadVectorFile(fileName);
    }

    public boolean isCompatible(IDList idList) {
        return idList.areAllVectors();
    }

    public TableContentSorter createTableSorter() {
        return new VectorTableContentSorter();
    }

    /**
     * Create empty line chart.
     */
    public JFreeChart createEmptyChart() {
        // create dataset
        XYSeriesCollection xyseriescollection = new XYSeriesCollection();

        // create chart
        JFreeChart jfreechart = ChartFactory.createXYLineChart(
                "Line Chart", "X", "Y",
                xyseriescollection, PlotOrientation.VERTICAL,
                true, true, false);
        jfreechart.setAntiAlias(false);
        jfreechart.setBackgroundPaint(Color.white);
        XYPlot xyplot = (XYPlot)jfreechart.getPlot();
        xyplot.setBackgroundPaint(Color.lightGray);
        xyplot.setAxisOffset(new RectangleInsets(5D, 5D, 5D, 5D));
        xyplot.setDomainGridlinePaint(Color.white);
        xyplot.setRangeGridlinePaint(Color.white);
        XYLineAndShapeRenderer xylineandshaperenderer = (XYLineAndShapeRenderer)xyplot.getRenderer();
        xylineandshaperenderer.setShapesVisible(false);
        xylineandshaperenderer.setShapesFilled(false);
        NumberAxis numberaxis = (NumberAxis)xyplot.getRangeAxis();
        numberaxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());
        return jfreechart;
    }

    public void updateDataset(JFreeChart chart, IDList idlist) {
        XYDataset ds = ChartHelper.createXYDataSet(idlist);
        chart.getXYPlot().setDataset(ds);
    }
}
