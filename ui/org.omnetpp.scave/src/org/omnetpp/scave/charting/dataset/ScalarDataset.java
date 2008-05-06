package org.omnetpp.scave.charting.dataset;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;
import static org.omnetpp.scave.engine.ResultItemField.RUN_ATTR_ID;
import static org.omnetpp.scave.engine.ResultItemField.RUN_ID;
import static org.omnetpp.scave.engine.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.engine.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.engine.RunAttribute.REPLICATION;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.engine.ScalarDataSorter;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.XYDataset;
import org.omnetpp.scave.model2.StatUtils;

/**
 * Class representing the dataset of a scalar chart.
 *
 * @author tomi
 */
public class ScalarDataset implements IAveragedScalarDataset {
	
	private ResultItemFields rowFields, columnFields;
	
	/** The row keys. */
    private List<String> rowKeys;

    /** The column keys. */
    private List<String> columnKeys;

    /** The data, each row is a group. */
    private XYDataset data;

    /**
     * Creates a dataset from the given scalars.
     * Groups are formed by {@code groupingFields}, other fields
     * determines the columns. 
     */
    public ScalarDataset(IDList idlist, List<String> groupByFields, List<String> barFields,
    						List<String> averagedFields, ResultFileManager manager) {
   		computeFields(groupByFields, barFields, averagedFields, idlist, manager);
   		ScalarDataSorter sorter = new ScalarDataSorter(manager);
    	this.data = sorter.groupAndAggregate(idlist, rowFields, columnFields);
    	this.data.sortRows();
    	this.data.sortColumns();
    	this.rowKeys = computeRowKeys(this.data, rowFields);
    	this.columnKeys = computeColumnKeys(this.data, columnFields);
    }
    
    public String getTitle(String format) {
		// TODO Auto-generated method stub
		return null;
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
       	return data.getValue(row, column).mean();
    }
    
	/**
	 * Returns the confidence interval for a given row and column.
	 * 
	 * @see IScalarDataset#getConfidenceInterval(int, int, double)
	 */
    public double getConfidenceInterval(int row, int column, double p) {
		return StatUtils.confidenceInterval(data.getValue(row, column), p);
	}
    
    private void computeFields(List<String> groupByFields, List<String> barFields, List<String> averagedFields,
    							IDList idlist, ResultFileManager manager) {
   		List<String> fields = Arrays.asList(ResultItemFields.getFieldNames().toArray());
   		List<String> rowFields = new ArrayList<String>();
   		List<String> columnFields = new ArrayList<String>();
   		List<String> unusedFields = new ArrayList<String>();
   		unusedFields.addAll(fields);
   		
   		if (groupByFields != null) {
   			rowFields.addAll(groupByFields);
   			unusedFields.removeAll(groupByFields);
   		}
   		if (barFields != null) {
   			columnFields.addAll(barFields);
   			unusedFields.removeAll(barFields);
   		}
   		if (averagedFields != null)
   			unusedFields.removeAll(averagedFields);

   		if (!unusedFields.isEmpty()) {
	   		Map<String,List<String>> dependencies = buildDependencyMap(fields, idlist, manager);
	   		rowFields = addDependentFields(rowFields, unusedFields, dependencies);
	   		columnFields = addDependentFields(columnFields, unusedFields, dependencies);
	   		applyDefaults(rowFields, columnFields, unusedFields);
   		}
   		
   		//System.out.format("Row fields: %s%n", StringUtils.formatList(rowFields, "%s", ","));
   		//System.out.format("Column fields: %s%n", StringUtils.formatList(columnFields, "%s", ","));

   		this.rowFields = new ResultItemFields(StringVector.fromArray(rowFields.toArray(new String[rowFields.size()])));
   		this.columnFields = new ResultItemFields(StringVector.fromArray(columnFields.toArray(new String[columnFields.size()])));
    }
    
    private Map<String,List<String>> buildDependencyMap(List<String> fields, IDList idlist, ResultFileManager manager) {
    	int size = fields.size();
    	Map<String,List<String>> map = new HashMap<String,List<String>>();
    	List<String> dependents;
    	for (int i = 0; i < size; ++i) {
    		ResultItemField f1 = new ResultItemField(fields.get(i));
    		map.put(f1.getName(), dependents = new ArrayList<String>());
    		for (int j = 0; j < size; ++j) {
    			ResultItemField f2 = new ResultItemField(fields.get(j));
    			if (i == j)
    				dependents.add(f2.getName());
    			else if (f1.getID() == RUN_ID && f2.getID() == RUN_ATTR_ID)
    				dependents.add(f2.getName());
    			else if (f1.getName().equals(REPLICATION) &&
    					 (f2.getName().equals(MEASUREMENT) || f2.getName().equals(EXPERIMENT)))
    				dependents.add(f2.getName());
    			else if (f1.getName().equals(MEASUREMENT) && f2.getName().equals(EXPERIMENT))
    				dependents.add(f2.getName());
    		}
    	}
    	return map;
    }
    
    private static List<String> addDependentFields(List<String> fields, List<String> unusedFields, Map<String,List<String>> dependencies) {
    	if (unusedFields.isEmpty())
    		return fields;
    	
   		List<String> result = new ArrayList<String>();
    	for (String field1 : fields) {
    		result.add(field1);
   			for (String field2 : dependencies.get(field1)) {
   				int index = unusedFields.indexOf(field2);
   				if (index >= 0) {
   					result.add(field2);
   					unusedFields.remove(index);
   				}
   			}
   		}
    	return result;
    }
    
    private static void applyDefaults(List<String> rowFields, List<String> columnFields, List<String> unusedFields) {
    	for (String field : unusedFields) {
    		if (field.equals(FILE) || field.equals(MODULE))
    			rowFields.add(field);
    		else if (field.equals(NAME))
    			columnFields.add(field);
    		// other fields are averaged
    	}
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
