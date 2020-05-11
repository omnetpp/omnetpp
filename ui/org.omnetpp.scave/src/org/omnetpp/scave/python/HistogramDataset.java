/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.python;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Unpickler;

public class HistogramDataset implements IHistogramDataset {

    class HistogramData {
        String key;
        String title;
        double sumWeights;
        double minValue;
        double maxValue;
        double[] binEdges; // N+1, >= 2
        double[] binValues; // N, >= 1
        double underflows;
        double overflows;
    }

    ArrayList<HistogramData> histograms = new ArrayList<HistogramData>();

    protected String generateUniqueKey() {
        int maxKey = 0;
        for (HistogramData hd: histograms) {
            try {
                int key = Integer.parseInt(hd.key);
                if (key > maxKey)
                    maxKey = key;
            }
            catch (Exception e) {
                // ignore
            }
        }
        return Integer.toString(maxKey + 1);
    }

    public List<String> addValues(byte[] pickledData) {
        Unpickler unpickler = new Unpickler();
        List<String> keys = new ArrayList<String>();

        try {
            @SuppressWarnings("unchecked")
            List<Map<String, Object>> data = (List<Map<String, Object>>) unpickler.loads(pickledData);

            for (Map<String, Object> d : data) {

                HistogramData histogramData = new HistogramData();

                String key = (String) d.get("key");
                if (key == null)
                    key = generateUniqueKey();

                for (HistogramData hd : histograms)
                    if (hd.key.equals(key))
                        System.out.println("WARNING: Series key '" + key + "' is not unique in HistogramDataset!");

                histogramData.key = key;
                keys.add(key);
                histogramData.title = (String) d.get("title");

                histogramData.sumWeights = (Double) d.get("sumweights");

                histogramData.minValue = (Double) d.get("min");
                histogramData.maxValue = (Double) d.get("max");

                histogramData.binEdges = ResultPicklingUtils.bytesToDoubles((byte[]) d.get("edges"));
                histogramData.binValues = ResultPicklingUtils.bytesToDoubles((byte[]) d.get("values"));

                histogramData.underflows = (Double) d.get("underflows");
                histogramData.overflows = (Double) d.get("overflows");

                histograms.add(histogramData);
            }

        }
        catch (PickleException | IOException e) {
            ScavePlugin.logError(e);
        }

        return keys;
    }

    @Override
    public int getSeriesCount() {
        return histograms.size();
    }

    @Override
    public String getSeriesKey(int series) {
        return histograms.get(series).key;
    }

    @Override
    public String getSeriesTitle(int series) {
        return histograms.get(series).title;
    }

    @Override
    public boolean isIntegerType(int series) {
        return false;
    }

    @Override
    public double getSumWeights(int series) {
        return histograms.get(series).sumWeights;
    }

    @Override
    public double getMinValue(int series) {
        return histograms.get(series).minValue;
    }

    @Override
    public double getMaxValue(int series) {
        return histograms.get(series).maxValue;
    }

    @Override
    public double getUnderflows(int series) {
        return histograms.get(series).underflows;
    }

    @Override
    public double getOverflows(int series) {
        return histograms.get(series).overflows;
    }

    @Override
    public int getCellsCount(int series) {
        return histograms.get(series).binValues.length;
    }

    @Override
    public double getCellLowerBound(int series, int index) {
        return histograms.get(series).binEdges[index];
    }

    @Override
    public double getCellUpperBound(int series, int index) {
        return histograms.get(series).binEdges[index + 1];
    }

    @Override
    public double getCellValue(int series, int index) {
        return histograms.get(series).binValues[index];
    }

}
