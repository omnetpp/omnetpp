package org.omnetpp.scave.python;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IScalarDataset;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Unpickler;

public class PythonScalarDataset implements IScalarDataset {
    String title;
    ArrayList<String> rowKeys = new ArrayList<String>();
    ArrayList<String> columnKeys = new ArrayList<String>();
    ArrayList<double[]> columns = new ArrayList<double[]>();

    public PythonScalarDataset(String title) {
        this.title = title;
    }

    @SuppressWarnings("unchecked")
    public void addValues(byte[] pickledData) {
        Unpickler unpickler = new Unpickler();

        try {
            Map<String, Object> data = (Map<String, Object>)unpickler.loads(pickledData);

            ArrayList<String> rowKeys = (ArrayList<String>)data.get("rowKeys");
            ArrayList<String> columnKeys = (ArrayList<String>)data.get("columnKeys");

            if (this.rowKeys.isEmpty())
                this.rowKeys = rowKeys;

            Assert.isTrue(this.rowKeys.equals(rowKeys));

            this.columnKeys.addAll(columnKeys);

            double[] doubleValues = ResultPicklingUtils.bytesToDoubles((byte[])data.get("values"));

            int numRows = doubleValues.length / columnKeys.size();
            for (int i = 0; i < columnKeys.size(); ++i)
                columns.add(Arrays.copyOfRange(doubleValues, i * numRows, (i + 1) * numRows));
        }
        catch (PickleException | IOException e) {
            ScavePlugin.logError(e);
        }
    }

    public void addColumn(String columnKey, byte[] valuesToAdd) {
        columnKeys.add(columnKey);
        columns.add(ResultPicklingUtils.bytesToDoubles(valuesToAdd));
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
        return columnKeys.size();
    }

    @Override
    public String getColumnKey(int column) {
        return columnKeys.get(column);
    }

    @Override
    public double getValue(int row, int column) {
        return columns.get(column)[row];
    }
}
