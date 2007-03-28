package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.Collection;

import org.apache.commons.collections.ListUtils;
import org.apache.commons.collections.map.MultiValueMap;
import org.apache.commons.lang.ArrayUtils;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.engine.IntVector;

public class HierarchicalAxisOrderByMinimizingCost extends AbstractAxisOrderByMinimizingCost {
	public HierarchicalAxisOrderByMinimizingCost(EventLogInput eventLogInput) {
		super(eventLogInput);
	}

	public void calculateOrdering(final ModuleTreeItem[] axisModules, int[] axisPositions, final IntVector axisMatrix) {
		calculateOrdering(axisModules[0].getRootModule(), axisModules, axisPositions, axisMatrix);
	}

	public void calculateOrdering(final ModuleTreeItem module, final ModuleTreeItem[] axisModules, final int[] axisPositions, final IntVector axisMatrix) {
		if (!ArrayUtils.contains(axisModules, module)) {
			final int numberOfAxis = axisModules.length;
			final ModuleTreeItem[] submodules = module.getSubmodules();
			final int[] submoduleIndicesToNumberOfAxis = new int[submodules.length];
			final MultiValueMap submoduleToAxisIndexes = new MultiValueMap();
			final int[] localAxisPositions = new int[numberOfAxis];

			if (submodules.length == 0)
				return;

			// calculate the number of axes and the axis module indexes that a submodule represents
			for (int i = 0; i < numberOfAxis; i++) {
				ModuleTreeItem submodule = axisModules[i].getAncestorModuleUnder(module);
			
				if (submodule != null) {
					submoduleIndicesToNumberOfAxis[ArrayUtils.indexOf(submodules, submodule)]++;
					submoduleToAxisIndexes.put(submodule, i);
				}
			}
			
			// first sort descendants recursively
			for (ModuleTreeItem submodule : submodules)
				calculateOrdering(submodule, axisModules, axisPositions, axisMatrix);

			// sort the direct submodules of this module
			int[] submoduleIndicesToSubmodulePositions = sortTimelinesByMinimizingCost(submodules, new IAxisOrderCostCalculator() {
				public int calculateCost(int[] submoduleIndicesToSubmodulePositions) {
					int cost = 0;

					System.arraycopy(axisPositions, 0, localAxisPositions, 0, numberOfAxis);
					calculateAxisPositions(axisPositions, submodules, submoduleIndicesToSubmodulePositions, submoduleIndicesToNumberOfAxis, submoduleToAxisIndexes);

					// sum up cost of messages to other axis
					for (ModuleTreeItem iSubmodule : submodules)
						for (ModuleTreeItem jSubmodule : submodules)
							if (iSubmodule != jSubmodule)
								for (Object iObject : getAxisModuleIndicesForSubmodule(submoduleToAxisIndexes, iSubmodule))
									for (Object jObject : getAxisModuleIndicesForSubmodule(submoduleToAxisIndexes, jSubmodule))
										cost += HierarchicalAxisOrderByMinimizingCost.this.calculateCost(axisPositions, (Integer)iObject, (Integer)jObject, axisMatrix);

					return cost;
				}
			});

			// calculate axis offsets and positions for submodules
			calculateAxisPositions(axisPositions, submodules, submoduleIndicesToSubmodulePositions, submoduleIndicesToNumberOfAxis, submoduleToAxisIndexes);
		}
		else
			// the positions will be updated each time when returning from this recursive function
			axisPositions[ArrayUtils.indexOf(axisModules, module)] = 0;
	}
	
	/**
	 * Calculates axis positions by shifting axes according to the module positions and count of axes per module.
	 */
	private void calculateAxisPositions(int[] axisPositions, ModuleTreeItem[] submodules, int[] submoduleIndicesToSubmodulePositions, int[] submoduleIndicesToNumberOfAxis, MultiValueMap submoduleToAxisIndexes) {
		int[] submodulePositionsToSubmoduleIndices = new int[submodules.length];
		for (int i = 0; i < submodules.length; i++)
			submodulePositionsToSubmoduleIndices[submoduleIndicesToSubmodulePositions[i]] = i;

		int count = 0;
		int[] submoduleIndicesToSubmoduleAxisOffsets = new int[submodules.length];
		for (int submodulePosition = 0; submodulePosition < submodules.length; submodulePosition++) {
			int c = submoduleIndicesToNumberOfAxis[submodulePositionsToSubmoduleIndices[submodulePosition]];
			submoduleIndicesToSubmoduleAxisOffsets[submodulePositionsToSubmoduleIndices[submodulePosition]] = count;
			count += c;
		}

		// recalculate positions
		for (int submoduleIndex = 0; submoduleIndex < submodules.length; submoduleIndex++) {
			ModuleTreeItem submodule = submodules[submoduleIndex];

			// move axes to the offset of their module
			for (Object i : getAxisModuleIndicesForSubmodule(submoduleToAxisIndexes, submodule))
				axisPositions[(Integer)i] += submoduleIndicesToSubmoduleAxisOffsets[submoduleIndex];
		}
	}

	/**
	 * Just a convenient helper function to return an empty collection if there's no value for the given key.
	 */
	private Collection getAxisModuleIndicesForSubmodule(MultiValueMap submoduleToAxisIndexes, ModuleTreeItem submodule) {
		Collection collection = submoduleToAxisIndexes.getCollection(submodule);
		
		if (collection != null)
			return collection;
		else
			return ListUtils.EMPTY_LIST;
	}
}