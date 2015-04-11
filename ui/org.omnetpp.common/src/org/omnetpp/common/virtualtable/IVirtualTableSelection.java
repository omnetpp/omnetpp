/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;

import org.omnetpp.common.collections.IRangeSet;

/**
 * Represents a selection in the virtual table.
 */
public interface IVirtualTableSelection<T> extends IRangeSelection<T> {

    /**
     * The input that should be displayed.
     */
    public Object getInput();

    /**
     * The set of selected elements.
     */
    public IRangeSet<T> getElements();
}