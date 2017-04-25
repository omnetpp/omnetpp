/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.DoubleVector;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.model2.DatasetManager;

public class HistogramDataset implements IHistogramDataset {

    private static class HistogramData {
        String key;
        boolean isIntegerType;
        long count;
        double minValue;
        double maxValue;
        double[] cellBreaks;
        double[] cellValues;

        public HistogramData(String key, boolean isIntegerType, long count, double minValue, double maxValue, double[] cellBreaks, double[] cellValues) {
            this.key = key;
            this.isIntegerType = isIntegerType;
            this.count = count;
            this.minValue = minValue;
            this.maxValue = maxValue;
            this.cellBreaks = cellBreaks;
            this.cellValues = cellValues;
        }
    }

    private HistogramData[] histograms;

    public HistogramDataset(IDList idlist, ResultFileManager manager) {
        Assert.isLegal(idlist.areAllHistograms());
        String[] keys = DatasetManager.getResultItemNames(idlist, null, manager);
        Assert.isTrue(keys.length == idlist.size());
        histograms = new HistogramData[idlist.size()];
        for (int i = 0; i < idlist.size(); ++i) {
            long id = idlist.get(i);
            HistogramResult histogram = manager.getHistogram(id);
            boolean isIntegerType = histogram.getDataType() == ResultItem.DataType.TYPE_INT;
            DoubleVector bins = histogram.getHistogram().getBinLowerBounds();
            int size = (int)bins.size();
            double[] cellBreaks = new double[size+1];
            System.arraycopy(bins.toArray(), 0, cellBreaks, 0, size);
            cellBreaks[size] = Double.POSITIVE_INFINITY;
            double[] cellValues = histogram.getHistogram().getBinValues().toArray();
            Statistics stat = histogram.getStatistics();
            histograms[i] = new HistogramData(keys[i], isIntegerType, stat.getCount(), stat.getMin(), stat.getMax(), cellBreaks, cellValues);
        }
    }

    public String getTitle(String format) {
        // TODO Auto-generated method stub
        return null;
    }

    public int getSeriesCount() {
        return histograms.length;
    }

    public String getSeriesKey(int series) {
        return histograms[series].key;
    }

    public boolean isIntegerType(int series) {
        return histograms[series].isIntegerType;
    }

    public long getValueCount(int series) {
        return histograms[series].count;
    }

    public double getMinValue(int series) {
        return histograms[series].minValue;
    }

    public double getMaxValue(int series) {
        return histograms[series].maxValue;
    }

    public int getCellsCount(int series) {
        return histograms[series].cellValues.length;
    }

    public double getCellLowerBound(int series, int index) {
        return histograms[series].cellBreaks[index];
    }

    public double getCellUpperBound(int series, int index) {
        return histograms[series].cellBreaks[index+1];
    }

    public double getCellValue(int series, int index) {
        return histograms[series].cellValues[index];
    }
}
