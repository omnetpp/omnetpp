/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

/**
 * Listener for notifying clients that the content of a data control
 * (DataTree/DataTable, the main widgets on the Browse Data page)
 * has changed.
 */
public interface IDataListener {
    void contentChanged(IDataControl control);
}
