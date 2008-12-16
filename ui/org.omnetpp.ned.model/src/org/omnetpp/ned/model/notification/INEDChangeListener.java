/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

/**
 * Implement this to get notified about changes in a NED tree.
 * @author andras
 */
public interface INEDChangeListener {

    /**
     * Callback invoked when a change occurs in the model.
     */
    void modelChanged(NEDModelEvent event);
    
}
