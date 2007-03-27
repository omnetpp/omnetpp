package org.omnetpp.sequencechart.widgets.axisorder;

import org.omnetpp.common.eventlog.ModuleTreeItem;

public interface IAxisOrder {
	/**
	 * Calculates the ordering of the given axis modules. The current positions will be
	 * passed in and should be updated.
	 */
	public void calculateOrdering(ModuleTreeItem[] axisModules, int[] axisPositions);
}

/**
 * Helper interface to calculate the cost of a candidate ordering for the axis being ordered.
 */
interface IAxisOrderCostCalculator {
	/**
	 * Used to estimate the cost of the given axis ordering.
	 */
	public int calculateCost(int[] axisPositions);
}
