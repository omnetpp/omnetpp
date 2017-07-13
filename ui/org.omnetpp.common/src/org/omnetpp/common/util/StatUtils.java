/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.apache.commons.math.MathException;
import org.apache.commons.math.distribution.DistributionFactory;
import org.apache.commons.math.distribution.TDistribution;
import org.eclipse.core.runtime.Assert;

/**
 * Utility functions for statistical computations.
 *
 * @author tomi
 */
public class StatUtils {

    private static final DistributionFactory factory = DistributionFactory.newInstance();

    /**
     * Computes a symmetric confidence interval for the expected value
     * of a normal distribution.
     *
     * @param p the level of confidence
     * @param stddev the experimental standard deviation
     * @param n the number of observations
     * @return
     */
    public static double confidenceInterval(double p, double stddev, long n) {
        Assert.isLegal(n >= 0);
        Assert.isLegal(0.0 <= p && p <= 1.0);

        if (n <= 1)
            return Double.NaN;

        long degreesOfFreedom = n - 1;
        double tValue = 1.0 - (1.0 - p) / 2;
        double normalizedStddev = stddev / Math.sqrt(n);

        TDistribution tDist = factory.createTDistribution(degreesOfFreedom);

        try {
            return tDist.inverseCumulativeProbability(tValue) * normalizedStddev;
        }
        catch (MathException e) {
            return Double.NaN;
        }
    }
}
