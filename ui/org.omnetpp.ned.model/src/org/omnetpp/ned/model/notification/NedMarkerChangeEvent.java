/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INedElement;

/**
 * Fired when a NedElement has problem markers added or cleared.
 *
 * @author Andras
 */
public class NedMarkerChangeEvent extends NedModelEvent
{
    public NedMarkerChangeEvent(INedElement source) {
        super(source);
    }
}
