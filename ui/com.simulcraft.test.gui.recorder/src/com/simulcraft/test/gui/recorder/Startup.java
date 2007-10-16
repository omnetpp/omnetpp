package com.simulcraft.test.gui.recorder;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IStartup;

public class Startup implements IStartup {

    public void earlyStartup() {
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                Display display = Display.getDefault();
                GUIRecorder recorder = new GUIRecorder();
                display.addFilter(SWT.KeyDown, recorder);
                display.addFilter(SWT.KeyUp, recorder);
                display.addFilter(SWT.MouseDown, recorder);
                display.addFilter(SWT.MouseUp, recorder);
                display.addFilter(SWT.MouseDoubleClick, recorder);
                display.addFilter(SWT.MouseMove, recorder);
                display.addFilter(SWT.MouseHover, recorder);
                display.addFilter(SWT.MouseWheel, recorder);
                display.addFilter(SWT.MouseEnter, recorder);
                display.addFilter(SWT.MouseExit, recorder);
            }
        });
    }

}
