package org.omnetpp.sequencechart.widgets.axisorder;

import org.omnetpp.common.eventlog.ModuleTreeItem;

public class AxisOrderByModuleId implements IAxisOrder {
	public void calculateOrdering(final ModuleTreeItem[] axisModules, int[] axisPositions) {
		Integer[] axisModulesIndices = new Integer[axisModules.length];
		
		for (int i = 0; i < axisModulesIndices.length; i++)
			axisModulesIndices[i] = i;
		
		java.util.Arrays.sort(axisModulesIndices, new java.util.Comparator<Integer>() {
				public int compare(Integer o1, Integer o2) {
					return ((Integer)axisModules[o1].getModuleId()).compareTo(axisModules[o2].getModuleId());
				}
			});
		
		for (int i = 0; i < axisModulesIndices.length; i++)
			axisPositions[axisModulesIndices[i]] = i;
	}
}
