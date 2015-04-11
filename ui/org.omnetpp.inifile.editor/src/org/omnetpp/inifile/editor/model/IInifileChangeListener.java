/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

/**
 * Implement this to get notified about inifile changes.
 * @author andras
 */
public interface IInifileChangeListener {
    /**
     * @param event Callback invoked when a change occurs in the model
     */
    public void modelChanged();
}
