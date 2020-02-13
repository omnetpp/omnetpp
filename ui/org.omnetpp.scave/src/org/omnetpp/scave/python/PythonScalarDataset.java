package org.omnetpp.scave.python;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IScalarDataset;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Unpickler;

public class PythonScalarDataset implements IScalarDataset {
    String title;

    class BarSeries {
        String key;
        String title;
        double[] values;
    }

    ArrayList<BarSeries> series = new ArrayList<BarSeries>();

    // X axis labels
    ArrayList<String> rowKeys = new ArrayList<String>();


    protected String generateUniqueKey() {
        int maxKey = 0;
        for (BarSeries bs : series) {
            try {
                int key = Integer.parseInt(bs.key);
                if (key > maxKey)
                    maxKey = key;
            }
            catch (Exception e) {
                // ignore
            }
        }
        return Integer.toString(maxKey + 1);
    }


    public PythonScalarDataset(String title) {
        this.title = title;
    }

    public List<String> addValues(byte[] pickledData) {
        Unpickler unpickler = new Unpickler();
        List<String> keys = new ArrayList<String>();

        try {
            @SuppressWarnings("unchecked")
            List<Map<String, Object>> data = (List<Map<String, Object>>) unpickler.loads(pickledData);

            for (Map<String, Object> d : data) {

                BarSeries barSeries = new BarSeries();

                String key = (String) d.get("key");
                if (key == null)
                    key = generateUniqueKey();

                for (BarSeries bs: series)
                    if (bs.key.equals(key))
                        System.out.println("WARNING: Series key '" + key + "' is not unique in HistogramDataset!");

                barSeries.key = key;
                keys.add(key);
                barSeries.title = (String) d.get("title");

                barSeries.values = ResultPicklingUtils.bytesToDoubles((byte[]) d.get("edges"));

                series.add(barSeries);
            }
        }
        catch (PickleException | IOException e) {
            ScavePlugin.logError(e);
        }

        return keys;
    }

    public void setRowKeys(ArrayList<String> rowKeys) {
        this.rowKeys = rowKeys;
    }

    @Override
    public String getTitle(String format) {
        return title;
    }

    @Override
    public int getRowCount() {
        return rowKeys.size();
    }

    @Override
    public String getRowKey(int row) {
        return rowKeys.get(row);
    }

    @Override
    public int getColumnCount() {
        return series.size();
    }

    @Override
    public String getColumnKey(int column) {
        return series.get(column).key;
    }

    @Override
    public double getValue(int row, int column) {
        return series.get(column).values[row];
    }
}
