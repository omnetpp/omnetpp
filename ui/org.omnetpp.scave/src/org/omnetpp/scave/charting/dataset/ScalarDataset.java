package org.omnetpp.scave.charting.dataset;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;
import static org.omnetpp.scave.engine.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.engine.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.engine.RunAttribute.REPLICATION;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.engine.ScalarDataSorter;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.XYDataset;

/**
 * Class storing the dataset of a scalar chart.
 *
 * @author tomi
 */
public class ScalarDataset implements IScalarDataset {
	
	private static ResultItemFields defaultGrouping = new ResultItemFields(MODULE).complement();

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
    	this(idlist, defaultGrouping, null, manager);
    }
    
    /**
     * Creates a dataset from the given scalars.
     * Groups are formed by {@code groupingFields}, other fields
     * determines the columns. 
     */
    public ScalarDataset(IDList idlist, StringVector groupingFields, ResultFileManager manager) {
    	this(idlist, new ResultItemFields(groupingFields), null, manager);
    }
    
    /**
     * Creates a dataset from the given scalars.
     * Groups are formed by {@code groupingFields}, other fields
     * determines the columns. 
     */
    public ScalarDataset(IDList idlist, ResultItemFields rowFields, ResultItemFields columnFields, ResultFileManager manager) {
    	if (rowFields == null)
    		rowFields = defaultGrouping;
    	if (columnFields == null)
    		columnFields = rowFields.complement();
    	//ScalarFields groupingFields = addDependentFields(rowFields, idlist, manager);
    	ScalarDataSorter sorter = new ScalarDataSorter(manager);
    	this.data = sorter.groupAndAggregate(idlist, rowFields, columnFields);
    	this.data.sortRows();
    	this.data.sortColumns();
    	this.rowKeys = computeRowKeys(this.data, rowFields);
    	this.columnKeys = computeColumnKeys(this.data, columnFields);
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
    
    private static final ResultItemField[] allFields = new ResultItemField[] {
    	new ResultItemField(FILE), new ResultItemField(RUN), new ResultItemField(MODULE),
    	new ResultItemField(NAME), 
    	new ResultItemField(EXPERIMENT), new ResultItemField(MEASUREMENT), new ResultItemField(REPLICATION) 
    };
    private static final char separator = ';';
    
    private static List<String> computeRowKeys(XYDataset data, ResultItemFields rowFields) {
    	List<String> keys = new ArrayList<String>(data.getRowCount());
    	for (int i = 0; i < data.getRowCount(); ++i) {
        	StringBuffer sb = new StringBuffer();
        	for (ResultItemField field : allFields) {
        		if (rowFields.hasField(field))
        			sb.append(data.getRowField(i, field)).append(separator);
        	}
        	if (sb.length() > 0)  // delete last separator
        		sb.deleteCharAt(sb.length()-1);
        	keys.add(sb.toString());
    	}
    	return keys;
    }
    
    private static List<String> computeColumnKeys(XYDataset data, ResultItemFields columnFields) {
    	int count = data.getColumnCount();
    	List<String> keys = new ArrayList<String>(count);
    	for (int i = 0; i < count; ++i) {
        	StringBuffer sb = new StringBuffer();
        	for (ResultItemField field : allFields) {
        		if (columnFields.hasField(field))
        			sb.append(data.getColumnField(i, field)).append(separator);
        	}
        	if (sb.length() > 0)  // delete last separator
        		sb.deleteCharAt(sb.length()-1);
        	keys.add(sb.toString());
    	}
    	return keys;
    }
}
