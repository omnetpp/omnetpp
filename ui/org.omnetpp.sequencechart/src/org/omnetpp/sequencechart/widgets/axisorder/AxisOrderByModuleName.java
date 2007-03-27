package org.omnetpp.sequencechart.widgets.axisorder;

import org.omnetpp.common.eventlog.ModuleTreeItem;

public class AxisOrderByModuleName implements IAxisOrder {
	public void calculateOrdering(final ModuleTreeItem[] axisModules, int[] axisPositions) {
		Integer[] axisModuleIndexes = new Integer[axisModules.length];
		
		for (int i = 0; i < axisModuleIndexes.length; i++)
			axisModuleIndexes[i] = i;
		
		java.util.Arrays.sort(axisModuleIndexes, new java.util.Comparator<Integer>() {
				public int compare(Integer o1, Integer o2) {
					return axisModules[o1].getModuleFullPath().compareTo(axisModules[o2].getModuleFullPath());
				}
			});
		
		for (int i = 0; i < axisModuleIndexes.length; i++)
			axisPositions[axisModuleIndexes[i]] = i;
	}
}
