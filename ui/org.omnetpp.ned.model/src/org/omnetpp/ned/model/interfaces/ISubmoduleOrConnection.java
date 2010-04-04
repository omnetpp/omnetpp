/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.DisplayString;

/**
 * Collects common interfaces.
 */
public interface ISubmoduleOrConnection extends IHasDisplayString, IHasParameters, IHasType {

    /**
     * Returns the display string for this submodule or connection, assuming
     * that the actual type is the module or channel type passed in the <code>actualType</code> 
     * parameter. This is useful when the submodule/channel is defined with the "like" 
     * keyword, and the actual submodule type is known (and not only the <code>likeType</code>).
     * The actual type usually comes from an ini file or some other source outside the INedElement 
     * tree. 
     * 
     * Note that it cannot be assumed that for a submodule the actualType is a module type
     * (and not a channel) and vica versa, because the NED source or inifile values may be 
     * bogus.
     */
    public DisplayString getDisplayString(INedTypeElement actualType);
     
}
