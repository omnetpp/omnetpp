package org.omnetpp.common.virtualtable;

import java.util.List;

import org.eclipse.jface.viewers.ISelection;

public interface IVirtualTableSelection<T> extends ISelection {
	public Object getInput();

	public List<T> getElements();
}
