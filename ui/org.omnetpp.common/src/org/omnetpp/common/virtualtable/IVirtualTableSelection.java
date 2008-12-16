/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;

import java.util.List;

import org.eclipse.jface.viewers.ISelection;

/**
 * Represents a selection in the virtual table.
 */
public interface IVirtualTableSelection<T> extends ISelection {
	/**
	 * The input that should be displayed.
	 */
	public Object getInput();

	/**
	 * The list of selected elements.
	 */
	public List<T> getElements();
}