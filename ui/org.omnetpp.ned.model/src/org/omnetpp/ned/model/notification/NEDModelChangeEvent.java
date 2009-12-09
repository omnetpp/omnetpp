/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INEDElement;

/**
 * Common base for all events that actually change the ned element tree
 * @author rhornig
 */
public class NEDModelChangeEvent extends NEDModelEvent {

    public NEDModelChangeEvent(INEDElement source) {
        super(source);
    }

}
