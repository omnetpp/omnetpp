package org.omnetpp.experimental.seqchart.widgets;

import java.util.List;

import org.eclipse.jface.viewers.ISelection;

public interface IVirtualTableSelection extends ISelection {
	public Object getInput();

	public List getElements();
}
