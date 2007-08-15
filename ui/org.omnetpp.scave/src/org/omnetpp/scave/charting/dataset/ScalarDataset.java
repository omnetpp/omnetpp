package org.omnetpp.scave.charting.dataset;

import static org.omnetpp.scave.engine.ScalarFields.ALL;
import static org.omnetpp.scave.engine.ScalarFields.EXPERIMENT;
import static org.omnetpp.scave.engine.ScalarFields.FILE;
import static org.omnetpp.scave.engine.ScalarFields.MEASUREMENT;
import static org.omnetpp.scave.engine.ScalarFields.MODULE;
import static org.omnetpp.scave.engine.ScalarFields.NAME;
import static org.omnetpp.scave.engine.ScalarFields.REPLICATION;
import static org.omnetpp.scave.engine.ScalarFields.RUN;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.IDVector;
import org.omnetpp.scave.engine.IDVectorVector;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarDataSorter;
import org.omnetpp.scave.engine.ScalarFields;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.XYDataset;
import org.omnetpp.scave.model2.RunAttribute;

/**
 * Class storing the dataset of a scalar chart.
 *
 * @author tomi
 */
// TODO check dependencies between result item fields
public class ScalarDataset implements IScalarDataset {
	
	private static ScalarFields defaultGrouping = new ScalarFields(ALL, MODULE);

	/** The row keys. */
    private List<String> rowKeys;

    /** The column keys. */
    private List<String> columnKeys;

    /** The data, each row is a group. */
    private XYDataset data;

    /**
     * Creates a dataset from the given scalars.
     * The columns are the different module names found.
     * Each different value of the other fields gives a row.  
     */
    public ScalarDataset(IDList idlist, ResultFileManager manager) {
    	this(idlist, defaultGrouping, manager);
    }
    
    /**
     * Creates a dataset from the given scalars.
     * Groups are formed by {@code groupingFields}, other fields
     * determines the columns. 
     */
    public ScalarDataset(IDList idlist, int groupingFields, ResultFileManager manager) {
    	this(idlist, new ScalarFields(groupingFields), manager);
    }
    
    /**
     * Creates a dataset from the given scalars.
     * Groups are formed by {@code groupingFields}, other fields
     * determines the columns. 
     */
    public ScalarDataset(IDList idlist, ScalarFields groupingFields, ResultFileManager manager) {
    	if (groupingFields == null)
    		groupingFields = defaultGrouping;
    	ScalarDataSorter sorter = new ScalarDataSorter(manager);
    	this.data = sorter.groupAndAggregate(idlist, groupingFields, groupingFields.complement());
    	this.data.sortRows();
    	this.data.sortColumns();
    	this.rowKeys = computeRowKeys(this.data);
    	this.columnKeys = computeColumnKeys(this.data);
    }

    /**
     * Returns the row count.
     *
     * @return The row count.
     */
    public int getRowCount() {
        return this.rowKeys.size();
    }
    
    
    /**
     * Returns the key for a given row.
     *
     * @param row  the row index (zero based).
     *
     * @return The row index.
     */
    public String getRowKey(int row) {
        return this.rowKeys.get(row);
    }

    /**
     * Returns the column count.
     *
     * @return The column count.
     */
    public int getColumnCount() {
        return this.columnKeys.size();
    }
    
    /**
     * Returns the key for a given column.
     *
     * @param column  the column.
     *
     * @return The key.
     */
    public String getColumnKey(int column) {
        return this.columnKeys.get(column);
    }

    /**
     * Returns the value for a given row and column.
     *
     * @param row  the row index.
     * @param column  the column index.
     *
     * @return The value.
     */
    public double getValue(int row, int column) {
       	return data.getValue(row, column);
    }
    
    private static final int[] allFields = new int[] {FILE, RUN, MODULE, NAME, EXPERIMENT, MEASUREMENT, REPLICATION };
    private static final char separator = ';';
    
    private static List<String> computeRowKeys(XYDataset data) {
    	List<String> keys = new ArrayList<String>(data.getRowCount());
    	ScalarFields fields = data.getRowFields();
    	for (int i = 0; i < data.getRowCount(); ++i) {
        	StringBuffer sb = new StringBuffer();
        	for (int field : allFields) {
        		if (fields.hasField(field))
        			sb.append(data.getRowField(i, field)).append(separator);
        	}
        	if (sb.length() > 0)  // delete last separator
        		sb.deleteCharAt(sb.length()-1);
        	keys.add(sb.toString());
    	}
    	return keys;
    }
    
    private static List<String> computeColumnKeys(XYDataset data) {
    	int count = data.getColumnCount();
    	List<String> keys = new ArrayList<String>(count);
    	ScalarFields fields = data.getColumnFields();
    	for (int i = 0; i < count; ++i) {
        	StringBuffer sb = new StringBuffer();
        	for (int field : allFields) {
        		if (fields.hasField(field))
        			sb.append(data.getColumnField(i, field)).append(separator);
        	}
        	if (sb.length() > 0)  // delete last separator
        		sb.deleteCharAt(sb.length()-1);
        	keys.add(sb.toString());
    	}
    	return keys;
    }
}
