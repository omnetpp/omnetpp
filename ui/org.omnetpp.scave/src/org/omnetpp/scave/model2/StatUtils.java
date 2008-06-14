package org.omnetpp.scave.model2;

import org.omnetpp.scave.engine.Statistics;

public class StatUtils extends org.omnetpp.common.util.StatUtils {

	public static double confidenceInterval(Statistics stat, double p) {
		return confidenceInterval(p, stat.getStddev(), stat.getCount());
	}
	
	public static Statistics singleValueStatistics(double value) {
		Statistics stat = new Statistics();
		stat.collect(value);
		return stat;
	}
}
