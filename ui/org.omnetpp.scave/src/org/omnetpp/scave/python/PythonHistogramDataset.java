package org.omnetpp.scave.python;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Unpickler;

public class PythonHistogramDataset implements IHistogramDataset {
    String title;

    class HistogramData {
        String key;
        String title;
        double sumWeights;
        double minValue;
        double maxValue;
        double[] binEdges; // N+1, >= 2
        double[] binValues; // N, >= 1
    }

    ArrayList<HistogramData> histograms = new ArrayList<HistogramData>();

    public PythonHistogramDataset(String title) {
        this.title = title;
    }

    public void addValues(byte[] pickledData) {
        Unpickler unpickler = new Unpickler();

        try {
            @SuppressWarnings("unchecked")
            List<Map<String, Object>> data = (List<Map<String, Object>>) unpickler.loads(pickledData);

            for (Map<String, Object> d : data) {

                HistogramData histogramData = new HistogramData();

                String key = (String) d.get("key");
                // TODO: if missing (null), generate a unique one, also, return the list of keys
                for (HistogramData hd : histograms)
                    if (hd.key.equals(key))
                        System.out.println("WARNING: Series key '" + key + "' is not unique in HistogramDataset!");

                histogramData.key = key;

                histogramData.title = (String) d.get("title");

                histogramData.sumWeights = (Double) d.get("sumweights");

                histogramData.minValue = (Double) d.get("min");
                histogramData.maxValue = (Double) d.get("max");

                histogramData.binEdges = ResultPicklingUtils.bytesToDoubles((byte[]) d.get("edges"));
                histogramData.binValues = ResultPicklingUtils.bytesToDoubles((byte[]) d.get("values"));

                histograms.add(histogramData);
            }

        }
        catch (PickleException | IOException e) {
            ScavePlugin.logError(e);
        }
    }

    @Override
    public String getTitle(String format) {
        return title;
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
