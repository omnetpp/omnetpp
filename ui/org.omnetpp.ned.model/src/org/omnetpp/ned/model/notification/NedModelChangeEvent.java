/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INedElement;

/**
 * Common base for all events that actually change the ned element tree
 * @author rhornig
 */
public class NedModelChangeEvent extends NedModelEvent {

    public NedModelChangeEvent(INedElement source) {
        super(source);
    }

}
