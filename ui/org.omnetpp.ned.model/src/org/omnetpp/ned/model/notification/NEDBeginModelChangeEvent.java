package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INEDElement;

public class NEDBeginModelChangeEvent extends NEDModelEvent
{
	public NEDBeginModelChangeEvent(INEDElement source) {
		super(source);
	}
}
