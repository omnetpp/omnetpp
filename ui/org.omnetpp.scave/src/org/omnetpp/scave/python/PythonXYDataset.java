package org.omnetpp.scave.python;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.engine.ScaveEngine;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Unpickler;

public class PythonXYDataset implements IXYDataset {
    String title;

    class SeriesData {
        String key;
        String title;
        ByteBuffer xs;
        ByteBuffer ys;

        public void dispose() {
            if (xs != null && xs.capacity() != 0)
                ScaveEngine.unmapSharedMemory(xs);
            if (ys != null && ys.capacity() != 0)
                ScaveEngine.unmapSharedMemory(ys);
        }
    }

    protected String generateUniqueKey() {
        int maxKey = 0;
        for (SeriesData sd: series) {
            try {
                int key = Integer.parseInt(sd.key);
                if (key > maxKey)
                    maxKey = key;
            }
            catch (Exception e) {
                // ignore
            }
        }
        return Integer.toString(maxKey + 1);
    }

    ArrayList<SeriesData> series = new ArrayList<SeriesData>();

    public void dispose() {
        for (SeriesData sd : series)
            sd.dispose();
        series.clear();
    }

    public PythonXYDataset(String title) {
        this.title = title;
    }

    private ByteBuffer map(String nameAndSize) {
        String[] parts = nameAndSize.split(" ");
        String name = parts[0];
        long size = Long.parseLong(parts[1]);

        if (name.equals("<EMPTY>") && size == 0)
            return ByteBuffer.allocate(0);
        else
            return (ByteBuffer)ScaveEngine.mapSharedMemory(name, size);
    }

    public List<String> addVectors(byte[] pickledData) {
        // TODO FIXME: should take props as parameter, and set them here instead of returning line keys
        Unpickler unpickler = new Unpickler();
        List<String> keys = new ArrayList<String>();

        try {
            @SuppressWarnings("unchecked")
            List<Map<String, Object>> data = (List<Map<String, Object>>) unpickler.loads(pickledData);

            for (Map<String, Object> d : data) {

                SeriesData seriesData = new SeriesData();

                String key = (String) d.get("key");
                if (key == null)
                    key = generateUniqueKey();

                seriesData.key = key;
                keys.add(key);
                seriesData.title = (String) d.get("title");

                String xName = (String)d.get("xs");
                String yName = (String)d.get("ys");

                seriesData.xs = map(xName);
                seriesData.ys = map(yName);

                series.add(seriesData);
            }

        }
        catch (PickleException | IOException e) {
            ScavePlugin.logError(e);
        }

        return keys;
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
    public String getSeriesTitle(int series) {
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
        return this.series.get(series).xs.capacity() / 8;
    }

    @Override
    public double getX(int series, int item) {
        return this.series.get(series).xs.getDouble(item*8);
    }

    @Override
    public BigDecimal getPreciseX(int series, int item) {
        return new BigDecimal(getX(series, item));
    }

    @Override
    public double getMinX() {
        double min = Double.POSITIVE_INFINITY;

        for (SeriesData sd : series)
            for (int i = 0; i < sd.xs.capacity() / 8; ++i) {
                double x = sd.xs.getDouble(i*8);
                if (x < min)
                    min = x;
            }

        return min;
    }

    @Override
    public double getMaxX() {
        double max = Double.NEGATIVE_INFINITY;

        for (SeriesData sd : series)
            for (int i = 0; i < sd.xs.capacity() / 8; ++i) {
                double x = sd.xs.getDouble(i*8);
                if (x > max)
                    max = x;
            }

        return max;
    }

    @Override
    public double getY(int series, int item) {
        return this.series.get(series).ys.getDouble(item*8);
    }

    @Override
    public BigDecimal getPreciseY(int series, int item) {
        return new BigDecimal(getY(series, item));
    }

    @Override
    public double getMinY() {
        double min = Double.POSITIVE_INFINITY;

        for (SeriesData sd : series)
            for (int i = 0; i < sd.ys.capacity() / 8; ++i) {
                double y = sd.ys.getDouble(i*8);
                if (y < min)
                    min = y;
            }

        return min;
    }

    @Override
    public double getMaxY() {
        double max = Double.NEGATIVE_INFINITY;

        for (SeriesData sd : series)
            for (int i = 0; i < sd.ys.capacity() / 8; ++i) {
                double y = sd.ys.getDouble(i*8);
                if (y > max)
                    max = y;
            }

        return max;
    }

}
