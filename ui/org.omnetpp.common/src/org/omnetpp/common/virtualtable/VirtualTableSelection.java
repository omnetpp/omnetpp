/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;

import java.util.List;

/**
 * Represents a selection in the virtual table. Default implementation.
 */
public class VirtualTableSelection<T> implements IVirtualTableSelection<T> {
	protected Object input;

	protected List<T> elements;

	public VirtualTableSelection(Object input, List<T> elements) {
		this.input = input;
		this.elements = elements;
	}

	public Object getInput() {
		return input;
	}

	public List<T> getElements() {
		return elements;
	}

	public boolean isEmpty() {
		return elements == null || elements.size() == 0;
	}
}
