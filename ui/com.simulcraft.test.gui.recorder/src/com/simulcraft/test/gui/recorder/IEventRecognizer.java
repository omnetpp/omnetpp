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
