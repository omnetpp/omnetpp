/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

/**
 * Common interface for chart datasets.
 *
 * @author tomi
 */
public interface IDataset {

    /**
     * The short description of the dataset displayed as the
     * default title of the chart.
     *
     * @return the title of the dataset, may be null
     */
    String getTitle(String format);
}
