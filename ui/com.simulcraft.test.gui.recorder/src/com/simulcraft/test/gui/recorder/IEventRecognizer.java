/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.recorder;

import org.eclipse.swt.widgets.Event;

/**
 * Recognizes a GUI event
 * @author Andras
 */
public interface IEventRecognizer {
    /**
     * Returns some code which produce the event when played back.
     */
    JavaSequence recognizeEvent(Event e);
}
