package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INEDElement;

public class NEDModelChangeBeginEvent extends NEDModelEvent
{
	public NEDModelChangeBeginEvent(INEDElement source) {
		super(source);
	}
}
