/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
