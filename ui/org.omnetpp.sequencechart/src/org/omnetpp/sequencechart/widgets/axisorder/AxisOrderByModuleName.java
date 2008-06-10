package org.omnetpp.sequencechart.widgets.axisorder;

import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.common.util.StringUtils;

public class AxisOrderByModuleName {
	public int[] calculateOrdering(final ModuleTreeItem[] axisModules) {
        int numberOfAxes = axisModules.length;
        int[] axisPositions = new int[numberOfAxes];
        Integer[] axisModuleIndexes = new Integer[numberOfAxes];
		for (int i = 0; i < axisModuleIndexes.length; i++)
			axisModuleIndexes[i] = i;
		
		java.util.Arrays.sort(axisModuleIndexes, new java.util.Comparator<Integer>() {
				public int compare(Integer o1, Integer o2) {
					return StringUtils.dictionaryCompare(axisModules[o1].getModuleFullPath(), axisModules[o2].getModuleFullPath());
				}
			});
		
		for (int i = 0; i < axisModuleIndexes.length; i++)
			axisPositions[axisModuleIndexes[i]] = i;
		
		return axisPositions;
	}
}
