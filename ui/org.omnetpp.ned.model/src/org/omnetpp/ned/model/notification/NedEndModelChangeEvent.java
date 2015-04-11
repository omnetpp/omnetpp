/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INedElement;

/**
 * Surrounding NED tree manipulation code by BeginChange / EndChange
 * notification events makes it possible for listening editors and views to
 * eliminate redundant refreshes.
 *
 * @author levy
 */
public class NedEndModelChangeEvent extends NedModelEvent
{
    public NedEndModelChangeEvent(INedElement source) {
        super(source);
    }
}
