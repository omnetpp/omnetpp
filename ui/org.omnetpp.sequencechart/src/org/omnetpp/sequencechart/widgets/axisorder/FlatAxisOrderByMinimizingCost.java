package org.omnetpp.sequencechart.widgets.axisorder;

import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.engine.IntVector;

public class FlatAxisOrderByMinimizingCost extends AbstractAxisOrderByMinimizingCost {
	public FlatAxisOrderByMinimizingCost(EventLogInput eventLogInput) {
		super(eventLogInput);
	}

	public void calculateOrdering(final ModuleTreeItem[] axisModules, int[] axisPositions, final IntVector axisMatrix) {
		int[] newAxisPositions = sortTimelinesByMinimizingCost(axisModules,
			new IAxisOrderCostCalculator() {
				public int calculateCost(int[] axisPositions) {
					int cost = 0;

					// sum up cost of messages to other axes
					for (int i = 0; i < axisModules.length; i++)
						for (int j = 0; j < axisModules.length; j++)
							if (i != j)
								cost += FlatAxisOrderByMinimizingCost.this.calculateCost(axisPositions, i, j, axisMatrix);
					
					return cost;
				}
			});

		System.arraycopy(newAxisPositions, 0, axisPositions, 0, axisPositions.length);
	}
}