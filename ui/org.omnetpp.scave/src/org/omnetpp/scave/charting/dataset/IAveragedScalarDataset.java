package org.omnetpp.scave.charting.dataset;

import org.omnetpp.scave.engine.Statistics;

public interface IAveragedScalarDataset extends IScalarDataset {

	/**
     * Returns the collected statistics for the value associated with the cell.
     * 
     * @param row  the row index
     * @param column  the column index
     * @return the statistics
     */
    Statistics getStatistics(int row, int column);
}
