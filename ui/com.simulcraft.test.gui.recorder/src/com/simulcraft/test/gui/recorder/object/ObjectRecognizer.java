package com.simulcraft.test.gui.recorder.object;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IObjectRecognizer;
import com.simulcraft.test.gui.recorder.RecognizerBase;

public abstract class ObjectRecognizer extends RecognizerBase implements IObjectRecognizer {
    public ObjectRecognizer(GUIRecorder recorder) {
        super(recorder);
    }
}