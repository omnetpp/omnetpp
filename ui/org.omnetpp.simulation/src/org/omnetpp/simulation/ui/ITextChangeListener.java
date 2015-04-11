/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.ui;


/**
 * The TextViewer widget implements this listener to receive
 * notifications when changes to the model occur.
 *
 * @author Andras
 */
public interface ITextChangeListener {
    /**
     * Called when the content has changed.
     */
    public void textChanged(ITextViewerContentProvider textViewer);
}

