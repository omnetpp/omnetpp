package com.simulcraft.test.gui.recorder.object;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IObjectRecognizer;
import com.simulcraft.test.gui.recorder.RecognizerUtil;

public abstract class ObjectRecognizer extends RecognizerUtil implements IObjectRecognizer {
    protected GUIRecorder recorder;
    
    public ObjectRecognizer(GUIRecorder recorder) {
        this.recorder = recorder;
    }
}