package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.Random;

import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.engine.IntIntMap;
import org.omnetpp.eventlog.engine.IntVector;

// TODO: this algorithm is kind of random and slow, so what about using the C++ ForceDirectedEmbedding algorithm?
public abstract class AbstractAxisOrderByMinimizingCost  implements IAxisOrder { 
	protected EventLogInput eventLogInput;

	public AbstractAxisOrderByMinimizingCost(EventLogInput eventLogInput) {
		this.eventLogInput = eventLogInput;
	}

	public void calculateOrdering(ModuleTreeItem[] axisModules, int[] axisPositions) {
		final IntIntMap moduleIdToAxisIdMap = new IntIntMap();
		for (int i = 0; i < axisModules.length; i++) {
			final Integer ii = i;
			ModuleTreeItem treeItem = axisModules[i];
			treeItem.visitLeaves(new ModuleTreeItem.IModuleTreeItemVisitor() {
				public void visit(ModuleTreeItem treeItem) {
					moduleIdToAxisIdMap.set(treeItem.getModuleId(), ii);
				}
			});
		}

		IntVector axisMatrix = eventLogInput.getSequenceChartFacade().getApproximateMessageDependencyCountAdjacencyMatrix(moduleIdToAxisIdMap, 100);
		calculateOrdering(axisModules, axisPositions, axisMatrix);
	}
	
	protected abstract void calculateOrdering(ModuleTreeItem[] axisModules, int[] axisPositions, IntVector axisMatrix);

	/**
	 * Cost is based on the number of crossed axis according to the positions.
	 */
	protected int calculateCost(int[] axisPositions, int i, int j, IntVector axisMatrix) {
		return Math.abs(axisPositions[i] - axisPositions[j]) *
			            (axisMatrix.get(axisPositions.length * i + j) +
                         axisMatrix.get(axisPositions.length * j + i));
	}

	protected int[] sortTimelinesByMinimizingCost(ModuleTreeItem[] modules, IAxisOrderCostCalculator axisOrderCostCalculator) {
		int cycleCount = 0;
		int moveCount = 0;
		int randomMoveCount = 0;
		int numberOfAxis = modules.length;
		int[] axisPositions = new int[numberOfAxis]; // actual positions of axis to be returned
		int[] candidateAxisPositions = new int[numberOfAxis]; // new positions of axis to be set (if better)
		int[] bestAxisPositions = new int[numberOfAxis]; // best positions choosen from a set of candidates
		Random r = new Random(0);
		double temperature = 1.0;

		// set initial axis positions
		new AxisOrderByModuleName().calculateOrdering(modules, axisPositions);
		
		while (cycleCount < 100 && (moveCount < numberOfAxis || randomMoveCount < numberOfAxis))
		{
			cycleCount++;
			
			eventLogInput.progress();
			
			// randomly swap axis based on temperature
			double t = temperature;
			randomMoveCount++;
			while (false && r.nextDouble() < t) {
				int i1 = r.nextInt(numberOfAxis);
				int i2 = r.nextInt(numberOfAxis);
				int i = axisPositions[i1];
				axisPositions[i1] = axisPositions[i2];
				axisPositions[i2] = i;
				randomMoveCount = 0;
				t--;
			}

			// choose an axis which we move to the best place (there are numberOfAxis possibilities)
			int selectedAxisIndex = cycleCount % numberOfAxis;
			int bestPositionOfSelectedAxis = -1;
			int costOfBestPositions = Integer.MAX_VALUE;

			// TODO: this is O(N^2) where N is the number of axis
			// assume moving axis at index to position i while keeping the order of others and calculate cost
			for (int newPositionOfSelectedAxis = 0; newPositionOfSelectedAxis < numberOfAxis; newPositionOfSelectedAxis++) {
				if (newPositionOfSelectedAxis == -1)
					continue;

				// set up candidateAxisPositions so that the order of other axis do not change
				for (int i = 0; i < numberOfAxis; i++) {
					int pos = axisPositions[i];
					if (newPositionOfSelectedAxis <= pos && pos < axisPositions[selectedAxisIndex])
						pos++;
					if (axisPositions[selectedAxisIndex] < pos && pos <= newPositionOfSelectedAxis)
						pos--;
					candidateAxisPositions[i] = pos;
				}
				candidateAxisPositions[selectedAxisIndex] = newPositionOfSelectedAxis;

				int cost = axisOrderCostCalculator.calculateCost(candidateAxisPositions);
				
				// find minimum cost
				if (cost < costOfBestPositions) {
					costOfBestPositions = cost;
					bestPositionOfSelectedAxis = newPositionOfSelectedAxis;
					System.arraycopy(candidateAxisPositions, 0, bestAxisPositions, 0, numberOfAxis);
				}
			}
			
			// move selected axis into best position if applicable
			if (bestPositionOfSelectedAxis != -1) {
				System.arraycopy(bestAxisPositions, 0, axisPositions, 0, numberOfAxis);
				moveCount = 0;
			}
			else
				moveCount++;

			// decrease temperature
			temperature *= 0.9;
		}

		return axisPositions;
	}
}