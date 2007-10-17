package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IEventRecognizer;
import com.simulcraft.test.gui.recorder.JavaSequence;
import com.simulcraft.test.gui.recorder.RecognizerUtil;

public abstract class EventRecognizer extends RecognizerUtil implements IEventRecognizer {
    protected GUIRecorder recorder;
    
    public EventRecognizer(GUIRecorder recorder) {
        this.recorder = recorder;
    }
    
    public JavaSequence chainE(Event e, String expr, double quality) {
        return chain(recorder.identifyObjectIn(e), expr, quality);
    }

    public JavaSequence chainO(Object uiObject, String expr, double quality) {
        return chain(recorder.identifyObject(uiObject), expr, quality);
    }

}