/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/
package org.omnetpp.scave.python;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.pychart.INativeChartPlotter;

/**
 * Implementation of the methods of the omnetpp.scave.plot Python module
 * for native charts.
 *
 * Collects the data to be put on the plot from the chart script, and handles
 * the batched setting of the visual plot properties.
 */
class NativeChartPlotter implements INativeChartPlotter {

    /** The native plot widget on which to set the visual properties. */
    private PlotBase plot;

    /** The native plot widget on which to set the visual properties. */
    private GroupsSeriesDataset scalarDataset = new GroupsSeriesDataset(null);
    private XYDataset xyDataset = new XYDataset(null);
    private HistogramDataset histogramDataset = new HistogramDataset(null);

    private Map<String, String> pendingPropertyChanges = new HashMap<>();

    public NativeChartPlotter(PlotBase plot) {
        this.plot = plot;
    }

    @Override
    public void plotScalars(byte[] pickledData, Map<String, String> props) {
        List<String> seriesKeys = scalarDataset.addValues(pickledData);

        pendingPropertyChanges.putAll(props);
        for (String seriesKey : seriesKeys)
            for (String propKey : props.keySet())
                pendingPropertyChanges.put(propKey + "/" + seriesKey, props.get(propKey));
    }

    @Override
    public void plotVectors(byte[] pickledData, Map<String, String> props) {
        List<String> lineKeys = xyDataset.addVectors(pickledData);

        for (String lineKey : lineKeys)
            for (String propKey : props.keySet())
                pendingPropertyChanges.put(propKey + "/" + lineKey, props.get(propKey));
    }

    @Override
    public void plotHistograms(byte[] pickledData, Map<String, String> props) {
        List<String> histKeys = histogramDataset.addValues(pickledData);

        for (String histKey : histKeys)
            for (String propKey : props.keySet())
                pendingPropertyChanges.put(propKey + "/" + histKey, props.get(propKey));
    }

    public GroupsSeriesDataset getScalarDataset() {
        return scalarDataset;
    }

    public XYDataset getXyDataset() {
        return xyDataset;
    }

    public HistogramDataset getHistogramDataset() {
        return histogramDataset;
    }

    @Override
    public boolean isEmpty() {
        return scalarDataset.getSeriesCount() == 0
                && scalarDataset.getGroupCount() == 0
                && xyDataset.getSeriesCount() == 0
                && histogramDataset.getSeriesCount() == 0;
    }

    protected void doSetProperty(String key, String value) {
        final RuntimeException exc[] = new RuntimeException[] { null };
        Display.getDefault().syncExec(() -> {
            try {
                plot.setProperty(key, value);
            }
            catch (RuntimeException e) {
                exc[0] = e;
            }
        });
        if (exc[0] != null)
            throw exc[0];
    }

    protected void doSetProperties(Map<String, String> props) {
        final RuntimeException exc[] = new RuntimeException[] { null };
        Display.getDefault().syncExec(() -> {
            try {
                plot.runBatchedUpdates(() -> {
                    for (String k : props.keySet())
                        plot.setProperty(k, props.get(k));
                });
            }
            catch (RuntimeException e) {
                exc[0] = e;
            }
        });
        if (exc[0] != null)
            throw exc[0];
    }

    public void applyPendingPropertyChanges() {
        try {
            doSetProperties(pendingPropertyChanges);
        }
        finally {
            pendingPropertyChanges.clear();
        }
    }

    @Override
    public void setProperty(String key, String value) {
        pendingPropertyChanges.put(key, value);
    }

    @Override
    public void setProperties(Map<String, String> properties) {
        pendingPropertyChanges.putAll(properties);
    }

    @Override
    public void setGroupTitles(List<String> titles) {
         scalarDataset.setGroupTitles(titles);
    }

    @Override
    public void setWarning(String warning) {
        Display.getDefault().syncExec(() -> {
            plot.setWarningText(warning);
        });
    }

    public void reset() {
        if (xyDataset != null)
            xyDataset.dispose();

        scalarDataset = new GroupsSeriesDataset(null);
        xyDataset = new XYDataset(null);
        histogramDataset = new HistogramDataset(null);
    }

    public void dispose() {
        if (xyDataset != null)
            xyDataset.dispose();
    }

    @Override
    public Set<String> getSupportedPropertyKeys() {
        Set<String> result = new HashSet<String>();
        result.addAll(Arrays.asList(plot.getPropertyNames()));
        return result;
    }
}
