package org.omnetpp.common.virtualtable;

import java.util.List;

public class VirtualTableSelection implements IVirtualTableSelection {
	protected Object input;
	protected List<Object> elements;

	public VirtualTableSelection(Object input, List<Object> elements) {
		this.input = input;
		this.elements = elements;
	}
	
	public Object getInput() {
		return input;
	}
	
	public List<Object> getElements() {
		return elements;
	}

	public boolean isEmpty() {
		return elements == null || elements.size() == 0;
	}
}
