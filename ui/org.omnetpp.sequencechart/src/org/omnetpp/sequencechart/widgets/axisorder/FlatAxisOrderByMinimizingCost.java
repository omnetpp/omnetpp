package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.HashMap;

import org.apache.commons.lang.ArrayUtils;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.engine.IntIntMap;
import org.omnetpp.eventlog.engine.IntVector;

public class FlatAxisOrderByMinimizingCost {
    protected EventLogInput eventLogInput;

    public FlatAxisOrderByMinimizingCost(EventLogInput eventLogInput) {
		this.eventLogInput = eventLogInput;
	}

    public int[] calculateOrdering(ModuleTreeItem[] axisModules, HashMap<Integer, Integer> moduleIdToAxisModuleIndexMap) {
        IntIntMap cppModuleIdToAxisModuleIndexMap = getCppModuleIdToAxisModuleIndexMap(moduleIdToAxisModuleIndexMap);
        IntVector cppAxisMessageDependecyWeightMatrix = eventLogInput.getSequenceChartFacade().getApproximateMessageDependencyCountAdjacencyMatrix(cppModuleIdToAxisModuleIndexMap, 100, 1, 0);
        int[][] axisMessageDependecyWeightMatrix = getAxisMessageDependecyWeightMatrix(cppAxisMessageDependecyWeightMatrix, axisModules.length);
        
        ModuleTreeItem[] orderedAxisModules = (ModuleTreeItem[])ArrayUtils.clone(axisModules);
        bubbleSort(orderedAxisModules, axisMessageDependecyWeightMatrix);

        return getAxisModulePositions(axisModules, orderedAxisModules);
	}

    private int[] getAxisModulePositions(ModuleTreeItem[] axisModules, ModuleTreeItem[] orderedAxisModules) {
        int numberOfAxes = axisModules.length;
        int[] axisModulePositions = new int[numberOfAxes];

        for (int i = 0; i < numberOfAxes; i++)
            axisModulePositions[i] = ArrayUtils.indexOf(orderedAxisModules, axisModules[i]);
        
        return axisModulePositions;
    }

    private void bubbleSort(ModuleTreeItem[] axisModules, int[][] axisMessageDependecyWeightMatrix) {
        int numberOfAxes = axisModules.length;

        while (true) {
            boolean swapped = false;

            for (int i = 0; i < numberOfAxes; i++)
                for (int j = i + 1; j < numberOfAxes; j++)
                    swapped |= swapWhenCostIsLess(axisModules, axisMessageDependecyWeightMatrix, i, j);
            
            if (!swapped)
                return;
        }
    }

    private boolean swapWhenCostIsLess(ModuleTreeItem[] axisModules, int[][] axisMessageDependecyWeightMatrix,int i, int j) {
        int costBefore = calculateCost(axisMessageDependecyWeightMatrix, i) + calculateCost(axisMessageDependecyWeightMatrix, j);
        swapAxes(axisModules, axisMessageDependecyWeightMatrix, i, j);
        int costAfter = calculateCost(axisMessageDependecyWeightMatrix, i) + calculateCost(axisMessageDependecyWeightMatrix, j);
        
        if (costAfter < costBefore)
            return true;
        else {
            swapAxes(axisModules, axisMessageDependecyWeightMatrix, i, j);
            return false;
        }
    }

    private void swapAxes(ModuleTreeItem[] axisModules, int[][] axisMessageDependecyWeightMatrix, int index1, int index2) {
        ModuleTreeItem oldModuleTreeItem = axisModules[index1];
        axisModules[index1] = axisModules[index2];
        axisModules[index2] = oldModuleTreeItem;

        int numberOfAxes = axisMessageDependecyWeightMatrix.length;

        for (int i = 0; i < numberOfAxes; i++) {
            int old = axisMessageDependecyWeightMatrix[index1][i];
            axisMessageDependecyWeightMatrix[index1][i] = axisMessageDependecyWeightMatrix[index2][i];
            axisMessageDependecyWeightMatrix[index2][i] = old;
        }

        for (int i = 0; i < numberOfAxes; i++) {
            int old = axisMessageDependecyWeightMatrix[i][index1];
            axisMessageDependecyWeightMatrix[i][index1] = axisMessageDependecyWeightMatrix[i][index2];
            axisMessageDependecyWeightMatrix[i][index2] = old;
        }
    }

    private int calculateCost(int[][] axisMessageDependecyWeightMatrix, int position) {
        int cost = 0;

        for (int i = 0; i < axisMessageDependecyWeightMatrix.length; i++)
            if (i != position)
                cost += (axisMessageDependecyWeightMatrix[position][i] + axisMessageDependecyWeightMatrix[i][position]) * ((position - i) * (position - i) - 1);

        return cost;
    }

    private int[][] getAxisMessageDependecyWeightMatrix(IntVector cppAxisMessageDependecyWeightMatrix, int numberOfAxes) {
        int[][] axisMessageDependecyWeightMatrix = new int[numberOfAxes][numberOfAxes];

        for (int i = 0; i < numberOfAxes; i++)
            for (int j = 0; j < numberOfAxes; j++)
                axisMessageDependecyWeightMatrix[i][j] = cppAxisMessageDependecyWeightMatrix.get(numberOfAxes * i + j);
        
        return axisMessageDependecyWeightMatrix;
    }

   private IntIntMap getCppModuleIdToAxisModuleIndexMap(HashMap<Integer, Integer> moduleIdToAxisModuleIndexMap) {
        IntIntMap cppModuleIdToAxisModuleIndexMap = new IntIntMap();

        for (Integer key : moduleIdToAxisModuleIndexMap.keySet())
            cppModuleIdToAxisModuleIndexMap.set(key, moduleIdToAxisModuleIndexMap.get(key));

        return cppModuleIdToAxisModuleIndexMap;
    }
}