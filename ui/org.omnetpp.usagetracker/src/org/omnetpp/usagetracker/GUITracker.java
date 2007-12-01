package org.omnetpp.usagetracker;

import com.simulcraft.test.gui.recorder.GUIRecorder;


/**
 * Records user activities into the log, as Java code
 *   
 * @author Andras
 */
public class GUITracker {
    protected GUIRecorder recorder;

    public GUITracker() {
        recorder = new GUIRecorder();
    }

    public void hookListeners() {
        recorder.hookListeners();
    }

    public void unhookListeners() {
        recorder.unhookListeners();
    }
}

