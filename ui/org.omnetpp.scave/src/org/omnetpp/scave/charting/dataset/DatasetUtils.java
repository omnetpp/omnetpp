/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

public class DatasetUtils {
    /**
     * Returns the index of the X value in the dataset series which is:
     * either the first of the values that equal "value", or (if there
     * are no such values) the last one which is less than "value".
     */
    public static int findXLowerLimit(IXYDataset dataset, int series, double value){
        int high = dataset.getItemCount(series);
        int low = -1;
        int mid;
        while (high - low > 1)
        {
            mid = (low + high) >>> 1;
            if (dataset.getX(series, mid) > value)
                high = mid;
            else
                low = mid;
        }
        return low;
    }

    /**
     * Returns the index of the X value in the dataset series which is:
     * either the last of the values that equal "value", or (if there
     * are no such values) the first one which is greater than "value".
     */
    //XXX to be tested!!!
    public static int findXUpperLimit(IXYDataset dataset, int series, double value){
        int high = dataset.getItemCount(series);
        int low = 0;
        int mid;
        while (high - low > 1)
        {
            mid = (low + high + 1) >>> 1;
            if (dataset.getX(series, mid) >= value)
                high = mid;
            else
                low = mid;
        }
        return high;
    }
}
