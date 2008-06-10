package org.omnetpp.sequencechart.widgets.axisorder;

import org.omnetpp.common.eventlog.ModuleTreeItem;

public class AxisOrderByModuleId {
	public int[] calculateOrdering(final ModuleTreeItem[] axisModules) {
	    int numberOfAxes = axisModules.length;
	    int[] axisPositions = new int[numberOfAxes];
	    Integer[] axisModuleIndexes = new Integer[numberOfAxes];
		for (int i = 0; i < axisModuleIndexes.length; i++)
		    axisModuleIndexes[i] = i;
		
		java.util.Arrays.sort(axisModuleIndexes, new java.util.Comparator<Integer>() {
				public int compare(Integer o1, Integer o2) {
					return ((Integer)axisModules[o1].getModuleId()).compareTo(axisModules[o2].getModuleId());
				}
			});
		
		for (int i = 0; i < axisModuleIndexes.length; i++)
			axisPositions[axisModuleIndexes[i]] = i;
		
		return axisPositions;
	}
}
