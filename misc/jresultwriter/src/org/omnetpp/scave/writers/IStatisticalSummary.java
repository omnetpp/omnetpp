package org.omnetpp.scave.writers;

/**
 * Reporting interface for basic univariate statistics.
 *
 * This is a copy of org.apache.commons.math.stat.descriptive.StatisticalSummary.
 *
 * @see IHistogramSummary
 *
 * @author Andras
 */
public interface IStatisticalSummary {
    /**
     * Returns the <a href="http://www.xycoon.com/arithmetic_mean.htm">
     * arithmetic mean </a> of the available values
     * @return The mean or Double.NaN if no values have been added.
     */
    public abstract double getMean();

    /**
     * Returns the variance of the available values.
     * @return The variance, Double.NaN if no values have been added
     * or 0.0 for a single value set.
     */
    public abstract double getVariance();

    /**
     * Returns the standard deviation of the available values.
     * @return The standard deviation, Double.NaN if no values have been added
     * or 0.0 for a single value set.
     */
    public abstract double getStandardDeviation();

    /**
     * Returns the maximum of the available values
     * @return The max or Double.NaN if no values have been added.
     */
    public abstract double getMax();

    /**
    * Returns the minimum of the available values
    * @return The min or Double.NaN if no values have been added.
    */
    public abstract double getMin();

    /**
     * Returns the number of available values
     * @return The number of available values
     */
    public abstract long getN();

    /**
     * Returns the sum of the values that have been added to Univariate.
     * @return The sum or Double.NaN if no values have been added
     */
    public abstract double getSum();

    /**
     * Returns the squared sum of the values that have been added to Univariate.
     * @return The squared sum or Double.NaN if no values have been added
     *
     * Note: this is not part of the Apache interface, but necessary if one
     * wants to completely restore the statistics calculation object.
     */
    public abstract double getSqrSum();
}
