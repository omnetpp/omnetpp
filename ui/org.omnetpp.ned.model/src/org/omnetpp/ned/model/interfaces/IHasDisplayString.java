/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;


/**
 * Interface for NED model objects that provide a display string (i.e. have a visual appearance).
 *
 * @author rhornig
 */
public interface IHasDisplayString extends INedElement {
    /**
     * Returns the display string object. The display string object can be modified
     * by clients, and it is automatically kept consistent both ways with the underlying
     * NedElement (a LiteralElement) that stores the value for the "@display()" NED property.
     * The fallback chain for the display string is automatically kept up to date as well.
     * Any change in the returned DisplayString object (e.g. calling a set(...) method)
     * will trigger a displayStringChanged() notification.
     */
    public DisplayString getDisplayString();
}