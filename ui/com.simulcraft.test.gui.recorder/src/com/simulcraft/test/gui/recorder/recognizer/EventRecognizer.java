package com.simulcraft.test.gui.recorder.recognizer;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IEventRecognizer;
import com.simulcraft.test.gui.recorder.RecognizerUtil;

public abstract class EventRecognizer extends RecognizerUtil implements IEventRecognizer {
    protected GUIRecorder recorder;
    
    public EventRecognizer(GUIRecorder recorder) {
        this.recorder = recorder;
    }
}