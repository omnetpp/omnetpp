package org.omnetpp.ned.model.ex;

import org.omnetpp.ned.model.notification.NEDModelEvent;

public class NEDModelReadOnlyEvent extends NEDModelEvent
{
	private boolean readOnly;

	public NEDModelReadOnlyEvent(NedFileElementEx source, boolean editable) {
		super(source);
		this.readOnly = editable;
	}
	
	public boolean getEditable() {
		return readOnly;
	}
}
