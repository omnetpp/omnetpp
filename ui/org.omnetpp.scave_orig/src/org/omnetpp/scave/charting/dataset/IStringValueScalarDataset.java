/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

/**
 * Implement this interface if custom formatting of the
 * values is needed (e.g. enum values).
 *
 * @author tomi
 */
public interface IStringValueScalarDataset extends IScalarDataset {
    String getValueAsString(int row, int column);
}
