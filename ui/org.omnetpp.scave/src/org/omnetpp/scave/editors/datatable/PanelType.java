/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import org.omnetpp.scave.engine.ResultFileManager;

/**
 * Type of a panel on the Browse Data page.
 *
 * @author andras
 */
public enum PanelType {
    ALL(ResultFileManager.PARAMETER | ResultFileManager.SCALAR | ResultFileManager.VECTOR | ResultFileManager.STATISTICS | ResultFileManager.HISTOGRAM),
    PARAMETERS(ResultFileManager.PARAMETER),
    SCALARS(ResultFileManager.SCALAR),
    VECTORS(ResultFileManager.VECTOR),
    HISTOGRAMS(ResultFileManager.STATISTICS | ResultFileManager.HISTOGRAM);

    private int itemTypes;

    private PanelType(int types) {
        itemTypes = types;
    }

    public int getItemTypes() {
        return itemTypes;
    }

    public boolean isSingleType() {
        return itemTypes == ResultFileManager.PARAMETER
            || itemTypes == ResultFileManager.SCALAR
            || itemTypes == ResultFileManager.VECTOR
            || itemTypes == ResultFileManager.STATISTICS
            || itemTypes == ResultFileManager.HISTOGRAM;
    }
}
