package org.omnetpp.scave.python;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IGroupsSeriesDataset;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Unpickler;

public class GroupsSeriesDataset implements IGroupsSeriesDataset {
    String title;

    class Series {
        String key;
        String title;
        double[] values;
    }

    ArrayList<Series> serieses = new ArrayList<Series>();

    // X axis labels
    ArrayList<String> rowKeys = new ArrayList<String>();


    protected String generateUniqueKey() {
        int maxKey = 0;
        for (Series bs : serieses) {
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


    public GroupsSeriesDataset(String title) {
        this.title = title;
    }

    public List<String> addValues(byte[] pickledData) {
        Unpickler unpickler = new Unpickler();
        List<String> keys = new ArrayList<String>();

        try {
            @SuppressWarnings("unchecked")
            List<Map<String, Object>> data = (List<Map<String, Object>>) unpickler.loads(pickledData);

            for (Map<String, Object> d : data) {

                Series barSeries = new Series();

                String key = (String) d.get("key");
                if (key == null)
                    key = generateUniqueKey();

                for (Series bs: serieses)
                    if (bs.key.equals(key))
                        System.out.println("WARNING: Series key '" + key + "' is not unique in HistogramDataset!");

                barSeries.key = key;
                keys.add(key);
                barSeries.title = (String) d.get("title");

                barSeries.values = ResultPicklingUtils.bytesToDoubles((byte[]) d.get("edges"));

                serieses.add(barSeries);
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
    public int getGroupCount() {
        return rowKeys.size();
    }

    @Override
    public String getGroupKey(int row) {
        return rowKeys.get(row);
    }

    @Override
    public int getSeriesCount() {
        return serieses.size();
    }

    @Override
    public String getSeriesKey(int column) {
        return serieses.get(column).key;
    }

    @Override
    public double getValue(int row, int column) {
        return serieses.get(column).values[row];
    }

    @Override
    public String getValueAsString(int row, int column) {
        return String.format("%g", getValue(row, column));
    }
}
