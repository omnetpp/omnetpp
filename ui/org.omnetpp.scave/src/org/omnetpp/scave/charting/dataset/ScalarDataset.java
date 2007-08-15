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

    /** The row data. */
    private List<List<Double>> rows;

    /**
     * Creates an empty dataset.
     */
    public ScalarDataset() {
        this.rowKeys = new ArrayList<String>();
        this.columnKeys = new ArrayList<String>();
        this.rows = new ArrayList<List<Double>>();
    }
    
    /**
     * Creates a dataset from the given scalars.
     * The 
     * @param idlist   the scalars contained by this dataset
     * @param manager
     */
    public ScalarDataset(IDList idlist, ResultFileManager manager) {
    	this(idlist, defaultGrouping, manager);
    }
    
    public ScalarDataset(IDList idlist, int groupingFields, ResultFileManager manager) {
    	this(idlist, new ScalarFields(groupingFields), manager);
    }
    
    public ScalarDataset(IDList idlist, ScalarFields groupingFields, ResultFileManager manager) {
    	this();
    	if (groupingFields == null)
    		groupingFields = defaultGrouping;
    	addScalars(idlist, groupingFields, manager);
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
        double result = Double.NaN;
        if (row >= 0 && row < rows.size()) {
            List<Double> rowData = this.rows.get(row);
            if (column >= 0 && column < rowData.size())
        	result = rowData.get(column);
        }
        return result;
    }
    
    private void addScalars(IDList idlist, ScalarFields groupingFields, ResultFileManager manager) {
    	ScalarFields nongroupingFields = groupingFields.complement();
    	ScalarDataSorter sorter = new ScalarDataSorter(manager);
    	IDVectorVector groups = sorter.groupByFields(idlist, groupingFields);
    	
    	for (int rowIndex=0; rowIndex < groups.size(); ++rowIndex) {
    		IDVector group = groups.get(rowIndex);
    		String rowKey = null;
    		List<Double> row = new ArrayList<Double>((int)group.size());
    		for (int colIndex = 0; colIndex < group.size(); ++colIndex) {
    			// add place for key for the column if this is the first row 
    			// (each group has the same number of ids)
    			if (rowIndex == 0)
        			columnKeys.add(null);

        		long id = group.get(colIndex);
    			if (id != -1) {
    				ScalarResult scalar = manager.getScalar(id);
    				if (rowKey == null)
    					rowKey = keyFor(scalar, groupingFields);
    				if (columnKeys.get(colIndex) == null)
    					columnKeys.set(colIndex, keyFor(scalar, nongroupingFields));
    				row.add(scalar.getValue());
    			}
    			else {
    				row.add(Double.NaN);
    			}
    		}
    		
    		// add non-empty rows
    		if (rowKey != null) {
    			rowKeys.add(rowKey);
    			rows.add(row);
    		}
    	}
    	
    	// remove empty columns
    	int colIndex;
    	while ((colIndex=columnKeys.indexOf(null)) >= 0) {
    		columnKeys.remove(colIndex);
    		for (List<Double> row : rows) {
    			if (colIndex < row.size())
    				row.remove(colIndex);
    		}
    	}
    }
    
    private String keyFor(ScalarResult scalar, ScalarFields fields) {
    	StringBuffer sb = new StringBuffer();
    	char sep = ';';
    	if (fields.hasField(FILE)) sb.append(scalar.getFileRun().getFile().getFileName()).append(sep);
    	if (fields.hasField(RUN)) sb.append(scalar.getFileRun().getRun().getRunName()).append(sep);
    	if (fields.hasField(MODULE)) sb.append(scalar.getModuleName()).append(sep);
    	if (fields.hasField(NAME)) sb.append(scalar.getName()).append(sep);
    	if (fields.hasField(EXPERIMENT)) sb.append(RunAttribute.getRunAttribute(scalar.getFileRun().getRun(), RunAttribute.EXPERIMENT)).append(sep);
    	if (fields.hasField(MEASUREMENT)) sb.append(RunAttribute.getRunAttribute(scalar.getFileRun().getRun(), RunAttribute.MEASUREMENT)).append(sep);
    	if (fields.hasField(REPLICATION)) sb.append(RunAttribute.getRunAttribute(scalar.getFileRun().getRun(), RunAttribute.REPLICATION)).append(sep);
    	if (sb.length() > 0) sb.deleteCharAt(sb.length()-1); // delete last ','
    	return sb.toString();
    }
}
