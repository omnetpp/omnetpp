package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INEDElement;

/**
 * Surrounding NED tree manipulation code by BeginChange / EndChange
 * notification events makes it possible for listening editors and views to
 * eliminate redundant refreshes.
 *  
 * @author levy
 */
public class NEDBeginModelChangeEvent extends NEDModelEvent
{
	public NEDBeginModelChangeEvent(INEDElement source) {
		super(source);
	}
}
