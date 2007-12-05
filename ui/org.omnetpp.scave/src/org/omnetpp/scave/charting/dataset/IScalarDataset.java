package org.omnetpp.scave.charting.dataset;


/**
 * Interface for datasets displayed on scalar charts.
 *
 * @author tomi
 */
public interface IScalarDataset extends IDataset {
	
    /**
     * Returns the row count.
     *
     * @return The row count.
     */
    public int getRowCount();
	
    /**
     * Returns the row key for a given index.
     *
     * @param row  the row index (zero-based).
     * @return The row key.
     * @throws IndexOutOfBoundsException if <code>row</code> is out of bounds.
     */
    public String getRowKey(int row);

    /**
     * Returns the column count.
     *
     * @return The column count.
     */
    public int getColumnCount();
    
    /**
     * Returns the column key for a given index.
     *
     * @param column  the column index (zero-based).
     * @return The column key.
     * @throws IndexOutOfBoundsException if <code>column</code> is out of bounds.
     */
    public String getColumnKey(int column);

    /**
     * Returns the value associated with the specified cell.
     *
     * @param row  the row index
     * @param column  the column index
     * @return The value.
     * @throws IndexOutOfBoundsException if <code>row</code> or <code>column</code> is out of bounds.
     */
    public double getValue(int row, int column);
}
