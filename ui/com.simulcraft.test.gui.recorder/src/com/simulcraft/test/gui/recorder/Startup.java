package com.simulcraft.test.gui.recorder;

import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IStartup;

public class Startup implements IStartup {

    public void earlyStartup() {
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                GUIRecorder recorder = new GUIRecorder();
                recorder.hookListeners();
                new RecorderUI(recorder).hookListeners();
            }
        });
    }

}
