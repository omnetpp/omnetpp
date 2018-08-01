package org.omnetpp.scave.python;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.charting.dataset.IXYDataset;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Unpickler;

public class PythonXYDataset implements IXYDataset {
    String title;

    class SeriesData {
        String key;
        String title;
        double[] xs;
        double[] ys;
    }

    ArrayList<SeriesData> series = new ArrayList<SeriesData>();

    public PythonXYDataset(String title) {
        this.title = title;
    }

    public void addVectors(byte[] pickledData) {
        Unpickler unpickler = new Unpickler();

        try {
            @SuppressWarnings("unchecked")
            List<Map<String, Object>> data = (List<Map<String, Object>>) unpickler.loads(pickledData);

            for (Map<String, Object> d : data) {

                SeriesData seriesData = new SeriesData();

                seriesData.key = (String) d.get("key");
                seriesData.title = (String) d.get("title");

                seriesData.xs = ResultPicklingUtils.bytesToDoubles((byte[]) d.get("xs"));
                seriesData.ys = ResultPicklingUtils.bytesToDoubles((byte[]) d.get("ys"));

                series.add(seriesData);
            }

        }
        catch (PickleException | IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Override
    public String getTitle(String format) {
        return title;
    }

    @Override
    public int getSeriesCount() {
        return series.size();
    }

    @Override
    public String getSeriesKey(int series) {
        return this.series.get(series).key;
    }

    @Override
    public String getSeriesTitle(int series, String format) {
        return this.series.get(series).title;
    }

    @Override
    public Type getSeriesType(int series) {
        return Type.Double;
    }

    @Override
    public InterpolationMode getSeriesInterpolationMode(int series) {
        return InterpolationMode.Linear;
    }

    @Override
    public int getItemCount(int series) {
        return this.series.get(series).xs.length;
    }

    @Override
    public double getX(int series, int item) {
        return this.series.get(series).xs[item];
    }

    @Override
    public BigDecimal getPreciseX(int series, int item) {
        return new BigDecimal(getX(series, item));
    }

    @Override
    public double getMinX() {
        double min = Double.POSITIVE_INFINITY;

        for (SeriesData sd : series)
            for (double x : sd.xs)
                if (x < min)
                    min = x;

        return min;
    }

    @Override
    public double getMaxX() {
        double max = Double.NEGATIVE_INFINITY;

        for (SeriesData sd : series)
            for (double x : sd.xs)
                if (x > max)
                    max = x;

        return max;
    }

    @Override
    public double getY(int series, int item) {
        return this.series.get(series).ys[item];
    }

    @Override
    public BigDecimal getPreciseY(int series, int item) {
        return new BigDecimal(getY(series, item));
    }

    @Override
    public double getMinY() {
        double min = Double.POSITIVE_INFINITY;

        for (SeriesData sd : series)
            for (double y : sd.ys)
                if (y < min)
                    min = y;

        return min;
    }

    @Override
    public double getMaxY() {
        double max = Double.NEGATIVE_INFINITY;

        for (SeriesData sd : series)
            for (double y : sd.ys)
                if (y > max)
                    max = y;

        return max;
    }

}
