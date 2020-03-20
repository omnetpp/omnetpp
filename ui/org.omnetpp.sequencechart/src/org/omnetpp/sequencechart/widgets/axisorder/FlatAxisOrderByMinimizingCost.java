/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.engine.IntIntMap;
import org.omnetpp.eventlog.engine.IntVector;
import org.omnetpp.eventlog.engine.SequenceChartFacade;

/**
 * This class implements a sort method that tries to minimize the total number of axes that arrows are crossing.
 * It takes a statistical sample from the eventlog and spends a limited amount of time to reorder axes.
 * Reordering uses bubble sort and two axes are swapped only when the result is better in terms of the number of crossings.
 * This will result in a local optimum.
 */
public class FlatAxisOrderByMinimizingCost {
    private EventLogInput eventLogInput;
    private long startEventPtr;
    private long endEventPtr;

    private static int TIME_LIMIT = 1000; // in milliseconds

    public FlatAxisOrderByMinimizingCost(EventLogInput eventLogInput, long startEventPtr, long endEventPtr) {
        this.eventLogInput = eventLogInput;
        this.startEventPtr = startEventPtr;
        this.endEventPtr = endEventPtr;
    }

    public int[] calculateOrdering(ModuleTreeItem[] axisModules, Map<Integer, Integer> moduleIdToAxisModuleIndexMap) {
        int eventCount = 1000;
        SequenceChartFacade sequenceChartFacade = eventLogInput.getSequenceChartFacade();
        IntIntMap cppModuleIdToAxisModuleIndexMap = getCppModuleIdToAxisModuleIndexMap(moduleIdToAxisModuleIndexMap);
        boolean isSmallEventRange = sequenceChartFacade.IEvent_getEventNumber(endEventPtr) - sequenceChartFacade.IEvent_getEventNumber(startEventPtr) < eventCount;
        IntVector cppAxisMessageDependecyWeightMatrix = isSmallEventRange ? 
                eventLogInput.getSequenceChartFacade().getMessageDependencyCountAdjacencyMatrix(cppModuleIdToAxisModuleIndexMap, startEventPtr, endEventPtr, 1, 0) :
                eventLogInput.getSequenceChartFacade().getApproximateMessageDependencyCountAdjacencyMatrix(cppModuleIdToAxisModuleIndexMap, eventCount, 1, 0);
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

    /**
     * This is the actual sort based on the statistical sample in the weight matrix.
     */
    private void bubbleSort(ModuleTreeItem[] axisModules, int[][] axisMessageDependecyWeightMatrix) {
        long begin = System.currentTimeMillis();
        int numberOfAxes = axisModules.length;

        while (System.currentTimeMillis() - begin < TIME_LIMIT) {
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

   private IntIntMap getCppModuleIdToAxisModuleIndexMap(Map<Integer, Integer> moduleIdToAxisModuleIndexMap) {
        IntIntMap cppModuleIdToAxisModuleIndexMap = new IntIntMap();

        for (Integer key : moduleIdToAxisModuleIndexMap.keySet())
            cppModuleIdToAxisModuleIndexMap.set(key, moduleIdToAxisModuleIndexMap.get(key));

        return cppModuleIdToAxisModuleIndexMap;
    }
}