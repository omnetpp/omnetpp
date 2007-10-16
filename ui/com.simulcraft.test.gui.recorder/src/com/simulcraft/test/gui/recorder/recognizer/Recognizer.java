package com.simulcraft.test.gui.recorder.recognizer;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IRecognizer;

public abstract class Recognizer implements IRecognizer {
    protected GUIRecorder recorder;
    
    public Recognizer(GUIRecorder recorder) {
        this.recorder = recorder;
    }
}