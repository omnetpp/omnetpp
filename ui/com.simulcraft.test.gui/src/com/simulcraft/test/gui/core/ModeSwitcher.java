package com.simulcraft.test.gui.core;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.common.color.ColorFactory;

/**
 * Pause, and control the speed of the playback.
 * - Pause key: pause/resume playback
 * - Shift+Pause: toggle fast/normal execution (implies resume from paused state)
 *   
 * @author Andras
 */
public class ModeSwitcher implements Listener {
    private int modifierState = 0;

    private boolean fast = false;

    public void handleEvent(Event e) {
        if (e.keyCode == SWT.SHIFT || e.keyCode == SWT.CONTROL || e.keyCode == SWT.ALT) {
            // housekeeping: we need to keep modifier states ourselves (it doesn't arrive in the event) 
            if (e.type==SWT.KeyDown) modifierState |= e.keyCode;
            if (e.type==SWT.KeyUp) modifierState &= ~e.keyCode;
        }
        else if (e.type==SWT.KeyDown && e.keyCode==SWT.PAUSE) {
            if ((modifierState&SWT.SHIFT)==0) {
                // without shift: pause/resume
                if (GUITestCase.paused) {
                    resume();
                }
                else {
                    pause();
                }
            }
            else {
                // with shift: toggle normal/fast mode; also resume
                fast = !fast;
                resume();
            }
        }
    }

    private void pause() {
        GUITestCase.setTimeScale(1);
        AnimationEffects.displayTextBox("Paused!", ColorFactory.RED, 16, 1000);
        GUITestCase.paused = true;
    }

    private void resume() {
        GUITestCase.setTimeScale(1);
        AnimationEffects.displayTextBox(fast ? "Lightning Mode..." : "Normal Mode...", ColorFactory.RED, 16, 1000);
        GUITestCase.setTimeScale(fast ? 0.2 : 1.0);
        GUITestCase.paused = false;
    }
}

