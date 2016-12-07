package org.omnetpp.scave.writers;

/**
 * Extends IStatisticalSummary with weighted statistics.
 *
 * @author Andras
 */
public interface IStatisticalSummary2 extends IStatisticalSummary {

    /**
     * Returns whether this is weighted statistics.
     */
    boolean isWeighted();

    /**
     * Returns the sum of weights.
     * @return The value or Double.NaN if no values have been added
     */
    double getWeights();

    /**
     * Returns the sum of weight*value products.
     * @return The value or Double.NaN if no values have been added
     */
    double getWeightedSum();

    /**
     * Returns the sum of squared weights.
     * @return The value or Double.NaN if no values have been added
     */
    double getSqrSumWeights();

    /**
     * Returns the sum of weight*value*value products.
     * @return The value or Double.NaN if no values have been added
     */
    double getWeightedSqrSum();
}
