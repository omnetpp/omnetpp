package org.omnetpp.scave.writers;

/**
 * For presenting histogram data.
 * 
 * @author Andras
 */
public interface IHistogramSummary extends IStatisticalSummary {
    /**
     * Returns the number of histogram cells used.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    int getNumCells();

    /**
     * Returns the kth cell boundary. Legal values for k are 0 through getNumCells(),
     * that is, there' one more basepoint than the number of cells.
     * getBasepoint(0) returns the low end of the first cell, and getBasepoint(getNumCells())
     * returns the high end of the last histogram cell.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    double getBasepoint(int k);

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     * Before transformation, this method may return zero. See transform().
     * This method is pure virtual, implementation is provided in subclasses.
     */
    double getCellValue(int k);

    /**
     * Returns the estimated value of the Probability Density Function
     * within the kth cell. This method simply divides the number of observations
     * in cell k with the cell size and the number of total observations collected.
     * Note that before transformation, getCellValue() and also this method may return zero.
     * See transform().
     */
    double getCellPDF(int k);

    /**
     * Returns number of observations that, being too small, fell out of the histogram
     * range.
     */
    long getUnderflowCell();

    /**
     * Returns number of observations that, being too large, fell out of the histogram
     * range.
     */
    long getOverflowCell();
}
