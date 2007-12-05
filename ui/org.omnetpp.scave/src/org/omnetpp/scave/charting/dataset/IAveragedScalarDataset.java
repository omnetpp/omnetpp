package org.omnetpp.scave.charting.dataset;

public interface IAveragedScalarDataset extends IScalarDataset {

	/**
     * Returns the half length of the confidence interval for the
     * value associated with the cell.
     * 
     * @param row  the row index
     * @param column  the column index
     * @param p  the confidence level
     * @return the half interval length of NaN if not applicable
     */
    double getConfidenceInterval(int row, int column, double p);
}
