package com.simulcraft.test.gui.recorder.event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IEventRecognizer;
import com.simulcraft.test.gui.recorder.RecognizerBase;

public abstract class EventRecognizer extends RecognizerBase implements IEventRecognizer {
    public EventRecognizer(GUIRecorder recorder) {
        super(recorder);
    }
}