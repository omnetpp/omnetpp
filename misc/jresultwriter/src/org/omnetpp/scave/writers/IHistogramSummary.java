package org.omnetpp.scave.writers;

/**
 * Interface for presenting histogram data.
 *
 * @author Andras
 */
public interface IHistogramSummary extends IStatisticalSummary {
    /**
     * Returns the number of histogram cells used.
     */
    int getNumCells();

    /**
     * Returns the kth cell boundary. Legal values for k are 0 through getNumCells(),
     * i.e. there are <code>getNumCells()+1</code> cell boundaries.
     * <code>getCellBounds(0)</code> returns the lower bound of the first cell,
     * and <code>getCellBounds(getNumCells())</code> returns the upper bound of the
     * last cell.
     */
    double getCellBoundary(int k);

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    double getCellValue(int k);

    /**
     * Returns the estimated value of the Probability Density Function
     * within the kth cell. This method simply divides the number of observations
     * in cell k with the cell size and the number of total observations collected.
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
