/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;


/**
 * Interface for datasets displayed on bar charts.
 *
 * @author tomi
 */
public interface IGroupsSeriesDataset extends IDataset {

    /**
     * Returns the group (row) count.
     */
    public int getGroupCount();

    /**
     * Returns the group key for a given index.
     */
    public String getGroupKey(int row);

    /**
     * Returns the series (column) count.
     */
    public int getSeriesCount();

    /**
     * Returns the series key for a given index.
     */
    public String getSeriesKey(int column);

    /**
     * Returns the value associated with the specified cell.
     */
    public double getValue(int row, int column);

    /**
     * For tooltips. Can be used for enum values.
     */
    String getValueAsString(int row, int column);
}
