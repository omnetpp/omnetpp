/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
