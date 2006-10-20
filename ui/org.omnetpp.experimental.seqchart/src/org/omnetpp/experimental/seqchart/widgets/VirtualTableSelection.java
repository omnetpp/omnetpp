package org.omnetpp.experimental.seqchart.widgets;

import java.util.List;

public class VirtualTableSelection implements IVirtualTableSelection {
	protected Object input;
	protected List elements;

	public VirtualTableSelection(Object input, List elements) {
		this.input = input;
		this.elements = elements;
	}
	
	public Object getInput() {
		return input;
	}
	
	public List getElements() {
		return elements;
	}

	public boolean isEmpty() {
		return elements.size() != 0;
	}
}
