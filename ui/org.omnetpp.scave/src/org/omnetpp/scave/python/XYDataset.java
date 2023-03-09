/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.python;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.engine.ScaveEngine;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Unpickler;

public class XYDataset implements IXYDataset {

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

                for (SeriesData sd : series)
                    if (sd.key.equals(key))
                        System.out.println("WARNING: Series key '" + key + "' is not unique in XYDataset!");

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
    public double getMinX() {
        double min = Double.POSITIVE_INFINITY;

        for (int i = 0; i < series.size(); ++i)
            min = Double.min(min, getMinX(i));

        return min;
    }

    @Override
    public double getMaxX() {
        double max = Double.NEGATIVE_INFINITY;

        for (int i = 0; i < series.size(); ++i)
            max = Double.max(max, getMaxX(i));

        return max;
    }

    @Override
    public double getMinX(int series) {
        double min = Double.POSITIVE_INFINITY;

        SeriesData sd = this.series.get(series);
        for (int i = 0; i < sd.xs.capacity() / 8; ++i)
            min = Double.min(min, sd.xs.getDouble(i*8));

        return min;
    }

    @Override
    public double getMaxX(int series) {
        double max = Double.NEGATIVE_INFINITY;

        SeriesData sd = this.series.get(series);
        for (int i = 0; i < sd.xs.capacity() / 8; ++i)
            max = Double.max(max, sd.xs.getDouble(i*8));

        return max;
    }

    @Override
    public double getY(int series, int item) {
        return this.series.get(series).ys.getDouble(item*8);
    }

    @Override
    public double getMinY() {
        double min = Double.POSITIVE_INFINITY;

        for (int i = 0; i < series.size(); ++i)
            min = Double.min(min, getMinY(i));

        return min;
    }

    @Override
    public double getMaxY() {
        double max = Double.NEGATIVE_INFINITY;

        for (int i = 0; i < series.size(); ++i)
            max = Double.max(max, getMaxY(i));

        return max;
    }

    @Override
    public double getMinY(int series) {
        double min = Double.POSITIVE_INFINITY;

        SeriesData sd = this.series.get(series);
        for (int i = 0; i < sd.ys.capacity() / 8; ++i)
            min = Double.min(min, sd.ys.getDouble(i*8));

        return min;
    }

    @Override
    public double getMaxY(int series) {
        double max = Double.NEGATIVE_INFINITY;

        SeriesData sd = this.series.get(series);
        for (int i = 0; i < sd.ys.capacity() / 8; ++i)
            max = Double.max(max, sd.ys.getDouble(i*8));

        return max;
    }

    private static String formatNumber(double number) {
        return StringUtils.formatNumber(number);
    }


    @Override
    public String getXAsString(int series, int item) {
        return formatNumber(getX(series, item));
    }

    @Override
    public String getYAsString(int series, int item) {
        return formatNumber(getY(series, item));
    }

}
